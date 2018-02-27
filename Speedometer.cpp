#include "Speedometer.h"

using namespace std;

double SpeedSpeedometer;
unsigned int lastPulse;
queue<unsigned int> timestamps;

boost::mutex lockTimestamps;
boost::mutex lockSpeedSpeedometer;
boost::mutex lockLastPulse;

void Speedometer( const ThreadManager* threadManager ) {
	unsigned int timestamp;

	pinMode( pulsePin, INPUT );
	pullUpDnControl( pulsePin, PUD_UP );

	wiringPiISR( pulsePin, INT_EDGE_RISING, pulse );

	while ( threadManager->getShouldRun() ) {
		delay( 100 );
		timestamp = millis();

		lockTimestamps.lock();

		while ( (timestamps.size() > 0) && (timestamp - timestamps.front() > maxDiff) )
			timestamps.pop();

		lockSpeedSpeedometer.lock();

		SpeedSpeedometer = (double)timestamps.size() * 1465.2 / (double)maxDiff;

		lockSpeedSpeedometer.unlock();
		lockTimestamps.unlock();
	}
}

void pulse() {
	lockTimestamps.lock();
	lockLastPulse.lock();

	unsigned int time = millis();

	if ( time - lastPulse > debounce ) {
		timestamps.push( millis() );
	}

	lastPulse = time;

	lockTimestamps.unlock();
	lockLastPulse.unlock();
}

double getSpeed() {
	lockSpeedSpeedometer.lock();

	double tmp = SpeedSpeedometer;

	lockSpeedSpeedometer.unlock();

	return tmp;
}