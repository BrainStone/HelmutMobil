#include "_CompileFlags.h"

#ifndef ThreadManager_H
#define ThreadManager_H

#include <atomic>
#include <boost/thread.hpp>
#include <cstdio>
#include <iostream>
#include <map>
#include <string>

class ThreadManager {
private:
	const int priority;
	const std::string id;
	boost::thread thread;
	std::atomic<bool> shouldRun;
	bool waitForReady;
	boost::mutex* readyLock;

	void startThread( int priority, void( *threadFunc )() );
	void startThreadId( int priority, void( *threadFunc )(std::string) );
	void startThreadObject( int priority, void( *threadFunc )(const ThreadManager*) );

	void logStart();
	void logEnd();

	static std::map<const std::string, const ThreadManager* const> threads;
	static boost::mutex lock;
public:
	ThreadManager( const std::string id, const int priority, void( *treadFunc )() );
	ThreadManager( const std::string id, const int priority, void( *treadFunc )(std::string) );
	ThreadManager( const std::string id, const int priority, void( *treadFunc )(const ThreadManager*), bool waitForReady = false );

	void wait();
	
	void softKill();
	void kill();
	void forceKill();

	bool getShouldRun() const;
	void setShouldRun( bool shouldRun );

	void ready() const;

	static const ThreadManager* const getThread( const std::string& id );

	static void setPriority( int priority );
	static void setPriority( boost::thread&, int priority );
	static void setPriority( const pthread_t&, int priority );
};

#endif // #ifndef ThreadManager_H

