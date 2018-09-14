import xml.etree.ElementTree as ET
from scipy import spatial
import numpy as np
from scipy.spatial import distance
import pathfind


def totuple(a):
    try:
        return tuple(totuple(i) for i in a)
    except TypeError:
        return a


class Node:
    disabled = False
    water = False
    speed = 0
    special = 0
    density = 0
    highway = False
    noGPS = False
    tunnel = False
    cantGoLeft = False
    leftTurnsOnly = False
    offRoad = False
    cantGoRight = False
    noBigVehicles = False
    indicateKeepLeft = False
    indicateKeepRight = False
    slipLane = False

    id = 0
    coords = (0, 0, 0)

    def __init__(self):
        self.links = list()

    def getNeighbours(self):
        neighbours = []
        for link in self.links:
            if (link.node1coords == self.coords):
                neighbours.append(link.node2coords)
            elif (link.node2coords == self.coords):
                neighbours.append(link.node1coords)
        return neighbours


class Link:
    width = 0
    lanesIn = 0
    lanesOut = 0
    narrow = False
    gpsBothWays = False
    blockIfNoLanes = False
    shortcut = False
    dontUseForNavigation = False

    coord = (0, 0, 0)
    direction = (0, 0, 0)

    node1coords = (0, 0, 0)
    node2coords = (0, 0, 0)

    ref1 = ""
    ref2 = ""


class GPS:
    nodeMap = {}
    nodeTree = {}

    def neighbors(self, current):
        n = self.nodeMap[current]
        return n.getNeighbours()

    def closestNodeCoord(self, coord):
        return totuple(self.nodeTree.data[self.nodeTree.query([coord])[1][0]])

    def cost(self, current, next):
        return spatial.distance.euclidean(current, next)

    def genPath(self, start, end):
        came_from, cost = pathfind.a_star_search(self, start, end)
        self.path = pathfind.reconstruct_path(came_from, start, end)


    def __init__(self):
        print("Parsing File")
        tree = ET.parse("./paths.xml")
        root = tree.getroot()
        i = 0

        tempNodeMap = {}
        tempLinks = {}
        print("Building nodes and links")
        for child in root[0]:
            if(child.attrib['class'] == "vehiclenode"):
                n = Node()
                n.speed = 4
                n.density = -1

                coordObj = child[0][0][0]
                x = float(coordObj.attrib["x"])
                y = float(coordObj.attrib["y"])
                z = float(coordObj.attrib["z"])
                n.coords = (x, y, z)

                for attr in child[1]:
                    if (attr.attrib.get("name") == "Disabled"):
                        n.disabled = attr.attrib["value"]
                    elif(attr.attrib.get("name") == "Water"):
                        n.water = attr.attrib["value"]
                    elif(attr.attrib.get("name") == "Speed"):
                        n.speed = attr.attrib["value"]
                    elif(attr.attrib.get("name") == "Special"):
                        n.special = attr.attrib["value"]
                    elif(attr.attrib.get("name") == "Density"):
                        n.density = attr.attrib["value"]
                    elif(attr.attrib.get("name") == "Highway"):
                        n.highway = attr.attrib["value"]
                    elif(attr.attrib.get("name") == "NoGps"):
                        n.noGPS = attr.attrib["value"]
                    elif(attr.attrib.get("name") == "Cannot Go Left"):
                        n.cantGoLeft = attr.attrib["value"]
                    elif(attr.attrib.get("name") == "Left Turns Only"):
                        n.leftTurnsOnly = attr.attrib["value"]
                    elif(attr.attrib.get("name") == "Off Road"):
                        n.offRoad = attr.attrib["value"]
                    elif(attr.attrib.get("name") == "Cannot Go Right"):
                        n.cantGoRight = attr.attrib["value"]
                    elif(attr.attrib.get("name") == "No Big Vehicles"):
                        n.noBigVehicles = attr.attrib["value"]
                    elif(attr.attrib.get("name") == "Indicate Keep Left"):
                        n.indicateKeepLeft = attr.attrib["value"]
                    elif(attr.attrib.get("name") == "Indicate Keep Right"):
                        n.indicateKeepRight = attr.attrib["value"]
                    elif(attr.attrib.get("name") == "Slip Lane"):
                        n.slipLane = attr.attrib["value"]
                    elif(attr.attrib.get("name") == "Tunnel"):
                        n.tunnel = attr.attrib["value"]
                tempNodeMap[child.attrib.get("guid")] = n
            elif(child.attrib['class'] == "vehiclelink"):
                l = Link()
                l.lanesIn = 1
                l.lanesOut = 1

                coordObj = child[0][0][0]
                x = float(coordObj.attrib["x"])
                y = float(coordObj.attrib["y"])
                z = float(coordObj.attrib["z"])
                l.coord = (x, y, z)

                for attr in child[1]:
                    if (attr.attrib.get("name") == "Width"):
                        l.width = attr.attrib["value"]
                    elif(attr.attrib.get("name") == "Lanes In"):
                        l.lanesIn = attr.attrib["value"]
                    elif(attr.attrib.get("name") == "Lanes Out"):
                        l.lanesOut = attr.attrib["value"]
                    elif(attr.attrib.get("name") == "Narrowroad"):
                        l.Narrowroad = attr.attrib["value"]
                    elif(attr.attrib.get("name") == "GpsBothWays"):
                        l.gpsBothWays = attr.attrib["value"]
                    elif(attr.attrib.get("name") == "Block If No Lanes"):
                        l.blockIfNoLanes = attr.attrib["value"]
                    elif(attr.attrib.get("name") == "Shortcut"):
                        l.shortcut = attr.attrib["value"]
                    elif(attr.attrib.get("name") == "Dont Use For Navigation"):
                        l.dontUseForNavigation = attr.attrib["value"]

                l.ref1 = child[3][0].attrib.get("guid")
                l.ref2 = child[3][1].attrib.get("guid")

                tempLinks[i] = l
                i = i+1
        print("Putting it together")
        #build final map
        for k in tempLinks.items():
            l = k[1]
            n1 = Node()
            n2 = Node()
            n1 = tempNodeMap[l.ref1]
            n2 = tempNodeMap[l.ref2]

            m = distance.euclidean(n1.coords, n2.coords)
            l.direction = ((n2.coords[0]-n1.coords[0])/m, (n2.coords[1]-n1.coords[1])/m, (n2.coords[2]-n1.coords[2])/m)

            if (n1.coords in self.nodeMap):
                n1 = self.nodeMap[n1.coords]
            if (n2.coords in self.nodeMap):
                n2 = self.nodeMap[n2.coords]
            l.node1coords = n1.coords
            l.node2coords = n2.coords

            n1.links.append(l)
            n2.links.append(l)

            self.nodeMap[n1.coords] = n1
            self.nodeMap[n2.coords] = n2
        print(";")
        print(";")
        self.nodeTree = spatial.KDTree(list(self.nodeMap.keys()))
