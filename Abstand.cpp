#include "Abstand.h"

using namespace std;

double distanceAverage[4];

void Abstand( const ThreadManager* threadManager ) {
	pinMode( 21, OUTPUT );
	pinMode( 13, INPUT );
	pinMode( 6, INPUT );
	pinMode( 11, INPUT );
	pinMode( 10, INPUT );

	wiringPiISR( 13, INT_EDGE_BOTH, measureTime<0, 13> );
	wiringPiISR( 6, INT_EDGE_BOTH, measureTime<1, 6> );
	wiringPiISR( 11, INT_EDGE_BOTH, measureTime<2, 11> );
	wiringPiISR( 10, INT_EDGE_BOTH, measureTime<3, 10> );

	digitalWrite( 21, 1 );
	delayMicroseconds( 10 );
	digitalWrite( 21, 0 );

	//cout << string( 4, '\n' ) << flush;
	delay( 50 );

	while ( true ) {
		digitalWrite( 21, 1 );
		delayMicroseconds( 50 );
		digitalWrite( 21, 0 );

		delay( 50 );

		/*cout << "\r\e[4A" << flush;

		for ( int i = 0; i < 4; i++ ) {
			cout << "\n\e[KDistance:\t" << distanceAverage[i] << " cm";
		}

		cout.flush();*/
	}
}

template<size_t sensor, int pin>
void measureTime() {
	static unsigned int microTime;
	static double distance;
	static double distanceList[bufferSize];
	static double sum;
	static size_t indexPos;

	if ( !digitalRead( pin ) ) {
		distance = (micros() - microTime) / 1000.0 * 17.15;

		if ( distance < 500.0 ) {
			distanceList[indexPos++] = distance;

			if ( indexPos >= bufferSize ) indexPos = 0;
		}

		sum = 0;

		for ( int i = 0; i < bufferSize; )
			sum += distanceList[i++];

		distanceAverage[sensor] = sum / bufferSizeD;
	} else {
		microTime = micros();
	}
}