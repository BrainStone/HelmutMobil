#include "_CompileFlags.h"

#ifndef Core_H
#define Core_H

// Includes
#include <boost/thread/mutex.hpp>
#include <fstream>
#include <iostream>
#include <pstream.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#include "Abstand.h"
#include "Arduino.h"
#include "BootScreen.h"
#include "Config.h"
#include "Control.h"
#include "GlobalVars.h"
#include "GUI.h"
#include "JoystickWrapper.h"
#include "Keyboard.h"
#include "LED.h"
#include "SignalHandler.h"
#include "Speedometer.h"
#include "ThreadManager.h"
#include "TouchScreen.h"

// Functions
int main();

// Variables

extern double distanceAverage[4];

#endif // #ifndef Core_H