#include <stdlib.h>     /* rand */
#include <iostream>
#include <time.h>
#include <float.h>
#include <cmath>        // std::abs

#include "data.h"


using namespace std;


#define SEED_ISLAND 2.0
#define GROW_ISLAND 22 

std::unordered_map<int64_t, struct MapSite> Data::MapContainer;

Data::Data(void){
    if(MapContainer.size() == 0){
        cout << "Initialising Data()\n";
        int i;
        MapSite _centre, _zero;
        _centre.x = 0;
        _centre.y = 0;
        _zero.x = MAP_SIZE/2;
        _zero.y = MAP_SIZE/2;

        float xValues[100* MAP_NUM_POINTS], yValues[100* MAP_NUM_POINTS];

        for (i = 0; i < MAP_NUM_POINTS; ++i){
            xValues[i] = MAP_SIZE * (float)rand()/RAND_MAX;
            yValues[i] = MAP_SIZE * (float)rand()/RAND_MAX;
            
            if(std::abs(xValues[i] - MAP_SIZE/2) + std::abs(yValues[i] - MAP_SIZE/2) < std::abs(_centre.x - MAP_SIZE/2) + std::abs(_centre.y - MAP_SIZE/2)){
                _centre.x = xValues[i];
                _centre.y = yValues[i];
            }
            if(std::abs(xValues[i] - MAP_SIZE/2) + std::abs(yValues[i] - MAP_SIZE/2) > std::abs(_zero.x - MAP_SIZE/2) + std::abs(_zero.y - MAP_SIZE/2)){
                _zero.x = xValues[i];
                _zero.y = yValues[i];
            }
        }

        centre = _centre.coord();


        cout << "Generating Voronoi...\n";
        {
        MyVoronoi voronoi;
        voronoi.SetRecursion(0);
        voronoi.RegisterContainer(&MapContainer);
        voronoi.generateVoronoi(xValues, yValues, MAP_NUM_POINTS, 0, MAP_SIZE, 0, MAP_SIZE, 0.1);
        }
        cout << "done!\n";

        cout << "Generating islands...\n";

        std::unordered_set<int64_t> seed_points;
        int islandCount = 0;

        for(auto it = MapContainer.begin(); it != MapContainer.end(); ++it){
            if(100.0 * (float)rand() / RAND_MAX <= SEED_ISLAND and it->second.x > MAP_SIZE/8 and it->second.x < 7* MAP_SIZE/8 and it->second.y > MAP_SIZE/8 and it->second.y < 7* MAP_SIZE/8){
                ++islandCount;
                //it->second.height = 10;
                seed_points.insert(it->second.coord());
            } else {
                it->second.height = 0;
            }
        }
        if(islandCount == 0){
            cout << "No natural islands. Using map centre point.\n";
            seed_points.insert(centre);
        }
        cout << "done!\n";

        cout << "Raising land....\n";
        RaiseLand(seed_points);
        cout << "done!\n";

        cout << "Defining shore....\n";
        seed_points.clear();
        seed_points.insert(_zero.coord());
        std::unordered_set<int64_t> shore;
        DefineShore(seed_points, shore);
        cout << "done!\n";

        cout << "Moar detail...\n";
        std::unordered_set<int64_t> newPoints;
        MoreDetail(shore, newPoints);
        cout << newPoints.size() << "\n";
        if(newPoints.size() > 100* MAP_NUM_POINTS){
            cout << "baws\n";
            return;
        }
        i = MAP_NUM_POINTS;
        for(auto it = newPoints.begin(); it != newPoints.end(); ++it){
            KeyToCoord(*it, xValues[i], yValues[i]);
            ++i;
            //cout << xValues[i] << "," << yValues[i] << "\n";
        }
        MyVoronoi voronoi;
        voronoi.RegisterContainer(&MapContainer);
        voronoi.SetRecursion(1);
        voronoi.generateVoronoi(xValues, yValues, newPoints.size() + MAP_NUM_POINTS, 0, MAP_SIZE, 0, MAP_SIZE, 0.1);
        //voronoi.generateVoronoi(xValues, yValues, 10, 0, MAP_SIZE, 0, MAP_SIZE, 0.1);

        ErodeShore();
        ErodeShore();
        ErodeShore();

        cout << "done!\n";

        cout << "Setting land heights...\n";
        shore = GetShore();
        SetHeight(shore);
        cout << "done!\n";
    }
}

/* returns a set of all the land polygons on the coast. */
std::unordered_set<int64_t> Data::GetShore(void){
    std::unordered_set<int64_t> shore;
    for(auto it = MapContainer.begin(); it != MapContainer.end(); ++it){
        if(it->second.isShore(1, MapContainer)){
            shore.insert(it->first);
        }
    }
    return shore;
}

/* Remove the first layer of polygons from arround all land masses. */
void Data::ErodeShore(void){
    std::unordered_set<int64_t> shore = GetShore();

    for(auto it = shore.begin(); it != shore.end(); ++it){
        MapContainer[*it].height = -1;
        MapContainer[*it].terrain = TERRAIN_SEA;
    }
}

void Data::MoreDetail(std::unordered_set<int64_t> shore, std::unordered_set<int64_t>& newPoints){
    MapSite working, tmpMapSite;
    float lowX, lowY, highX, highY, x, y;
    for(auto it = shore.begin(); it != shore.end(); ++it){

/*        // add this point and it's neighbours so we can re-calculate the Voronoi tree.
        newPoints.insert(*it);
        working = MapContainer[*it];
        for(auto it_sites = working.site_begin(0); it_sites != working.site_end(0); ++it_sites){
            newPoints.insert(*it_sites);
        }*/

        // Make some new points to add if they fall within the polygon.
        working = MapContainer[*it];
        working.boundingBox(0, lowX, lowY, highX, highY);
        for(int p = 0; p < 500; ++p){
            x = lowX + rand() % (int)(highX - lowX);
            y = lowY + rand() % (int)(highY - lowY);
            if(working.isPointInHill(0, x, y)){
                tmpMapSite.x = x;
                tmpMapSite.y = y;
                newPoints.insert(tmpMapSite.coord());
            }
        }
    }
}

void Data::RaiseLand(std::unordered_set<int64_t>& open){
    std::unordered_set<int64_t> closed;
    std::unordered_set<int64_t> next;

    while(open.size() > 0){
        for(auto it = open.begin(); it != open.end(); ++it){
            MapContainer[*it].height = 10;
            MapContainer[*it].terrain = TERRAIN_LAND;
            closed.insert(*it);
            if(MapContainer[*it].x > MAP_SIZE/8 and MapContainer[*it].y > MAP_SIZE/8 and MapContainer[*it].x < 7*MAP_SIZE/8 and MapContainer[*it].y < 7*MAP_SIZE/8){
                for(auto it_adjacent = MapContainer[*it].site_begin(0); it_adjacent != MapContainer[*it].site_end(0); ++it_adjacent){
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

void Data::DefineShore(std::unordered_set<int64_t>& open, std::unordered_set<int64_t>& shore){
    std::unordered_set<int64_t> closed;
    std::unordered_set<int64_t> next;

    while(open.size() > 0){
        for(auto it = open.begin(); it != open.end(); ++it){
            MapContainer[*it].height = -1;
            MapContainer[*it].terrain = TERRAIN_SEA;
            closed.insert(*it);
            for(auto it_adjacent = MapContainer[*it].site_begin(0); it_adjacent != MapContainer[*it].site_end(0); ++it_adjacent){
                if(!closed.count(*it_adjacent)){
                    if(MapContainer[*it_adjacent].height < 1){
                        next.insert(*it_adjacent);
                        //MapContainer[*it_adjacent].terrain = TERRAIN_SEA;
                    } else {
                        MapContainer[*it_adjacent].height = 1;
                        MapContainer[*it_adjacent].terrain = TERRAIN_SHORE;
                        shore.insert(*it_adjacent);
                    }
                }
            }
        }
        next.swap(open);
        next.clear();
    }
}

void Data::SetHeight(std::unordered_set<int64_t>& open){
    std::unordered_set<int64_t> closed;
    std::unordered_set<int64_t> next;

    int height = 1;
    while(open.size() > 0){
        for(auto it = open.begin(); it != open.end(); ++it){
            MapContainer[*it].height = height;
            closed.insert(*it);
            for(auto it_adjacent = MapContainer[*it].site_begin(10); it_adjacent != MapContainer[*it].site_end(10); ++it_adjacent){
                if(!closed.count(*it_adjacent) and MapContainer[*it_adjacent].height >= 0){
                    next.insert(*it_adjacent);
                }
            }
        }
        next.swap(open);
        next.clear();
        height += MAP_SIZE / 2000;
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
    x = (float)(0x00000000FFFFFFFF & key) * MAP_MIN_RES;
    y = (float)((0xFFFFFFFF00000000 & key) >> 32) * MAP_MIN_RES;
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
