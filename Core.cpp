#include "Core.h"
#include <libconfig.h++>

using namespace std;

void lol();

int handle;
boost::mutex waitForBoot;

ThreadManager* threadLEDControler;
ThreadManager* threadMotorControler;

template <int signum>
void signal_callback_handler() {
	printf( "\nCaught interrupt signal %i\n", signum );
	// Cleanup and close up stuff here

	config::safeConfig();

	Arduino::setRelais( 4, true );

	if ( threadLEDControler != NULL ) {
		threadLEDControler->softKill();

		threadLEDControler->wait();

		delete threadLEDControler;
		threadLEDControler = NULL;
	}

	if ( threadMotorControler != NULL ) {
		threadMotorControler->softKill();

		threadMotorControler->wait();

		delete threadMotorControler;
		threadMotorControler = NULL;
	}

	// Terminate program
	exit( signum );
}

void bootComplete() {
	waitForBoot.unlock();
	waitForBoot.unlock();
}

int main() {
	setlocale( LC_ALL, "de_DE.UTF-8" );

	ThreadManager::setPriority( 99 );

	ofstream pidFile;

	pidFile.open( "/tmp/HelmutMobil.pid" );
	pidFile << getpid();
	pidFile.close();

#ifdef SKIP_ERRORS
	cout << "!!!WARNING!!!\tSKIP_ERRORS enabled!\t!!!WARNING!!!" << endl;
#endif // #ifdef SKIP_ERRORS
#ifdef SKIP_WAIT
	cout << "!!!WARNING!!!\tSKIP_WAIT enabled!\t!!!WARNING!!!" << endl;
#endif // #ifdef SKIP_WAIT

	SignalHandler::SignalHandlerHandle bootCompleteInterruptHandler1 =
		SignalHandler::addHandler( SIGINT, bootComplete );
	SignalHandler::SignalHandlerHandle bootCompleteInterruptHandler2 =
		SignalHandler::addHandler( SIGUSR1, bootComplete );

	setBootMessage( "Booting" );
	ThreadManager threadBootScreen( "BootScreen", 1, renderBootScreen );

	startReading();

	// Waiting for Boot to complete
#ifndef SKIP_WAIT
	waitForBoot.lock();
	waitForBoot.lock();
#endif // #ifndef SKIP_WAIT

	setBootMessage( "Checking keyboard" );
	if ( checkForKey() ) {
		cerr << "Starting aborted" << endl;
		setBootMessage( "Starting aborted" );

		endBootScreen();
		threadBootScreen.wait();

		exit( -1 );
	}

	setBootMessage( "Loading config" );

	config::loadConfig();

	setBootMessage( "Setting up Signal Handlers" );
	SignalHandler::addHandler( SIGINT, signal_callback_handler<SIGINT> );
	SignalHandler::addHandler( SIGKILL, signal_callback_handler<SIGKILL> );
	SignalHandler::addHandler( SIGTERM, signal_callback_handler<SIGTERM> );
	SignalHandler::removeHandler( bootCompleteInterruptHandler1 );
	SignalHandler::removeHandler( bootCompleteInterruptHandler2 );

	setBootMessage( "Starting Touchscreen" );
	ThreadManager threadTouchScreen( "TouchScreen", 50, touchScreenThread, true );

	setBootMessage( "Initializing GPIOs" );
	wiringPiSetup();

	setBootMessage( "Initializing Arduino" );
	Arduino::init();

	setBootMessage( "Initializing Joystick" );
	ThreadManager threadJoystick( "Joystick", 50, JoystickHandler, true );

	setBootMessage( "Starting LED Controler" );
	threadLEDControler = new ThreadManager( "LEDControler", 1, LEDStreifen::LEDControler );

	setBootMessage( "Starting Motor Controller" );
	threadMotorControler = new ThreadManager( "MotorController", 80, MotorControler );

	setBootMessage( "Starting Speedometer" );
	ThreadManager threadSpeedometer( "Speedometer", 1, Speedometer );

	setBootMessage( "Starting Ultrasonic Sensors" );
	ThreadManager threadUltrasonicSensors( "UltrasonicSensors", 1, Abstand );

	endBootScreen();
	threadBootScreen.wait();

	//ThreadManager threadlol( "Lol", 1, lol );
	ThreadManager threadGUI( "GUI", 1, GUI::GUILoop );

	char option;
	double value;
	double v = 0.3;

	while ( true ) {
		scanf( "%c%lf", &option, &value );

		if ( option == 'h' ) {
			LEDStreifen::rainbow = false;

			LEDStreifen::orangeH = value;
			LEDStreifen::fullOrange = LEDStreifen::hsv2rgb( LEDStreifen::orangeH, 1, 1 );
		} else if ( option == 'r' ) {
			LEDStreifen::rainbow = true;
		} else if ( option == 'b' ) {
			LEDStreifen::ledstrip.channel[1].brightness = value;
		} else if ( option == 'v' ) {
			v = value;
			LEDStreifen::fullStandLicht = LEDStreifen::hsv2rgb( LEDStreifen::bremsLichtH, 1, v );
		} else if ( option == 'l' ) {
			LEDStreifen::bremsLichtH = value;
			LEDStreifen::fullBremsLicht = LEDStreifen::hsv2rgb( LEDStreifen::bremsLichtH, 1, 1 );
			LEDStreifen::fullStandLicht = LEDStreifen::hsv2rgb( LEDStreifen::bremsLichtH, 1, v );
		}

		config::config.lookup( "Rainbow" ) = LEDStreifen::rainbow;
	}

	/*vector<string> mplayerArgs( 9 );
	mplayerArgs[0] = "mplayer";
	mplayerArgs[1] = "-slave";
	mplayerArgs[2] = "-quiet";
	mplayerArgs[4] = "-input";
	mplayerArgs[5] = "nodefault-bindings";
	mplayerArgs[6] = "-noconfig";
	mplayerArgs[7] = "all";
	mplayerArgs[8] = "/root/sounds/Satellites (feat. Oscar Del Amor) _ (Official Audio) _ Klaypex  (2015).m4a";

	redi::pstream mplayer( mplayerArgs,
						   redi::pstreams::pstdout | redi::pstreams::pstderr | redi::pstream::pstdout | redi::pstream::pstdin );
	double total, current, percent;
	string tmp;

	mplayer << "get_time_length" << endl;
	while ( getline( mplayer.out(), tmp ) && (tmp.find( "ANS_LENGTH=" ) == string::npos) ) {
	}

	while ( !mplayer.eof() ) {
		mplayer << "get_time_length" << endl;
		getline( mplayer.out(), tmp, '=' );
		mplayer >> total;

		mplayer << "get_time_pos" << endl;
		getline( mplayer.out(), tmp, '=' );
		mplayer >> current;

		mplayer << "get_percent_pos" << endl;
		getline( mplayer.out(), tmp, '=' );
		mplayer >> percent;

		if ( percent >= 5 )
			mplayer << "quit" << endl;

		cout << "\r\e[K" << current << '/' << total << " (" << percent << "%)" << flush;

		delay( 100 );
	}

	cout << "\r\e[KEnd!" << endl;*/

	threadGUI.wait();
	threadJoystick.wait();
	threadMotorControler->wait();
	Arduino::waitForThreads();
}