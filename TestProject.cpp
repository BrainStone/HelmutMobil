#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include "VGHelper.h"
#include <wiringPi.h>
#include <cmath>
#include <string>
#include "Speedometer.h"
#include <atomic>
#include "Button.h"
#include "Control.h"

using namespace std;

int width, height;

const float voltFactor = 0.07811;
const int minPoti = round( 44.0 / voltFactor );
const int maxPoti = round( 53.5 / voltFactor );

void setup();
void lampe();
void draw( double power );

double sin1( double rad );

void lol() {
	setup();

	lampe();

	finish();
}

void setup() {
	init( &width, &height );
}

void ausSchalten() {
	Arduino::requestValue( Arduino::POWEROFF );

	system( "halt &" );
}

void lampe() {
	double power;
	Button aus( "aus", 100, 100 );
	aus.addEventCallback( ausSchalten );

	while ( /*millis() < 60000*/ true ) {
		power = sin1( millis() / 20000.0 );

		Start( width, height );

		draw( power );
		aus.render();

		End();
	}
}

void draw( double power ) {
	unsigned int t = millis();
	double rgb1 = sin1( t / 1568.0 ),
		rgb2 = sin1( t / 5863.0 ),
		rgb3 = sin1( t / 423.0 );

	//Start( width, height );

	Background( 0, 0, 0 );

	Fill( 255.0 * rgb1, 255.0 * rgb2, 255.0 * rgb3, 1 );
	Rect( 0, 0, width, power * height );

	Fill( 255, 0, 0, 1 );
	Rect( 0, height / 7.0, width * rgb1, height / 7.0 );

	Fill( 0, 255, 0, 1 );
	Rect( 0, (3.0 * height) / 7.0, width * rgb2, height / 7.0 );

	Fill( 0, 0, 255, 1 );
	Rect( 0, (5.0 * height) / 7.0, width * rgb3, height / 7.0 );

	//TextMid( width / 2, height / 2, getSpeedString() + " km/h", SansTypeface, 30 );

	VGfloat percentage = mapRange( minPoti, maxPoti, 0, 1000, Arduino::valuePOTI3_AVG ) / 1000.0;
	Fill( (percentage <= 0.5) ? 255 : 255 * (2.0 - (2.0 * percentage)),
		  (percentage >= 0.5) ? 255 : 255 * (2.0 * percentage), 0, 1 );
	Rect( width - 50, 0, 50, height * percentage );

	Fill( 0, 0, 0, 1 );

	char* out = new char[10];
	sprintf( out, "%5.1f%%", percentage * 100.0 );

	TextEnd( width, 0, out, SansTypeface, 20 );

	sprintf( out, "%4.1f V", (VGfloat)Arduino::valuePOTI3_AVG * voltFactor );
	TextEnd( width, 30, out, SansTypeface, 20 );

	delete[] out;

	//End();
}

double sin1( double rad ) {
	return (sin( rad ) + 1.0) / 2.0;
}