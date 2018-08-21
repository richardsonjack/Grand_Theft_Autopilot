#pragma once

#include <map>
#include <vector>
#include <string>
#include "..\dependencies\inc\types.h"
#include "..\dependencies\inc\natives.h"

struct tNodeAttr{
	bool disabled; //Disabled (0)
	bool water; //Water (0)
	int speed; //Speed (4)
	int special; //Special (0)
	int density; //Density (-1)
	bool highway; //Highway (0)
	bool noGPS; //NoGps (0)
	bool tunnel; //Tunnel (0)
	bool cantGoLeft; //Cannot Go Left (0)
	bool leftTurnsOnly; //Left Turns Only (0)
	bool offRoad; //Off Road (0)
	bool cantGoRight; //Cannot Go Right (0)
	bool noBigVehicles; //No Big Vehicles (0)
	bool indicateKeepLeft; //Indicate Keep Left (0)
	bool indicateKeepRight; //Indicate Keep Right (0) 
	bool slipLane; //Slip Lane (0)
};

struct tLinkAttr{
	int width; //Width (0)
	int lanesIn; //Lanes In (1) 
	int lanesOut; //Lanes Out (1) 
	bool narrowRoad; //Narrowroad (0)
	bool gpsBothWays; //GpsBothWays (0)
	bool blockIfNoLanes; //Block If No Lanes (0)
	bool shortcut; //Shortcut (0)
	bool dontUseForNavigation; //Dont Use For Navigation (0)
};

struct tLinePoint{
	Vector3 coord;
	bool laneIn;
};

struct tLink{
	Vector3 coord;
	Vector3 direction;
	tLinkAttr attr;
	std::vector<tLinePoint> linePoints;

	std::string _ref1;
	std::string _ref2;
    
    int firstNodeID;
    int secondNodeID;
    
};

struct tNode{
	int id;
	Vector3 coord;
	tNodeAttr attr;
	std::vector<tLink> links;
    
    std::vector<tNode*> getLinkedNodes(std::map<int, tNode>* nodes);
} ;

struct aStarNode{
    tNode* node;
    double cost;
    double estimate;
    aStarNode* parent;
    
    void calcParams(aStarNode* goal);
    bool operator == (const aStarNode& x);
} ;

float dist(float x1, float y1, float z1, float x2, float y2, float z2);

class GPS {
private:
	//helpers
	void populateNodes(const char* pathsfile);
	void setLinePoints(tNode* node, tLink link);
	const char* pathsfile = "./paths.xml";
	
public:
	GPS();
	~GPS();
	std::map<int, tNode> nodes;
	tNode getLocationNode(Vehicle vehicle);
	bool getDestinationNode(tNode & myNode);
	std::vector<tNode*> findPath(int startID, int endID);
    std::vector<tNode*> findPath(tNode startNode, tNode endNode);
};
