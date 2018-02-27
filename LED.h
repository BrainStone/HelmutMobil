#include "_CompileFlags.h"

#ifndef LED_H
#define LED_H

// Includes
#include "Arduino.h"
#include "Config.h"
#include "GraphicsHelper.h"
#include "JoystickWrapper.h"
#include "ThreadManager.h"
#include "wiringPi.h"
#include "wiringPiSPI.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <board_info.h>
#include <ws2811.h>

#ifdef __cplusplus
}
#endif

#define LED LEDStreifen::ledstrip.channel[1].leds

namespace LEDStreifen {
	// Functions
	void LEDControler( const ThreadManager* threadManager );
	template<const int numLeds>
	void blinker( int i, int index, double lampValue, bool bremse = false );
	void finish_leds();

	template<bool links>
	void toggleBlinker();
	void blinkerAus();
	void fussraumHelligkeit( short value );

	ws2811_led_t hsv2rgb( double h, double s, double v );
	double mod( double x, double y );
	double mindiff( double ref, double can1, double can2 );
	double randDouble();
	double randDoubleOpen();

	// Constants
	const int NumLED = 60;
	const int GpioPin = 13;

	// Variables
	extern ws2811_t ledstrip;

	extern double orangeH;
	extern ws2811_led_t fullOrange;
	extern bool rainbow;

	extern double bremsLichtH;
	extern ws2811_led_t fullBremsLicht;
	extern ws2811_led_t fullStandLicht;
	extern bool bremsLicht;

	extern double fussraumH, fussraumS, fussraumV;
	extern ws2811_led_t fussraum;
}

#endif // #ifndef LED_H