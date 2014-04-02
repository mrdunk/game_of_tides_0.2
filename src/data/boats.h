#ifndef __GOT_BOATS_H_
#define __GOT_BOATS_H_

#define SAIL_MAIN       1
#define SAIL_SQUARE     2
#define SAIL_FORE       3

#include <vector>
#include <string>
#include <OgreVector3.h>
#include <iostream>

struct HullSection{
    float                                   position;
    std::vector<std::pair<float,float> >    widthHeight;
};

struct Mast{
    float                                   position;
    float                                   height;
    std::vector<struct Sail>                sails;
};

struct Sail{
    int                                     type;           // SAIL_MAIN, SAIL_SQUARE, SAIL_FORE
    float                                   footHeight;     // Height the bottom crosses the mast for SAIL_MAIN and SAIL_SQUARE or the forestay for SAIL_FORE.
    float                                   headHeight;     // Height the top crosses the mast for SAIL_MAIN and SAIL_SQUARE or the forestay for SAIL_FORE.
    float                                   footLength;     // Length of the bottom of sail.
    float                                   headLength;     // Length of top of sail. This will be 0 for a most triangular sails: fore-sails, bermudan-mains, etc.
    float                                   footPosition;   // How far the tack is ahead of the mast on mainsails.
    float                                   headPosition;   // How far the throat is ahead of the mast on mainsails.
    int                                     footAngle;      // Angle from horizontal of the bottom edge. 0 for most mainsails. ~30d for a high aspect jib.
    int                                     headAngle;      // Angle from horizontal of the top edge. Not applicable for most foresails, bermudan-mains, etc.
    int                                     footSpar;       // Does the sail have a boom? True or false.
    int                                     headSpar;       // Does the sail have a gaff? True or false.
    int                                     reefPoints;     // 
    int                                     sailAngle;      // Angle af sail relative to boat.

    Ogre::Vector3                           bl, br, tl, tr, footRotatePos, headRotatePos;
    void                                    update(Mast &mast);
};

struct Vessel{
    std::string                         description;

    int                                 windDir;
    int                                 relWindDir;
    float                               speed;
    float                               heading;
    float                               x, y;

    std::vector<struct HullSection>     sections;
    std::vector<struct Mast>            masts;

    Ogre::Vector3                       midPoint;
    void                                update(void);
    void                                update(int windDir, float speed, int heading, int sailAngle);    // Update all sails on boat to new AoA.
    void                                setPos(float _x, float _y, float heading);
};

Vessel testBoat(void);



#endif // __GOT_BOATS_H_
