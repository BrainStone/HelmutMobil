#include "_CompileFlags.h"

#ifndef JoystickWrapper_H
#define JoystickWrapper_H

// Includes
#include <atomic>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <mutex>

#include "BootScreen.h"
#include "joystick.h"
#include "ThreadManager.h"
#include "wiringPi.h"

// Typedefs
typedef std::function<void( short )> axeEventHandler_t;
typedef std::function<void()>        buttonEventHandler_t;

// Functions
void registerAxeEvent( int axe,           axeEventHandler_t axeEventHandler );
void registerButtonDownEvent( int button, buttonEventHandler_t buttonDownEventHandler );
void registerButtonUpEvent( int button,	  buttonEventHandler_t buttonUpEventHandler );

void JoystickHandler( const ThreadManager* threadManager );


// Variables
constexpr int CountJoystickAxes = 4;
constexpr int CountJoystickButtons = 7;

extern std::atomic<short> JoystickAxes[CountJoystickAxes];
extern std::atomic<bool> JoystickButtons[CountJoystickButtons];

extern std::mutex AxeEventContainerMutex;
extern std::mutex ButtonEventContainerMutex;

extern std::map<int, std::list<axeEventHandler_t>>    AxeEventContainer;
extern std::map<int, std::list<buttonEventHandler_t>> ButtonDownEventContainer;
extern std::map<int, std::list<buttonEventHandler_t>> ButtonUpEventContainer;

const short AxeMax = 32767;
const short AxeMin = -32767;

#endif // #ifndef JoystickWrapper_H