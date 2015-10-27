#ifndef EXECSYSTEM_H
#define EXECSYSTEM_H

#include "stdafx.h"
#include <Windows.h>
#include <queue>
#include <mutex>
#include "WorkerThreadPool.h"

typedef void (*JobFuncPtr)(void);

class Job
{
public:
	Job(JobFuncPtr fptr);
	JobFuncPtr GetProc() { return m_jobPtr; }
private:
	JobFuncPtr m_jobPtr;
	bool m_finished; //Temporary state variable. Fix this later!
};

// Contains a queue of executable jobs
// Threads pick up the job and start running them
class RunQ 
{
public:
	RunQ();
	~RunQ();

	void EnQ(Job* j);
	bool DeQ(Job** jPtr);
private:
	std::queue<Job*> m_rq;
	std::mutex qMutex; //Apparently std mutex on windows uses critical sections
};

class ExecSystem
{
public:
	void AddJob(Job* j);
	bool GetJob(Job **jPtr);
private:
	RunQ m_runQ;
	WorkerThreadPool m_ThreadPool;
};

#endif // EXECSYSTEM_H