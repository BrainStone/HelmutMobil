#include "Control.h"

using namespace std;

atomic_short axeMotorBremse( 0 );
atomic_short axeLenkung( 0 );

list<double> plot1( 1056 ), plot2( 1056 ), plot3( 1056 ), plot4( 1056 );
boost::mutex plots;

void MotorControler( const ThreadManager* threadManager ) {
	// Motor
	pinMode( pinMotorRelais, OUTPUT );

	// Bremse
	pinMode( pinBremseEnable1, OUTPUT );
	pinMode( pinBremseEnable2, OUTPUT );

	registerAxeEvent( 1, handleAxeMovement<axeMotorBremse> );
	registerAxeEvent( 0, handleAxeMovement<axeLenkung> );
	registerButtonDownEvent( buttonHupe, hupe<true> );
	registerButtonUpEvent( buttonHupe, hupe<false> );

	double inputBremse = 0, outputBremse = 0, setpointBremse = 0;
	double inputLenkung = 0, outputLenkung = 0, setpointLenkung = 0;
	double axeLenkungTweak;
	int motorPower = 0;
	int motorStartSamples = 0;
	bool bremseActive = true;

	PID bremsePID( &inputBremse, &outputBremse, &setpointBremse,
				   (*config::PID_Bremse)["P"], (*config::PID_Bremse)["I"], (*config::PID_Bremse)["D"], DIRECT, sampleRate );
	bremsePID.SetOutputLimits( -bremseMax, bremseMax );
	bremsePID.SetMode( AUTOMATIC );

	PID lenkungPID( &inputLenkung, &outputLenkung, &setpointLenkung,
					(*config::PID_Lenkung)["P"], (*config::PID_Lenkung)["I"], (*config::PID_Lenkung)["D"], DIRECT, sampleRate );
	lenkungPID.SetOutputLimits( lenkungMin, lenkungMax );
	lenkungPID.SetMode( AUTOMATIC );

	Arduino::setRelais( 1, false );
	Arduino::setRelais( 3, false );

	while ( threadManager->getShouldRun() ) {
		delay( 10 );

		// Motor
		if ( axeMotorBremse < 0 ) {
			if ( motorStartSamples < 15 ) {
				++motorStartSamples;
				motorPower = motorMin;
			} else {
				motorPower = mapRange( 0, AxeMin + 1, motorMin, motorMax, axeMotorBremse );
			}

			setPoti( motorPower );
			digitalWrite( pinMotorRelais, 1 );
		} else {
			motorStartSamples = 0;

			setPoti( motorPower, motorMin );
			digitalWrite( pinMotorRelais, 0 );
		}

		// Bremse
		if ( axeMotorBremse >= 0 ) {
			if ( axeMotorBremse == 0 )
				setpointBremse = (int)(*config::PID_Bremse)["Points"]["Neutral"] - 20;
			else
				setpointBremse = mapRange( 0, AxeMax, (int)(*config::PID_Bremse)["Points"]["Max"] - 20,
				(int)(*config::PID_Bremse)["Points"]["Min"] + 20, axeMotorBremse );
			inputBremse = Arduino::valuePOTI1;

			if ( bremsePID.Compute() ) {
				if ( ((Arduino::valuePOTI1 > (int)(*config::PID_Bremse)["Points"]["Neutral"]) && (outputBremse > 0.0)) ||
					 ((Arduino::valuePOTI1 < (int)(*config::PID_Bremse)["Points"]["Min"]) && (outputBremse < 0.0)) ||
					 (abs( outputBremse ) < 80.0) )
					 outputBremse = 0.0;

				Arduino::I2CWriteReg8( 5 | Arduino::FLAG_PWM, abs( outputBremse ) );

				digitalWrite( pinBremseEnable1, outputBremse >= 0.0 );
				digitalWrite( pinBremseEnable2, outputBremse <= 0.0 );

				bremseActive = true;
			}
		} else if ( bremseActive ) {
			Arduino::I2CWriteReg8( 5 | Arduino::FLAG_PWM, bremseMin );

			digitalWrite( pinBremseEnable1, 0 );
			digitalWrite( pinBremseEnable2, 0 );

			bremseActive = false;
		}

		LEDStreifen::bremsLicht = axeMotorBremse > 0;

		// Lenkung
		axeLenkungTweak = copysign( pow( abs( (double)axeLenkung ) / 32768.0,
			(*config::PID_Lenkung)["Potenz"] ), axeLenkung ) * 32768.0;

		if ( axeLenkung < 0 )
			setpointLenkung = mapRange( AxeMin, 0, (int)(*config::PID_Lenkung)["Points"]["Max"] - 20,
			(int)(*config::PID_Lenkung)["Points"]["Mitte"], axeLenkungTweak );
		else
			setpointLenkung = mapRange( 0, AxeMax, (int)(*config::PID_Lenkung)["Points"]["Mitte"],
			(int)(*config::PID_Lenkung)["Points"]["Min"] + 20, axeLenkungTweak );

		inputLenkung = (int)(log( Arduino::valuePOTI2 ) / log( 1023 ) * 1023);

		if ( lenkungPID.Compute() ) {
			if ( ((inputLenkung >( int )(*config::PID_Lenkung)["Points"]["Max"]) && (outputLenkung > 0.0)) ||
				 ((inputLenkung < (int)(*config::PID_Lenkung)["Points"]["Min"]) && (outputLenkung < 0.0)) ||
				 (abs( outputLenkung ) < (double)(*config::PID_Lenkung)["AntiPieps"]) )
				 outputLenkung = 0.0;

			Arduino::I2CWriteReg8( 9 | Arduino::FLAG_PWM, (outputLenkung < 0.0) ? -outputLenkung : 0 );
			Arduino::I2CWriteReg8( 10 | Arduino::FLAG_PWM, (outputLenkung > 0.0) ? outputLenkung : 0 );
		}
	}

	Arduino::setRelais( 0, true );
	Arduino::setRelais( 1, true );
	Arduino::setRelais( 3, true );

	setPoti( 0, true );
	digitalWrite( pinMotorRelais, 0 );

	Arduino::I2CWriteReg8( 5 | Arduino::FLAG_PWM, 0 );
	digitalWrite( pinBremseEnable1, 0 );
	digitalWrite( pinBremseEnable2, 0 );

	Arduino::I2CWriteReg8( 9 | Arduino::FLAG_PWM, 0 );
	Arduino::I2CWriteReg8( 10 | Arduino::FLAG_PWM, 0 );
}

int mapRange( int inMin, int inMax, int outMin, int outMax, int value ) {
	if ( inMax < inMin ) {
		inMin *= -1;
		inMax *= -1;
		value *= -1;
	}

	if ( value <= inMin )
		return outMin;
	else if ( value >= inMax )
		return outMax;

	return ((outMax - outMin) * (value - inMin)) / (inMax - inMin) + outMin;
}

template<atomic_short& var>
void handleAxeMovement( short value ) {
	var = value;
}

template<bool an>
void hupe() {
	Arduino::setRelais( 0, !an );
}

void setPoti( char value, bool shutdown ) {
	static int SPIfd = -111;
	static short SPIdata;
	static const short CMDwrite = 0x11;
	static const short CMDreset = 0x21;

	if ( SPIfd == -111 ) {
		SPIfd = wiringPiSPISetup( 0, 5000000 );

		pinMode( pinMC41100Enable, OUTPUT );
		digitalWrite( pinMC41100Enable, 1 );
	}

	if ( SPIfd != -1 ) {
		SPIdata = (shutdown ? CMDreset : CMDwrite) | (value << 8);

		digitalWrite( pinMC41100Enable, 0 );
		write( SPIfd, &SPIdata, 2 );
		digitalWrite( pinMC41100Enable, 1 );

		if ( shutdown ) {
			close( SPIfd );
			SPIfd = -111;
		}
	}
}