#include "ThreadManager.h"

using namespace std;

map<const string, const ThreadManager* const> ThreadManager::threads;
boost::mutex ThreadManager::lock;

ThreadManager::ThreadManager( const string id, const int priority, void( *treadFunc )() )
	: id( id ), priority( priority ), thread( &ThreadManager::startThread, this, priority, treadFunc ),
	shouldRun( true ), waitForReady( false ), readyLock( NULL ) {
	lock.lock();
	threads.insert( pair<const string, const ThreadManager* const>( id, this ) );
	lock.unlock();
}

ThreadManager::ThreadManager( const string id, const int priority, void( *treadFunc )(string) )
	: id( id ), priority( priority ), thread( &ThreadManager::startThreadId, this, priority, treadFunc ),
	shouldRun( true ), waitForReady( false ), readyLock( NULL ) {
	lock.lock();
	threads.insert( pair<const string, const ThreadManager* const>( id, this ) );
	lock.unlock();
}

ThreadManager::ThreadManager( const string id, const int priority, void( *treadFunc )(const ThreadManager*), bool waitForReady )
	: id( id ), priority( priority ), thread( &ThreadManager::startThreadObject, this, priority, treadFunc ),
	shouldRun( true ), waitForReady( waitForReady ), readyLock( waitForReady ? new boost::mutex() : NULL ) {
	lock.lock();
	threads.insert( pair<const string, const ThreadManager* const>( id, this ) );
	lock.unlock();

	if ( waitForReady ) {
		readyLock->lock();
		readyLock->lock();

		readyLock->unlock();
	}
}

void ThreadManager::startThread( int priority, void( *threadFunc )() ) {
	setPriority( thread, priority );
	logStart();

	threadFunc();

	logEnd();
}

void ThreadManager::startThreadId( int priority, void( *threadFunc )(string) ) {
	setPriority( thread, priority );
	logStart();

	threadFunc( id );

	logEnd();
}

void ThreadManager::startThreadObject( int priority, void( *threadFunc )(const ThreadManager*) ) {
	setPriority( thread, priority );
	logStart();

	threadFunc( this );

	logEnd();
}

void ThreadManager::logStart() {
	clog << "Starting thread \"" << id << "\" with a priority of " << priority << "." << endl;
}

void ThreadManager::logEnd() {
	clog << "Thread \"" << id << "\" finished!" << endl;
}

void ThreadManager::wait() {
	thread.join();
}

void ThreadManager::softKill() {
	shouldRun = false;
}

void ThreadManager::kill() {
	thread.interrupt();
}

void ThreadManager::forceKill() {
	pthread_cancel( (pthread_t)thread.native_handle() );
}

bool ThreadManager::getShouldRun() const {
	return shouldRun;
}

void ThreadManager::setShouldRun( bool shouldRun ) {
	this->shouldRun = shouldRun;
}

void ThreadManager::ready() const {
	if ( waitForReady && (readyLock != NULL) )
		readyLock->unlock();
}

const ThreadManager* const ThreadManager::getThread( const std::string& id ) {
	return threads[id];
}

void ThreadManager::setPriority( int priority ) {
	setPriority( pthread_self(), priority );
}

void ThreadManager::setPriority( boost::thread& thread, int priority ) {
	setPriority( (pthread_t)thread.native_handle(), priority );
}

void ThreadManager::setPriority( const pthread_t& threadID, int priority ) {
	priority = (priority < 1) ? 1 : ((priority > 99) ? 99 : priority);

	int retcode;
	int policy;

	struct sched_param param;

	if ( (retcode = pthread_getschedparam( threadID, &policy, &param )) != 0 ) {
		errno = retcode;
		perror( "pthread_getschedparam" );

		return;
	}


	policy = SCHED_FIFO;
	param.sched_priority = priority;

	if ( (retcode = pthread_setschedparam( threadID, policy, &param )) != 0 ) {
		errno = retcode;
		perror( "pthread_setschedparam" );

		return;
	}
}
