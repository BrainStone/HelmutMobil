#include "_CompileFlags.h"

#ifndef Keyboard_H
#define Keyboard_H

// Includes
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>
#include <string.h>
#include <stdio.h>
#include <list>
#include <sstream>

// Functions
void startReading();
bool checkForKey();

// Constants
const __u16 exitKey = KEY_B;
extern std::list<int>* fds;

static const char *const evval[3] = {
	"RELEASED",
	"PRESSED ",
	"REPEATED"
};

#endif // #ifndef Keyboard_H