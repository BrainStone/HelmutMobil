#include "_CompileFlags.h"

#ifndef BootScreen_H
#define BootScreen_H

#include <atomic>
#include <boost/thread.hpp>
#include <cstring>
#include <string>
#include <wiringPi.h>

#include "VG/openvg.h"
#include "VG/vgu.h"

#include "Button.h"
#include "fontinfo.h"
#include "GraphicsHelper.h"
#include "shapes.h"

#include <iostream>

void renderBootScreen();
void setBootMessage( const std::string& newBootMessage );
void endBootScreen();

void displayError( const std::string& message );

void actionIgnore();
void actionReboot();

#endif // #ifndef BootScreen_H