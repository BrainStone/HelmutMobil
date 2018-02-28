#include "Keyboard.h"

using namespace std;

list<int>* fds;

void startReading() {
	int i = 0;
	fds = new list<int>();

	do {
		stringstream sstm;
		sstm << "/dev/input/event" << i;

		fds->push_back( open( sstm.str().c_str(), O_RDONLY | O_NONBLOCK ) );

		i++;
	} while ( fds->back() != -1 );

	fds->pop_back();
}

bool checkForKey() {
	if ( fds->size() == 0 )
		return false;

	bool abort = false;
	struct input_event ev;
	ssize_t n;

	for ( list<int>::iterator it = fds->begin(); it != fds->end(); it++ ) {
		while ( true ) {
			n = read( *it, &ev, sizeof ev );

			if ( n == (ssize_t)-1 ) {
				if ( errno == EINTR )
					continue;
				else
					break;
			} else if ( n != sizeof ev ) {
				errno = EIO;
				break;
			}

			if ( (ev.type == EV_KEY) && (ev.value == 1) && (ev.code == exitKey) )
				abort = true;
		}

		close( *it );
	}

	delete fds;

	return abort;
}