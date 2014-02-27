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

//#include "../voronoi/boost/polygon/polygon.hpp"
#include <boost/polygon/polygon.hpp>
//#include "../voronoi/boost/polygon/voronoi.hpp"
#include <boost/polygon/voronoi.hpp>
//using voronoi::polygon::voronoi_builder;
//using voronoi::polygon::voronoi_diagram;
using boost::polygon::voronoi_builder;
using boost::polygon::voronoi_diagram;

#define MAP_NUM_POINTS  1000//000
#define MAP_SIZE        100000000
#define MAP_MIN_RES     100


#define RECURSE         3

#define TYPE_SITE       0
#define TYPE_CORNER     1

using namespace std;

class MapNode;
class pairHash;

//typedef boost::polygon::polygon_data<int> Polygon;
typedef boost::polygon::point_data<int> Point;
typedef std::unordered_map<Point, MapNode, pairHash> MapType;
//typedef MapType::iterator MapTypeIter;

class MapNode{
    public:
        /* constructors */
        MapNode(Point _coordinates, Point _parent, int _minRecursion) : coordinates(_coordinates), parent(_parent), minRecursion(_minRecursion) {}
        MapNode(Point _coordinates, int _minRecursion) : coordinates(_coordinates), minRecursion(_minRecursion) {}
        
        int type;
        Point coordinates;
        Point parent;
        int minRecursion;
        int getMaxRecursion(void);

        int pushSite(int recursion, Point site);
        set<Point>::iterator beginSite(const int recursion);
        set<Point>::iterator endSite(const int recursion);
        int numSite(int recursion);
        
        int pushCorner(int recursion, Point corner);
        set<Point>::iterator beginCorner(const int recursion);
        set<Point>::iterator endCorner(const int recursion);
        int numCorner(int recursion);

        int setHeight(int recursion, int height);
        int getHeight(const int recursion);

        /* Only used for type == TYPE_SITE.
         * returns true if point is inside the polygon described by corners.
         */
        bool isInside(int recursion, Point point);
    private:
        void _increaseRecursion(int recursion);
        vector<int> heights;
        vector<set<Point> > sites;
        vector<set<Point> > corners;
};

/* Hashing for Point type. */
class pairHash{
    public:
        size_t operator()(const Point &k) const{
            //cout << (((size_t)k.x() << (4* sizeof(size_t))) ^ (size_t)k.y()) << "\n";
            return ((size_t)k.x() << (4* sizeof(size_t))) ^ (size_t)k.y();
        }               
};

/* Return only keys from iterator.
 * http://stackoverflow.com/a/16527081/2669284 */
class key_iterator : public MapType::iterator{
    public:
        key_iterator() : MapType::iterator() {};
        key_iterator(MapType::iterator s) : MapType::iterator(s) {};
        Point* operator->() { return (Point* const)&(MapType::iterator::operator->()->first); }
        Point operator*() { return MapType::iterator::operator*().first; }
};

class MapData{
    public:
        MapData(bool testing);
        MapData(void){MapData(false);};
        
        // Main container for all map data.
        static MapType MapContainer;
        
        // Key to the entry closest to the map centre.
        int64_t centre;

        // Key to  entry closest to the 0,0 coordinate.
        int64_t zero;

        void insert(MapNode node);
        int count(Point point);
        MapType::const_iterator find(Point point);

        key_iterator begin();
        key_iterator end();
};

#endif
