/*
 * g++ -std=c++0x -isystem /home/duncan/Working/gtest-1.7.0/include/ -pthread ./main.cpp ../data.cpp ../../Voronoi/voronoi.cpp ../../Voronoi/MapManagerLibrary/voronoi/VoronoiDiagramGenerator.cpp /home/duncan/Working/gtest-1.7.0/libgtest.a
 */

#include <stdlib.h>     /* srand, rand */

#include "../data.h"
#include "gtest/gtest.h"
//#include <stdlib.h>     /* rand */

namespace {


// The fixture for testing class Foo.
class MapSiteTest : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

  MapSiteTest() {
    // You can do set-up work for each test here.
  }

  virtual ~MapSiteTest() {
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

TEST_F(MapSiteTest, BasicInitialise) {
  MapSite mapSite = {};
  ASSERT_NEAR(0.0, mapSite.x, MAP_MIN_RES/2);
  ASSERT_NEAR(0.0, mapSite.y, MAP_MIN_RES/2);
  EXPECT_EQ(0, mapSite.coord());
  EXPECT_EQ(0, mapSite.recursionSize());
  EXPECT_EQ(-1, mapSite.num_sites(0));

  mapSite.push_site(0, 1);
  mapSite.push_site(0, 2);
  mapSite.push_site(0, 1);     // Will make no difference as array already contains this value.
  EXPECT_EQ(1, mapSite.recursionSize());
  EXPECT_EQ(2, mapSite.num_sites(0));
  EXPECT_EQ(-1, mapSite.num_corners(0));

  mapSite.push_corner(0, 11);
  mapSite.push_corner(0, 12);
  mapSite.push_corner(0, 11);     // Will make no difference as array already contains this value.
  EXPECT_EQ(1, mapSite.recursionSize());
  EXPECT_EQ(2, mapSite.num_sites(0));
  EXPECT_EQ(2, mapSite.num_corners(0));
}

TEST_F(MapSiteTest, BasicInitialiseWithRecursion) {
    MapSite mapSite = {};

    mapSite.push_site(0, 1);
    mapSite.push_site(0, 2);
    mapSite.push_site(0, 1);     // Will make no difference as array already contains this value.
    EXPECT_EQ(1, mapSite.recursionSize());
    EXPECT_EQ(2, mapSite.num_sites(0));
    EXPECT_EQ(-1, mapSite.num_corners(0));

    mapSite.push_site(1, 11);
    mapSite.push_site(1, 12);
    mapSite.push_site(1, 11);     // Will make no difference as array already contains this value.
    mapSite.push_site(1, 13);
    EXPECT_EQ(2, mapSite.recursionSize());
    EXPECT_EQ(2, mapSite.num_sites(0));
    EXPECT_EQ(-1, mapSite.num_corners(0));
    EXPECT_EQ(3, mapSite.num_sites(1));

    // Try deeper recursion level without lower.
    MapSite mapSite2 = {};
    mapSite2.push_site(2, 21);
    mapSite2.push_site(1, 12);
    EXPECT_EQ(3, mapSite2.recursionSize());
    EXPECT_EQ(0, mapSite2.num_sites(0));
    EXPECT_EQ(1, mapSite2.num_sites(2));
    EXPECT_EQ(1, mapSite2.num_sites(1));
}

TEST_F(MapSiteTest, CoordinatesToKey) {
    MapSite mapSite = {};
    mapSite.x = 100;
    mapSite.y = 200;
    float x, y;
    KeyToCoord(mapSite.coord(), x, y);
    
    EXPECT_FLOAT_EQ(mapSite.x, x);
    EXPECT_FLOAT_EQ(mapSite.y, y);
}


TEST_F(MapSiteTest, Sorting) {
    MapSite generateCoords = {};

    generateCoords.x = 999;
    generateCoords.y = 1100;
    int64_t c_0999_1100 = generateCoords.coord();

    generateCoords.x = 1000;
    generateCoords.y = 1100;
    int64_t c_1000_1100 = generateCoords.coord();

    generateCoords.x = 1001;
    generateCoords.y = 1100;
    int64_t c_1001_1100 = generateCoords.coord();

    generateCoords.x = 1100;
    generateCoords.y = 1100;
    int64_t c_1100_1100 = generateCoords.coord();

    generateCoords.x = 1100;
    generateCoords.y = 1050;
    int64_t c_1100_1050 = generateCoords.coord();

    generateCoords.x = 1100;
    generateCoords.y = 1000;
    int64_t c_1100_1000 = generateCoords.coord();

    generateCoords.x = 1100;
    generateCoords.y = 900;
    int64_t c_1100_0900 = generateCoords.coord();

    generateCoords.x = 999;
    generateCoords.y = 900;
    int64_t c_0999_0900 = generateCoords.coord();

    generateCoords.x = 1000;
    generateCoords.y = 900;
    int64_t c_1000_0900 = generateCoords.coord();

    generateCoords.x = 1001;
    generateCoords.y = 900;
    int64_t c_1001_0900 = generateCoords.coord();

    generateCoords.x = 900;
    generateCoords.y = 900;
    int64_t c_0900_0900 = generateCoords.coord();

    generateCoords.x = 900;
    generateCoords.y = 1000;
    int64_t c_0900_1000 = generateCoords.coord();

    generateCoords.x = 900;
    generateCoords.y = 1100;
    int64_t c_0900_1100 = generateCoords.coord();

    MapSite mapSite = {};
    mapSite.x = 1000;
    mapSite.y = 1000;

    /*std::cout << "c_1000_1100 " << c_1000_1100 << "\n";
    std::cout << "c_1001_1100 " << c_1001_1100 << "\n";
    std::cout << "c_1100_1100 " << c_1100_1100 << "\n";
    std::cout << "c_1100_1050 " << c_1100_1050 << "\n";
    std::cout << "c_1100_1000 " << c_1100_1000 << "\n";
    std::cout << "c_1100_0900 " << c_1100_0900 << "\n";
    std::cout << "c_1000_0900 " << c_1000_0900 << "\n";
    std::cout << "c_0999_0900 " << c_0999_0900 << "\n";
    std::cout << "c_0900_0900 " << c_0900_0900 << "\n";
    std::cout << "c_0900_1000 " << c_0900_1000 << "\n";
    std::cout << "c_0900_1100 " << c_0900_1100 << "\n";
    std::cout << "c_0999_1100 " << c_0999_1100 << "\n";*/


    mapSite.push_site(0, c_1100_1050);
    mapSite.push_site(0, c_0999_1100);
    mapSite.push_site(0, c_0900_1100);
    mapSite.push_site(0, c_0999_0900);
    mapSite.push_site(0, c_1100_1100);
    mapSite.push_site(0, c_1000_1100);
    mapSite.push_site(0, c_0900_0900);
    mapSite.push_site(0, c_1000_1100);
    mapSite.push_site(0, c_1001_1100);
    mapSite.push_site(0, c_1100_1100);
    mapSite.push_site(0, c_0999_1100);
    mapSite.push_site(0, c_1001_0900);
    mapSite.push_site(0, c_0900_1100);
    mapSite.push_site(0, c_0900_1000);
    mapSite.push_site(0, c_1000_0900);
    mapSite.push_site(0, c_1100_0900); 
    mapSite.push_site(0, c_1100_1000);
    mapSite.push_site(0, c_0999_0900);


    std::set<int64_t, CompCoord>::iterator it = mapSite.site_begin(0);
    

    EXPECT_EQ(c_1000_1100, *it);
    ++it;
    EXPECT_EQ(c_1001_1100, *it);
    ++it;
    EXPECT_EQ(c_1100_1100, *it);
    ++it;
    EXPECT_EQ(c_1100_1050, *it);
    ++it;
    EXPECT_EQ(c_1100_1000, *it);
    ++it;
    EXPECT_EQ(c_1100_0900, *it);
    ++it;
    EXPECT_EQ(c_1001_0900, *it);
    ++it;
    EXPECT_EQ(c_1000_0900, *it);
    ++it;
    EXPECT_EQ(c_0999_0900, *it);
    ++it;
    EXPECT_EQ(c_0900_0900, *it);
    ++it;
    EXPECT_EQ(c_0900_1000, *it);
    ++it;
    EXPECT_EQ(c_0900_1100, *it);
    ++it;
    EXPECT_EQ(c_0999_1100, *it);
    ++it;
}

TEST_F(MapSiteTest, SortingMultipleSite) {
    std::unordered_map<int64_t, struct MapSite> container;

    for(int i = 0; i < 1000; ++i){
        float x = rand() % 9900 + 100;
        float y = rand() % 9900 + 100;
        MapSite generateCoords = {};

        generateCoords.x = x + 0;
        generateCoords.y = y + 100;
        int64_t c_000_100 = generateCoords.coord();

        generateCoords.x = x + 100;
        generateCoords.y = y + 100;
        int64_t c_100_100 = generateCoords.coord();

        generateCoords.x = x + 100;
        generateCoords.y = y + 0;
        int64_t c_100_000 = generateCoords.coord();

        generateCoords.x = x + 100;
        generateCoords.y = y + -100;
        int64_t c_100m100 = generateCoords.coord();

        generateCoords.x = x + 0;
        generateCoords.y = y + -100;
        int64_t c_000m100 = generateCoords.coord();

        generateCoords.x = x + -100;
        generateCoords.y = y + -100;
        int64_t cm100m100 = generateCoords.coord();

        generateCoords.x = x + -100;
        generateCoords.y = y + 0;
        int64_t cm100_000 = generateCoords.coord();

        generateCoords.x = x + -100;
        generateCoords.y = y + 100;
        int64_t cm100_100 = generateCoords.coord();

        MapSite mapSite = {};
        mapSite.x = x;
        mapSite.y = y;

        mapSite.push_site(0, cm100_000);
        mapSite.push_site(0, c_000_100);
        mapSite.push_site(0, c_000m100);
        mapSite.push_site(0, cm100m100);
        mapSite.push_site(0, c_100_000);
        mapSite.push_site(0, c_100m100);
        mapSite.push_site(0, cm100_100);
        mapSite.push_site(0, c_100_100);

        container[mapSite.coord()] = mapSite;
    }

    for(auto it = container.begin(); it != container.end(); ++it){
        float x = it->second.x;
        float y = it->second.y;

        MapSite testCoords = {};

        std::set<int64_t>::iterator site_it = it->second.site_begin(0);

        testCoords.x = x + 0;
        testCoords.y = y + 100;
        EXPECT_EQ(testCoords.coord(), *site_it);
        
        ++site_it;
        testCoords.x = x + 100;
        testCoords.y = y + 100;
        EXPECT_EQ(testCoords.coord(), *site_it);

        ++site_it;
        testCoords.x = x + 100;
        testCoords.y = y + 0;
        EXPECT_EQ(testCoords.coord(), *site_it);
     
        ++site_it;
        testCoords.x = x + 100;
        testCoords.y = y + -100;
        EXPECT_EQ(testCoords.coord(), *site_it);
     
        ++site_it;
        testCoords.x = x + 0;
        testCoords.y = y + -100;
        EXPECT_EQ(testCoords.coord(), *site_it);
     
        ++site_it;
        testCoords.x = x + -100;
        testCoords.y = y + -100;
        EXPECT_EQ(testCoords.coord(), *site_it);
     
        ++site_it;
        testCoords.x = x + -100;
        testCoords.y = y + 0;
        EXPECT_EQ(testCoords.coord(), *site_it);
     
        ++site_it;
        testCoords.x = x + -100;
        testCoords.y = y + 100;
        EXPECT_EQ(testCoords.coord(), *site_it);
    }
}

TEST_F(MapSiteTest, SortingMultipleCorner) {
    std::unordered_map<int64_t, struct MapSite> container;

    for(int i = 0; i < 1000; ++i){
        float x = rand() % 9900 + 100;
        float y = rand() % 9900 + 100;
        MapSite generateCoords = {};

        generateCoords.x = x + 0;
        generateCoords.y = y + 100;
        int64_t c_000_100 = generateCoords.coord();

        generateCoords.x = x + 100;
        generateCoords.y = y + 100;
        int64_t c_100_100 = generateCoords.coord();

        generateCoords.x = x + 100;
        generateCoords.y = y + 0;
        int64_t c_100_000 = generateCoords.coord();

        generateCoords.x = x + 100;
        generateCoords.y = y + -100;
        int64_t c_100m100 = generateCoords.coord();

        generateCoords.x = x + 0;
        generateCoords.y = y + -100;
        int64_t c_000m100 = generateCoords.coord();

        generateCoords.x = x + -100;
        generateCoords.y = y + -100;
        int64_t cm100m100 = generateCoords.coord();

        generateCoords.x = x + -100;
        generateCoords.y = y + 0;
        int64_t cm100_000 = generateCoords.coord();

        generateCoords.x = x + -100;
        generateCoords.y = y + 100;
        int64_t cm100_100 = generateCoords.coord();

        MapSite mapSite = {};
        mapSite.x = x;
        mapSite.y = y;

        mapSite.push_corner(0, cm100_000);
        mapSite.push_corner(0, c_000_100);
        mapSite.push_corner(0, c_000m100);
        mapSite.push_corner(0, cm100m100);
        mapSite.push_corner(0, c_100_000);
        mapSite.push_corner(0, c_100m100);
        mapSite.push_corner(0, cm100_100);
        mapSite.push_corner(0, c_100_100);

        container[mapSite.coord()] = mapSite;
    }

    for(auto it = container.begin(); it != container.end(); ++it){
        float x = it->second.x;
        float y = it->second.y;

        MapSite testCoords = {};

        std::set<int64_t>::iterator corner_it = it->second.corner_begin(0);

        testCoords.x = x + 0;
        testCoords.y = y + 100;
        EXPECT_EQ(testCoords.coord(), *corner_it);

        ++corner_it;
        testCoords.x = x + 100;
        testCoords.y = y + 100;
        EXPECT_EQ(testCoords.coord(), *corner_it);

        ++corner_it;
        testCoords.x = x + 100;
        testCoords.y = y + 0;
        EXPECT_EQ(testCoords.coord(), *corner_it);

        ++corner_it;
        testCoords.x = x + 100;
        testCoords.y = y + -100;
        EXPECT_EQ(testCoords.coord(), *corner_it);

        ++corner_it;
        testCoords.x = x + 0;
        testCoords.y = y + -100;
        EXPECT_EQ(testCoords.coord(), *corner_it);

        ++corner_it;
        testCoords.x = x + -100;
        testCoords.y = y + -100;
        EXPECT_EQ(testCoords.coord(), *corner_it);

        ++corner_it;
        testCoords.x = x + -100;
        testCoords.y = y + 0;
        EXPECT_EQ(testCoords.coord(), *corner_it);

        ++corner_it;
        testCoords.x = x + -100;
        testCoords.y = y + 100;
        EXPECT_EQ(testCoords.coord(), *corner_it);
    }
}

TEST_F(MapSiteTest, CountSite) {
    MapSite mapSite = {};

    mapSite.push_site(0, 1);
    EXPECT_EQ(mapSite.site_count(0, 1), 1);
    EXPECT_EQ(mapSite.site_count(0, 2), 0);
    EXPECT_EQ(mapSite.site_count(1, 1), -1);

    mapSite.push_site(0, 2);
    EXPECT_EQ(mapSite.site_count(0, 1), 1);
    EXPECT_EQ(mapSite.site_count(0, 2), 1);
    EXPECT_EQ(mapSite.site_count(0, 3), 0);
    EXPECT_EQ(mapSite.site_count(1, 1), -1);

    mapSite.push_site(1, 1);
    EXPECT_EQ(mapSite.site_count(0, 1), 1);
    EXPECT_EQ(mapSite.site_count(0, 2), 1);
    EXPECT_EQ(mapSite.site_count(0, 3), 0);
    EXPECT_EQ(mapSite.site_count(1, 1), 1);
    EXPECT_EQ(mapSite.site_count(1, 2), 0);
    EXPECT_EQ(mapSite.site_count(2, 1), -1);

    mapSite.push_site(3, 1);
    EXPECT_EQ(mapSite.site_count(0, 1), 1);
    EXPECT_EQ(mapSite.site_count(0, 2), 1); 
    EXPECT_EQ(mapSite.site_count(0, 3), 0); 
    EXPECT_EQ(mapSite.site_count(1, 1), 1);
    EXPECT_EQ(mapSite.site_count(1, 2), 0);
    EXPECT_EQ(mapSite.site_count(2, 1), 0);
    EXPECT_EQ(mapSite.site_count(3, 1), 1);
    EXPECT_EQ(mapSite.site_count(3, 2), 0);
    EXPECT_EQ(mapSite.site_count(4, 1), -1);
}

TEST_F(MapSiteTest, CountCorner) {
    MapSite mapSite = {};

    mapSite.push_corner(0, 1);
    EXPECT_EQ(mapSite.corner_count(0, 1), 1);
    EXPECT_EQ(mapSite.corner_count(0, 2), 0);
    EXPECT_EQ(mapSite.corner_count(1, 1), -1);

    mapSite.push_corner(0, 2);
    EXPECT_EQ(mapSite.corner_count(0, 1), 1);
    EXPECT_EQ(mapSite.corner_count(0, 2), 1);
    EXPECT_EQ(mapSite.corner_count(0, 3), 0);
    EXPECT_EQ(mapSite.corner_count(1, 1), -1);

    mapSite.push_corner(1, 1);
    EXPECT_EQ(mapSite.corner_count(0, 1), 1);
    EXPECT_EQ(mapSite.corner_count(0, 2), 1); 
    EXPECT_EQ(mapSite.corner_count(0, 3), 0); 
    EXPECT_EQ(mapSite.corner_count(1, 1), 1);
    EXPECT_EQ(mapSite.corner_count(1, 2), 0);
    EXPECT_EQ(mapSite.corner_count(2, 1), -1);

    mapSite.push_corner(3, 1);
    EXPECT_EQ(mapSite.corner_count(0, 1), 1);
    EXPECT_EQ(mapSite.corner_count(0, 2), 1); 
    EXPECT_EQ(mapSite.corner_count(0, 3), 0); 
    EXPECT_EQ(mapSite.corner_count(1, 1), 1);
    EXPECT_EQ(mapSite.corner_count(1, 2), 0);
    EXPECT_EQ(mapSite.corner_count(2, 1), 0);
    EXPECT_EQ(mapSite.corner_count(3, 1), 1);
    EXPECT_EQ(mapSite.corner_count(3, 2), 0);
    EXPECT_EQ(mapSite.corner_count(4, 1), -1);

    mapSite.push_corner(3, (int64_t)6072899082123057);
    EXPECT_EQ(mapSite.corner_count(3, (int64_t)6072899082123057), 1);
}

}  // namespace

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
