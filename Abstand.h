#include "_CompileFlags.h"

#ifndef Abstand_H
#define Abstand_H

// Includes
#include <iostream>
#include <wiringPi.h>

#include "ThreadManager.h"

// Functions
void Abstand( const ThreadManager* threadManager );
template<size_t sensor, int pin> void measureTime();

// Constants
const size_t bufferSize = 10;
const double bufferSizeD = bufferSize;
extern double distanceAverage[4];

#endif // #ifndef Abstand_H