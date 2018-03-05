#include "Arduino.h"

using namespace std;

const string Arduino::threadReadID = "Serial READ Thread";
const string Arduino::threadGyroID = "Gyro Thread";
const string Arduino::threadI2CID = "I2C Thread";
const string Arduino::threadTester1ID = "Serial Tester Thread 1";
const string Arduino::threadTester2ID = "Serial Tester Thread 2";
queue<Arduino::Value> Arduino::buffer;
boost::circular_buffer<arduino_int> Arduino::POTI3Values( 100 );
boost::mutex Arduino::I2CLock;
int Arduino::serialHandle = -1;
boost::mutex Arduino::readLock;
bool Arduino::initStatus;
int Arduino::I2Cfd;
int Arduino::Gyrofd;
ThreadManager* Arduino::threadRead = NULL;
ThreadManager* Arduino::threadGyro = NULL;
ThreadManager* Arduino::threadI2C = NULL;
ThreadManager* Arduino::threadTester1 = NULL;
ThreadManager* Arduino::threadTester2 = NULL;

arduino_int Arduino::valuePOTI1 = 0;
arduino_int Arduino::valuePOTI2 = 0;
arduino_int Arduino::valuePOTI3 = 0;
arduino_int Arduino::valuePOTI3_AVG = 0;

arduino_double Arduino::valueGYRO_TEMP = 0;
arduino_int Arduino::valueGYRO_AcX = 0;
arduino_int Arduino::valueGYRO_AcY = 0;
arduino_int Arduino::valueGYRO_AcZ = 0;
arduino_int Arduino::valueGYRO_GyX = 0;
arduino_int Arduino::valueGYRO_GyY = 0;
arduino_int Arduino::valueGYRO_GyZ = 0;

int _serialGetchar( int fd ) {
	for ( int i = 0; i < 100000; i++ ) {
		if ( serialDataAvail( fd ) > 0 )
			return serialGetchar( fd );

		usleep( 1 );
	}

	return -1;
}

void Arduino::readThread( const ThreadManager* threadManager ) {
	initStatus = false;

	serialHandle = serialOpen( "/dev/ttyAMA0", 115200 );

	if ( serialHandle < 0 ) {
		threadManager->ready();

		return;
	}

	serialFlush( serialHandle );

	int in;
	size_t bytesToRead, bytesRead;
	Value* value = NULL;
	char* readInBuffer = NULL;
	char out;

	serialPutchar( serialHandle, START_ECHO_MODE );

	for ( int i = 0; i < 128; i++ ) {
		if ( i == 127 )
			out = END_ECHO_MODE;
		else
			out = rand() % 255 + 1;

		serialPutchar( serialHandle, out );

		for ( int j = 0; j < 2; j++ ) {
			in = _serialGetchar( serialHandle );

			if ( (in == -1) || ((char)in != ((j) ? (out ^ 0xFF) : out)) ) {
				threadManager->ready();

				return;
			}
		}
	}

	initStatus = true;
	threadManager->ready();

	while ( threadManager->getShouldRun() ) {
		if ( buffer.size() <= 0 ) {
			readLock.lock();
			readLock.lock();
			readLock.unlock();
		}

		value = &buffer.back();

		readInBuffer = (char*)value->variable;
		bytesToRead = value->bytes;
		bytesRead = 0;

		do {
			if ( (in = serialGetchar( serialHandle )) != -1 ) {
				*readInBuffer = (char)in;

				++readInBuffer;
				++bytesRead;

				while ( (bytesRead < bytesToRead) && (serialDataAvail( serialHandle ) > 0) ) {
					in = serialGetchar( serialHandle );
					*readInBuffer = (char)in;

					++readInBuffer;
					++bytesRead;
				}
			} else {
				cerr << "Serial answer needed too long!" << endl;

				break;
			}
		} while ( bytesRead < bytesToRead );

		buffer.pop();
	}

	serialClose( serialHandle );
}

void Arduino::gyroThread( const ThreadManager* threadManager ) {
	Gyrofd = wiringPiI2CSetup( 0x69 );

	if ( Gyrofd == -1 ) {
		displayError( "No connection to the Gyro!" );
		threadManager->ready();

		return;
	}

	wiringPiI2CWriteReg8( Gyrofd, 0x6B, 0 );

	const uint8_t out = 0x3B;
	uint8_t* const in = new uint8_t[14];
	boost::circular_buffer<arduino_int> valuesGYRO_TEMP( 128 );

	while ( threadManager->getShouldRun() ) {
		write( Gyrofd, &out, 1 );
		read( Gyrofd, in, 14 );

		valueGYRO_AcX = (in[0] << 8) | in[1];
		valueGYRO_AcY = (in[2] << 8) | in[3];
		valueGYRO_AcZ = (in[4] << 8) | in[5];
		valuesGYRO_TEMP.push_back( (in[6] << 8) | in[7] );
		valueGYRO_GyX = (in[8] << 8) | in[9];
		valueGYRO_GyY = (in[10] << 8) | in[11];
		valueGYRO_GyZ = (in[12] << 8) | in[13];

		valueGYRO_TEMP = 0;

		for ( boost::circular_buffer<arduino_int>::iterator it = valuesGYRO_TEMP.begin(); it != valuesGYRO_TEMP.end(); ++it )
			valueGYRO_TEMP += *it;

		valueGYRO_TEMP = (valueGYRO_TEMP / valuesGYRO_TEMP.size() / 340.00) + 36.53;

		usleep( 100000 );
	}
}

void Arduino::I2CThread( const ThreadManager* threadManager ) {
	I2Cfd = wiringPiI2CSetup( 0x4E );

	if ( I2Cfd == -1 ) {
		displayError( "No connection to the I2C Arduino!" );
		threadManager->ready();

		return;
	}

	unsigned int i;

	for ( i = 0; (i < 10) && (wiringPiI2CReadReg16( I2Cfd, I2C_POTI1 ) == -1); i++ ) {
		if ( errno != 0 ) {
			cerr << errno << ": " << strerror( errno ) << endl;
		}

		if ( i == 9 ) {
			displayError( "No connection to the I2C Arduino!" );
			threadManager->ready();

			return;
		}

		usleep( 100000 );
	}

	usleep( 100000 );

	for ( i = 0; i < 10; i++ ) {
		if ( wiringPiI2CWriteReg8( I2Cfd, I2C_LAMP, ~i & 1 ) == -1 ) {
			displayError( "No connection to the I2C Arduino!" );
			threadManager->ready();

			return;
		}

		usleep( 20000 );
	}

	threadManager->ready();
	i = 100;

	while ( threadManager->getShouldRun() ) {
		readAnalog( valuePOTI1, I2C_POTI1 );
		usleep( I2CWaitTime );

		readAnalog( valuePOTI2, I2C_POTI2 );
		usleep( I2CWaitTime );

		if ( i >= 100 ) {
			readAnalog( valuePOTI3, I2C_POTI3 );

			POTI3Values.push_back( valuePOTI3 );

			int tmp_valuePOTI3_AVG = 0;
			for ( boost::circular_buffer<arduino_int>::iterator it = POTI3Values.begin(); it != POTI3Values.end(); ++it )
				tmp_valuePOTI3_AVG += *it;
			valuePOTI3_AVG = tmp_valuePOTI3_AVG / POTI3Values.size();

			i = 0;

			usleep( I2CWaitTime );
		}

		++i;
	}

	close( I2Cfd );
}

void Arduino::readAnalog( arduino_int& value, const byte reg ) {
	arduino_int tmp = I2CReadReg16( reg );

	if ( (tmp >= 0) && (tmp < 1024) )
		value = tmp;
	/*else
		cout << "I2C reading failed (" << (int)reg << ')' << endl;*/
}

void Arduino::requestValue( byte index, void* variable, size_t bytes, bool wait ) {
	boost::mutex* lock = NULL;
	bool answerRequested = (variable != NULL) && (bytes != 0);
	wait &= answerRequested;

	if ( wait ) {
		lock = new boost::mutex();
	}

	serialPutchar( serialHandle, index );

	if ( answerRequested ) {
		buffer.push( Value( variable, bytes, lock ) );
		readLock.unlock();
	}

	if ( wait ) {
		lock->lock();
		lock->lock();
		lock->unlock();

		delete lock;
	}
}

void Arduino::init() {
	pinMode( pinResetSerial, OUTPUT );
	pinMode( pinResetI2C, OUTPUT );
	digitalWrite( pinResetSerial, 1 );
	digitalWrite( pinResetI2C, 1 );
	usleep( 10000 );

	digitalWrite( pinResetSerial, 0 );
	digitalWrite( pinResetI2C, 0 );
	usleep( 10000 );

	digitalWrite( pinResetSerial, 1 );
	digitalWrite( pinResetI2C, 1 );
	usleep( 2000000 );

	threadI2C = new ThreadManager( Arduino::threadI2CID, 99, Arduino::I2CThread, true );

	threadRead = new ThreadManager( Arduino::threadReadID, 99, Arduino::readThread, true );

	if ( !initStatus ) {
		if ( serialHandle != -1 ) {
			serialPutchar( serialHandle, END_ECHO_MODE );
			serialClose( serialHandle );
		}

		displayError( "No connection to the Serial Arduino!" );

		return;
	}

	threadGyro = new ThreadManager( Arduino::threadGyroID, 99, Arduino::gyroThread );
}

void Arduino::waitForThreads() {
	threadRead->wait();
	threadGyro->wait();
	threadI2C->wait();
}

void Arduino::I2CWriteReg8( int reg, int data ) {
	I2CLock.lock();

	wiringPiI2CWriteReg8( I2Cfd, reg, data );

	I2CLock.unlock();

	usleep( I2CWaitTime );
}

void Arduino::I2CWriteReg16( int reg, int data ) {
	I2CLock.lock();

	wiringPiI2CWriteReg16( I2Cfd, reg, data );

	I2CLock.unlock();

	usleep( I2CWaitTime );
}

int Arduino::I2CReadReg8( int reg ) {
	int out;
	I2CLock.lock();

	write( I2Cfd, &reg, 1 );
	usleep( 1000 );
	read( I2Cfd, &out, 1 );

	I2CLock.unlock();

	usleep( I2CWaitTime );

	return out;
}

int Arduino::I2CReadReg16( int reg ) {
	int out( 0 );
	I2CLock.lock();

	write( I2Cfd, &reg, 1 );
	usleep( 1000 );
	read( I2Cfd, &out, 2 );

	I2CLock.unlock();

	usleep( I2CWaitTime );

	return out;
}

void Arduino::setRelais( int relais, bool value ) {
	int tmp;
	requestValue( (relais << 5) | (value << 4) | 0xF, &tmp, 1 );
}