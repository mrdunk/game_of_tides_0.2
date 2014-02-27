#include <stdlib.h>     /* rand */
#include <iostream>
#include <time.h>
#include <float.h>
#include <cmath>        // std::abs

#include "data.h"


using namespace std;


#define SEED_ISLAND 5.0
#define GROW_ISLAND 18 

Point emptyPoint(0, 0);
set<Point> emptySet;

void MapNode::_increaseRecursion(int recursion){
    while((int)heights.size() - 1 + minRecursion < recursion){
        heights.push_back(0);
    }

    while((int)sites.size() - 1 + minRecursion < recursion){
        sites.push_back(emptySet);
    }

    while((int)corners.size() - 1 + minRecursion < recursion){
        corners.push_back(emptySet);
    }
}

int MapNode::getMaxRecursion(void){
    int sz = (int)heights.size();
    if(sz == 0) return -1;          // not yet initialised.
    return sz - 1 + minRecursion;
}

int MapNode::setHeight(int recursion, int height){
    if(recursion < minRecursion) return -1;

    if((int)heights.size() -1 < recursion - minRecursion){
        _increaseRecursion(recursion);
    }

    heights[recursion - minRecursion] = height;
    return height;
}

int MapNode::getHeight(const int recursion){
    if((int)sites.size() -1 < recursion - minRecursion) return -1;

    return heights[recursion - minRecursion];
}

int MapNode::pushSite(int recursion, Point site){
    if(recursion < minRecursion) return -1;

    if((int)sites.size() -1 < recursion - minRecursion){
        _increaseRecursion(recursion);
    }

    sites[recursion - minRecursion].insert(site);
    return sites[recursion - minRecursion].size();
}

int MapNode::numSite(int recursion){
    int sz = (int)heights.size();
    if(sz == 0) return -1;          // not yet initialised.

    recursion -= minRecursion;
    if(sz -1 < recursion){
        return -1;
    }

    return sites[recursion].size();
}

int MapNode::pushCorner(int recursion, Point corner){
    if(recursion < minRecursion) return -1;
    
    if((int)corners.size() -1 < recursion - minRecursion){
        _increaseRecursion(recursion);
    }

    corners[recursion - minRecursion].insert(corner);
    return corners[recursion - minRecursion].size();
}

int MapNode::numCorner(int recursion){
    int sz = (int)heights.size();
    if(sz == 0) return -1;          // not yet initialised.

    recursion -= minRecursion;
    if(sz -1 < recursion){
        return -1;
    }

    return corners[recursion].size();
}

set<Point>::iterator MapNode::beginSite(const int recursion){
    //if(heights.size() -1 + minRecursion < recursion) return emptySet.end();

    return sites[recursion - minRecursion].begin();
}

set<Point>::iterator MapNode::endSite(const int recursion){
    //if(heights.size() -1 + minRecursion < recursion) return emptySet.end();

    return sites[recursion - minRecursion].end();
}

set<Point>::iterator MapNode::beginCorner(const int recursion){
    //if(heights.size() -1 + minRecursion < recursion) return emptySet.end();

    return corners[recursion - minRecursion].begin();
}   

set<Point>::iterator MapNode::endCorner(const int recursion){
    //if(heights.size() -1 + minRecursion < recursion) return emptySet.end();

    return corners[recursion - minRecursion].end();
}

// http://stackoverflow.com/questions/2049582/how-to-determine-a-point-in-a-triangle
bool pointInsideTriangle(float px, float py, float c1x, float c1y, float c2x, float c2y, float c3x, float c3y)
{
    int as_x = px-c1x;
    int as_y = py-c1y;

    bool s_ab = (c2x-c1x)*as_y-(c2y-c1y)*as_x > 0;

    if(((c3x-c1x)*as_y-(c3y-c1y)*as_x > 0) == s_ab) return false;

    if(((c3x-c2x)*(py-c2y)-(c3y-c2y)*(px-c2x) > 0) != s_ab) return false;

    return true;
}

bool pointInsideTriangle(Point p, Point c1, Point c2, Point c3){
    return pointInsideTriangle(p.x(), p.y(), c1.x(), c1.y(), c2.x(), c2.y(), c3.x(), c3.y());
}

bool MapNode::isInside(int recursion, Point point){
    if(type != TYPE_SITE) return 0;

    Point first(0,0);
    Point previous(0,0);
    for(auto it = beginCorner(recursion); it != endCorner(recursion); ++it){
        if(first.x() == 0 and first.y() == 0){
            first = *it;
        } else {
            if(pointInsideTriangle(point, coordinates, previous, *it)) return 1;
        }
        previous = *it;
    }
    return pointInsideTriangle(point, coordinates, previous, first);
}



std::unordered_map<Point, struct MapNode, pairHash> MapData::MapContainer;

MapData::MapData(bool testing){
    if(!testing){
        Point rootPoint(0,0);
        Point insertPoint;

        std::vector<Point> points;

        for (int i = 0; i < MAP_NUM_POINTS; ++i){
            insertPoint.x((int)(rand() % MAP_SIZE));
            insertPoint.y((int)(rand() % MAP_SIZE));

            insert(MapNode(insertPoint, rootPoint, 0));
            points.push_back(insertPoint);
        }
        cout << points.size() << "\n";

        voronoi_diagram<double> vd;
        //construct_voronoi((std::vector<Point>::iterator)MapContainer.begin(), (std::vector<Point>::iterator)MapContainer.end(), &vd);
        construct_voronoi(points.begin(), points.end(), &vd);
    }
}

void MapData::insert(MapNode node){
    if(node.coordinates.x() >= 0 and node.coordinates.y() >= 0 and node.coordinates.x() < MAP_SIZE and node.coordinates.y() < MAP_SIZE){
        MapContainer.insert(make_pair(node.coordinates, node));
    }
}

int MapData::count(Point point){
    return MapContainer.count(point);
}

MapType::const_iterator MapData::find(Point point){
    return MapContainer.find(point);
}


key_iterator MapData::begin(void){
    return MapContainer.begin();
}

key_iterator MapData::end(void){
    return MapContainer.end();
}
