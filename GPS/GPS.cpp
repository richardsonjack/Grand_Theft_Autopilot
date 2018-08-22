#include <map>
#include <math.h>
#include <iostream>
#include <algorithm>
#include <list>
#include "GPS.h"
#include "tinyxml2.h"
#include <limits>


float dist(float x1, float y1, float z1, float x2, float y2, float z2) {
    return sqrt(pow(x2-x1, 2)+pow(y2-y1, 2)+0*pow(z2-z1, 2));
}


float vecDist(Vector3 src, Vector3 goal)
{
    return abs(dist(src.x, src.y, src.z, goal.x, goal.y, goal.z));
}

void aStarNode::calcParams(aStarNode* goal)
{
	cost = 0;
	if (parent != NULL) {
		cost += parent->cost + vecDist(this->node->coord, parent->node->coord);
	}
	estimate = vecDist(this->node->coord, parent->node->coord);
}

GPS::GPS() {
	while (nodes.size() == 0) {
		if (PATHFIND::_0xF7B79A50B905A30D(-8192.0f, 8192.0f, -8192.0f, 8192.0f)) {
			populateNodes(pathsfile);
		}
		WAIT(0);
	}
}

GPS::~GPS(){
	nodes.clear();
}

std::vector<tNode*> tNode::getLinkedNodes(std::map<int, tNode>* nodes){
    std::vector<tNode*> res;
    
    for (int i = 0; i < links.size(); i++){
        if(nodes->at(links[i].firstNodeID).id != id ){
            res.push_back(&nodes->at(links[i].firstNodeID));
        }else {
            res.push_back(&nodes->at(links[i].secondNodeID));
        }
    }
    return res;
}


//helpers
void GPS::populateNodes(const char* pathsfile){
	int ii = 0;
	std::map<std::string, tNode> tmpnodes;

	tLink *tmplinks = (tLink*)malloc(80592 * sizeof(*tmplinks)); //Too large for the stack, need to store in the heap

	int i = 0;

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* object;
	tinyxml2::XMLElement* coords;
	tinyxml2::XMLElement* attr;
	tinyxml2::XMLElement* ref1;
	tinyxml2::XMLElement* ref2;

	if (doc.LoadFile(pathsfile) != 0){
		return;
	}

	object = doc.FirstChildElement()->FirstChildElement()->FirstChildElement();
	
	for (object; object; object = object->NextSiblingElement()) {
		if (object->Attribute("class", "vehiclenode")) {
			tNode node = { 0 };
			node.attr.speed = 4;
			node.attr.density = -1;

			coords = object->FirstChildElement()->FirstChildElement()->FirstChildElement();
			node.coord.x = coords->FloatAttribute("x");
			node.coord.y = coords->FloatAttribute("y");
			node.coord.z = coords->FloatAttribute("z");

			attr = object->FirstChildElement()->NextSiblingElement()->FirstChildElement();
			if (attr != NULL) {
				for (attr; attr; attr = attr->NextSiblingElement()) {
					if (attr->Attribute("name", "Disabled")) {
						node.attr.disabled = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Water")) {
						node.attr.water = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Speed")) {
						node.attr.speed = attr->IntAttribute("value");
					}
					else if (attr->Attribute("name", "Special")) {
						node.attr.special = attr->IntAttribute("value");
					}
					else if (attr->Attribute("name", "Density")) {
						node.attr.density = attr->IntAttribute("value");
					}
					else if (attr->Attribute("name", "Highway")) {
						node.attr.highway = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "NoGps")) {
						node.attr.noGPS = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Tunnel")) {
						node.attr.tunnel = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Cannot Go Left")) {
						node.attr.cantGoLeft = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Left Turns Only")) {
						node.attr.leftTurnsOnly = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Off Road")) {
						node.attr.offRoad = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Cannot Go Right")) {
						node.attr.cantGoRight = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "No Big Vehicles")) {
						node.attr.noBigVehicles = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Indicate Keep Left")) {
						node.attr.indicateKeepLeft = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Indicate Keep Right")) {
						node.attr.indicateKeepRight = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Slip Lane")) {
						node.attr.slipLane = attr->BoolAttribute("value");
					}

				}
			}
			node.id = PATHFIND::GET_NTH_CLOSEST_VEHICLE_NODE_ID(node.coord.x, node.coord.y, node.coord.z, 1, 1, 300, 300);
			tmpnodes[std::string(object->Attribute("guid"))] = node;
		}
		else if (object->Attribute("class", "vehiclelink")) {
			tLink link = { 0 };
			link.attr.lanesIn = 1;
			link.attr.lanesOut = 1;

			coords = object->FirstChildElement()->FirstChildElement()->FirstChildElement();
			link.coord.x = coords->FloatAttribute("x");
			link.coord.y = coords->FloatAttribute("y");
			link.coord.z = coords->FloatAttribute("z");

			attr = object->FirstChildElement()->NextSiblingElement()->FirstChildElement();
			if (attr != NULL) {
				for (attr; attr; attr = attr->NextSiblingElement()) {
					if (attr->Attribute("name", "Width")) {
						link.attr.width = attr->IntAttribute("value");
					}
					else if (attr->Attribute("name", "Lanes In")) {
						link.attr.lanesIn = attr->IntAttribute("value");
					}
					else if (attr->Attribute("name", "Lanes Out")) {
						link.attr.lanesOut = attr->IntAttribute("value");
					}
					else if (attr->Attribute("name", "Narrowroad")) {
						link.attr.narrowRoad = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "GpsBothWays")) {
						link.attr.gpsBothWays = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Block If No Lanes")) {
						link.attr.blockIfNoLanes = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Shortcut")) {
						link.attr.shortcut = attr->BoolAttribute("value");
					}
					else if (attr->Attribute("name", "Dont Use For Navigation")) {
						link.attr.dontUseForNavigation = attr->BoolAttribute("value");
					}
				}
			}

			ref1 = object->FirstChildElement()->NextSiblingElement()->NextSiblingElement()->NextSiblingElement()->FirstChildElement();
			ref2 = ref1->NextSiblingElement();

			link._ref1 = std::string(ref1->Attribute("guid"));
			link._ref2 = std::string(ref2->Attribute("guid"));
			tmplinks[i] = link;
			i++;
		}
	}

	//Puts everything into place for fast searching by nodeID
	tNode node1;
	tNode node2;
	tLink link;
	float m;
	for (i = 0; i < 80592; i++){
		link = tmplinks[i];
		node1 = tmpnodes[link._ref1];
		node2 = tmpnodes[link._ref2];
		m = dist(node1.coord.x, node1.coord.y, 0, node2.coord.x, node2.coord.y, 0);
		link.direction.x = (node2.coord.x - node1.coord.x) / m; //Unitary vector pointing in the direction of the road
		link.direction.y = (node2.coord.y - node1.coord.y) / m;

		try {
			node1 = nodes.at(node1.id);
			node2 = nodes.at(node2.id);
		}
		catch (const std::out_of_range) {
			//Empty on purpose
		}
        
        link.firstNodeID = node1.id;
        link.secondNodeID = node2.id;
        
		setLinePoints(&node1, link);
		setLinePoints(&node2, link);
		nodes[node1.id] = node1;
		nodes[node2.id] = node2;
	}
	free(tmplinks);
}

void GPS::setLinePoints(tNode* node, tLink link){

	tLinePoint linePoint;
	int linesIn, linesOut, starti;
	float laneWidth, lineOffset;

	linesIn = link.attr.lanesIn + 1;
	linesOut = link.attr.lanesOut + 1;

	if (node->attr.highway) laneWidth = 6.0;
	else if (link.attr.narrowRoad) laneWidth = 4.2;
	else laneWidth = 5.5;


	if (link.attr.width > 0) { //Usually urban scenarios
		lineOffset = laneWidth / 2;
		starti = 0;
	}
	else if (link.attr.lanesOut == 0){ //Usually interurban scenarios TODO

		if ((link.attr.lanesIn % 2) == 0) {
			lineOffset = 0;
			linesIn = (linesIn / 2) + 1;
			starti = -(linesIn - 1);
		}
		else {
			lineOffset = laneWidth / 2;
			linesIn = linesIn / 2;
			starti = -linesIn;
		}

		linesOut = starti; //Skips linesOut loop
	}
	else { //Urban and interurban scenarios
		lineOffset = 0;
		starti = 1;

		linePoint.coord = node->coord;
		linePoint.laneIn = false;
		link.linePoints.push_back(linePoint);
	}

	linePoint.laneIn = true;
	for (int i = starti; i < linesIn; i++) {
		linePoint.coord.x = node->coord.x + link.direction.y*(lineOffset + laneWidth*i);
		linePoint.coord.y = node->coord.y - link.direction.x*(lineOffset + laneWidth*i);
		linePoint.coord.z = node->coord.z;
		link.linePoints.push_back(linePoint);
	}

	linePoint.laneIn = false;
	for (int i = starti; i < linesOut; i++) {
		linePoint.coord.x = node->coord.x - link.direction.y*(lineOffset + laneWidth*i);
		linePoint.coord.y = node->coord.y + link.direction.x*(lineOffset + laneWidth*i);
		linePoint.coord.z = node->coord.z;
		link.linePoints.push_back(linePoint);
	}

	node->links.push_back(link);
}

tNode GPS::getLocationNode(Vehicle vehicle){
	Vector3 currentPosition = ENTITY::GET_ENTITY_COORDS(vehicle, FALSE);

	tNode myNode;

	myNode.id = -1;
	myNode.coord = currentPosition;


	for (int i = 1; i <= 3; i++) {
		int nodeID = PATHFIND::GET_NTH_CLOSEST_VEHICLE_NODE_ID(currentPosition.x, currentPosition.y, currentPosition.z, i, 1, 300, 300);
		tNode linkedNode = nodes[nodeID];
		
		tLink link;
		link.attr.lanesIn = 1;
		link.attr.lanesOut = 1;

		float dist = GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(myNode.coord.x, myNode.coord.y, 0, linkedNode.coord.x, linkedNode.coord.y, 0, FALSE);
		link.direction.x = (linkedNode.coord.x - myNode.coord.x) / dist; //Unitary vector pointing in the direction of the road
		link.direction.y = (linkedNode.coord.y - myNode.coord.y) / dist;

		link.firstNodeID = myNode.id;
        link.secondNodeID = linkedNode.id;

        myNode.links.push_back(link);

	}
	return myNode;
}

 bool getCoordsFromMarker(Vector3 &v)
{
	// search for marker blip
	int blipIterator = UI::_GET_BLIP_INFO_ID_ITERATOR();
	for (Blip i = UI::GET_FIRST_BLIP_INFO_ID(blipIterator); UI::DOES_BLIP_EXIST(i) != 0; i = UI::GET_NEXT_BLIP_INFO_ID(blipIterator))
	{
		if (UI::GET_BLIP_INFO_ID_TYPE(i) == 4) // number 4 is the ID of marker on the built-in map
		{
			v = UI::GET_BLIP_INFO_ID_COORD(i);
			return true;
		}
	}

	return false;
}

 bool GPS::getDestinationNode(tNode  &myNode){
	 Vector3 dest;

	

	if (!getCoordsFromMarker(dest))
	{
		return false;
	}


	myNode.id = -2;
	myNode.coord = dest;


	for (int i = 1; i <= 3; i++) {
		int nodeID = PATHFIND::GET_NTH_CLOSEST_VEHICLE_NODE_ID(dest.x, dest.y, dest.z, i, 1, 300, 300);
		tNode linkedNode = nodes[nodeID];

		tLink link;
		link.attr.lanesIn = 1;
		link.attr.lanesOut = 1;

		float dist = GAMEPLAY::GET_DISTANCE_BETWEEN_COORDS(myNode.coord.x, myNode.coord.y, 0, linkedNode.coord.x, linkedNode.coord.y, 0, FALSE);
		link.direction.x = (linkedNode.coord.x - myNode.coord.x) / dist; //Unitary vector pointing in the direction of the road
		link.direction.y = (linkedNode.coord.y - myNode.coord.y) / dist;

		link.firstNodeID = myNode.id;
		link.secondNodeID = linkedNode.id;

		myNode.links.push_back(link);

	}
	return true;
}

bool fComp(aStarNode *x, aStarNode *y){
    return x->cost+ x->estimate < y->cost + y->estimate;
}

bool sameCoord(Vector3 l, Vector3 r) {
	return (r.x == l.x) && (r.y == l.y) && (r.z == l.z);
}

std::vector<tNode*> GPS::findPath(int startID, int endID){
    
    std::map<int, aStarNode*> aStarNodeMap;
    
    std::vector<tNode*> path;
    std::list<aStarNode*> openList;
    std::list<aStarNode*> closedList;
    
    aStarNode* start_node = new aStarNode{&nodes[startID],std::numeric_limits<double>::infinity(),0,NULL};
    aStarNode* goal_node = new aStarNode{&nodes[endID],std::numeric_limits<double>::infinity(),0,NULL};
    
    aStarNodeMap[startID] = start_node;
    aStarNodeMap[endID] = goal_node;
    
    std::cout << start_node->node->coord.x << " " << start_node->node->coord.y << " " << start_node->node->coord.x << std::endl;
    std::cout << goal_node->node->coord.x << " " << goal_node->node->coord.y << " " << goal_node->node->coord.x << std::endl;

    
    openList.push_back(start_node);
    
    bool goalFound = false;
    
    //while nodes are yet to be explored
    while (!openList.empty()) {
        
        //get node with least cost
        aStarNode *curNode = *std::min_element(openList.begin(), openList.end(), fComp);
        
        
        curNode->calcParams(goal_node);
        std::cout << curNode->estimate << std::endl;
        
        closedList.push_back(curNode);
        openList.remove(curNode);
        
        //check if this is the goal
        if (sameCoord(curNode->node->coord,goal_node->node->coord)) {
            goalFound = true;
            break;
        }
        
        //generate all moves
        std::vector<tNode*> linkedNodes = curNode->node->getLinkedNodes(&nodes);
        for(int i = 0; i < linkedNodes.size(); i++) {
            aStarNode* nextMove;
            if (aStarNodeMap[linkedNodes[i]->id] == NULL){
               nextMove = new aStarNode{linkedNodes[i],std::numeric_limits<double>::infinity(),0,NULL};
                aStarNodeMap[linkedNodes[i]->id] = nextMove;
            }else {
                nextMove = aStarNodeMap[linkedNodes[i]->id];
            }
            
            
            //check move validity
            if (std::find(closedList.begin(), closedList.end(), nextMove) != closedList.end()) {
                //std::cout << "REPEAT\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
                continue;
            }
            
            nextMove->calcParams(goal_node);
            
            //check if location is on open list and update f value and parent value accordingly
            aStarNode* openListElem = *std::find(openList.begin(), openList.end(), nextMove);
            //not in open list
            
            if(openListElem == *openList.end()){
                nextMove->parent = curNode;
                nextMove->calcParams(goal_node);
                openList.push_back(nextMove);
            }
            //in open list and more efficient
            else if(nextMove->cost < openListElem->cost){
                nextMove->parent = curNode;
            }
        }
        
    }
    
    if(!goalFound) {
        std::cout << "No Valid Path\n";
        exit(1);
    }
    
    aStarNode* inClosed = *std::find(closedList.begin(), closedList.end(), goal_node);
    
    if (inClosed != *closedList.end()) {
        while (!sameCoord(inClosed->node->coord, start_node->node->coord)) {
            path.insert(path.begin(), inClosed->node);
            inClosed = inClosed->parent;
        }
    }
    
    
    return path;
}

std::vector<tNode*> GPS::findPath(tNode startNode, tNode endNode){
    
    std::map<int, aStarNode*> aStarNodeMap;
    
    std::vector<tNode*> path;
    std::list<aStarNode*> openList;
    std::list<aStarNode*> closedList;
    
    aStarNode* start_node = new aStarNode{&startNode,std::numeric_limits<double>::infinity(),0,NULL};
    aStarNode* goal_node = new aStarNode{&endNode,std::numeric_limits<double>::infinity(),0,NULL};
    
    aStarNodeMap[startNode.id] = start_node;
    aStarNodeMap[endNode.id] = goal_node;
    
    std::cout << start_node->node->coord.x << " " << start_node->node->coord.y << " " << start_node->node->coord.x << std::endl;
    std::cout << goal_node->node->coord.x << " " << goal_node->node->coord.y << " " << goal_node->node->coord.x << std::endl;

    
    openList.push_back(start_node);
    
    bool goalFound = false;
    
    //while nodes are yet to be explored
    while (!openList.empty()) {
        
        //get node with least cost
        aStarNode *curNode = *std::min_element(openList.begin(), openList.end(), fComp);
        
        
        curNode->calcParams(goal_node);
        std::cout << curNode->estimate << std::endl;
        
        closedList.push_back(curNode);
        openList.remove(curNode);
        
		//check if this is the goal
		if (sameCoord(curNode->node->coord, goal_node->node->coord)) {
			goalFound = true;
			break;
		}

		//generate all moves
		std::vector<tNode*> linkedNodes = curNode->node->getLinkedNodes(&nodes);
		for (int i = 0; i < linkedNodes.size(); i++) {
			aStarNode* nextMove;
			if (aStarNodeMap[linkedNodes[i]->id] == NULL) {
				nextMove = new aStarNode{ linkedNodes[i],std::numeric_limits<double>::infinity(),0,NULL };
				aStarNodeMap[linkedNodes[i]->id] = nextMove;
			}
			else {
				nextMove = aStarNodeMap[linkedNodes[i]->id];
			}


			//check move validity
			if (std::find(closedList.begin(), closedList.end(), nextMove) != closedList.end()) {
				//std::cout << "REPEAT\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";
				continue;
			}

			nextMove->calcParams(goal_node);

			//check if location is on open list and update f value and parent value accordingly
			aStarNode* openListElem = *std::find(openList.begin(), openList.end(), nextMove);
			//not in open list

			if (openListElem == *openList.end()) {
				nextMove->parent = curNode;
				nextMove->calcParams(goal_node);
				openList.push_back(nextMove);
			}
			//in open list and more efficient
			else if (nextMove->cost < openListElem->cost) {
				nextMove->parent = curNode;
			}
		}

	}

	if (!goalFound) {
		std::cout << "No Valid Path\n";
		exit(1);
	}

	aStarNode* inClosed = *std::find(closedList.begin(), closedList.end(), goal_node);

	if (inClosed != *closedList.end()) {
		while (!sameCoord(inClosed->node->coord, start_node->node->coord)) {
			path.insert(path.begin(), inClosed->node);
			inClosed = inClosed->parent;
		}
	}

    
    return path;
}
