/*
 * $ g++ -std=c++0x -isystem /home/duncan/Working/gtest-1.7.0/include/ -I /home/duncan/Working/git/game-of-tides_0.2/src/boost_1_55_0/ -pthread ./main.cpp ../data.cpp /home/duncan/Working/gtest-1.7.0/libgtest.a
 */

#include <stdlib.h>     /* srand, rand */
#include "../data.h"
#include "gtest/gtest.h"

namespace {


// The fixture for testing class Foo.
class MapNodeTest : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

  MapNodeTest() {
    // You can do set-up work for each test here.
  }

  virtual ~MapNodeTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }

  // Objects declared here can be used by all tests in the test case for Foo.
};

TEST_F(MapNodeTest, InitialiseBottomLevel) {
    MapNode rootSite(Point(50,50), 0);
    
    //ASSERT_NEAR(0.0, mapSite.x, MAP_MIN_RES/2);
    EXPECT_EQ(rootSite.coordinates,     Point(50,50));
    EXPECT_EQ(rootSite.parent,          Point(0,0));
    EXPECT_EQ(rootSite.minRecursion,    0);
    EXPECT_EQ(rootSite.getMaxRecursion(), -1);
    EXPECT_EQ(rootSite.numSite(0), -1);
    EXPECT_EQ(rootSite.numCorner(0), -1);

    rootSite.setHeight(1);
    EXPECT_EQ(rootSite.getHeight(), 1);

    EXPECT_EQ(rootSite.pushSite(0, Point(0,0)), 1);
    EXPECT_EQ(rootSite.pushCorner(0, Point(0,0)), 1);
    EXPECT_EQ(rootSite.getMaxRecursion(), 0);

    EXPECT_EQ(rootSite.pushSite(0, Point(0,0)), 1);     // As the underlying container is a set, this will not increase the output number.
    EXPECT_EQ(rootSite.pushSite(0, Point(1,0)), 2);
    EXPECT_EQ(rootSite.pushSite(0, Point(0,1)), 3);
    EXPECT_EQ(rootSite.getMaxRecursion(), 0);

    EXPECT_EQ(rootSite.numSite(0), 3);
    EXPECT_EQ(rootSite.numCorner(0), 1);
    EXPECT_EQ(rootSite.numSite(1), -1);
    EXPECT_EQ(rootSite.numCorner(1), -1);

    EXPECT_EQ(1, rootSite.pushSite(1, Point(0,0)));
    EXPECT_EQ(1, rootSite.numSite(1));
    EXPECT_EQ(0, rootSite.numCorner(1));
    EXPECT_EQ(1, rootSite.getMaxRecursion());
}

TEST_F(MapNodeTest, InitialiseHigherLevel) {
    MapNode rootSite(Point(50,50), 0);
    MapNode childSite(Point(10,10), rootSite.coordinates, 1);

    EXPECT_EQ(Point(10,10), childSite.coordinates);
    EXPECT_EQ(Point(50,50), childSite.parent);
    EXPECT_EQ(1, childSite.minRecursion);
    EXPECT_EQ(-1, childSite.getMaxRecursion());
    EXPECT_EQ(-1, childSite.numSite(0));
    EXPECT_EQ(-1, childSite.numCorner(0));

    EXPECT_EQ(-1, childSite.pushSite(0, Point(0,0)));   // trying to set recursion level below minimum.
    EXPECT_EQ(-1, childSite.pushCorner(0, Point(0,0)));
    EXPECT_EQ(-1, childSite.getMaxRecursion());
    EXPECT_EQ(-1, childSite.numSite(0));
    EXPECT_EQ(-1, childSite.numCorner(0));

    EXPECT_EQ(1, childSite.pushSite(1, Point(1,2)));
    EXPECT_EQ(2, childSite.pushSite(1, Point(2,2)));
    EXPECT_EQ(1, childSite.getMaxRecursion());
    EXPECT_EQ(2, childSite.numSite(1));
    EXPECT_EQ(0, childSite.numCorner(1));

    EXPECT_EQ(1, childSite.pushCorner(3, Point(1,3)));
    EXPECT_EQ(2, childSite.pushCorner(3, Point(2,3)));
    EXPECT_EQ(3, childSite.getMaxRecursion());
    EXPECT_EQ(2, childSite.numSite(1));
    EXPECT_EQ(0, childSite.numCorner(1));
    EXPECT_EQ(0, childSite.numSite(2));
    EXPECT_EQ(0, childSite.numCorner(2));
    EXPECT_EQ(0, childSite.numSite(3));
    EXPECT_EQ(2, childSite.numCorner(3));
}

TEST_F(MapNodeTest, Height){
    MapNode rootSite(Point(50,50), 0);
    MapNode childSite(Point(10,10), rootSite.coordinates, 2);

    childSite.setHeight(33);
    EXPECT_EQ(33, childSite.getHeight());
}

TEST_F(MapNodeTest, Iterators) {
    MapNode rootSite(Point(50,50), 0);
    MapNode childSite(Point(10,10), rootSite.coordinates, 2);

    EXPECT_EQ(Point(10,10), childSite.coordinates);
    EXPECT_EQ(Point(50,50), childSite.parent);

    EXPECT_EQ(1, childSite.pushCorner(3, Point(1,3)));
    EXPECT_EQ(2, childSite.pushCorner(3, Point(2,3)));
    EXPECT_EQ(3, childSite.pushCorner(3, Point(3,3)));
    EXPECT_EQ(4, childSite.pushCorner(3, Point(4,3)));
    EXPECT_EQ(5, childSite.pushCorner(3, Point(5,3)));
    EXPECT_EQ(6, childSite.pushCorner(3, Point(6,3)));
    EXPECT_EQ(3, childSite.getMaxRecursion());
    EXPECT_EQ(0, childSite.numCorner(2));
    EXPECT_EQ(6, childSite.numCorner(3));
    int lastX = 0;
    for(auto it = childSite.beginCorner(3); it != childSite.endCorner(3); ++it){
        EXPECT_EQ(lastX +1, it->x());
        lastX = it->x();
        EXPECT_EQ(3, it->y());
    }

    for(auto it = childSite.beginSite(3); it != childSite.endSite(3); ++it){
        EXPECT_EQ(1, 2);    // we should never fail here because sites[3] is empty.
    }
}

TEST_F(MapNodeTest, Point){
    Point p(10.1,10.9);
    EXPECT_EQ(10, p.x());
    EXPECT_EQ(10, p.y());

    p.x(10.999999);
    EXPECT_EQ(10, p.x());

    p.x(10.99999999999999999999999999);
    EXPECT_EQ(11, p.x());

    EXPECT_EQ(p, p);
    EXPECT_TRUE((p == p));
    EXPECT_FALSE((p == Point(100,1000)));
}


class MapContainerTest : public ::testing::Test {
    virtual void TearDown() {
        MapData mapData(true);      // call without initialising points.
        mapData.MapContainer.clear();
    }
};

/*TEST_F(MapContainerTest, MapHashing){
    Point p0(0, 0);
    Point p1(MAP_SIZE, 0);

    pairHash ph;

    EXPECT_EQ(0, ph.operator()(p0));
    EXPECT_NE(0, ph.operator()(p1));
}*/

TEST_F(MapContainerTest, Init){
    MapData mapData(true);      // call without initialising points.

    Point p(1, 2);
    MapNode m(p, 0);

    EXPECT_EQ(0, mapData.count(p));
    mapData.insert(m);
    EXPECT_EQ(1, mapData.count(p));
    EXPECT_EQ(p, mapData.find(p)->first);
}

TEST_F(MapContainerTest, IterateCoords){
    MapData mapData(true);      // call without initialising points.
    
    EXPECT_EQ(0, mapData.MapContainer.size());

    mapData.insert(MapNode(Point(0,1), 0));
    mapData.insert(MapNode(Point(0,2), 0));
    mapData.insert(MapNode(Point(0,3), 0));
    mapData.insert(MapNode(Point(0,4), 0));

    EXPECT_EQ(4, mapData.MapContainer.size());

    MapType::iterator it = mapData.MapContainer.begin();
    EXPECT_EQ(1, it->first.y());
    ++it;
    EXPECT_EQ(2, it->first.y());
    ++it;
    EXPECT_EQ(3, it->first.y());
    ++it;
    EXPECT_EQ(4, it->first.y());

    auto it3 = mapData.begin();
    EXPECT_EQ(1, it3->first.y());
    ++it3;
    EXPECT_EQ(2, it3->first.y());
    ++it3;
    EXPECT_EQ(3, it3->first.y());
    ++it3;
    EXPECT_EQ(4, it3->first.y());
    ++it3;
    EXPECT_EQ(mapData.end(), it3);
}

TEST_F(MapContainerTest, MapData){
    MapData mapData(false);
}


}  // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
