#include "_CompileFlags.h"

#ifndef TouchScreen_H
#define TouchScreen_H

// Includes
#include <errno.h>
#include <fcntl.h>
#include <fstream>
#include <linux/input.h>
#include <list>

#include "BootScreen.h"
#include "ThreadManager.h"

// Functions
void touchScreenThread( const ThreadManager* threadManager );
void readLoop( const ThreadManager* threadManager, int fd );

void registerMoveEvent ( void( *moveEventHandler )	(double, double) );
void registerMouseEvent( void( *mouseEventHandler )	(bool) );

// Variables
extern std::list<void( *)(double, double)>	MoveEventContainer;
extern std::list<void( *)(bool)>			MouseEventContainer;

extern double mouseX, mouseY;
extern bool isMouseDown;

#endif // #ifndef TouchScreen_H