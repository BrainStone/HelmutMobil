#include "BootScreen.h"

using namespace std;

atomic<bool> booting( true );
char* bootMessage = NULL;
char* errorMessage = NULL;
VGfloat screenWidth, screenHeight;
const int fontSize = 40;

atomic<bool> showErrorScreen(false);
boost::mutex* ErrorLock;

Button* buttonIgnore;
Button* buttonReboot;

GraphicsHelper gh;

void renderBootScreen() {
	unsigned char points = 0;
	char* pointStrings[] = { "", ".", "..", "..." };
	char* imageFilename = "/hm/logo_helmutmobil.jpg";
	VGfloat textWidth;

	int width, height;
	init( &width, &height );

	screenWidth = width;
	screenHeight = height;

	const int imageWidth = 1056, imageHeight = 302;
	const VGfloat imageX = 0.0, imageY = screenHeight - imageHeight;
	const VGfloat textHeight = (screenHeight - imageHeight) / 2.0;

	while ( booting ) {
		if ( showErrorScreen ) {
			Start( screenWidth, screenHeight );

			Background( 255, 0, 0 );

			Fill( 255, 255, 0, 1.0 );
			TextMid( screenWidth * 0.5, screenHeight * 0.7, "!!! WARNING !!!", SansTypeface, fontSize * 2 );
			TextMid( screenWidth * 0.5, screenHeight * 0.5, errorMessage, SansTypeface, fontSize );

			buttonIgnore->render();
			buttonReboot->render();

			End();

			gh.limitFPS( 30.0 );
		} else {
			Start( screenWidth, screenHeight );

			Image( imageX, imageY, imageWidth, imageHeight, imageFilename );

			textWidth = TextWidth( bootMessage, SansTypeface, fontSize );

			Text( (screenWidth - textWidth) / 2.0, textHeight, bootMessage, SansTypeface, fontSize );
			Text( (screenWidth + textWidth) / 2.0, textHeight, pointStrings[points], SansTypeface, fontSize );

			End();

			points = (points + 1) % 4;

			gh.limitFPS( 1.0 );

			//clog << booting << endl;
		}
	}
}

void setBootMessage( const string& newBootMessage ) {
	if ( bootMessage != NULL )
		delete[] bootMessage;

	bootMessage = new char[newBootMessage.length() + 1];
	strcpy( bootMessage, newBootMessage.c_str() );

	gh.stopWaiting();
}

void endBootScreen() {
	booting = false;

	usleep( 10000 );

	delete buttonIgnore;
	delete buttonReboot;
	delete ErrorLock;
}

void displayError( const string& message ) {
	cerr << message << endl;

#ifndef SKIP_ERRORS
	showErrorScreen = true;

	if ( errorMessage != NULL )
		delete[] errorMessage;

	errorMessage = new char[message.length() + 1];
	strcpy( errorMessage, message.c_str() );

	if ( ErrorLock != NULL )
		delete ErrorLock;
	if ( buttonIgnore != NULL )
		delete buttonIgnore;
	if ( buttonReboot != NULL )
		delete buttonReboot;

	const VGfloat buttonWidth = 300.0;
	Button::Color up( 255, 255, 0 );
	Button::Color down( 255, 127, 0 );
	Button::Color font( 0, 0, 0 );

	ErrorLock = new boost::mutex();
	buttonIgnore = new Button( "Ignore", (screenWidth - (2.0 * buttonWidth)) / 3.0, screenHeight * 0.2, 300, 100, 30, up, down, font );
	buttonReboot = new Button( "Reboot", ((2.0 * screenWidth) - buttonWidth) / 3.0, screenHeight * 0.2, 300, 100, 30, up, down, font );

	buttonIgnore->addEventCallback( actionIgnore );
	buttonReboot->addEventCallback( actionReboot );

	ErrorLock->lock();
	ErrorLock->lock();
#endif // #ifndef SKIP_ERRORS
}

void actionIgnore() {
	showErrorScreen = false;
	gh.stopWaiting();

	ErrorLock->unlock();
	ErrorLock->unlock();
}

void actionReboot() {
	showErrorScreen = false;
	gh.stopWaiting();

	setBootMessage( "Rebooting" );

	system( "reboot" );
}