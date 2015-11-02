#include "stdafx.h"
#include "Perf.h"
#include <Windows.h>
#include <iostream>
#include <assert.h>

HANDLE gWaitEvent;
LARGE_INTEGER *gTimerArray;
int gNumThreads = 1000;

DWORD WINAPI DummyThreadExec(LPVOID params)
{	
	int *j = (int*)(params);
	assert(*j >= 0 && *j<gNumThreads);
	// assert (gNumThreads == 1000);
	QueryPerformanceCounter(&gTimerArray[*j]);
	//WaitForSingleObject(gWaitEvent, INFINITE);
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
				int k = j;
				threadArray[j] = CreateThread(
					NULL,	// Default security attributes
					0,		// Default stack size
					DummyThreadExec,
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