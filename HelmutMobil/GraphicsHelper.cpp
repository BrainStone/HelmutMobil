#include "GraphicsHelper.h"

GraphicsHelper::GraphicsHelper() : lastFrame( 0 ), keepWaiting( true ) {}

void GraphicsHelper::limitFPS( double fps ) {
	microsPerFrame = 1000000.0 / fps;
	delay = (microsPerFrame + lastFrame) - micros();

	while ( keepWaiting && (delay > 0) && (delay <= microsPerFrame) ) {
		wait = (delay > 1000) ? 1000 : delay;
		
		delayMicroseconds( wait );

		delay = (microsPerFrame + lastFrame) - micros();
	}

	lastFrame = micros();
	keepWaiting = true;
}

void GraphicsHelper::stopWaiting() {
	keepWaiting = false;
}