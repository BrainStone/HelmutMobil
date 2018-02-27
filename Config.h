#include "_CompileFlags.h"

#ifndef Config_H
#define Config_H

#include <libconfig.h++>
#include <string>

namespace config {
	void loadConfig();
	void safeConfig();

	extern libconfig::Config config;

	extern libconfig::Setting* PID_Bremse;
	extern libconfig::Setting* PID_Lenkung;
	extern libconfig::Setting* Battery;
}

#endif // #ifndef Config_H