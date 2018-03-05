#include "LED.h"

using namespace std;

namespace LEDStreifen {
	ws2811_t ledstrip = generate_ws2811_t_object( WS2811_TARGET_FREQ, 5, 0, 0, 0, 0, GpioPin, 0, NumLED, 0x7F );
	ws2811_return_t ret;

	void farbverlauf( double h1, double s1, double v1, double h2, double s2, double v2 ) {
		const double divisor = NumLED - 1.0;

		h2 = mindiff( h1, mindiff( h1, h2, h2 + 360.0 ), h2 - 360.0 );
		double diff_h = (h2 - h1) / divisor;
		double diff_s = (s2 - s1) / divisor;
		double diff_v = (v2 - v1) / divisor;

		for ( int i = 0; i < NumLED; i++ ) {
			LED[i] = hsv2rgb( h1 + diff_h * i, s1 + diff_s * i, v1 + diff_v * i );
		}
	}

	double orangeH;
	ws2811_led_t fullOrange;
	bool rainbow;

	double bremsLichtH;
	ws2811_led_t fullBremsLicht;
	ws2811_led_t fullStandLicht;
	bool bremsLicht = false;

	unsigned int start;
	bool blinkerLinks = false;
	bool blinkerRechts = false;
	bool warnblinkerAktivierung = false;

	double fussraumH, fussraumS, fussraumV;
	ws2811_led_t fussraum;

	void LEDControler( const ThreadManager* threadManager ) {
		registerButtonUpEvent( 0, blinkerAus );
		registerButtonUpEvent( 2, toggleBlinker<true> ); // Links
		registerButtonUpEvent( 3, toggleBlinker<false> ); // Rechts
		registerAxeEvent( 5, fussraumHelligkeit );

		if ( (ret = ws2811_init( &ledstrip )) != WS2811_SUCCESS ) {
			cerr << "ws2811_init failed: %s\n" << ws2811_get_return_t_str( ret ) << endl;
			return;
		}

		// srand( (unsigned)time( NULL ) );
		// timet_t start = time( NULL );
		// double offset = 0;

		const double animationDuration = 1000000 / 1.25;
		orangeH = 25;
		fullOrange = hsv2rgb( orangeH, 1, 1 );
		rainbow = config::config.lookup("Rainbow");

		bremsLichtH = 0;
		fullBremsLicht = hsv2rgb( bremsLichtH, 1, 1 );
		fullStandLicht = hsv2rgb( bremsLichtH, 1, 0.3 );

		fussraumH = 20;
		fussraumS = 0.95;
		fussraumV = min( 1.0, (1.0 - ((double)((int)JoystickAxes[5] + 0x8000) / (double)0xFFFF)) * 1.2 );

		fussraum = hsv2rgb( fussraumH, fussraumS, fussraumV );

		LED[43] = fussraum;
		LED[44] = fussraum;

		start = micros();
		int indexVorne, indexHinten;
		double lampValue;
		GraphicsHelper gh;
		const ws2811_led_t warmWhite = hsv2rgb( 31, 0.5, 1 );

		while ( threadManager->getShouldRun() ) {
			if ( blinkerLinks || blinkerRechts ) {
				lampValue = mod( (micros() - start) / animationDuration, 1.0 );

				Arduino::setRelais( 2, lampValue < 0.5 );

				lampValue *= lampValue;
			}

			for ( int i = 0; i < 15; i++ ) {
				indexHinten = i + 15;
				indexVorne = i + 45;

				if ( blinkerLinks ) {
					blinker<15>( i, indexHinten, lampValue, bremsLicht );
					blinker<13>( i, indexVorne, lampValue );
				} else {
					LED[indexHinten] = bremsLicht ? fullBremsLicht : fullStandLicht;
					if ( i < 13 ) LED[indexVorne] = warmWhite;
				}

				indexHinten = 14 - i;
				indexVorne = 42 - i;

				if ( blinkerRechts ) {
					blinker<15>( i, indexHinten, lampValue, bremsLicht );
					blinker<13>( i, indexVorne, lampValue );
				} else {
					LED[indexHinten] = bremsLicht ? fullBremsLicht : fullStandLicht;
					if ( i < 13 ) LED[indexVorne] = warmWhite;
				}
			}

			ws2811_render( &ledstrip );
			gh.limitFPS( 60 );
		}

		Arduino::setRelais( 2, true );

		finish_leds();
	}

	void finish_leds() {
		for ( int i = 0; i < NumLED; i++ )
			LED[i] = 0;
		ws2811_render( &ledstrip );

		ws2811_fini( &ledstrip );
	}

	template<const int numLeds>
	void blinker( int i, int index, double lampValue, bool bremse ) {
		//                                                seconds
		static const double rainbowOffsetFactor = -360.0 / (5.0 * 1000000.0);
		static const double offsetShift = 360.0 / (double)numLeds;

		if ( i >= numLeds )
			return;

		double lampPos;
		int lampPosInt;

		lampValue = modf( lampValue * (double)numLeds, &lampPos );
		lampPosInt = lampPos;

		if ( rainbow ) {
			double offset = mod( micros() * rainbowOffsetFactor, 360 );

			if ( i < lampPosInt ) {
				LED[index] = hsv2rgb( offset + offsetShift * i, 1, 1 );
			} else if ( i == lampPosInt ) {
				LED[index] = hsv2rgb( offset + offsetShift * i, 1, lampValue );
			} else {
				LED[index] = bremse ? fullBremsLicht : 0x000000;
			}
		} else {
			if ( i < lampPosInt ) {
				LED[index] = fullOrange;
			} else if ( i == lampPosInt ) {
				LED[index] = hsv2rgb( orangeH, 1, lampValue );
			} else {
				LED[index] = bremse ? fullBremsLicht : 0x000000;
			}
		}
	}

	template<bool links>
	void toggleBlinker() {
		if ( JoystickButtons[0] ) {
			blinkerRechts = true;
			blinkerLinks = true;
			warnblinkerAktivierung = true;
		} else if ( warnblinkerAktivierung ) {
			warnblinkerAktivierung = false;
		} else if ( links ) {
			blinkerLinks = !blinkerLinks;
			blinkerRechts = false;
		} else {
			blinkerRechts = !blinkerRechts;
			blinkerLinks = false;
		}

		if ( blinkerLinks || blinkerRechts )
			start = micros();
		else
			Arduino::setRelais( 2, true );
	}

	void blinkerAus() {
		if ( JoystickButtons[2] || JoystickButtons[3] ) {
			blinkerRechts = true;
			blinkerLinks = true;
			warnblinkerAktivierung = true;
		} else if ( warnblinkerAktivierung ) {
			warnblinkerAktivierung = false;
		} else {
			blinkerRechts = false;
			blinkerLinks = false;

			Arduino::setRelais( 2, true );
		}
	}

	void fussraumHelligkeit( short value ) {
		fussraumV = min(1.0, (1.0 - ((double)((int)value + 0x8000) / (double)0xFFFF)) * 1.2);

		fussraum = hsv2rgb( fussraumH, fussraumS, fussraumV );

		LED[43] = fussraum;
		LED[44] = fussraum;
	}

	ws2811_led_t hsv2rgb( double h, double s, double v ) {
		double      hh, p, q, t, ff;
		long        i;
		double      r, g, b;

		if ( s <= 0.0 ) {       // < is bogus, just shuts up warnings
			return 0x000000;
		}
		hh = mod( h, 360.0 );
		hh /= 60.0;
		i = (long)hh;
		ff = hh - i;
		p = v * (1.0 - s);
		q = v * (1.0 - (s * ff));
		t = v * (1.0 - (s * (1.0 - ff)));

		switch ( i ) {
		case 0:
			r = v;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = v;
			b = p;
			break;
		case 2:
			r = p;
			g = v;
			b = t;
			break;

		case 3:
			r = p;
			g = q;
			b = v;
			break;
		case 4:
			r = t;
			g = p;
			b = v;
			break;
		case 5:
		default:
			r = v;
			g = p;
			b = q;
			break;
		}

		return ((ws2811_led_t)round( r * 255.0 ) << 16) | ((ws2811_led_t)round( g * 255.0 ) << 8) | ((ws2811_led_t)round( b * 255.0 ));
	}

	double mod( double x, double y ) {
		return fmod( fmod( x, y ) + y, y );
	}

	double mindiff( double ref, double can1, double can2 ) {
		double diff1 = abs( ref - can1 );
		double diff2 = abs( ref - can2 );

		if ( diff1 > diff2 )
			return can2;
		else
			return can1;
	}

	double randDouble() {
		return rand() / (double)RAND_MAX;
	}

	double randDoubleOpen() {
		return rand() / (RAND_MAX + 1.);
	}
}