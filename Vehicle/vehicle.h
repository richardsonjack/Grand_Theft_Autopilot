#pragma once

#include "./GPS/GPS.h"
class Autopilot {
private:
	GPS nav;
	Vehicle vehicle = NULL;
	Player player = NULL;
	Ped ped = NULL;
	Cam camera = NULL;
	Vector3 dir;


public:
	void moveToNode(GPS::tNode node);

	Vector3 getCurrentLoc();

	void initVehicle();
	void setSteering(float ammt);
	void setAcceleration(float ammt);
	void setBrake(float ammt);
};