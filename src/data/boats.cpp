

#include "boats.h"

void Vessel::update(void){
    Vessel::update(0, 0, 0, -90);
}

void Vessel::update(int windDir, float speed, int heading, int sailAngle){
    relWindDir = windDir;  // TODO
    std::cout << "relWindDir: " << relWindDir << "\n";

    float lowLen = 9999999, highLen = -9999999;
    for(auto itSection = sections.begin(); itSection != sections.end(); ++itSection){
        if(itSection->position > highLen) highLen = itSection->position;
        if(itSection->position < lowLen) lowLen = itSection->position;
    }

    midPoint = Ogre::Vector3(0, lowLen + highLen, lowLen + highLen / 2);

    for(auto itMasts = masts.begin(); itMasts != masts.end(); ++itMasts){
        for(auto itSails = itMasts->sails.begin(); itSails != itMasts->sails.end(); ++itSails){
            itSails->sailAngle = sailAngle;
            itSails->update(*itMasts);
        }
    }
}


void Sail::update(Mast& mast){
    float thicknes = mast.height / 100;
    if(type == SAIL_MAIN or type == SAIL_FORE){
        bl = Ogre::Vector3(0, footHeight - footPosition * sin(3.15 * footAngle / 180),
                mast.position - footPosition * cos(3.15 * footAngle / 180) + thicknes);
        br = Ogre::Vector3(0, footHeight + (footLength - footPosition) * sin(3.15 * footAngle / 180),
                mast.position + (footLength - footPosition) * cos(3.15 * footAngle / 180) + thicknes);
        tl = Ogre::Vector3(0, headHeight - headPosition * sin(3.15 * headAngle / 180),
                mast.position - headPosition * cos(3.15 * headAngle / 180) + thicknes);
        tr = Ogre::Vector3(0, headHeight + (headLength - headPosition) * sin(3.15 * headAngle / 180),
                mast.position + (headLength - headPosition) * cos(3.15 * headAngle / 180) + thicknes);
    } else {
        // TODO: type == SAIL_SQUARE 
    }

    //Ogre::Quaternion rotation(Ogre::Degree(sailAngle), Ogre::Vector3::UNIT_Y);

    footRotatePos = Ogre::Vector3(0, 0, mast.position);
    if(footSpar == 0){
        footRotatePos = bl;     // rotate round tack rather than mast.
    }
    //bl = (rotation * (bl - rotatePos)) + rotatePos;
    //br = (rotation * (br - rotatePos)) + rotatePos;

    headRotatePos = Ogre::Vector3(0, 0, mast.position);
    if(headSpar == 0){
        headRotatePos = tl;
    }
    //tl = (rotation * (tl - rotatePos)) + rotatePos;
    //tr = (rotation * (tr - rotatePos)) + rotatePos;
}

Vessel testBoat(void){
    Vessel returnBoat;
    returnBoat.description = "Test Boat";

    HullSection s1, s2, s3, s4, s5, s6, s7, s8;

    s1.position = 0;
    s1.widthHeight.push_back(std::make_pair(1.0, 0));
    s1.widthHeight.push_back(std::make_pair(0.4, 0));
    s1.widthHeight.push_back(std::make_pair(0.0, 0));
    s1.widthHeight.push_back(std::make_pair(-0.3, 0));
    s1.widthHeight.push_back(std::make_pair(-0.4, 0));

    s2.position = 1.5;
    s2.widthHeight.push_back(std::make_pair(1.0, 0.8));
    s2.widthHeight.push_back(std::make_pair(0.0, 1.1));
    s2.widthHeight.push_back(std::make_pair(-0.8, 0.8));
    s2.widthHeight.push_back(std::make_pair(-1.3, 0.1));
    s2.widthHeight.push_back(std::make_pair(-1.6, 0.0));

    s3.position = 3.5;
    s3.widthHeight.push_back(std::make_pair(1.0, 1.2));
    s3.widthHeight.push_back(std::make_pair(0.0, 1.6));
    s3.widthHeight.push_back(std::make_pair(-1.2, 1.1));
    s3.widthHeight.push_back(std::make_pair(-1.7, 0.1));
    s3.widthHeight.push_back(std::make_pair(-2.0, 0.0));

    s4.position = 3.5;
    s4.widthHeight.push_back(std::make_pair(0.5, 1.4));
    s4.widthHeight.push_back(std::make_pair(0.0, 1.6));
    s4.widthHeight.push_back(std::make_pair(-1.2, 1.1));
    s4.widthHeight.push_back(std::make_pair(-1.7, 0.1));
    s4.widthHeight.push_back(std::make_pair(-2.0, 0.0));

    s5.position = 7.5;
    s5.widthHeight.push_back(std::make_pair(0.5, 1.4));
    s5.widthHeight.push_back(std::make_pair(0.0, 1.6));
    s5.widthHeight.push_back(std::make_pair(-1.2, 1.1));
    s5.widthHeight.push_back(std::make_pair(-1.7, 0.1));
    s5.widthHeight.push_back(std::make_pair(-2.0, 0.0));

    s6.position = 7.5;
    s6.widthHeight.push_back(std::make_pair(1.0, 1.2));
    s6.widthHeight.push_back(std::make_pair(0.0, 1.6));
    s6.widthHeight.push_back(std::make_pair(-1.2, 1.1));
    s6.widthHeight.push_back(std::make_pair(-1.7, 0.1));
    s6.widthHeight.push_back(std::make_pair(-2.0, 0.0));

    s7.position = 9;
    s7.widthHeight.push_back(std::make_pair(1.0, 1.0));
    s7.widthHeight.push_back(std::make_pair(0.0, 1.3));
    s7.widthHeight.push_back(std::make_pair(-1.0, 0.8));
    s7.widthHeight.push_back(std::make_pair(-1.6, 0.1));
    s7.widthHeight.push_back(std::make_pair(-2.0, 0.0));

    s8.position = 10;
    s8.widthHeight.push_back(std::make_pair(1.0, 0.7));
    s8.widthHeight.push_back(std::make_pair(0.0, 0.9));
    s8.widthHeight.push_back(std::make_pair(0.0, 0.6));
    s8.widthHeight.push_back(std::make_pair(0.0, 0.1));
    s8.widthHeight.push_back(std::make_pair(0.0, 0.0));

    returnBoat.sections.push_back(s1);
    returnBoat.sections.push_back(s2);
    returnBoat.sections.push_back(s3);
    returnBoat.sections.push_back(s4);
    returnBoat.sections.push_back(s5);
    returnBoat.sections.push_back(s6);
    returnBoat.sections.push_back(s7);
    returnBoat.sections.push_back(s8);

    Mast m1, m2;

    m1.position = 2;
    m1.height = 10;

    m2.position = 7.5;
    m2.height = 8;

    Sail sail1, sail2, sail3;

    sail1.type = SAIL_MAIN;
    sail1.footHeight = 1.2;
    sail1.headHeight = 9;
    sail1.footLength = 6;
    sail1.headLength = 5;
    sail1.footPosition = 2;
    sail1.headPosition = 1;
    sail1.footAngle = 0;
    sail1.headAngle = 40;
    sail1.footSpar = 0;
    sail1.headSpar = 1;

    m1.sails.push_back(sail1);

    sail3.type = SAIL_FORE;
    sail3.footHeight = 1.2;
    sail3.headHeight = 7;
    sail3.footLength = 4;
    sail3.headLength = 0;
    sail3.footPosition = 4;
    sail3.headPosition = 1.5;
    sail3.footAngle = 0;
    sail3.headAngle = 0;
    sail3.footSpar = 0;
    sail3.headSpar = 0;

    m1.sails.push_back(sail3);


    sail2.type = SAIL_MAIN;
    sail2.footHeight = 1.2;
    sail2.headHeight = 7;
    sail2.footLength = 4;
    sail2.headLength = 4;
    sail2.footPosition = 0;
    sail2.headPosition = 1;
    sail2.footAngle = 0;
    sail2.headAngle = 40;
    sail2.footSpar = 0;
    sail2.headSpar = 1;

    m2.sails.push_back(sail2);


    returnBoat.masts.push_back(m1);
    returnBoat.masts.push_back(m2);


    returnBoat.update();

    return returnBoat;
}
