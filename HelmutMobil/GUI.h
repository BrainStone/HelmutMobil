#include "_CompileFlags.h"

#ifndef GUI_H
#define GUI_H

// Includes
#include <string>
#include <pstream.h>

#include "Abstand.h"
#include "Arduino.h"
#include "Button.h"
#include "Config.h"
#include "Control.h"
#include "GraphicsHelper.h"
#include "Speedometer.h"
#include "ThreadManager.h"
#include "VGHelper.h"

// Class
class GUI {
private:
	static const ThreadManager* thread;
	static Button* herunterfahrenButton;
	static Button* bestaetigenButtonJa;
	static Button* bestaetigenButtonNein;
	static int width, height;
	static VGfloat percentage;
	static bool shouldRenderBestaetigung;
	static bool shuttingDown;
	static const char* imageFilename;
	static const int imageWidth = 528;
	static const int imageHeight = 151;

	static void renderBattery();
	static void renderOffButton( bool down, VGfloat x, VGfloat y, int width, int height );
	static void renderBestaetigung();
	static void renderGeschwindigkeit();
	static void renderLogo();
	static void renderUhrzeit();
	static void renderTemp();
	static void renderNeigung();
	static void renderAbstand();

	static void herunterfahrenButtonCallback();
	static void bestaetigenButtonJaCallback();
	static void bestaetigenButtonNeinCallback();

public:
	static void GUILoop( const ThreadManager* thread );
};

#endif // #ifndef GUI_H