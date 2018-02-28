#include "_CompileFlags.h"

#ifndef Speedometer_H
#define Speedometer_H

// Includes
#include <boost/thread/mutex.hpp>
#include <string>
#include <queue>

#include "ThreadManager.h"
#include "wiringPi.h"

#include "Arduino.h"

// Functions
void Speedometer( const ThreadManager* threadManager );
void pulse();
double getSpeed();

// Constants
const int pulsePin = 7;
const unsigned int maxDiff = 1000;
const unsigned int debounce = 10;

#endif // #ifndef Speedometer_H