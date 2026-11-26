// Filename		: RunObject.h
// Author		: Siddharth Barman
// Date			: 18 Sept 2005
// Description	: Defined interface IRunObject which is passed in to the 
//				  the Thread Pool. The Run() method of this interface is
//				  invoked in the thread.
//				  Derive a class from this struct and write your business logic
//				  here. 
//				  Make sure you create the instance of the derived class on the
//				  heap or a at a global scope.
//				  If you create the object in the heap, you can implement the 
//				  AutoDelete() function to return true. If it returns true, 
//				  the thread pool will use 'delete' to free the object. If 
//				  AutoDelete() returns false, the thread pool will not do 
//				  any memory management. It up to you to do that.
//------------------------------------------------------------------------------
#ifndef IRUN_OBJECT_DEFINED
#define IRUN_OBJECT_DEFINED

class CThreadPool;

struct IRunObject
{
	IRunObject() : pThreadPool(NULL) {}
	virtual void Run() = 0;
	virtual bool AutoDelete() = 0;
	virtual ~IRunObject(){};
	CThreadPool* pThreadPool;
};

#endif
