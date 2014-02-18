#ifndef __GOT_DATA_H_
#define __GOT_DATA_H_

#include <iostream>
#include <vector>
#include <utility>      // std::pair, std::make_pair
#include <math.h>       /* round, floor, ceil, trunc */
#include <unordered_map>
#include <inttypes.h>
#include <algorithm>    // std::sort
#include <unordered_set>
#include <vector>
#include <set>

#include "../Voronoi/voronoi.h"

#define MAP_NUM_POINTS  1000//0//0
#define MAP_SIZE        1000000
#define MAP_MIN_RES     0.01f

#define TYPE_SITE       0
#define TYPE_CORNER     1
#define TYPE_UNDEFINED  -1

#define RECURSE         3

#define TERRAIN_UNDEFINED   0
#define TERRAIN_OCEAN       1
#define TERRAIN_SHORE       2
#define TERRAIN_LAND        3

void KeyToCoord(const int64_t key, float& x, float& y);
bool pointInsideTriangle(float px, float py, float c1x, float c1y, float c2x, float c2y, float c3x, float c3y);

struct MapSite;

/* Comparison functor for coordinates.
 * Must set x and y to be the centre of a group of points, then the comparison sorts points in a clockwise direction.
 */
class CompCoord {
    public:
        static void PassParentCoord(float _x, float _y);
        bool operator() (const int64_t& i, const int64_t& j) const;
    private:
        static float x;
        static float y;
};

class MapSite{
    public:
        int type;       // TYPE_SITE or TYPE_CORNER
        int recDepth;   // This element created at this recursion level.
        float x;
        float y;
        float height;
        int terrain;
        MapSite(void){
            type = TYPE_UNDEFINED;
            x = 0;
            y = 0;
            height = -1;
            recDepth = 0;
            terrain = TERRAIN_UNDEFINED;
        }

        int recursionSize(void){
            return std::max(site.size(), corner.size());
        }

        bool isShore(int recursion, std::unordered_map<int64_t, struct MapSite> &MapContainer){
            for(auto it = site_begin(recursion); it != site_end(recursion); ++it){
                if(MapContainer[*it].height < 0 and height >= 0) return true;
                //if(MapContainer[*it].height >= 0 and height < 0) return true;
            }
            return false;
        }

        bool isPointInHill(int recursion, float px, float py){
            recursion = min(recursion, recursionSize() -1);

            int64_t first = *(corner_begin(recursion));
            int64_t last = 0;
            float c1x, c1y, c2x, c2y;
            for(auto it = corner_begin(recursion); it != corner_end(recursion); ++it){
                if(it != corner_begin(recursion)){
                    KeyToCoord(last, c1x, c1y);
                    KeyToCoord(*it, c2x, c2y);
                    if(pointInsideTriangle(px, py, x, y, c1x, c1y, c2x, c2y)){
                        return true;
                    }
                }
                last = *it;
            }
            KeyToCoord(last, c1x, c1y);
            KeyToCoord(first, c2x, c2y);
            if(pointInsideTriangle(px, py, x, y, c1x, c1y, c2x, c2y)){
                return true;
            }
            return false;
        }

        void boundingBox(int recursion, float& lowX, float& lowY, float& highX, float& highY){
            lowX = MAP_SIZE;
            lowY = MAP_SIZE;
            highX = 0;
            highY = 0;
            float x, y;
            recursion = min(recursion, recursionSize() -1);
            for(auto it = corner_begin(recursion); it != corner_end(recursion); ++it){
                KeyToCoord(*it, x, y);
                if(x > highX) highX = x;
                if(y > highY) highY = y;
                if(x < lowX) lowX = x;
                if(y < lowY) lowY = y;
            }
        }

        int num_sites(int recursion){
            if(recursion >= (int)site.size()) return -1;
            return site[recursion].size();
        }

        int num_corners(int recursion){
            if(recursion >= (int)corner.size()) return -1;
            return corner[recursion].size();
        }

        void push_site(int recursion, int64_t s){
            CompCoord::PassParentCoord(x, y);
            if(recursion >= (int)site.size()){
                std::set<int64_t, CompCoord> empty;
                empty.clear();

                // create all missing recursion levels
                while(recursion >= (int)site.size()){
                    site.push_back(empty);
                }
            }
            site[recursion].insert(s);
        }

        void push_corner(int recursion, int64_t s){
            //cout << "push_corner(" << recursion << ", " << s << ")\n";
            CompCoord::PassParentCoord(x, y);
            if(recursion >= (int)corner.size()){
                //cout << corner.size() << "\n";
                std::set<int64_t, CompCoord> empty;
                empty.clear();

                while(recursion >= (int)corner.size()){
                    corner.push_back(empty);
                }
            }
            corner[recursion].insert(s);
        }

        std::set<int64_t, CompCoord>::iterator site_begin(int recursion){
            //cout << "site_begin(" << recursion << ")\n";

            /*if((int)recursion >= (int)site.size()){
                if(site.size() == 0){
                    std::set<int64_t, CompCoord> empty;
                    empty.clear();
                    site.push_back(empty);
                }
                recursion = site.size() -1;
            }*/
            CompCoord::PassParentCoord(x, y);
            return site[recursion].begin();
        }

        std::set<int64_t, CompCoord>::iterator corner_begin(int recursion){
            //cout << "corner_begin(" << recursion << ")\n";

            /*if((int)recursion >= (int)corner.size()){
                if(corner.size() == 0){
                    std::set<int64_t, CompCoord> empty;
                    empty.clear();
                    corner.push_back(empty);
                }
                recursion = corner.size() -1;
            }*/
            CompCoord::PassParentCoord(x, y);
            return corner[recursion].begin();
        }

        std::set<int64_t, CompCoord>::iterator site_end(int recursion){
            //cout << "site_end(" << recursion << ")\n";

            if((int)recursion >= (int)site.size()){
                recursion = site.size() -1;
            }
            CompCoord::PassParentCoord(x, y);
            return site[recursion].end();
        }

        std::set<int64_t, CompCoord>::iterator corner_end(int recursion){
            //cout << "corner_end(" << recursion << ")\n";

            if((int)recursion >= (int)corner.size()){
                recursion = corner.size() -1;
            }
            CompCoord::PassParentCoord(x, y);
            return corner[recursion].end();
        }

        int site_count(int recursion, int64_t s){
            //cout << "site_count(" << recursion << ", " << s << ")\n";

            if((int)(site.size() -1) < (int)recursion) return -1;
            
            CompCoord::PassParentCoord(x, y);
            return site[recursion].count(s);
        }

        int corner_count(int recursion, int64_t s){
            if((int)(corner.size() -1) < (int)recursion) return -1;
            CompCoord::PassParentCoord(x, y);
            return corner[recursion].count(s);
        }

        // Coordinates rounded to nearest MAP_MIN_RES
        int64_t coord(void){
            int32_t xx = round(x / (float)MAP_MIN_RES);
            int32_t yy = round(y / (float)MAP_MIN_RES);
            int64_t c;
            c = (int64_t)yy << 32 | (int64_t)xx;
            return c;
        }

    private:
        std::vector<std::set<int64_t, CompCoord> > site;
        std::vector<std::set<int64_t, CompCoord> > corner;
};


class Data {
    public:
        Data(void);
        void Lock(void);
        void UnLock(void);
        int ContainerSize(void);
        
        // Main container for all map data.
        static std::unordered_map<int64_t, struct MapSite> MapContainer;

        // Key to the entry closest to the map centre.
        int64_t centre;

        // Key to  entry closest to the 0,0 coordinate.
        int64_t zero;

        void RaiseLand(int recursion);
        void SetHeight(void);
        void MoreDetail(int recursion, std::vector<float>& xValuesV, std::vector<float>& yValuesV);
        void ErodeShore(int recursion);
        std::unordered_set<int64_t> GetShore(int recursion);
        std::unordered_set<int64_t> DefineShore(void);
        void Section(float lowX, float lowY, float highX, float highY);
    private:
        int totNumPoints;
};

void KeyToCoord(const int64_t key, float& x, float& y);


#endif  // __GOT_DATA_H_
