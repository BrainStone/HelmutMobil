#include "_CompileFlags.h"

#ifndef SignalHandler_H
#define SignalHandler_H

#include <list>
#include <map>
#include <signal.h>

class SignalHandler {
private:
	static std::map<int, std::list<void (*)()> > handlers;

	SignalHandler();

	static void handleSignal( int signum );

public:
	class SignalHandlerHandle {
		friend class SignalHandler;

	public:
		SignalHandlerHandle( int signum, std::list<void( *)()> &list, std::list<void( *)()>::iterator iterator ) :
			signum( signum ),
			list( list ),
			iterator( iterator ) {}

	private:
		int signum;
		std::list<void( *)()> & list;
		std::list<void( *)()>::iterator iterator;

		void destroy() {
			if ( iterator != list.end() ) {
				list.erase( iterator );
				iterator = list.end();
			}
		}
	};

	static SignalHandlerHandle addHandler( int signum, void( *handlerFunc )() );
	static void removeHandler( SignalHandlerHandle& handle );
};

#endif // #ifndef SignalHandler_H