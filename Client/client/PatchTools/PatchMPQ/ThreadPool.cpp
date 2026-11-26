// Filename		: ThreadPool.cpp
// Author		: Siddharth Barman
// Date			: 18 Sept 2005
// Description	: Implementation file for CThreadPool class. 
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "ThreadPool.h"
#include <process.h>
#include <memory>
#include <new>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//------------------------------------------------------------------------------

volatile long CThreadPool::m_lInstance = 0;

/* Parameters	: Pointer to a _threadData structure.
   Description	: This is the internal thread function which will run 
				  continuously till the Thread Pool is deleted. Any user thread
				  functions will run from within this function.
*/
#ifdef USE_WIN32API_THREAD
DWORD WINAPI CThreadPool::_ThreadProc(LPVOID pParam)
#else
UINT __stdcall CThreadPool::_ThreadProc(LPVOID pParam)
#endif
{
	DWORD					dwWait;
	CThreadPool*			pool;
#ifdef _DEBUG
	HANDLE					hThread = GetCurrentThread();
#endif
	LPTHREAD_START_ROUTINE  proc = NULL;
	LPVOID					data = NULL;
	DWORD					dwThreadId = GetCurrentThreadId();
	HANDLE					hWaits[2];
	IRunObject*				runObject = NULL;
	bool					bAutoDelete = false;

	ASSERT(pParam != NULL);
	if(NULL == pParam)
	{
		return (UINT)-1;
	}

	WorkerStart* startup = static_cast<WorkerStart*>(pParam);
	pool = startup->pool;
	hWaits[0] = startup->task;
	hWaits[1] = startup->shutdown;
	delete startup;
	
	loop_here:

	dwWait = WaitForMultipleObjects(2, hWaits, FALSE, INFINITE);

	if(dwWait - WAIT_OBJECT_0 == 0)
	{
		if(pool->CheckThreadStop())
		{
			return 0;
		}

		// a new function was added, go and get it
		if(pool->GetThreadProc(dwThreadId, proc, &data, &runObject))
		{	
			pool->BusyNotify(dwThreadId);
			
			if(proc == NULL)
			{
				// a function object is being used instead of 
				// a function pointer.
				bAutoDelete = runObject->AutoDelete();
				
				runObject->Run();

				// see if we need to free this object
				if(bAutoDelete)
				{
					#ifdef _DEBUG
					TCHAR szMessage[256];
					_stprintf_s(szMessage, 256, _T("Deleting Run Object of thread , handle = %p, id = %d\n"),
					 hThread, dwThreadId);
					TRACE(szMessage);
					#endif

					delete runObject;
				}
				else
				{
					runObject->pThreadPool = NULL;
					#ifdef _DEBUG
					TCHAR szMessage[256];
					_stprintf_s(szMessage, 256, _T("Not Deleted Run Object of thread , handle = %p, id = %d\n"),
					 hThread, dwThreadId);
					TRACE(szMessage);
					#endif
				}
			}
			else
			{
				// note: the user's data is wrapped inside UserPoolData object
				proc(data);
				
				// Note: data is created by the pool and is deleted by the pool.
				//       The internal user data is not deleted by the pool.
				UserPoolData* pPoolData = static_cast<UserPoolData*>(data);
				delete pPoolData;	
			}

			pool->FinishNotify(dwThreadId); // tell the pool, i am now free
		}

		goto loop_here;
	}
			
	return 0;
}

//------------------------------------------------------------------------------
/* Parameters	: Pool size, indicates the number of threads that will be 
				  available in the queue.
*******************************************************************************/
CThreadPool::CThreadPool(int nPoolSize, bool bCreateNow, int nWaitTimeForThreadsToComplete)
{
	m_state = Destroyed;
	m_hNotifyShutdown = NULL;
	ZeroMemory(m_szPoolName, sizeof(m_szPoolName));
	m_nPoolSize = nPoolSize;
	m_nWaitForThreadsToDieMS = nWaitTimeForThreadsToComplete;

	// this is used to protect the shared data like the list and map
	InitializeCriticalSection(&m_csLifetime);
	InitializeCriticalSection(&m_csFuncList); 
	InitializeCriticalSection(&m_csThreads); 

	if(bCreateNow)
	{
		if(!Create())
		{
			DeleteCriticalSection(&m_csThreads);
			DeleteCriticalSection(&m_csFuncList);
			DeleteCriticalSection(&m_csLifetime);
			throw 1;
		}
	}
}

//------------------------------------------------------------------------------

/* Description	: Use this method to create the thread pool. The constructor of
				  this class by default will create the pool. Make sure you 
				  do not call this method without first calling the Destroy() 
				  method to release the existing pool.
   Returns		: true if everything went fine else returns false.
  *****************************************************************************/
bool CThreadPool::Create()
{
	Guard lifetime(m_csLifetime);
	int poolSize = 0;
	{
		Guard functions(m_csFuncList);
		poolSize = m_nPoolSize;
	}
	if (m_state != Destroyed || poolSize <= 0)
		return false;
	const LONG instance = InterlockedIncrement(&CThreadPool::m_lInstance);
	_stprintf_s(m_szPoolName, sizeof(m_szPoolName) / sizeof(m_szPoolName[0]), _T("Pool%ld"), instance);
	m_hNotifyShutdown = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (!m_hNotifyShutdown)
		return false;

	for (int index = 0; index < poolSize; ++index)
	{
		TCHAR eventName[64];
		_stprintf_s(eventName, sizeof(eventName) / sizeof(eventName[0]),
			_T("PID:%lu IID:%ld TDX:%d"), GetCurrentProcessId(), instance, index);
		_ThreadData thread;
		thread.WaitHandle = CreateEvent(NULL, TRUE, FALSE, eventName);
		if (!thread.WaitHandle)
			break;
		WorkerStart* startup = new (std::nothrow) WorkerStart(this, thread.WaitHandle, m_hNotifyShutdown);
		if (!startup)
		{
			CloseHandle(thread.WaitHandle);
			break;
		}
#ifdef USE_WIN32API_THREAD
		thread.hThread = CreateThread(NULL, 0, CThreadPool::_ThreadProc, startup, 0, &thread.dwThreadId);
#else
		UINT id = 0;
		thread.hThread = (HANDLE)_beginthreadex(NULL, 0, CThreadPool::_ThreadProc, startup, 0, &id);
		thread.dwThreadId = id;
#endif
		if (!thread.hThread || thread.hThread == INVALID_HANDLE_VALUE)
		{
			delete startup;
			CloseHandle(thread.WaitHandle);
			break;
		}
		thread.bFree = true;
		bool inserted = false;
		try
		{
			Guard threads(m_csThreads);
			inserted = m_threads.insert(ThreadMap::value_type(thread.dwThreadId, thread)).second;
		}
		catch (...)
		{
		}
		if (!inserted)
		{
			m_state = Destroying;
			SetEvent(m_hNotifyShutdown);
			WaitForSingleObject(thread.hThread, INFINITE);
			CloseHandle(thread.WaitHandle);
			CloseHandle(thread.hThread);
			ReleaseWorkers();
			m_state = Destroyed;
			return false;
		}
	}
	if ((int)m_threads.size() != poolSize)
	{
		m_state = Destroying;
		ReleaseWorkers();
		m_state = Destroyed;
		return false;
	}
	m_state = Ready;
	return true;
}
//------------------------------------------------------------------------------

CThreadPool::~CThreadPool()
{
	Destroy();
	ReleaseMemory();	
	DeleteCriticalSection(&m_csFuncList);
	DeleteCriticalSection(&m_csThreads);
	DeleteCriticalSection(&m_csLifetime);
}
//------------------------------------------------------------------------------

void CThreadPool::ReleaseMemory()
{
	FunctionList pending;
	{
		Guard functions(m_csFuncList);
		pending.swap(m_functionList);
	}
	for (FunctionList::iterator iter = pending.begin(); iter != pending.end(); ++iter)
	{
		if (iter->lpStartAddress)
			delete static_cast<UserPoolData*>(iter->pData);
		else if (iter->pData)
		{
			IRunObject* object = static_cast<IRunObject*>(iter->pData);
			if (object->AutoDelete())
				delete object;
			else
				object->pThreadPool = NULL;
		}
	}
}

//------------------------------------------------------------------------------

/* Description	: Use this method to destory the thread pool. The destructor of
				  this class will destory the pool anyway. Make sure you 
				  this method before calling a Create() when an existing pool is 
				  already existing.
   Returns		: void
  *****************************************************************************/
void CThreadPool::Destroy()
{
	Guard lifetime(m_csLifetime);
	{
		Guard functions(m_csFuncList);
		if (m_state == Destroyed || m_state == Destroying)
			return;
		m_state = Destroying;
	}
	ReleaseWorkers();
	ReleaseMemory();
	m_state = Destroyed;
}

void CThreadPool::ReleaseWorkers()
{
	if (m_hNotifyShutdown)
		SetEvent(m_hNotifyShutdown);
	// Closing a handle does not stop its thread. Keep all pool data alive until every worker exits.
	for (ThreadMap::iterator iter = m_threads.begin(); iter != m_threads.end(); ++iter)
		WaitForSingleObject(iter->second.hThread, INFINITE);
	Guard threads(m_csThreads);
	for (ThreadMap::iterator iter = m_threads.begin(); iter != m_threads.end(); ++iter)
	{
		CloseHandle(iter->second.WaitHandle);
		CloseHandle(iter->second.hThread);
	}
	m_threads.clear();
	if (m_hNotifyShutdown)
		CloseHandle(m_hNotifyShutdown);
	m_hNotifyShutdown = NULL;
}
//------------------------------------------------------------------------------

int CThreadPool::GetPoolSize()
{
	Guard functions(m_csFuncList);
	return m_nPoolSize;
}
//------------------------------------------------------------------------------

/* Parameters	: nSize - number of threads in the pool.   
   ****************************************************************************/
void CThreadPool::SetPoolSize(int nSize)
{
	Guard functions(m_csFuncList);
	ASSERT(nSize > 0);
	if(nSize <= 0)
	{
		return;
	}

	m_nPoolSize = nSize;
}

//------------------------------------------------------------------------------
HANDLE CThreadPool::GetShutdownHandle()
{
	return m_hNotifyShutdown;
}
//------------------------------------------------------------------------------

/* Parameters	: hThread - Handle of the thread that is invoking this function.
   Return		: A ThreadProc function pointer. This function pointer will be 
			      executed by the actual calling thread.
				  NULL is returned if no functions list is empty.
																			  */
bool CThreadPool::GetThreadProc(DWORD dwThreadId, LPTHREAD_START_ROUTINE& Proc,
	LPVOID* Data, IRunObject** runObject)
{
	Proc = NULL;
	if (Data) *Data = NULL;
	if (runObject) *runObject = NULL;
	if (!Data || !runObject)
		return false;
	Guard functions(m_csFuncList);
	if (m_state != Ready || m_functionList.empty())
	{
		Guard threads(m_csThreads);
		ThreadMap::iterator thread = m_threads.find(dwThreadId);
		if (thread != m_threads.end())
		{
			thread->second.bFree = true;
			ResetEvent(thread->second.WaitHandle);
		}
		return false;
	}
	const _FunctionData& function = m_functionList.front();
	Proc = function.lpStartAddress;
	if (Proc)
		*Data = function.pData;
	else
		*runObject = static_cast<IRunObject*>(function.pData);
	{
		Guard threads(m_csThreads);
		ThreadMap::iterator thread = m_threads.find(dwThreadId);
		if (thread != m_threads.end())
			thread->second.bFree = false;
	}
	m_functionList.pop_front();
	return true;
}
//------------------------------------------------------------------------------

/* Parameters	: hThread - Handle of the thread that is invoking this function.
   Description	: When ever a thread finishes executing the user function, it 
				  should notify the pool that it has finished executing.      
																			  */
void CThreadPool::FinishNotify(DWORD dwThreadId)
{
	Guard functions(m_csFuncList);
	Guard threads(m_csThreads);
	ThreadMap::iterator thread = m_threads.find(dwThreadId);
	if (thread == m_threads.end())
		return;
	thread->second.bFree = true;
	if (m_functionList.empty())
		ResetEvent(thread->second.WaitHandle);
}
//------------------------------------------------------------------------------

void CThreadPool::BusyNotify(DWORD dwThreadId)
{
	ThreadMap::iterator iter;
	
	EnterCriticalSection(&m_csThreads);

	iter = m_threads.find(dwThreadId);

	if(iter == m_threads.end())	// if search found no elements
	{
		LeaveCriticalSection(&m_csThreads);
		ASSERT(!_T("No matching thread found."));
	}
	else
	{		
#if defined(_DEBUG)
		CString str;
		str.Format(_T("%d bFree = false BusyNtf\n"), dwThreadId);
		_debugString += str;
#endif

		m_threads[dwThreadId].bFree = false;		

		#ifdef _DEBUG
		TCHAR szMessage[256];
		_stprintf_s(szMessage, 256, _T("Thread busy, thread id = %d\n"), dwThreadId);
		TRACE(szMessage);
		#endif

		LeaveCriticalSection(&m_csThreads);
	}	
}
//------------------------------------------------------------------------------

/* Parameters	: pFunc - function pointer of type ThreadProc
				  pData - An LPVOID pointer

   Returns		: true is all goes well.
                  false if the pool is in the process of getting destroyed.
   
   Decription	: This function is to be called by clients which want to make 
				  use of the thread pool.
  *****************************************************************************/
bool CThreadPool::Run(LPTHREAD_START_ROUTINE pFunc, LPVOID pData, ThreadPriority priority)
{
	if (!pFunc)
		return false;
	Guard functions(m_csFuncList);
	if (m_state != Ready)
		return false;
	std::unique_ptr<UserPoolData> wrapper(new UserPoolData);
	wrapper->pData = pData;
	wrapper->pThreadPool = this;
	_FunctionData function;
	function.lpStartAddress = pFunc;
	function.pData = wrapper.get();
	if (priority == Low)
		m_functionList.push_back(function);
	else
		m_functionList.push_front(function);
	wrapper.release();
	WakeWorker();
	return true;
}
//------------------------------------------------------------------------------

/* Parameters	: runObject - Pointer to an instance of class which implements
							  IRunObject interface.
				  priority  - Low or high. Based on this the object will be
							  added to the front or back of the list.
   Returns		: true is all goes well.
                  false if the pool is in the process of getting destroyed.

   Decription	: This function is to be called by clients which want to make 
				  use of the thread pool.
  *****************************************************************************/
bool CThreadPool::Run(IRunObject* object, ThreadPriority priority)
{
	if (!object)
		return false;
	Guard functions(m_csFuncList);
	if (m_state != Ready)
		return false;
	_FunctionData function;
	function.pData = object;
	if (priority == Low)
		m_functionList.push_back(function);
	else
		m_functionList.push_front(function);
	object->pThreadPool = this;
	WakeWorker();
	return true;
}

void CThreadPool::WakeWorker()
{
	Guard threads(m_csThreads);
	for (ThreadMap::iterator thread = m_threads.begin(); thread != m_threads.end(); ++thread)
	{
		if (thread->second.bFree)
		{
			thread->second.bFree = false;
			SetEvent(thread->second.WaitHandle);
			break;
		}
	}
}
//------------------------------------------------------------------------------

/* Parameters	: ThreadId - the id of the thread for which the wait handle is 
							 being requested.
   Returns		: NULL if no mathcing thread id is present.
				  The HANDLE which can be used by WaitForXXXObject API.
  *****************************************************************************/
HANDLE CThreadPool::GetWaitHandle(DWORD dwThreadId)
{
	HANDLE hWait;
	ThreadMap::iterator iter;
	
	EnterCriticalSection(&m_csThreads);
	iter = m_threads.find(dwThreadId);
	
	if(iter == m_threads.end())	// if search found no elements
	{
		LeaveCriticalSection(&m_csThreads);
		return NULL;
	}
	else
	{		
		hWait = m_threads[dwThreadId].WaitHandle;
		LeaveCriticalSection(&m_csThreads);
	}	

	return hWait;
}
//------------------------------------------------------------------------------

bool CThreadPool::CheckThreadStop()
{
	// This is function expected to be called by thread functions or IRunObject
	// derived. The expectation is that the code will check this 'property' of
	// the pool and stop it's processing as soon as possible.
	return (m_state == Destroying || m_state == Destroyed);
}

//------------------------------------------------------------------------------
 
int CThreadPool::GetWorkingThreadCount()
{
	Guard threads(m_csThreads);
	int count = 0;
	for (ThreadMap::iterator thread = m_threads.begin(); thread != m_threads.end(); ++thread)
		if (!thread->second.bFree)
			++count;
	return count;
}
//------------------------------------------------------------------------------

PoolState CThreadPool::GetState()
{
	return m_state;
}

bool CThreadPool::IsAllTaskCompleted()
{
	Guard functions(m_csFuncList);
	Guard threads(m_csThreads);
	if (!m_functionList.empty())
		return false;
	for (ThreadMap::iterator thread = m_threads.begin(); thread != m_threads.end(); ++thread)
		if (!thread->second.bFree)
			return false;
	return true;
}

//------------------------------------------------------------------------------