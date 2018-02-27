#include "SignalHandler.h"

using namespace std;

map<int, list<void( *)()> > SignalHandler::handlers = map<int, list<void( *)()> >();

void SignalHandler::handleSignal( int signum ) {
	map<int, list<void( *)()> >::iterator handlersList = handlers.find( signum );

	if ( handlersList == handlers.end() )
		return;

	list<void( *)()>::iterator end( handlersList->second.end() );
	for ( list<void( *)()>::iterator it = handlersList->second.begin(); it != end; ++it ) {
		(*it)();
	}
}

SignalHandler::SignalHandlerHandle SignalHandler::addHandler( int signum, void( *handlerFunc )() ) {
	map<int, list<void( *)()> >::iterator handlersList = handlers.find( signum );

	if ( handlersList == handlers.end() ) {
		signal( signum, handleSignal );

		handlersList = handlers.insert( pair<int, list<void( *)()> >( signum, list<void( *)()>() ) ).first;
	} else if ( handlersList->second.empty() ) {
		signal( signum, handleSignal );
	}

	handlersList->second.push_back( handlerFunc );

	return SignalHandlerHandle( signum, handlersList->second, --handlersList->second.end() );
}

void SignalHandler::removeHandler( SignalHandler::SignalHandlerHandle& handle ) {
	handle.destroy();

	if ( handle.list.empty() )
		signal( handle.signum, SIG_DFL );
}