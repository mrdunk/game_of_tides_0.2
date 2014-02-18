#include <stdlib.h>     /* rand */
#include <iostream>
#include <time.h>
#include <float.h>
#include <cmath>        // std::abs

#include "data.h"


using namespace std;


#define SEED_ISLAND 5.0
#define GROW_ISLAND 18 

std::unordered_map<int64_t, struct MapSite> Data::MapContainer;

Data::Data(void){
    if(MapContainer.size() == 0){

        cout << "Initialising Data()\n";
        int i;
        MapSite _centre, _zero, _tmpSite;
        _centre.x = 0;
        _centre.y = 0;
        _zero.x = MAP_SIZE/2;
        _zero.y = MAP_SIZE/2;

        std::vector<float> xValuesV, yValuesV;
        std::unordered_set<int64_t> newPoints;  // A set is an easy way to see if we have added this point yet.

        for (i = 0; i < MAP_NUM_POINTS; ++i){
            _tmpSite.x = 10 * (int)((rand() % MAP_SIZE) / 10);
            _tmpSite.y = 10 * (int)((rand() % MAP_SIZE) / 10);
            if(newPoints.count(_tmpSite.coord()) == 0){
                newPoints.insert(_tmpSite.coord());
                xValuesV.push_back(_tmpSite.x);
                yValuesV.push_back(_tmpSite.y);

                if(std::abs(xValuesV.back() - MAP_SIZE/2) + std::abs(yValuesV.back() - MAP_SIZE/2) < std::abs(_centre.x - MAP_SIZE/2) + std::abs(_centre.y - MAP_SIZE/2)){
                    _centre.x = xValuesV.back();
                    _centre.y = yValuesV.back();
                }
                if(std::abs(xValuesV.back() - MAP_SIZE/2) + std::abs(yValuesV.back() - MAP_SIZE/2) > std::abs(_zero.x - MAP_SIZE/2) + std::abs(_zero.y - MAP_SIZE/2)){
                    _zero.x = xValuesV.back();
                    _zero.y = yValuesV.back();
                }
            }
        }
        totNumPoints = newPoints.size();;

        centre = _centre.coord();
        zero = _zero.coord();

        cout << "Generating Voronoi...\n";
        {
            MyVoronoi voronoi;
            voronoi.SetRecursion(0);
            voronoi.RegisterContainer(&MapContainer);
            // We are using a vector rather than an array that generateVoronoi() expects.
            // "&xValuesV[0]" is the pointer to the start of array.
            voronoi.generateVoronoi(&xValuesV[0], &yValuesV[0], MAP_NUM_POINTS, 0, MAP_SIZE, 0, MAP_SIZE, 1);//MAP_MIN_RES);
            voronoi.VerifyGraph(0);
        }
        cout << "done!\n";

        cout << "Raising land....\n";
        RaiseLand(0);
        cout << "done!\n";

        std::unordered_set<int64_t> shore;
        for(int i = 1; i <= RECURSE; ++i){
            cout << "Moar detail...\n";
            MoreDetail(i, xValuesV, yValuesV);
            cout << "done!\n";
        }


        cout << "Setting land heights...\n";
        SetHeight();
        cout << "done!\n";

        cout << "Testing...\n";
        //TestData();
        cout << "done!\n";
    }
}

/* Generate data for subsection of map.*/
void Data::Section(float lowX, float lowY, float highX, float highY){
    if(lowX < 0) lowX = 0;
    if(highX > MAP_SIZE) highX = MAP_SIZE;
    if(lowY < 0) lowY = 0;
    if(highY > MAP_SIZE) highY = MAP_SIZE;

    cout << "low:\n";
    cout << lowX << "\t" << lowY << "\n";
    cout << "high:\n";
    cout << highX << "\t" << highY << "\n";

    float sizeX = highX - lowX;
    float sizeY = highY - lowY;

    cout << sizeX << "\t,\t" << sizeY << "\n";
    
    float sizeR = MAP_SIZE;
    int recursion;
    for(recursion = 0; recursion < 10; ++recursion){
        if(sizeX >= sizeR or sizeY >= sizeR) break;
        sizeR /= 2;
    }
    cout << recursion << "\t" << sizeR << "\n";

    lowX = sizeR * ((int)lowX / (int)sizeR);
    lowY = sizeR * ((int)lowY / (int)sizeR);
    highX = sizeR * (((int)highX / (int)sizeR) +1);
    highY = sizeR * (((int)highY / (int)sizeR) +1);

    cout << lowX << "\t,\t" << lowY << "\n" << highX << "\t,\t" << highY << "\n\n\n";
}

/* returns a set of all the land polygons which are adjacent to water. */
std::unordered_set<int64_t> Data::GetShore(int recursion){
    std::unordered_set<int64_t> shore;
    for(auto it = MapContainer.begin(); it != MapContainer.end(); ++it){
        if(it->second.isShore(recursion, MapContainer)){
            shore.insert(it->first);
        }
    }
    return shore;
}

/* Remove the first layers of polygons from arround all land masses. */
void Data::ErodeShore(int recursion){

    // Get the average size of a polygon at this recusion level so we can mainly filter out the big ones later.
    float averageHillSize = 0;
    int count = 0;
    float lowX, lowY, highX, highY;
    for(auto it = MapContainer.begin(); it != MapContainer.end(); ++it){
        if(it->second.recDepth == recursion){
            ++count;
            it->second.boundingBox(recursion, lowX, lowY, highX, highY);
            averageHillSize += max(highX - lowX, highY - lowY);
        }
    }
    averageHillSize /= count;
    
    vector<float> removeSizes = {averageHillSize * 100,
                                 averageHillSize * 100,
                                 averageHillSize,
                                 averageHillSize,
                                 averageHillSize / 4};

    for(auto size = removeSizes.begin(); size != removeSizes.end(); ++size){
        std::unordered_set<int64_t> shore = DefineShore();
        //std::unordered_set<int64_t> shore = GetShore(recursion);
        float lowX, lowY, highX, highY;

        for(auto it = shore.begin(); it != shore.end(); ++it){
            if(MapContainer[*it].recDepth == recursion){
                MapContainer[*it].boundingBox(recursion, lowX, lowY, highX, highY);
                if(highX - lowX > *size or highY - lowY > *size){
                    MapContainer[*it].height = -1;
                }
            }
        }
    }
}

void Data::MoreDetail(int recursion, std::vector<float>& xValuesV, std::vector<float>& yValuesV){
    std::unordered_set<int64_t> shore = DefineShore();

    std::unordered_set<int64_t> newPoints;
    MapSite working, tmpMapSite;
    float lowX, lowY, highX, highY, x, y;

    for(auto it = shore.begin(); it != shore.end(); ++it){
        // Make some new points and add if they fall within the polygon.
        working = MapContainer[*it];
        working.boundingBox(recursion, lowX, lowY, highX, highY);
        for(int p = 0; p < 60; ++p){
            x = 10 * (int)((lowX + rand() % (int)(highX - lowX)) / 10);
            y = 10 * (int)((lowY + rand() % (int)(highY - lowY)) / 10);

            if(working.isPointInHill(recursion, x, y)){
                tmpMapSite.x = x;
                tmpMapSite.y = y;
                if(MapContainer.count(tmpMapSite.coord()) == 0 and newPoints.count(tmpMapSite.coord()) == 0){
                    // not already an node here so safe to add.
                    newPoints.insert(tmpMapSite.coord());
                    xValuesV.push_back(x);
                    yValuesV.push_back(y);
                }
            }
        }
    }

    // Calculate a new voronoi diagram.
    totNumPoints += newPoints.size();
    {
        MyVoronoi voronoi;
        voronoi.RegisterContainer(&MapContainer);
        voronoi.SetRecursion(recursion);
        voronoi.generateVoronoi(&xValuesV[0], &yValuesV[0], totNumPoints, 0, MAP_SIZE, 0, MAP_SIZE, 1);//MAP_MIN_RES);
        voronoi.VerifyGraph(recursion);
        //cout << "\n---\n";
        //voronoi.PopulateCorners(recursion);
    }

    // If everything went well so far, set the height above sea level.
    for(auto it = newPoints.begin(); it != newPoints.end(); ++it){
        if(MapContainer.count(*it) > 0){
            MapContainer[*it].height = 10;
        } else {
            cout << " baws ";
        }
    }
    cout << "\n";

    ErodeShore(recursion);
}

void Data::RaiseLand(int recursion){
    int islandCount = 0;
    std::unordered_set<int64_t> open;
    for(auto it = MapContainer.begin(); it != MapContainer.end(); ++it){
        if(100.0 * (float)rand() / RAND_MAX <= SEED_ISLAND and it->second.x > MAP_SIZE/8 and 
                it->second.x < 7* MAP_SIZE/8 and it->second.y > MAP_SIZE/8 and it->second.y < 7* MAP_SIZE/8){
            ++islandCount;
            it->second.height = 10;
            open.insert(it->second.coord());
        }
    }
    if(islandCount == 0){
        cout << "No natural islands. Using map centre point.\n";
        open.insert(centre);
    }


    std::unordered_set<int64_t> closed;
    std::unordered_set<int64_t> next;

    while(open.size() > 0){
        for(auto it = open.begin(); it != open.end(); ++it){
            MapContainer[*it].height = 10;
            closed.insert(*it);
            if(MapContainer[*it].x > MAP_SIZE/8 and MapContainer[*it].y > MAP_SIZE/8 and MapContainer[*it].x < 7*MAP_SIZE/8 and MapContainer[*it].y < 7*MAP_SIZE/8){
                for(auto it_adjacent = MapContainer[*it].site_begin(recursion); it_adjacent != MapContainer[*it].site_end(recursion); ++it_adjacent){
                    if(!closed.count(*it_adjacent) and rand() % 100 <= GROW_ISLAND){
                        next.insert(*it_adjacent);
                    }
                }
            }
        }
        next.swap(open);
        next.clear();
    }
}

/* Unlike GetShore() this method does a flood fill from the ocean inwards so removes "lakes" and other low spot from land masses. */
std::unordered_set<int64_t> Data::DefineShore(void){
    for(auto it = MapContainer.begin(); it != MapContainer.end(); ++it){
        it->second.terrain = TERRAIN_LAND;
    }

    std::unordered_set<int64_t> open;
    open.insert(zero);
    std::unordered_set<int64_t> closed;
    std::unordered_set<int64_t> next;
    std::unordered_set<int64_t> shore;

    while(open.size() > 0){
        for(auto it = open.begin(); it != open.end(); ++it){
            MapContainer[*it].height = -1;
            closed.insert(*it);
            int rec = MapContainer[*it].recursionSize() -1;
            for(auto it_adjacent = MapContainer[*it].site_begin(rec); it_adjacent != MapContainer[*it].site_end(rec); ++it_adjacent){
                if(!closed.count(*it_adjacent)){
                    if(MapContainer[*it_adjacent].height <= 0){
                        MapContainer[*it_adjacent].terrain = TERRAIN_OCEAN;
                        next.insert(*it_adjacent);
                    } else {
                        MapContainer[*it_adjacent].height = 10;
                        MapContainer[*it_adjacent].terrain = TERRAIN_SHORE;
                        shore.insert(*it_adjacent);
                    }
                }
            }
        }
        next.swap(open);
        next.clear();
    }
    return shore;
}

void Data::SetHeight(void){
    std::unordered_set<int64_t> open = DefineShore();
    std::unordered_set<int64_t> closed;
    std::unordered_set<int64_t> next;

    int height = 1;
    while(open.size() > 0){
        for(auto it = open.begin(); it != open.end(); ++it){
            MapContainer[*it].height = height;
            closed.insert(*it);
            int rec = MapContainer[*it].recursionSize() -1;
            for(auto it_adjacent = MapContainer[*it].site_begin(rec); it_adjacent != MapContainer[*it].site_end(rec); ++it_adjacent){
                if(!closed.count(*it_adjacent) and MapContainer[*it_adjacent].terrain != TERRAIN_OCEAN){  // MapContainer[*it_adjacent].height > 0){
                    next.insert(*it_adjacent);
                }
            }
        }
        next.swap(open);
        next.clear();
        height += MAP_SIZE / 4000;
    }
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

void KeyToCoord(const int64_t key, float& x, float& y){
    x = (float)(0x00000000FFFFFFFF & key) * (float)MAP_MIN_RES;
    y = (float)((0xFFFFFFFF00000000 & key) >> 32) * (float)MAP_MIN_RES;
}

/* Comparison functor for sorting the std::set datastructure in a clockwise direction.
 * x and y are the centre points to be sorted round. */
float CompCoord::x = 0;
float CompCoord::y = 0;

/* Set x and y values. This must be done before any data is stored in the std::set datastructure. */
void CompCoord::PassParentCoord(float _x, float _y){
    x = _x;
    y = _y;
}

/* Comparison operator for sorting the std::set datastructure in a clockwise direction.  */
bool CompCoord::operator() (const int64_t& i, const int64_t& j) const {
        //cout << i << "\t,\t" << j << "\t";

        int32_t ix = (int32_t)(0x00000000FFFFFFFF & i);
        int32_t iy = (int32_t)((0xFFFFFFFF00000000 & i) >> 32);
        int32_t jx = (int32_t)(0x00000000FFFFFFFF & j);
        int32_t jy = (int32_t)((0xFFFFFFFF00000000 & j) >> 32);
        //cout << ix << "\t,\t" << iy << "\t\t\t" << jx << "\t,\t" << jy << "\n";
        
        int32_t centre_x = round(x / MAP_MIN_RES);
        int32_t centre_y = round(y / MAP_MIN_RES);
        //cout << centre_x << "\t,\t" << centre_y << "\n";
        
        int32_t dix = ix - centre_x;
        int32_t diy = iy - centre_y;
        int32_t djx = jx - centre_x;
        int32_t djy = jy - centre_y;
        //cout << dix << "\t,\t" << diy << "\t\t" <<  djx << "\t,\t" << djy << "\n";
            
        float ai, aj;
        if(dix == 0){
            //if(diy >= 0) return true;
            //else return (djx <= 0);
            if(diy >= 0) ai = FLT_MAX;
            else ai = -FLT_MAX;
        } else ai = (float)diy/dix;

        if(djx == 0){
            //if(djy >= 0) return false;
            //else return (dix >= 0);
            if(djy >= 0) aj = FLT_MAX;
            else aj = -FLT_MAX;
        } else aj = (float)djy/djx;

        //cout << ai << "\t\t\t\t\t" <<  aj << "\n";

        if(dix >= 0 and djx < 0){
            return true;
        }
        if(dix < 0 and djx >= 0){
            return false;
        }
        if(ai == aj) return i > j;  // failed to see any difference in the angle, so points are in a line.
        //std::cout << (ai > aj) << "\n";
        return ai > aj;
}

/*
int main (){
    Data d;
    Data d2;
    return 0;
}
*/
