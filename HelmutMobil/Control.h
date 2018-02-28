#include "_CompileFlags.h"

#ifndef Control_H
#define Control_H

// Inlcudes
#include <atomic>
#include <vector>
#include <softServo.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "Arduino.h"
#include "Config.h"
#include "JoystickWrapper.h"
#include "LED.h"
#include "ThreadManager.h"
#include "PID.h"

// Functions
void MotorControler( const ThreadManager* threadManager );
int mapRange( int inMin, int inMax, int outMin, int outMax, int value );

template<std::atomic_short& var>
void handleAxeMovement( short value );
template<bool an>
void hupe();

void setPoti( char value, bool shutdown = false );

// Constants
const int sampleRate = 20;

const int buttonHupe = 1;

const int pinMotorRelais = 22;

const int pinBremseEnable1 = 4;
const int pinBremseEnable2 = 5;

const int motorMin = 175;
const int motorMax = 240;

const int bremseMin = 0;
const int bremseMax = 255;

/*const int potiBremseMin = 300;
const int potiBremseMax = 750;*/

const int lenkungMin = -255;
const int lenkungMax = 255;

const int pinMC41100Enable = 27;

extern std::atomic_short axeMotorBremse;
extern std::atomic_short axeLenkung;

#endif // #ifndef Control_H