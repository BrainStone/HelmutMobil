#include "_CompileFlags.h"

#ifndef JoystickWrapper_H
#define JoystickWrapper_H

// Includes
#include <atomic>
#include <iostream>
#include <list>
#include <map>

#include "BootScreen.h"
#include "joystick.h"
#include "ThreadManager.h"
#include "wiringPi.h"

// Functions
void registerAxeEvent           ( int axe,    void( *axeEventHandler )		  (short) );
void registerButtonDownEvent	( int button, void( *buttonDownEventHandler ) () );
void registerButtonUpEvent		( int button, void( *buttonUpEventHandler )	  () );

void JoystickHandler( const ThreadManager* threadManager );


// Variables
extern std::atomic<short> JoystickAxes[6];
extern std::atomic<bool> JoystickButtons[4];

extern std::map<int, std::list<void( *)(short)>> AxeEventContainer;
extern std::map<int, std::list<void( *)()>>		 ButtonDownEventContainer;
extern std::map<int, std::list<void( *)()>>		 ButtonUpEventContainer;

const short AxeMax = 32767;
const short AxeMin = -32768;

#endif // #ifndef JoystickWrapper_H