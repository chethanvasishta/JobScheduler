// Scheduler.cpp : Defines the entry point for the console application.
//
// Creates a set of worker threads. Schedules a set of jobs on the threads using co-operative multi tasking
// 

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <queue>
#include "ExecSystem.h"
#include "ErrorCode.h"
#include "DummyJobs.h"
#include "Perf.h"
using namespace std;

ExecSystem gExecSystem;

int _tmain(int argc, _TCHAR* argv[])
{
	//for(int i = 0 ; i < 10 ; ++i)
	//{
	//	Job* j = new Job(Foo1);
	//	gExecSystem.AddJob(j); //clear these job allocations!
	//}
	//int x;
	//cin >> x; //wait for all threads to finish
	//return 0;

	DoPerformanceAnalysis();
	return 0;
}

//Next step : Have a way of sending parameters to the jobs in a type safe way