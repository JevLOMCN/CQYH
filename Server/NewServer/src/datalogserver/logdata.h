#ifndef _LOG_DATA_H_
#define _LOG_DATA_H_

#include "thread.h"
#include "circle_list.h"
#include "dl_def.h"
#include <map>
#include <string>

#define FOLDER_NAME			"logdata"

#define MAX_LOGDATA_PARAM_SIZE          (65535)
#define MAX_LOGDATA_FLUSH_INTERVAL		(1)

class CLogData
{
public:
	CLogData();
	~CLogData();

	BOOL	Init();
	BOOL	UnInit();

	BOOL	AsyncWriteLog(INT32 nModuleIndex, const char* pMessage);

private:
	static CLogData ms_Instance;

	void _CreateHeader(HANDLE hFile, INT32 nModuleIndex);
	static THREAD_DECLARE(_WorkerThread)(void * pParam);

private:
	CThread         m_Thread[dlmtTotal];
	std::atomic<bool> m_bStopThread{false};
	CCircleList     m_PushList[dlmtTotal];
};

#endif  // _LOG_DATA_H_