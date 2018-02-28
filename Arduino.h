#include "_CompileFlags.h"

#ifndef Arduino_H
#define Arduino_H

#include <boost/circular_buffer.hpp>
#include <boost/thread.hpp>
#include <list>
#include <queue>
#include <wiringPiI2C.h>
#include <wiringSerial.h>

#include "BootScreen.h"
#include "ThreadManager.h"

typedef uint8_t byte;
typedef short arduino_int;
typedef float arduino_double;

const size_t size_arduino_int = sizeof( arduino_int );
const size_t size_arduino_double = sizeof( arduino_double );

class Arduino {
private:
	struct Value {
		void* variable;
		size_t bytes;
		boost::mutex* lock;

		Value( void* variable, size_t bytes, boost::mutex* lock = NULL ) : variable( variable ), bytes( bytes ), lock( lock ) {}
		~Value() {
			if ( lock != NULL ) {
				lock->unlock();
				//lock->unlock();

				//delete lock;
			}
		}

	private:
		Value();
	};

	static std::queue<Value> buffer;
	static boost::circular_buffer<arduino_int> POTI3Values;
	static int serialHandle;
	static boost::mutex readLock;
	static bool initStatus;
	static const int pinResetSerial = 24;
	static const int pinResetI2C = 26;

	static int I2Cfd;
	static int Gyrofd;
	static boost::mutex I2CLock;
	static const __useconds_t I2CWaitTime = 5000;

	Arduino();

	static void readThread( const ThreadManager* threadManager );
	static void gyroThread( const ThreadManager* threadManager );
	static void I2CThread( const ThreadManager* threadManager );
	static void readAnalog( arduino_int& value, const byte reg );

public:
	static ThreadManager* threadRead;
	static ThreadManager* threadGyro;
	static ThreadManager* threadI2C;
	static ThreadManager* threadTester1;
	static ThreadManager* threadTester2;
	static const std::string threadReadID, threadGyroID, threadI2CID, threadTester1ID, threadTester2ID;

	static void requestValue( byte index, void* variable = NULL, size_t bytes = 0, bool wait = false );
	static void init();
	static void waitForThreads();

	static void I2CWriteReg8( int reg, int data );
	static void I2CWriteReg16( int reg, int data );
	static int  I2CReadReg8( int reg );
	static int  I2CReadReg16( int reg );

	static void setRelais( int relais, bool value );

	static const byte POTI1 = 0x00;
	static const byte POTI2 = 0x01;
	static const byte POTI3 = 0x02;
	static const byte ONB_TEMP = 0x03;
	static const byte GYRO_AcX = 0x04;
	static const byte GYRO_AcY = 0x05;
	static const byte GYRO_AcZ = 0x06;
	static const byte GYRO_TEMP = 0x07;
	static const byte GYRO_GyX = 0x08;
	static const byte GYRO_GyY = 0x09;
	static const byte GYRO_GyZ = 0x0A;
	static const byte DROSSEL = 0x0B;
	static const byte POWEROFF = 0x0C;

	static const byte START_ECHO_MODE = 0x0E;
	static const byte END_ECHO_MODE = 0x00;

	static const byte I2CAddress = 0x4E;
	static const byte FLAG_PWM = 0x80;

	static const byte I2CLAMP = 13;
	static const byte I2CPOTI1 = 15;	// A1
	static const byte I2CPOTI2 = 16;	// A2
	static const byte I2CPOTI3 = 17;	// A3

	static arduino_int valuePOTI1;
	static arduino_int valuePOTI2;
	static arduino_int valuePOTI3;
	static arduino_int valuePOTI3_AVG;

	static arduino_double valueGYRO_TEMP;
	static arduino_int valueGYRO_AcX;
	static arduino_int valueGYRO_AcY;
	static arduino_int valueGYRO_AcZ;
	static arduino_int valueGYRO_GyX;
	static arduino_int valueGYRO_GyY;
	static arduino_int valueGYRO_GyZ;
};

#endif // #ifndef Arduino_H