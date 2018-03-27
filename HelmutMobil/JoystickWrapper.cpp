#include "JoystickWrapper.h"

std::atomic<short> JoystickAxes[CountJoystickAxes];
std::atomic<bool> JoystickButtons[CountJoystickButtons];

std::mutex AxeEventContainerMutex;
std::mutex ButtonEventContainerMutex;

std::map<int, std::list<axeEventHandler_t>>    AxeEventContainer;
std::map<int, std::list<buttonEventHandler_t>> ButtonDownEventContainer;
std::map<int, std::list<buttonEventHandler_t>> ButtonUpEventContainer;

void JoystickHandler( const ThreadManager* threadManager ) {
	//sleep( 1 );

	//Joystick joystick( "/dev/input/by-id/usb-Mega_World_USB_Game_Controllers-joystick" );
	Joystick joystick( "/dev/input/by-id/usb-Arduino_LLC_Arduino_Leonardo_HIDFH-if02-joystick" );
	JoystickEvent event;

	std::list<axeEventHandler_t>    *tmpAxeEventHandlers = NULL;
	std::list<buttonEventHandler_t> *tmpButtonEventHandlers = NULL;

	if ( !joystick.isFound() ) {
		displayError( "No joystick detected!" );
		threadManager->ready();

		return;
	}

	threadManager->ready();

	while ( threadManager->getShouldRun() ) {
		while ( joystick.sample( &event ) ) {
			if ( event.isButton() ) {
				std::lock_guard<std::mutex> guard( ButtonEventContainerMutex );

				JoystickButtons[event.number] = event.value;

				if ( event.value )
					tmpButtonEventHandlers = &ButtonDownEventContainer[event.number];
				else
					tmpButtonEventHandlers = &ButtonUpEventContainer[event.number];

				for ( buttonEventHandler_t &buttonHandler : *tmpButtonEventHandlers )
					buttonHandler();
			} else if ( event.isAxis() ) {
				std::lock_guard<std::mutex> guard( AxeEventContainerMutex );

				JoystickAxes[event.number] = event.value;

				tmpAxeEventHandlers = &AxeEventContainer[event.number];

				for ( axeEventHandler_t& axeHandler : *tmpAxeEventHandlers )
					axeHandler( event.value );
			}
		}
	}
}

void registerAxeEvent( int axe, axeEventHandler_t axeEventHandler ) {
	if ( axe < 0 )
		throw std::underflow_error( "axe index cannot be negative" );
	else if ( axe >= CountJoystickAxes )
		throw std::overflow_error( "axe index is too high" );

	std::lock_guard<std::mutex> guard( AxeEventContainerMutex );

	AxeEventContainer[axe].push_back( axeEventHandler );
}

void registerButtonDownEvent( int button, buttonEventHandler_t buttonDownEventHandler ) {
	if ( button < 0 )
		throw std::underflow_error( "button index cannot be negative" );
	else if ( button >= CountJoystickButtons )
		throw std::overflow_error( "button index is too high" );

	std::lock_guard<std::mutex> guard( ButtonEventContainerMutex );

	ButtonDownEventContainer[button].push_back( buttonDownEventHandler );
}

void registerButtonUpEvent( int button, buttonEventHandler_t buttonUpEventHandler ) {
	if ( button < 0 )
		throw std::underflow_error( "button index cannot be negative" );
	else if ( button >= CountJoystickButtons )
		throw std::overflow_error( "button index is too high" );

	std::lock_guard<std::mutex> guard( ButtonEventContainerMutex );

	ButtonUpEventContainer[button].push_back( buttonUpEventHandler );
}