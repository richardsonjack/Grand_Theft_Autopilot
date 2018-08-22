#pragma once

#include "../GPS/GPS.h"
#include <fstream>

class Autopilot {
private:
	GPS nav;
	Vehicle vehicle = NULL;
	Player player = NULL;
	Ped ped = NULL;
	Cam camera = NULL;
	Vector3 dir;

	float x, y;
	const char* _vehicle = "blista";

public:
	Autopilot();
	void moveToDest();

	Vector3 getCurrentLoc();

	void initVehicle();
	void setSteering(float ammt);
	void setAcceleration(float ammt);
	void setBrake(float ammt);

	void alogger(std::string s) {
		std::ofstream logfile;
		logfile.open("autopilot.log", std::ios_base::trunc);
		logfile << s << "\n";
		logfile.close();
	}
};