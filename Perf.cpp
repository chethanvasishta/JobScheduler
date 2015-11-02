#include "stdafx.h"
#include "Perf.h"
#include <Windows.h>
#include <iostream>
#include <assert.h>
using namespace std;

// ------------------ Thread Creation Cost -----------------------

HANDLE gWaitEvent;
LARGE_INTEGER *gTimerArray;
int gNumThreads = 1000;

DWORD WINAPI CreateThreadExec(LPVOID params)
{	
	int *j = (int*)(params);
	assert(*j >= 0 && *j<gNumThreads);	
	QueryPerformanceCounter(&gTimerArray[*j]);
	return 0;
}

//Is this the only way to get 
LARGE_INTEGER Max_LargeInteger(LARGE_INTEGER* timerArray, uint n)
{
	LARGE_INTEGER max = timerArray[0];
	for(uint i = 0 ; i < n ; ++i)
	{
		if(max.QuadPart <  timerArray[i].QuadPart)
			max = timerArray[i];
	}
	return max;
}

double MeasureThreadCreationCost(const uint maxThreads, const uint stepSize, const uint numIterations)
{	
	const uint numSteps = maxThreads/stepSize;	
	double *threadCreationTimes = new double[numSteps];

	//Create stepSize, 2*stepSize, ... upto maxThreads and measure the time
	for(uint numThreads = stepSize, iterCount = 0 ; numThreads <= maxThreads; numThreads += stepSize, ++iterCount)
	{
		gNumThreads = numThreads;
		HANDLE *threadArray = new HANDLE[numThreads];
		
		gTimerArray = new LARGE_INTEGER[numThreads];

		//Time measurement taken from stack overflow : http://stackoverflow.com/questions/14337278/precise-time-measurement
		LARGE_INTEGER frequency;        // ticks per second
		LARGE_INTEGER t1, t2;           // ticks
		double elapsedTime = 0;

		// get ticks per second
		QueryPerformanceFrequency(&frequency);

		for(uint i = 0 ; i < numIterations; ++i)
		{
			// start timer
			QueryPerformanceCounter(&t1);
		
			for(uint j = 0 ; j < numThreads ; ++j)
			{
				int k = j; // Need to send a copy of j as CreateThread() is asynchronous and j would have changed by the time thread is created
				threadArray[j] = CreateThread(
					NULL,	// Default security attributes
					0,		// Default stack size
					CreateThreadExec,
					(void*)(&k),	// Parameters to function
					0,		// Default creation flags		
					NULL	// Address of variable to store the thread identifier
				);

				assert(threadArray[j] != NULL);
			}		

			//Wait for all threads to be created and executed
			WaitForMultipleObjects(numThreads, threadArray, true, INFINITE);

			// Get the timer values of the threads
			t2 = Max_LargeInteger(gTimerArray, numThreads);
			
			// compute and print the elapsed time in millisec
			elapsedTime += (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;

		}

		elapsedTime = elapsedTime / numIterations;

		threadCreationTimes[iterCount] = elapsedTime/numThreads;

		std::cout << numThreads << " threads created in " << elapsedTime << " milliseconds,\t per thread : " << threadCreationTimes[iterCount] << std::endl;

		// delete the threads and threadArray
		for(uint j = 0 ; j < numThreads ; ++j)
			CloseHandle(threadArray[j]);		

		delete[] threadArray;
		delete[] gTimerArray;
	}

	//Measure the slope of threadCreationTimes	
	double time = threadCreationTimes[0];
	delete[] threadCreationTimes;
	return time;
}

// ------------------ Context Switch Cost -----------------------

HANDLE* gWakeUpEventArray;
HANDLE* gThreadCreatedEventArray;
HANDLE gThreadStartEvent;
uint numContextSwitches = -1;

DWORD WINAPI ContextSwitchExec(LPVOID params)
{
	int threadID = *((int*)params);
	uint nextThreadID = (threadID + 1) % gNumThreads;

	assert(threadID >= 0 && threadID < gNumThreads);
	assert(nextThreadID >= 0 && nextThreadID < gNumThreads);

	SetEvent(gThreadCreatedEventArray[threadID]);

	int i = numContextSwitches;
	while(i >= 0)
	{
		WaitForSingleObject(gWakeUpEventArray[threadID], INFINITE);
		//cout << "In thread " << threadID << endl;
		SetEvent(gWakeUpEventArray[nextThreadID]);
		--i;
	}
	
	return 0;
}

double MeasureThreadContextSwitchCost(const uint maxThreads, const uint nContextSwitches)
{
	// create 1 to maxThreads threads, and similar number of events. Each thread gets an ID to wake up. Each thread also waits on its own event
	double elapsedTime = 0;
	double *threadContextSwitchTimes = new double[maxThreads - 1]; // we start from 2

	for(uint numThreads = 2 ; numThreads <= maxThreads ; ++numThreads) //minimum of two threads to do a context switch	
	{
		numContextSwitches = nContextSwitches;
		gNumThreads = numThreads;
		gWakeUpEventArray = new HANDLE[numThreads];
		gThreadCreatedEventArray = new HANDLE[numThreads];
		HANDLE* threadArray = new HANDLE[numThreads];
		for(uint i = 0 ; i < numThreads ; ++i)
		{
			gWakeUpEventArray[i] = CreateEvent(
						NULL,	// Default security attributes
						FALSE,	// Auto manual reset event
						FALSE,	// Initially reset
						NULL // Object name
						);

			gThreadCreatedEventArray[i] = CreateEvent(
						NULL,	// Default security attributes
						FALSE,	// Auto manual reset event
						FALSE,	// Initially reset
						NULL // Object name
						);

			uint threadID = i;			
			threadArray[i] = CreateThread(
					NULL,	// Default security attributes
					0,		// Default stack size
					ContextSwitchExec,
					(void*)(&threadID),	// Parameters to function
					0,		// Default creation flags		
					NULL	// Address of variable to store the thread identifier
				);
			Sleep(100);
		}

		LARGE_INTEGER frequency;        // ticks per second
		LARGE_INTEGER t1, t2;           // ticks

		// get ticks per second
		QueryPerformanceFrequency(&frequency);

		//Wait for all the threads to be created
		WaitForMultipleObjects(numThreads, gThreadCreatedEventArray, true, INFINITE);

		// start timer
		QueryPerformanceCounter(&t1);

		SetEvent(gWakeUpEventArray[0]); //trigger the first thread

		//Wait for all threads to be created and executed
		WaitForMultipleObjects(numThreads, threadArray, true, INFINITE);

		// end timer
		QueryPerformanceCounter(&t2);

		elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
		threadContextSwitchTimes[numThreads - 2] = elapsedTime;
		cout << "Cost of switching " << numContextSwitches << " times on " << numThreads << " threads : " << elapsedTime << " ms\t, Per context switch cost = " << elapsedTime/numThreads/numContextSwitches << endl;

		// clean up the handles and the array
		for(uint i = 0 ; i < numThreads ; ++i)
		{
			CloseHandle(gWakeUpEventArray);
			CloseHandle(gThreadCreatedEventArray);
			CloseHandle(threadArray);
		}

		delete[] threadArray;
		delete[] gWakeUpEventArray;
		delete[] gThreadCreatedEventArray;
	}

	elapsedTime = threadContextSwitchTimes[0]/2 /*numThreads*/ /numContextSwitches;
	delete[] threadContextSwitchTimes;
	return elapsedTime;
}