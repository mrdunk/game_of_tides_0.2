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
vector<Point> emptySet;

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
    // TODO we can probably optimize these vectors for memory usage by picking a sensible starting size.
    if(recursion < minRecursion) return -1;

    if((int)sites.size() -1 < recursion - minRecursion){
        _increaseRecursion(recursion);
    }

    // TODO should we alwas check for duplicate entries or are there times this just wastes time?
    for(auto it = sites[recursion - minRecursion].begin(); it !=  sites[recursion - minRecursion].end(); ++it){
        if(*it == site){ return sites[recursion - minRecursion].size(); }
    }
    sites[recursion - minRecursion].push_back(site);
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
    // TODO we can probably optimize these vectors for memory usage by picking a sensible starting size.
    if(recursion < minRecursion) return -1;
    
    if((int)corners.size() -1 < recursion - minRecursion){
        _increaseRecursion(recursion);
    }

    // TODO should we alwas check for duplicate entries or are there times this just wastes time?
    for(auto it = corners[recursion - minRecursion].begin(); it !=  corners[recursion - minRecursion].end(); ++it){
        if(*it == corner){ return corners[recursion - minRecursion].size(); }
    }
    corners[recursion - minRecursion].push_back(corner);
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

vector<Point>::iterator MapNode::beginSite(const int recursion){
    //if(heights.size() -1 + minRecursion < recursion) return emptySet.end();

    return sites[recursion - minRecursion].begin();
}

vector<Point>::iterator MapNode::endSite(const int recursion){
    //if(heights.size() -1 + minRecursion < recursion) return emptySet.end();

    return sites[recursion - minRecursion].end();
}

vector<Point>::iterator MapNode::beginCorner(const int recursion){
    //if(heights.size() -1 + minRecursion < recursion) return emptySet.end();

    return corners[recursion - minRecursion].begin();
}   

vector<Point>::iterator MapNode::endCorner(const int recursion){
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


MapType MapData::MapContainer;

MapData::MapData(bool testing){
    static bool initialised = false;

    if(!testing and !initialised){
        cout << "Initialising data.\n";
        initialised = true;
        Point rootPoint(0,0);
        Point insertPoint, insertPoint2, previousPoint;
        MapNode insertData;
        unordered_multimap<Point,Point,pairHash> tempEdges;

        // Anoyingly we need to keep a simple itterable of input points to feed construct_voronoi() with.
        // I tried overloading the MapType::iterater to return only the keys which could be made work but 
        // we have no way of knowing what order points in an unordered_map come in there is no way to tie 
        // the voronoi data back to the coordinates.
        std::vector<Point> points;

        // Create initial points.
        for (int i = 0; i < MAP_NUM_POINTS; ++i){
            insertPoint.x((int)(rand() % MAP_SIZE * MAP_MIN_RES));
            insertPoint.y((int)(rand() % MAP_SIZE * MAP_MIN_RES));

            insertData = MapNode(insertPoint, rootPoint, 0);
            insertData.type = TYPE_SITE;

            insert(insertData);
            points.push_back(insertPoint);
        }
        cout << points.size() << "\n";


        voronoi_diagram<double> vd;
        construct_voronoi(points.begin(), points.end(), &vd);


        int result = 0;
        for (voronoi_diagram<double>::const_cell_iterator it = vd.cells().begin(); it != vd.cells().end(); ++it) {
            const voronoi_diagram<double>::cell_type &cell = *it;
            const voronoi_diagram<double>::edge_type *edge = cell.incident_edge();

            Point siteCoord = points[cell.source_index()];
            MapType::iterator site = find(siteCoord);

            tempEdges.clear();

            // This is convenient way to iterate edges around Voronoi cell.
            do {
                if (edge->is_primary() and edge->is_finite()){
                    ++result;

                    // Find all the corners.
                    // We only want vertex0. vertex1 would just give us duplicates.
                    insertPoint.x((int)edge->vertex0()->x());
                    insertPoint.y((int)edge->vertex0()->y());

                    // Create new map node if it doesn't exist already.
                    if(!count(insertPoint)){
                        insertData = MapNode(insertPoint, rootPoint, 0);
                        insertData.type = TYPE_CORNER;
                        insert(insertData);
                    }

                    // Update the map node.
                    if(count(insertPoint)){
                        //MapType::iterator corner = find(insertPoint);
                        //corner->second.pushSite(0, siteCoord);
                        MapContainer.find(insertPoint)->second.pushSite(0, siteCoord);

                        site->second.pushCorner(0, insertPoint);
                    }

                    // Find the neghbouring sites.
                    insertPoint.x(points[edge->twin()->cell()->source_index()].x());
                    insertPoint.y(points[edge->twin()->cell()->source_index()].y());

                    // Update the map node.
                    site->second.pushSite(0, insertPoint);

                }
                edge = edge->next();
            } while (edge != cell.incident_edge());

        }
        cout << result << " " << MapContainer.size() << "\n";
    }
}

void MapData::insert(MapNode node){
    if(node.coordinates.x() >= 0 and node.coordinates.y() >= 0 and node.coordinates.x() < MAP_SIZE * MAP_MIN_RES and node.coordinates.y() < MAP_SIZE * MAP_MIN_RES){
        MapContainer.insert(make_pair(node.coordinates, node));
    }
}

int MapData::count(Point point){
    return MapContainer.count(point);
}

MapType::iterator MapData::find(Point point){
    return MapContainer.find(point);
}


MapType::iterator MapData::begin(void){
    return MapContainer.begin();
}

MapType::iterator MapData::end(void){
    return MapContainer.end();
}
