#include "stdafx.h"
#include "ExecSystem.h"
#include <iostream>
using namespace std;

// ------------- RunQ ------------

RunQ::RunQ() 
{	

}

RunQ::~RunQ() 
{
	//Delete the mutex
}

void RunQ::EnQ(Job* j) 
{
	qMutex.lock();
	m_rq.push(j);
	qMutex.unlock();
}

bool RunQ::DeQ(Job** j)
{
	bool hasJobs = true;
	qMutex.lock();
	if(m_rq.empty())
		hasJobs = false;
	else {
		*j = m_rq.front();
		m_rq.pop();
	}
	qMutex.unlock();
	return hasJobs;
}

// ------------- Job ------------

Job::Job(JobFuncPtr jPtr) : m_finished(false), m_jobPtr(jPtr)
{
	
}

// ------------- Exec System ------------

void ExecSystem::AddJob(Job* job)
{
	m_runQ.EnQ(job);
	m_ThreadPool.NotifyJobAdded();
}
	
bool ExecSystem::GetJob(Job **jPtr)
{
	return m_runQ.DeQ(jPtr);
}

// ------------- Exec Function ------------
extern HANDLE gWakeUpEvent;
extern ExecSystem gExecSystem;

// WINAPI == _stdcall; LPVOID = void far*
DWORD WINAPI Exec(LPVOID lpParam)
{
	while(true)
	{
		WaitForSingleObject(gWakeUpEvent, INFINITE);
		std::cout << "Got a signal. Will go check the runQ!" << endl;
		//while jobs still available, pick a job and run
		Job *j;
		while(gExecSystem.GetJob(&j))
		{
			(j->GetProc())();
		}
	}
	return 0;
}