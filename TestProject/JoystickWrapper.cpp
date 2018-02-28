#include "JoystickWrapper.h"

using namespace std;

atomic<short> JoystickAxes[6];
atomic<bool> JoystickButtons[4];

map<int, list<void( *)(short)>> AxeEventContainer;
map<int, list<void( *)()>>		ButtonDownEventContainer;
map<int, list<void( *)()>>		ButtonUpEventContainer;

void JoystickHandler( const ThreadManager* threadManager ) {
	//sleep( 1 );

	Joystick joystick( "/dev/input/by-id/usb-Mega_World_USB_Game_Controllers-joystick" );
	JoystickEvent event;

	list<void( *)(short)>* tmpAxeEventHandlers    = NULL;
	list<void( *)()>*	   tmpButtonEventHandlers = NULL;

	list<void( *)(short)>::iterator axeIt;
	list<void( *)()>::iterator      buttonIt;

	if ( !joystick.isFound() ) {
		displayError("No joystick detected!");
		threadManager->ready();

		return;
	}

	threadManager->ready();

	while ( threadManager->getShouldRun() ) {
		while ( joystick.sample( &event ) ) {
			if ( event.isButton() ) {
				JoystickButtons[event.number] = event.value;

				if ( event.value )
					tmpButtonEventHandlers = &ButtonDownEventContainer[event.number];
				else
					tmpButtonEventHandlers = &ButtonUpEventContainer[event.number];

				for ( buttonIt = tmpButtonEventHandlers->begin(); buttonIt != tmpButtonEventHandlers->end(); buttonIt++ )
					(*buttonIt)();
			} else if ( event.isAxis() ) {
				JoystickAxes[event.number] = event.value;

				tmpAxeEventHandlers = &AxeEventContainer[event.number];

				for ( axeIt = tmpAxeEventHandlers->begin(); axeIt != tmpAxeEventHandlers->end(); axeIt++ )
					(*axeIt)(event.value);
			}
		}
	}
}

void registerAxeEvent( int axe, void( *axeEventHandler )(short) ) {
	AxeEventContainer[axe].push_back( axeEventHandler );
}

void registerButtonDownEvent( int button, void( *buttonDownEventHandler )() ) {
	ButtonDownEventContainer[button].push_back( buttonDownEventHandler );
}

void registerButtonUpEvent( int button, void( *buttonUpEventHandler )() ) {
	ButtonUpEventContainer[button].push_back( buttonUpEventHandler );
}