#include "Autopilot.h"
#include "../GPS/GPS.h"
#include <math.h>
#include <time.h>

void Autopilot::setSteering(float ammt) {
	CONTROLS::_SET_CONTROL_NORMAL(27, 59, ammt); //[-1,1]
}
void Autopilot::setAcceleration(float ammt){
	CONTROLS::_SET_CONTROL_NORMAL(27, 71, ammt); //[0,1];
}
void Autopilot::setBrake(float ammt){
	CONTROLS::_SET_CONTROL_NORMAL(27, 72, ammt); //[0,1];
}

Autopilot::Autopilot()
{
	AUDIO::PLAY_SOUND_FRONTEND(-1, "NAV_UP_DOWN", "HUD_FRONTEND_DEFAULT_SOUNDSET", 0);
	AUDIO::PLAY_SOUND_FRONTEND(-1, "NAV_UP_DOWN", "HUD_FRONTEND_DEFAULT_SOUNDSET", 0);
	AUDIO::PLAY_SOUND_FRONTEND(-1, "NAV_UP_DOWN", "HUD_FRONTEND_DEFAULT_SOUNDSET", 0);
	nav = GPS();
	AUDIO::PLAY_SOUND_FRONTEND(-1, "NAV_UP_DOWN", "HUD_FRONTEND_DEFAULT_SOUNDSET", 0);
	AUDIO::PLAY_SOUND_FRONTEND(-1, "NAV_UP_DOWN", "HUD_FRONTEND_DEFAULT_SOUNDSET", 0);
	AUDIO::PLAY_SOUND_FRONTEND(-1, "NAV_UP_DOWN", "HUD_FRONTEND_DEFAULT_SOUNDSET", 0);
	
}

void Autopilot::moveToDest(){
	bool arrived = false;

	tNode myLoc = nav.getLocationNode(vehicle);
	tNode dest;
	if (!nav.getDestinationNode(dest)) {
		return;
	}

	std::vector<tNode*> path = nav.findPath(myLoc, dest);

	int i = 0;

	while (!arrived){

		tNode* nextNode = path[i];

		Vector3 nodeLoc = nextNode->coord;
		Vector3 myLoc = getCurrentLoc();

		if (dist(myLoc.x, myLoc.y, 0, nodeLoc.x, nodeLoc.y, 0) < 0.5){
			i++;
			nextNode = path[i];
			nodeLoc = nextNode->coord;
			if (i == path.size()){
				arrived = true;
				break;
			}
		}

		float myHeading = ENTITY::GET_ENTITY_HEADING(vehicle);

		float goalDirection = atan((myLoc.y - nodeLoc.y)/(myLoc.x - nodeLoc.x));

		float stearingVal = (-1 * (myHeading - goalDirection) )/180;

		//if outside range, flip 180 degrees
		if (stearingVal < -1){
			stearingVal = 2 + stearingVal;
		} else if (stearingVal > 1) {
			stearingVal = stearingVal - 2;
		}

		setSteering(stearingVal);
		setAcceleration(0.1);

	}

}

Vector3 Autopilot::getCurrentLoc()
{
	return ENTITY::GET_ENTITY_COORDS(vehicle, FALSE);
}

void Autopilot::initVehicle() {
	Vector3 pos, rotation;
	Hash vehicleHash;
	float heading;

	GAMEPLAY::SET_RANDOM_SEED(time(NULL));
	while (!PATHFIND::_0xF7B79A50B905A30D(-8192.0f, 8192.0f, -8192.0f, 8192.0f)) WAIT(0);
	PATHFIND::GET_CLOSEST_VEHICLE_NODE_WITH_HEADING(x, y, 0, &pos, &heading, 0, 0, 0);

	ENTITY::DELETE_ENTITY(&vehicle);
	vehicleHash = GAMEPLAY::GET_HASH_KEY((char*)_vehicle);
	STREAMING::REQUEST_MODEL(vehicleHash);
	while (!STREAMING::HAS_MODEL_LOADED(vehicleHash)) WAIT(0);
	while (!ENTITY::DOES_ENTITY_EXIST(vehicle)) {
		vehicle = VEHICLE::CREATE_VEHICLE(vehicleHash, pos.x, pos.y, pos.z, heading, FALSE, FALSE);
		WAIT(0);
	}
	VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(vehicle);

	while (!ENTITY::DOES_ENTITY_EXIST(ped)) {
		ped = PLAYER::PLAYER_PED_ID();
		WAIT(0);
	}

	player = PLAYER::PLAYER_ID();
	PLAYER::START_PLAYER_TELEPORT(player, pos.x, pos.y, pos.z, heading, 0, 0, 0);
	while (PLAYER::IS_PLAYER_TELEPORT_ACTIVE()) WAIT(0);

	PED::SET_PED_INTO_VEHICLE(ped, vehicle, -1);
	STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(vehicleHash);

	GAMEPLAY::SET_WEATHER_TYPE_NOW_PERSIST((char*)_weather);

	rotation = ENTITY::GET_ENTITY_ROTATION(vehicle, 1);
	CAM::DESTROY_ALL_CAMS(TRUE);
	camera = CAM::CREATE_CAM("DEFAULT_SCRIPTED_CAMERA", TRUE);
	if (strcmp(_vehicle, "packer") == 0) CAM::ATTACH_CAM_TO_ENTITY(camera, vehicle, 0, 2.35, 1.7, TRUE);
	else CAM::ATTACH_CAM_TO_ENTITY(camera, vehicle, 0, 0.5, 0.8, TRUE);
	CAM::SET_CAM_FOV(camera, 60);
	CAM::SET_CAM_ACTIVE(camera, TRUE);
	CAM::SET_CAM_ROT(camera, rotation.x, rotation.y, rotation.z, 1);
	CAM::SET_CAM_INHERIT_ROLL_VEHICLE(camera, TRUE);
	CAM::RENDER_SCRIPT_CAMS(TRUE, FALSE, 0, TRUE, TRUE);

	AI::CLEAR_PED_TASKS(ped);
}