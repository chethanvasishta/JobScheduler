#ifndef WORKERTHREADPOOL_H
#define WORKERTHREADPOOL_H

#include "stdafx.h"
#include <Windows.h>
#include "ErrorCode.h"

class WorkerThreadPool
{
public:
	WorkerThreadPool();
	~WorkerThreadPool();
	ErrorCode CreateWorkerThreads();
	void NotifyJobAdded();
private:
	HANDLE *threadArray;
	size_t m_threadCount;
};

#endif // WORKERTHREADPOOL_H