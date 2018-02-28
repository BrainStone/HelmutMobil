#include "Config.h"

namespace config {
	using namespace libconfig;

	const char* const configFile = "/hm/config.cfg";

	Config config;

	Setting* PID_Bremse;
	Setting* PID_Lenkung;
	Setting* Battery;

	void loadConfig() {
		config.readFile( configFile );

		PID_Bremse = &config.lookup( "PID.Bremse" );
		PID_Lenkung = &config.lookup( "PID.Lenkung" );
		Battery = &config.lookup( "Battery" );
	}

	void safeConfig() {
		config.writeFile( configFile );
	}
}