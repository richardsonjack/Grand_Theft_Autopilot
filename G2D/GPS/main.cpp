#include "GPS.h"
#include <iostream>
#include <fstream>

using namespace std;

int main() {
	GPS x = GPS("/Users/JackRichardson/Desktop/GTAMap/paths.xml");
	cout << "Finished generation" << endl;
    
    ofstream myfile ("/Users/JackRichardson/Desktop/GTAMap/nodes.txt");
    for (std::map<int, tNode>::iterator it =  x.nodes.begin(); myfile.is_open() && it != x.nodes.end(); it++)
    {
        myfile << it->second.coord.x << "," << it->second.coord.y <<  endl;
    }
    
    std::vector<tNode*> path = x.findPath(1, 45000);
    ofstream myfile2 ("/Users/JackRichardson/Desktop/GTAMap/path.txt");
    for (int i = 0; i < path.size(); i++)
    {
        myfile2 << path[i]->coord.x << "," << path[i]->coord.y <<  endl;
    }
    return 0;
}
