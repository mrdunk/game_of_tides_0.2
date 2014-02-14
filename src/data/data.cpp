#include <stdlib.h>     /* rand */
#include <iostream>
#include <time.h>
#include <float.h>
#include <cmath>        // std::abs

#include "data.h"


using namespace std;


#define SEED_ISLAND 5.0
#define GROW_ISLAND 15 

std::unordered_map<int64_t, struct MapSite> Data::MapContainer;

Data::Data(void){
    if(MapContainer.size() == 0){

        cout << "Testing...\n";
        TestData();
        cout << "done!\n";

        cout << "Initialising Data()\n";
        int i;
        MapSite _centre, _zero;
        _centre.x = 0;
        _centre.y = 0;
        _zero.x = MAP_SIZE/2;
        _zero.y = MAP_SIZE/2;

        std::vector<float> xValuesV, yValuesV;

        for (i = 0; i < MAP_NUM_POINTS; ++i){
            xValuesV.push_back(MAP_SIZE * (float)rand()/RAND_MAX);
            yValuesV.push_back(MAP_SIZE * (float)rand()/RAND_MAX);
            
            if(std::abs(xValuesV.back() - MAP_SIZE/2) + std::abs(yValuesV.back() - MAP_SIZE/2) < std::abs(_centre.x - MAP_SIZE/2) + std::abs(_centre.y - MAP_SIZE/2)){
                _centre.x = xValuesV.back();
                _centre.y = yValuesV.back();
            }
            if(std::abs(xValuesV.back() - MAP_SIZE/2) + std::abs(yValuesV.back() - MAP_SIZE/2) > std::abs(_zero.x - MAP_SIZE/2) + std::abs(_zero.y - MAP_SIZE/2)){
                _zero.x = xValuesV.back();
                _zero.y = yValuesV.back();
            }
        }

        centre = _centre.coord();
        zero = _zero.coord();

        cout << "Generating Voronoi...\n";
        {
            MyVoronoi voronoi;
            voronoi.SetRecursion(0);
            voronoi.RegisterContainer(&MapContainer);
            // We are using a vector rather than an array that generateVoronoi() expects.
            // "&xValuesV[0]" is the pointer to the start of array.
            voronoi.generateVoronoi(&xValuesV[0], &yValuesV[0], MAP_NUM_POINTS, 0, MAP_SIZE, 0, MAP_SIZE, MAP_MIN_RES);
        }
        cout << "done!\n";


        cout << "Generating point islands...\n";

        std::unordered_set<int64_t> seed_points;
        int islandCount = 0;

        for(auto it = MapContainer.begin(); it != MapContainer.end(); ++it){
            if(100.0 * (float)rand() / RAND_MAX <= SEED_ISLAND and it->second.x > MAP_SIZE/8 and it->second.x < 7* MAP_SIZE/8 and it->second.y > MAP_SIZE/8 and it->second.y < 7* MAP_SIZE/8){
                ++islandCount;
                it->second.height = 10;
                seed_points.insert(it->second.coord());
            } //else {
            //    it->second.height = -1;
            //}
        }
        if(islandCount == 0){
            cout << "No natural islands. Using map centre point.\n";
            seed_points.insert(centre);
        }
        cout << "done!\n";

        cout << "Raising land arround point islands....\n";
        RaiseLand(seed_points);
        cout << "done!\n";

        cout << "Defining shore....\n";
        std::unordered_set<int64_t> shore = DefineShore();
        cout << "done!\n";

        //std::unordered_set<int64_t> shore;
        int totNumPoints = MAP_NUM_POINTS;
        std::unordered_set<int64_t> newPoints;
        for(int i = 1; i <= RECURSE; ++i){
            cout << "Moar detail...\n";
            newPoints.clear();
            shore = DefineShore();
            //shore = GetShore(i -1);
            MoreDetail(i, shore, newPoints);

            // copy newPoints into xValuesV and yValuesV for voronoi.generateVoronoi().
            for(auto it = newPoints.begin(); it != newPoints.end(); ++it){
                float tmpX, tmpY;
                KeyToCoord(*it, tmpX, tmpY);
                xValuesV.push_back(tmpX);
                yValuesV.push_back(tmpY);
            }
            totNumPoints += newPoints.size();
            {
                MyVoronoi voronoi;
                voronoi.RegisterContainer(&MapContainer);
                voronoi.SetRecursion(i);
                voronoi.generateVoronoi(&xValuesV[0], &yValuesV[0], totNumPoints, 0, MAP_SIZE, 0, MAP_SIZE, MAP_MIN_RES);
            }
            for(auto it = newPoints.begin(); it != newPoints.end(); ++it){
                if(MapContainer.count(*it)>0){
                    MapContainer[*it].height = 10;
                } else cout << "***************\n";
            }

            ErodeShore(i);
            ErodeShore(i);
            ErodeShore(i);
            //ErodeShore(i);


            cout << "done!\n";
        }


        cout << "Setting land heights...\n";
        //shore = GetShore(RECURSE);
        shore = DefineShore();
        SetHeight(shore);
        cout << "done!\n";

        cout << "Testing...\n";
        TestData();
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

void Data::TestData(void){
    bool error;
    for(auto it = MapContainer.begin(); it != MapContainer.end(); ++it){
            error = false;
            MapSite testNode = it->second;
            if(testNode.coord() != it->first) cout << it->first << " != " << testNode.coord() << "\n";
            float x, y;
            KeyToCoord(testNode.coord(), x, y);
            if(fabs(testNode.x - x) > MAP_MIN_RES *10 or fabs(testNode.y - y) > MAP_MIN_RES *10){
                if(!error) cout << "\n";
                error = true;
                cout << fabs(testNode.x - x) << "\t" << fabs(testNode.y - y) << "\t" << testNode.type << "\n";
                cout << testNode.x << "\t" << x << "\t" << testNode.y << "\t" << y << "\n";
            }

            if(testNode.recursionSize() > RECURSE +1){
                if(!error) cout << "\n";
                error = true;
                cout << testNode.recursionSize() << "\n";
            }
            
            bool cont = true;
            unsigned int recurse = 0;
            while(cont and recurse <= RECURSE){
                if(testNode.type == TYPE_SITE){
                for(auto itSites = testNode.site_begin(recurse); itSites != testNode.site_end(recurse); ++itSites){
                    MapSite site = MapContainer[*itSites];
                    bool exists = false;
                    if(testNode.type == TYPE_SITE){
                        for(auto itSites2 = site.site_begin(recurse); itSites2 != site.site_end(recurse); ++itSites2){
                            MapSite site2 = MapContainer[*itSites2];
                            if(site2.coord() == testNode.coord()){
                                exists = true;
                                break;
                            }
                        }
                    } else if(testNode.type == TYPE_VPOINT){
                        for(auto corners = site.corner_begin(recurse); corners != site.corner_end(recurse); ++corners){
                            MapSite corner = MapContainer[*corners];
                            if(corner.coord() == testNode.coord()){
                                exists = true;
                                break;
                            }
                        }
                    }
                    if(!exists){
                        if(!error) cout << "\n";
                        error = true;
                        cout << " s" << recurse << " ";
                    }
                }
                for(auto itCorners = testNode.corner_begin(recurse); itCorners != testNode.corner_end(recurse); ++itCorners){
                    MapSite corner = MapContainer[*itCorners];
                    bool exists = false;
                    if(testNode.type == TYPE_SITE){
                        for(auto itSites2 = corner.site_begin(recurse); itSites2 != corner.site_end(recurse); ++itSites2){
                            MapSite site2 = MapContainer[*itSites2];
                            if(site2.coord() == testNode.coord()){
                                exists = true;
                                break;
                            }
                        }
                    }
                    if(!exists){
                        if(!error) cout << "\n";
                        error = true;
                        cout << " c" << recurse << " ";
                    }
                }
                }

                cont = false;
                if(recurse <= testNode.recursionSize() -1) cont = true;
                ++recurse;
            }
            if(error){ 
                if(testNode.type == TYPE_SITE) cout << " s:" << testNode.recDepth;
                else if(testNode.type == TYPE_VPOINT) cout << " p:" << testNode.recDepth;
                else cout << " u:" << testNode.recDepth;
            }

            if(error){
                cout << "\n";
            } else {
                //if(testNode.type == TYPE_SITE) cout << ".";
                //else if(testNode.type == TYPE_VPOINT) cout << "-";
                //else cout << "*";
            }
            if(testNode.type == TYPE_UNDEFINED) cout << " ** " << testNode.x << "," << testNode.y << "\n";

    }
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

/* Remove the first layer of polygons from arround all land masses. */
void Data::ErodeShore(int recursion){
    //std::unordered_set<int64_t> shore = GetShore(recursion);
    std::unordered_set<int64_t> shore = DefineShore();
//    float lowX, lowY, highX, highY;

    for(auto it = shore.begin(); it != shore.end(); ++it){
        if(MapContainer[*it].recDepth == recursion){
//            MapContainer[*it].boundingBox(recursion, lowX, lowY, highX, highY);
//            if(highX - lowX > 2000 or highY - lowY > 2000){
                MapContainer[*it].height = -1;
//            }
        }
    }
}

void Data::MoreDetail(int recursion, std::unordered_set<int64_t> shore, std::unordered_set<int64_t>& newPoints){
    MapSite working, tmpMapSite;
    float lowX, lowY, highX, highY, x, y;
    for(auto it = shore.begin(); it != shore.end(); ++it){
        // Make some new points and add if they fall within the polygon.
        working = MapContainer[*it];
        working.boundingBox(recursion, lowX, lowY, highX, highY);
        for(int p = 0; p < 80; ++p){
            x = lowX + rand() % (int)(highX - lowX);
            y = lowY + rand() % (int)(highY - lowY);
            if(working.isPointInHill(recursion, x, y)){
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
            closed.insert(*it);
            if(MapContainer[*it].x > MAP_SIZE/8 and MapContainer[*it].y > MAP_SIZE/8 and MapContainer[*it].x < 7*MAP_SIZE/8 and MapContainer[*it].y < 7*MAP_SIZE/8){
                for(auto it_adjacent = MapContainer[*it].site_begin(10); it_adjacent != MapContainer[*it].site_end(10); ++it_adjacent){
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
            for(auto it_adjacent = MapContainer[*it].site_begin(10); it_adjacent != MapContainer[*it].site_end(10); ++it_adjacent){
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

void Data::SetHeight(std::unordered_set<int64_t>& open){
    std::unordered_set<int64_t> closed;
    std::unordered_set<int64_t> next;

    int height = 1;
    while(open.size() > 0){
        for(auto it = open.begin(); it != open.end(); ++it){
            MapContainer[*it].height = height;
            closed.insert(*it);
            for(auto it_adjacent = MapContainer[*it].site_begin(10); it_adjacent != MapContainer[*it].site_end(10); ++it_adjacent){
                if(!closed.count(*it_adjacent) and MapContainer[*it_adjacent].terrain != TERRAIN_OCEAN){  // MapContainer[*it_adjacent].height > 0){
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
