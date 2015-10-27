#include "stdafx.h"
#include "WorkerThreadPool.h"
#include "ErrorCode.h"

HANDLE gWakeUpEvent;

DWORD WINAPI Exec(LPVOID lpParam);

WorkerThreadPool::WorkerThreadPool()
{
	m_threadCount = 4; //get the actual core count
	threadArray = new HANDLE[m_threadCount];
	gWakeUpEvent = CreateEvent(
						NULL,	// Default security attributes
						FALSE,	// Not a manual reset event. !!! What's a manual reset event?
						FALSE,	// Initially reset
						TEXT("Wake Up Event") // Object name
						);
	CreateWorkerThreads();
}

WorkerThreadPool::~WorkerThreadPool()
{
	//Destroy the threads
	//...
	//delete the event
	//...
	//delete the array
	delete[] threadArray;
}

ErrorCode WorkerThreadPool::CreateWorkerThreads()
{
	for(uint i = 0 ; i < m_threadCount ; ++i)
	{
		threadArray[i] = CreateThread(
			NULL,	// Default security attributes
			0,		// Default stack size
			Exec,
			NULL,	// Parameters to function
			0,		// Default creation flags		
			NULL	// Address of variable to store the thread identifier
			);
	}
	return NoError;
}

void WorkerThreadPool::NotifyJobAdded()
{
	SetEvent(gWakeUpEvent);
}