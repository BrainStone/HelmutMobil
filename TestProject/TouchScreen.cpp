#include "TouchScreen.h"

using namespace std;

list<void( *)(double, double)>	MoveEventContainer;
list<void( *)(bool)>			MouseEventContainer;

double mouseX = 0, mouseY = 0;
bool isMouseDown = false;

void touchScreenThread( const ThreadManager* threadManager ) {
	bool keepTrying = true;
	int fd = open( "/dev/input/by-id/usb-eGalax_Inc._Touch-event-mouse", O_RDONLY );

	if ( fd == -1 ) {
		displayError( "No Touch Screen detected!" );
		threadManager->ready();

		return;
	}

	threadManager->ready();

	while ( keepTrying && threadManager->getShouldRun() ) {
		readLoop( threadManager, fd );

		close( fd );

		if ( threadManager->getShouldRun() ) {
			clog << "Connection to the Touch Screen was interupped\nTrying to connect again!" << endl;

			fd = -1;
			keepTrying = false;

			for ( int i = 0; i < 10; i++ ) {
				fd = open( "/dev/input/by-id/usb-eGalax_Inc._Touch-event-mouse", O_RDONLY );

				if ( fd != -1 ) {
					keepTrying = true;

					break;
				} else {
					clog << "Touch Screen could not be found!\nTrying again!" << endl;

					sleep( 1 );
				}
			}
		}
	}
}

void readLoop( const ThreadManager* threadManager, int fd ) {
	struct input_event ev;
	ssize_t n;

	bool isMoveEvent = false;
	bool isMouseEvent = false;

	list<void( *)(double, double)>::iterator	itMove;
	list<void( *)(bool)>::iterator				itMouse;

	while ( threadManager->getShouldRun() ) {
		n = read( fd, &ev, sizeof ev );

		if ( n == (ssize_t)-1 ) {
			if ( errno == EINTR )
				continue;
			else
				break;
		} else if ( n != sizeof ev ) {
			errno = EIO;
			break;
		}

		switch ( ev.type ) {
		case EV_SYN:
			if ( isMoveEvent )
				for ( itMove = MoveEventContainer.begin(); itMove != MoveEventContainer.end(); ++itMove )
					(*itMove)(mouseX, mouseY);

			if ( isMouseEvent )
				for ( itMouse = MouseEventContainer.begin(); itMouse != MouseEventContainer.end(); ++itMouse )
					(*itMouse)(isMouseDown);

			isMoveEvent = false;
			isMouseEvent = false;
			break;
		case EV_KEY:
			if ( ev.code == BTN_TOUCH ) {
				isMouseEvent = true;

				isMouseDown = ev.value;
			}
			break;
		case EV_ABS:
			if ( ev.code == 0 )
				mouseX = 0.540745f * ev.value - 16.2603f;
			else if ( ev.code == 1 )
				mouseY = 828.868f - 0.416258f * ev.value;

			isMoveEvent = true;
			break;
		}
	}
}

void registerMoveEvent( void( *moveEventHandler ) (double, double) ) {
	MoveEventContainer.push_back( moveEventHandler );
}

void registerMouseEvent( void( *mouseEventHandler ) (bool) ) {
	MouseEventContainer.push_back( mouseEventHandler );
}