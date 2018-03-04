#include "GUI.h"

using namespace std;

const ThreadManager* GUI::thread;
Button* GUI::herunterfahrenButton;
Button* GUI::bestaetigenButtonJa;
Button* GUI::bestaetigenButtonNein;
int GUI::width, GUI::height;
VGfloat GUI::percentage;
bool GUI::shouldRenderBestaetigung = false;
bool GUI::shuttingDown = false;
const char* GUI::imageFilename = "/hm/logo_helmutmobil_small.jpg";

double ausschaltenWidth;

void GUI::renderBattery() {
	static const float voltFactor = (*config::Battery)["voltFactor"];
	static const int minPoti = round( (float)(*config::Battery)["empty"] / voltFactor );
	static const int maxPoti = round( (float)(*config::Battery)["full"] / voltFactor );

	Fill( 0, 0, 0, 1 );
	Roundrect( width - 110, 40, 100, height - 175, 10, 10 );
	Roundrect( width - 76.666666, height - 145, 33.333333, 25, 10, 10 );

	percentage = mapRange( minPoti, maxPoti, 0, 1000, Arduino::valuePOTI3_AVG ) / 1000.0;
	Fill( (percentage <= 0.5) ? 255 : 255 * (2.0 - (2.0 * percentage)),
		  (percentage >= 0.5) ? 255 : 255 * (2.0 * percentage), 0, 1 );
	Roundrect( width - 100, 50, 80, (height - 195) * percentage, 5, 5 );

	char* out = new char[10];
	sprintf( out, "%3.0f%%", percentage * 100.0 );

	Fill( 255, 255, 255, 1 );

	Rotate( -90 );
	TextMid( 47.5 - height / 2.0, width - 90, out, SansTypeface, 60 );
	Rotate( 90 );

	Fill( 0, 0, 0, 1 );

	sprintf( out, "%3.1f V", (VGfloat)Arduino::valuePOTI3_AVG * voltFactor );
	TextMid( width - 60, 10, out, SansTypeface, 20 );

	delete[] out;
}

void GUI::renderOffButton( bool down, VGfloat x, VGfloat y, int width, int height ) {
	VGfloat stops[] = {
		0.0, 1.0, 1.0, 1.0, 0.5,
		1.0 / 3.0, 1.0, 1.0, 1.0, 0.5,
		2.0 / 3.0, 1.0, 0.0, 0.0, 0.5,
		1.0, 1.0, 1.0, 1.0, 0.5,
	};

	if ( down ) {
		stops[11] = 0.0;
		stops[12] = 1.0;
	}

	FillRadialGradient( x + 50, y + 45, x + 50, y + 45, 45, stops, 4 );
	StrokeWidth( 0 );
	Circle( x + 50, y + 45, 90 );

	Fill( 0, 0, 0, 0.0 );
	Stroke( 0, 0, 0, 1.0 );
	StrokeWidth( 8 );

	Line( x + 50, y + 50, x + 50, y + 90 );
	Arc( x + 50, y + 45, 60, 60, 110, 320 );

	Fill( 0, 0, 0, 1.0 );
	Stroke( 0, 0, 0, 0.0 );
	StrokeWidth( 0 );
}

void GUI::renderBestaetigung() {
	Fill( 200, 200, 200, 0.9 );
	Stroke( 0, 0, 0, 1.0 );
	StrokeWidth( 2.5 );

	Rect( (width - ausschaltenWidth) / 2.0, height / 2.0 - 67.5, ausschaltenWidth, 135 );

	StrokeWidth( 0 );
	Fill( 0, 0, 0, 1.0 );

	if ( shuttingDown ) {
		TextMid( width / 2.0, height / 2.0 - 15, "Wird heruntergefahren!", SansTypeface, 30 );
	} else {
		TextMid( width / 2.0, height / 2.0 + 15, "Wirklich ausschalten?", SansTypeface, 30 );

		bestaetigenButtonJa->render();
		bestaetigenButtonNein->render();
	}
}

void GUI::renderGeschwindigkeit() {
	static char* out = new char[8];
	static const double shiftWidth = TextWidth( "99", SansTypeface, 100 ) / 2.0 - TextWidth( " km/h", SansTypeface, 50 ) / 2.0;

	sprintf( out, "%2.0f", getSpeed() );

	Fill( 0, 0, 0, 1.0 );

	TextEnd( width / 2.0 + shiftWidth, height / 2.0 - 50, out, SansTypeface, 100 );
	Text( width / 2.0 + shiftWidth, height / 2.0 - 50, " km/h", SansTypeface, 50 );

	sprintf( out, "%2.0f km", percentage * 20.0 );

	TextMid( width / 2.0, height / 3.8, out, SansTypeface, 25 );
	Image( width / 2.0 - 150, height / 3.8, 68, 44, "/hm/symbols/charger.jpg" );
}

void GUI::renderLogo() {
	Image( (width - imageWidth) / 2.0, height - imageHeight, imageWidth, imageHeight, imageFilename );
}

void GUI::renderUhrzeit() {
	static char* out = new char[100];
	static time_t curTime;
	static struct tm* curTimeStruct;

	time( &curTime );
	curTimeStruct = localtime( &curTime );
	strftime( out, 100, "%A, %d.%m.%Y", curTimeStruct );

	TextMid( width / 2, height - imageHeight - 40, out, SansTypeface, 20 );

	strftime( out, 100, "%T", curTimeStruct );

	TextMid( width / 2, height - imageHeight - 85, out, SansTypeface, 30 );

	/*sprintf( out, "%8i    %8i", (int)axeMotorBremse, (int)axeLenkung );

	TextMid( width / 2, height - imageHeight - 130, out, SansTypeface, 30 );*/
}

void GUI::renderTemp() {
	static char* out = new char[64];
	static const VGfloat posX = 40;
	static const VGfloat posY = height - 85;

	Fill( 0, 0, 0, 0 );
	Stroke( 0, 0, 0, 1 );
	StrokeWidth( 5 );

	Circle( posX, posY, 30 );
	Line( posX - 10, posY + 10, posX - 10, posY + 55 );
	Line( posX + 10, posY + 10, posX + 10, posY + 55 );
	Arc( posX, posY + 55, 20, 20, 0, 180 );

	Fill( 255, 0, 0, 1 );
	Stroke( 255, 255, 255, 1 );
	StrokeWidth( 2.5 );

	Rect( posX - 6.25, posY, 12.5, 30 );

	StrokeWidth( 0 );

	Circle( posX, posY, 20 );

	Fill( 0, 0, 0, 0 );
	Stroke( 0, 0, 0, 1 );
	StrokeWidth( 5 );

	Line( posX - 12.5, posY + 20, posX, posY + 20 );
	Line( posX - 12.5, posY + 35, posX, posY + 35 );
	Line( posX - 12.5, posY + 50, posX, posY + 50 );

	Fill( 0, 0, 0, 1 );
	StrokeWidth( 0 );

	sprintf( out, "%.1f °C", Arduino::valueGYRO_TEMP );

	Text( posX * 2, posY + 10, out, SansTypeface, 30 );
}

void GUI::herunterfahrenButtonCallback() {
	herunterfahrenButton->setActive( false );
	bestaetigenButtonJa->setActive( true );
	bestaetigenButtonNein->setActive( true );

	shouldRenderBestaetigung = true;
}

void GUI::bestaetigenButtonJaCallback() {
	shuttingDown = true;

	bestaetigenButtonJa->setActive( false );
	bestaetigenButtonNein->setActive( false );

	Arduino::requestValue( Arduino::POWEROFF );

	system( "halt &" );

	sleep( 5 );

	system( "service aaahelmutmobil stop" );
}

void GUI::bestaetigenButtonNeinCallback() {
	herunterfahrenButton->setActive( true );
	bestaetigenButtonJa->setActive( false );
	bestaetigenButtonNein->setActive( false );

	shouldRenderBestaetigung = false;
}

void GUI::renderNeigung() {
	static const double RAD_TO_DEG = 180.0 / M_PI;
	static char* out = new char[64];
	static const VGfloat x = 150, y = 500;
	static VGfloat angle;

	angle = atan2( (double)Arduino::valueGYRO_AcY, (double)Arduino::valueGYRO_AcZ ) * RAD_TO_DEG;

	Fill( 0, 0, 0, 1 );
	Stroke( 0, 0, 0, 1 );
	StrokeWidth( 5 );

	Translate( x, y );
	Rotate( -90 );

	for ( int i = -90; i <= 90; i += 15 ) {
		if ( i % 45 == 0 )
			TextMid( 0, 85, to_string( i ) + "°", SansTypeface, 20 );
		Line( 0, 70, 0, 80 );

		Rotate( 15 );
	}

	Rotate( angle - 105.0 );

	Stroke( 255, 0, 0, 1 );
	StrokeWidth( 2.5 );

	Line( 0, 25, 0, 65 );

	StrokeWidth( 0 );

	Rect( -40, -20, 80, 40 );

	Rotate( -angle );
	Translate( -x, -y );
}

void GUI::renderAbstand() {
	static const VGfloat x = 150, y = 200;
	static VGfloat distance;

	Translate( x, y );

	Fill( 0, 0, 0, 0 );
	Stroke( 0, 0, 0, 1 );

	//distance = 50 * sin( millis() / 1000.0 ) + 50;

	StrokeWidth( 10 );

	// Front
	distance = max(0.0, min(100.0, distanceAverage[0] / 2.5));
	if ( distance != 0.0 ) {
		Stroke( 0, 0, 0, 1.0 - distance / 100.0 );
		Arc( 0, -40, distance * 2.0 + 177.5, distance * 2.0 + 177.5, 76, 28 );
	}

	// Back
	distance = max( 0.0, min( 100.0, distanceAverage[3] / 2.5 ) );
	if ( distance != 0.0 ) {
		Stroke( 0, 0, 0, 1.0 - distance / 100.0 );
		Arc( 0, 40, distance * 2.0 + 177.5, distance * 2.0 + 177.5, -76, -28 );
	}

	// Right
	distance = max( 0.0, min( 100.0, distanceAverage[1] / 2.5 ) );
	if ( distance != 0.0 ) {
		Stroke( 0, 0, 0, 1.0 - distance / 100.0 );
		Arc( -20, 0, distance * 2.0 + 96.6, distance * 2.0 + 96.6, -22.5, 45 );
	}
	
	// Left
	distance = max( 0.0, min( 100.0, distanceAverage[2] / 2.5 ) );
	if ( distance != 0.0 ) {
		Stroke( 0, 0, 0, 1.0 - distance / 100.0 );
		Arc( 20, 0, distance * 2.0 + 96.6, distance * 2.0 + 96.6, 157.5, 45 );
	}

	Fill( 0, 0, 0, 1 );
	StrokeWidth( 0 );

	Rect( -20, -40, 40, 80 );

	Translate( -x, -y );
}

void GUI::GUILoop( const ThreadManager* thread ) {
	GUI::thread = thread;
	GraphicsHelper gh;

	init( &width, &height );

	ausschaltenWidth = TextWidth( "Wird heruntergefahren!", SansTypeface, 30 ) + 25.0;



	herunterfahrenButton = new ButtonCustomRender( renderOffButton, width - 110, height - 110, 100, 100 );
	herunterfahrenButton->addEventCallback( herunterfahrenButtonCallback );

	bestaetigenButtonJa = new Button( "Ja", width / 2.0 - (125 + ((ausschaltenWidth - 250.0) / 6.0)), height / 2.0 - 57.5, 125, 50 );
	bestaetigenButtonNein = new Button( "Nein", width / 2.0 + ((ausschaltenWidth - 250.0) / 6.0), height / 2.0 - 57.5, 125, 50 );

	bestaetigenButtonJa->setActive( false );
	bestaetigenButtonNein->setActive( false );

	bestaetigenButtonJa->addEventCallback( bestaetigenButtonJaCallback );
	bestaetigenButtonNein->addEventCallback( bestaetigenButtonNeinCallback );

	Arduino::setRelais( 4, false );

	while ( thread->getShouldRun() ) {
		Start( width, height );

		Background( 255, 255, 255 );

		renderBattery();
		renderGeschwindigkeit();
		renderLogo();
		renderUhrzeit();
		renderTemp();
		herunterfahrenButton->render();
		renderNeigung();
		renderAbstand();

		if ( shouldRenderBestaetigung )
			renderBestaetigung();

		End();

		gh.limitFPS( 30.0 );
	}
}