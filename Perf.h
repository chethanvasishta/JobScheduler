#ifndef PERF_H
#define PERF_H

#include "stdafx.h"

double MeasureThreadCreationCost(const uint maxThreads = 100, const uint stepSize = 10, const uint numIterations = 1000);
double MeasureThreadContextSwitchCost(const uint maxThreads = 2, const uint nContextSwitches = 50000);
void MeasureCostOfAtomicInstructions(const uint maxThreads = 1,  const uint iterations = 1000);
void DoPerformanceAnalysis();

#endif // PERF_H