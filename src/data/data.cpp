#include <stdlib.h>     /* rand */
#include <iostream>
#include <time.h>
#include <float.h>
#include <cmath>        // std::abs

#include "data.h"


using namespace std;


#define SEED_ISLAND 5.0
#define GROW_ISLAND 18 
#define MAX_RECURSEION 4
#define HEIGHT_MULTIPLIER   MAP_MIN_RES / 5

Point emptyPoint(0, 0);
vector<Point> emptySet;

void MapNode::_increaseRecursion(int recursion){
    while((int)sites.size() - 1 + minRecursion < recursion){
        sites.push_back(emptySet);
    }

    while((int)corners.size() - 1 + minRecursion < recursion){
        corners.push_back(emptySet);
    }
}

int MapNode::getMaxRecursion(void){
    int sz = (int)corners.size();
    if(sz == 0) return -1;          // not yet initialised.
    return sz - 1 + minRecursion;
}

void MapNode::setHeight(int height){
    groundHeight = height;
}

int MapNode::getHeight(void){
    return groundHeight;
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
    int sz = (int)sites.size();
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
    int sz = (int)corners.size();
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

bool MapNode::isInside(const int recursion, Point point){
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

bool MapNode::cornersInside(const int recursion, MapNode site){
    if(type != TYPE_SITE) return 0;

    for(auto itCorner = site.beginCorner(recursion); itCorner != site.endCorner(recursion); ++itCorner){
        if(!isInside(recursion -1, *itCorner)) return false;
    }
    return true;
}

void MapNode::boundingBox(const int recursion, Point& bl, Point& tr){
    tr.x(coordinates.x());
    tr.y(coordinates.y());
    bl.x(coordinates.x());
    bl.y(coordinates.y());
    if(numCorner(recursion) <= 0) return;
    
    for(auto it = beginCorner(recursion); it != endCorner(recursion); ++it){
        if(it->x() < bl.x()) bl.x(it->x());
        if(it->y() < bl.y()) bl.y(it->y());
        if(it->x() > tr.x()) tr.x(it->x());
        if(it->y() > tr.y()) tr.y(it->y());
    }
}

MapType MapData::MapContainer;
Point MapData::centre;
int MapData::maxRecursion = 0;

MapData::MapData(bool testing){
    static bool initialised = false;

    if(!testing and !initialised){
        cout << "Initialising data.\n";
        initialised = true;
        maxRecursion = 0;
        bool firstLoop = true;
        Point rootPoint(0,0);
        Point insertPoint;
        MapNode insertData;

        // Anoyingly we need to keep a simple itterable of input points to feed construct_voronoi() with.
        // I tried overloading the MapType::iterater to return only the keys which could be made work but 
        // we have no way of knowing what order points in an unordered_map come in there is no way to tie 
        // the voronoi data back to the coordinates.
        std::vector<Point> seedPoints;

        // Create initial points.
        for (int i = 0; i < MAP_NUM_POINTS; ++i){
            insertPoint.x((int)(rand() % MAP_SIZE * MAP_MIN_RES));
            insertPoint.y((int)(rand() % MAP_SIZE * MAP_MIN_RES));

            insertData = MapNode(insertPoint, rootPoint, 0);
            insertData.type = TYPE_SITE;

            insert(insertData);
            seedPoints.push_back(insertPoint);

            if(firstLoop or
                    abs(insertPoint.x() - (MAP_SIZE * MAP_MIN_RES /2)) + abs(insertPoint.y() - (MAP_SIZE * MAP_MIN_RES /2)) < 
                    abs(centre.x() - (MAP_SIZE * MAP_MIN_RES /2)) + abs(centre.y() - (MAP_SIZE * MAP_MIN_RES /2))){
                centre = insertPoint;
            }

            firstLoop = false;
        }
        cout << centre.x() << "," << centre.y() << "\n";

        populateVoronoi(seedPoints, 0);

        raiseLand();

        std::unordered_set<Point, pairHash> shore;
        int rec;
        for(rec = 1; rec <= MAX_RECURSEION; ++rec){
            shore = getShore(rec -1);
            moreDetail(rec, shore, seedPoints);
        }
        getShore(rec -1);

        setHeights();
    }
}

void MapData::setHeights(void){
    cout << "MapData::setHeights.... " << maxRecursion << "\n";
    deque<Point> open0, open1;
    unordered_set<Point, pairHash> closed;

    Point bl = closestSiteTo(0, Point(0, 0));
    Point br = closestSiteTo(0, Point(MAP_SIZE * MAP_MIN_RES, 0));
    Point tl = closestSiteTo(0, Point(0, MAP_SIZE * MAP_MIN_RES));
    Point tr = closestSiteTo(0, Point(MAP_SIZE * MAP_MIN_RES, MAP_SIZE * MAP_MIN_RES));
    
    open0.push_back(bl);
    open0.push_back(br);
    open0.push_back(tl);
    open0.push_back(tr);

    closed.insert(bl);
    closed.insert(br);
    closed.insert(tl);
    closed.insert(tr);

    Point working;
    while(!open0.empty()){
        working = open0.front();
        open0.pop_front();
        for(auto it = find(working)->second.beginSite(maxRecursion); it != find(working)->second.endSite(maxRecursion); ++it){
            if(closed.count(*it) <= 0){
                if(find(*it)->second.getHeight() > 0){
                    // shoreline.
                    open1.push_back(*it);
                } else {
                    open0.push_back(*it);
                }
                closed.insert(*it);
            }
        }
    }
    cout << open1.size() << "\n";

    int distFromSea = 1;
    while(!open0.empty() or !open1.empty()){
        while(!open1.empty()){
            working = open1.front();
            open1.pop_front();
            if(count(working) > 0){
                find(working)->second.setHeight(1.0 + (float)distFromSea * (float)HEIGHT_MULTIPLIER / (find(working)->second.minRecursion +1));
                for(auto it = find(working)->second.beginSite(maxRecursion); it != find(working)->second.endSite(maxRecursion); ++it){
                    if(closed.count(*it) <= 0){
                        open0.push_back(*it);
                        closed.insert(*it);
                    }
                }
            } else {
                cout << " * " << working.x() << "," << working.y() << "\n";
            }
        }
        ++distFromSea;
        cout << open0.size() << "\t" << open1.size() << "\n";
        open0.swap(open1);
    }

    cout << "...\n";

    float totHeight;
    int numHeight;
    for(auto it = begin(); it != end(); ++it){
        if(it->second.type == TYPE_CORNER){
            totHeight = 0;
            numHeight = 0;
            if(it->second.getMaxRecursion() >= maxRecursion){
                for(auto itSite = it->second.beginSite(maxRecursion); itSite != it->second.endSite(maxRecursion); ++itSite){
                    ++numHeight;
                    totHeight += find(*itSite)->second.getHeight();
                }
                if(numHeight > 0){
                    it->second.setHeight(totHeight/numHeight);
                } else {
                    it->second.setHeight(0);
                }
            }
        }
    }

    cout << "...done\n";
}

void MapData::moreDetail(const int recursion, std::unordered_set<Point, pairHash> shore, std::vector<Point>& seedPoints){
    maxRecursion = recursion;
    cout << "MapData::moreDetail... " << maxRecursion << " " << shore.size() << "\n";
    Point insertPoint;
    MapNode insertData;
    Point bl, tr;
    for(auto itSite = shore.begin(); itSite != shore.end(); ++itSite){
        find(*itSite)->second.boundingBox(recursion -1, bl, tr);
        for(int i = 0; i < 40; ++i){
            if(tr.x() == bl.x() or tr.y() == bl.y()) break;

            insertPoint.x(bl.x() + (rand() % (tr.x() - bl.x())));
            insertPoint.y(bl.y() + (rand() % (tr.y() - bl.y())));

            insertData = MapNode(insertPoint, *itSite, recursion);
            insertData.type = TYPE_SITE;

            if(count(insertPoint) <= 0 and find(*itSite)->second.isInside(recursion -1, insertPoint)){
                insertData = MapNode(insertPoint, *itSite, recursion);
                insertData.type = TYPE_SITE;
                insertData.setHeight(1);
                insert(insertData);
                seedPoints.push_back(insertPoint);
            }
        }
    }
    cout << "\n";
    cout << "  Points added.\n";
    populateVoronoi(seedPoints, recursion);
    cout << "  Voronoi calculated.\n";

    // Set heights.
    // TODO Maybe there is a better place to do this?
    deque<Point> open;
    unordered_set<Point, pairHash> closed;
    Point working;
    for(auto it = begin(); it != end(); ++it){
        if(it->second.type == TYPE_SITE){
            //if(it->second.minRecursion == recursion -1 and it->second.getHeight(recursion -1) <= 0){
            for(int r = it->second.minRecursion; r < recursion; ++r){
                if(it->second.getHeight() <= 0){
                    open.clear();
                    closed.clear();

                    // Check for parent overlap with neghbouring cells.
                    for(auto itSite = it->second.beginSite(recursion); itSite != it->second.endSite(recursion); ++itSite){
                        if(find(*itSite)->second.minRecursion == recursion and planesOverlap(recursion -1, it->first, recursion, *itSite)){
                            open.push_back(*itSite);
                        }
                    }
                    while(!open.empty()){
                        working = open.front();
                        open.pop_front();
                        closed.insert(working);
                        find(working)->second.setHeight(0);

                        // Check for parent overlap with cells 2 neghbours away.
                        for(auto itSite = find(working)->second.beginSite(recursion); itSite != find(working)->second.endSite(recursion); ++itSite){
                            if(find(*itSite)->second.minRecursion == recursion and closed.count(*itSite) <= 0 and planesOverlap(recursion -1, it->first, recursion, *itSite)){
                                open.push_back(*itSite);
                            }
                        }
                    }
                }
            }
        }
    }
    cout << seedPoints.size() << " ...done\n";
}

bool MapData::planesOverlap(const int recursion1, Point point1, const int recursion2, Point point2){
    MapNode site1 = find(point1)->second;
    MapNode site2 = find(point2)->second;
    for(auto itCorner = site2.beginCorner(recursion2); itCorner != site2.endCorner(recursion2); ++itCorner){
        if(site1.isInside(recursion1, *itCorner)){
            return true;
        }
    }
    return false;
}

void MapData::raiseLand(void){
    int islandCount = 0;
    std::unordered_set<Point, pairHash> open, closed, next;
    for(auto it = begin(); it != end(); ++it){
        if(it->second.type == TYPE_SITE){
            if(rand() % 100 < SEED_ISLAND and 
                    it->first.x() > AVOID_EDGE * MAP_MIN_RES and it->first.y() > AVOID_EDGE * MAP_MIN_RES and 
                    it->first.x() < (MAP_SIZE - AVOID_EDGE) * MAP_MIN_RES and it->first.y() < (MAP_SIZE - AVOID_EDGE) * MAP_MIN_RES){
                it->second.setHeight(1);
                ++islandCount;
                open.insert(it->first);
            }
        }
    }
    if(islandCount == 0){
        cout << "No natural islands. Using centre point.\n";
        MapContainer[centre].setHeight(1);
    }


    while(open.size() > 0){
        //cout << "open:   " << open.size() << "\n";
        //cout << "closed: " << closed.size() << "\n";
        for(auto it = open.begin(); it != open.end(); ++it){
            for(auto itSites = find(*it)->second.beginSite(0); itSites != find(*it)->second.endSite(0); ++itSites){
                if(rand() % 100 < GROW_ISLAND and open.count(*itSites) <= 0 and closed.count(*itSites) <= 0 and
                        itSites->x() > AVOID_EDGE * MAP_MIN_RES and itSites->y() > AVOID_EDGE * MAP_MIN_RES and
                        itSites->x() < (MAP_SIZE - AVOID_EDGE) * MAP_MIN_RES and itSites->y() < (MAP_SIZE - AVOID_EDGE) * MAP_MIN_RES){
                    find(*itSites)->second.setHeight(1);
                    next.insert(*itSites);
                }
            }
            closed.insert(*it);
        }

    next.swap(open);
    next.clear();
    }
}

std::unordered_set<Point, pairHash> MapData::getShore(const int recursion){
    //cout << "MapData::getShore...\n";
    std::unordered_set<Point, pairHash> closed, shore;
    std::deque<Point> open;

    Point bl = closestSiteTo(0, Point(0, 0));
    Point br = closestSiteTo(0, Point(MAP_SIZE * MAP_MIN_RES, 0));
    Point tl = closestSiteTo(0, Point(0, MAP_SIZE * MAP_MIN_RES));
    Point tr = closestSiteTo(0, Point(MAP_SIZE * MAP_MIN_RES, MAP_SIZE * MAP_MIN_RES));

    open.push_back(bl);
    open.push_back(br);
    open.push_back(tl);
    open.push_back(tr);

    closed.insert(bl);
    closed.insert(br);
    closed.insert(tl);
    closed.insert(tr);

    Point working;
    while(open.size() > 0){
        working = open.front();
        open.pop_front();
        for(auto itSites = find(working)->second.beginSite(recursion); itSites != find(working)->second.endSite(recursion); ++itSites){
            if(find(*itSites)->second.getHeight() > 0){
                shore.insert(*itSites);
            } else {
                if(closed.count(*itSites) <= 0 and shore.count(*itSites) <= 0){
                    find(*itSites)->second.setHeight(0);
                    open.push_back(*itSites);
                    closed.insert(*itSites);
                }
            }
        }
    }

    return shore;
}
    
void MapData::populateVoronoi(std::vector<Point>& seedPoints, const int recursion){
    Point insertPoint, insertPoint2, previousPoint;
    MapNode insertData;
    unordered_multimap<Point,Point,pairHash> tempEdges;   
    
    voronoi_diagram<double> vd;
    construct_voronoi(seedPoints.begin(), seedPoints.end(), &vd);


    for (voronoi_diagram<double>::const_cell_iterator it = vd.cells().begin(); it != vd.cells().end(); ++it) {
        const voronoi_diagram<double>::cell_type &cell = *it;
        const voronoi_diagram<double>::edge_type *edge = cell.incident_edge();

        Point siteCoord = seedPoints[cell.source_index()];
        MapType::iterator site = find(siteCoord);

        tempEdges.clear();

        // This is convenient way to iterate edges around Voronoi cell.
        do {
            if (edge->is_primary() and edge->is_finite()){

                // Find all the corners.
                // We only want vertex0. vertex1 would just give us duplicates.
                insertPoint.x((int)edge->vertex0()->x());
                insertPoint.y((int)edge->vertex0()->y());

                // Create new map node if it doesn't exist already.
                if(!count(insertPoint)){
                    insertData = MapNode(insertPoint, MapContainer[siteCoord].parent, recursion);
                    insertData.type = TYPE_CORNER;
                    insert(insertData);
                }

                // Update the map node.
                if(count(insertPoint)){
                    //MapType::iterator corner = find(insertPoint);
                    //corner->second.pushSite(0, siteCoord);
                    MapContainer.find(insertPoint)->second.pushSite(recursion, siteCoord);

                    site->second.pushCorner(recursion, insertPoint);
                }

                // Find the neghbouring sites.
                insertPoint.x(seedPoints[edge->twin()->cell()->source_index()].x());
                insertPoint.y(seedPoints[edge->twin()->cell()->source_index()].y());

                // Update the map node.
                site->second.pushSite(recursion, insertPoint);

            }
            edge = edge->next();
        } while (edge != cell.incident_edge());
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

Point MapData::closestSiteTo(const int recursion, Point target){
    return closestSiteTo(recursion, target, centre);
}

Point MapData::closestSiteTo(const int recursion, Point target, Point hint){
    //cout << "MapData::closestSiteTo... " << target.x() << "," << target.y() << "\n";
    Point working = Point(0, 0);
    Point closest = hint;

    float a, b;
    long dist, closestDist = (long)MAP_SIZE * (long)MAP_MIN_RES * (long)MAP_SIZE * (long)MAP_MIN_RES;
    for(int r = 0; r <= recursion; ++r){
        while(closest != working){
            working = closest;
            for(auto it = find(working)->second.beginSite(r); it != find(working)->second.endSite(r); ++it){
                a = it->x() - target.x();
                b = it->y() - target.y();
                dist = a*a + b*b;       // this tells us the same as pythagorus but without the expensive square root.
                if(dist < closestDist){
                    closestDist = dist;
                    closest = *it;
                    //cout << closest.x() << "," << closest.y() << "\t" << closestDist << "\n";
                }
            }
        }
        working = Point(0, 0);
    }
    //cout << "...done. " << closest.x() << "," << closest.y() << "\n";
    return closest;
}

std::unordered_set<Point, pairHash> MapData::cornersInBox(const int recursion, Point bl, Point tr){
    Point centre((bl.x() + tr.x())/2, (bl.y() + tr.y())/2);

    deque<Point> open;
    unordered_set<Point, pairHash> closed;
    unordered_set<Point, pairHash> retSet;
    //open.push_back(closestSiteTo(recursion, centre));

    centre = closestSiteTo(recursion, centre);

    /*if(centre.x() < bl.x() or centre.x() > tr.x() or centre.y() < bl.y() or centre.y() > tr.y()){
        centre = closestSiteTo(recursion, bl);
        cout << "a";
    }

    if(centre.x() < bl.x() or centre.x() > tr.x() or centre.y() < bl.y() or centre.y() > tr.y()){
        centre = closestSiteTo(recursion, tr);
        cout << "b";
    }

    if(centre.x() < bl.x() or centre.x() > tr.x() or centre.y() < bl.y() or centre.y() > tr.y()){
        centre = closestSiteTo(recursion, Point(bl.x(), tr.y()));
        cout << "c";
    }

    if(centre.x() < bl.x() or centre.x() > tr.x() or centre.y() < bl.y() or centre.y() > tr.y()){
        centre = closestSiteTo(recursion, Point(tr.x(), bl.y()));
        cout << "d";
    }*/

    if(centre.x() >= bl.x() and centre.x() <= tr.x() and centre.y() >= bl.y() and centre.y() <= tr.y()){
        retSet.insert(centre);
    } 
    open.push_back(centre);

    Point working;

    while(open.size() > 0){
        working = open.front();
        open.pop_front();
        for(auto it = find(working)->second.beginSite(recursion); it != find(working)->second.endSite(recursion); ++it){
            if(closed.count(*it) == 0 and retSet.count(*it) == 0){
                if(it->x() >= bl.x() and it->x() <= tr.x() and it->y() >= bl.y() and it->y() <= tr.y()){
                    open.push_back(*it);
                    retSet.insert(*it);
                }
            }
        }
    }

    if(retSet.size() == 0){
        // retSet is still empty so let's try again but including children of neghbouring nodes that are outwith check area.
        open.push_back(centre);

        while(open.size() > 0){
            working = open.front();
            open.pop_front();
            for(auto it = find(working)->second.beginSite(recursion); it != find(working)->second.endSite(recursion); ++it){
                if(closed.count(*it) == 0 and retSet.count(*it) == 0){
                    if(it->x() >= bl.x() and it->x() <= tr.x() and it->y() >= bl.y() and it->y() <= tr.y()){
                        open.push_back(*it);
                        retSet.insert(*it);
                    } else if(working.x() >= bl.x() and working.x() <= tr.x() and working.y() >= bl.y() and working.y() <= tr.y()){
                        // Allow imediate neighbours not in retSet to contribute child sites.
                        open.push_back(*it);
                        closed.insert(*it);
                    }
                }
            }
            for(auto it = find(working)->second.beginCorner(recursion); it != find(working)->second.endCorner(recursion); ++it){
                if(closed.count(*it) == 0){
                    if(it->x() >= bl.x() and it->x() <= tr.x() and it->y() >= bl.y() and it->y() <= tr.y()){
                        open.push_back(*it);
                        closed.insert(*it);
                    } else if(working.x() >= bl.x() and working.x() <= tr.x() and working.y() >= bl.y() and working.y() <= tr.y()){
                        // Allow imediate neighbours not in retSet to contribute child sites.
                        open.push_back(*it);
                        closed.insert(*it);
                    }
                }
            }
        }
    }

    cout << retSet.size() << "\t";
    return retSet;
}


