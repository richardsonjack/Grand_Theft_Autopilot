#ifndef LOGGER

	#include <string>
	#include <fstream>

	void logger(std::string s) {
		std::ofstream logfile;
		logfile.open("autopilot.log", std::ios_base::trunc);
		logfile << s << "\n";
		logfile.close();
	}

#endif // !LOGGER


