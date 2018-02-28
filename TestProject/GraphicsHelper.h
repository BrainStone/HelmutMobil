#include "_CompileFlags.h"

#ifndef GraphicsHelper_H
#define GraphicsHelper_H

#include <atomic>

#include "wiringPi.h"

class GraphicsHelper {
private:
	unsigned int lastFrame;
	std::atomic<bool> keepWaiting;

	unsigned int microsPerFrame;
	unsigned int delay;
	unsigned int wait;

public:
	GraphicsHelper();

	void limitFPS( double fps );
	void stopWaiting();
};

#endif // #ifndef GraphicsHelper_H