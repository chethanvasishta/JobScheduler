#ifndef PERF_H
#define PERF_H

#include "stdafx.h"

double MeasureThreadCreationCost(const uint maxThreads = 100, const uint stepSize = 10, const uint numIterations = 1000);


#endif // PERF_H