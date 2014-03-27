#ifndef __GOT_DRAW_THINGS_H_
#define __GOT_DRAW_THINGS_H_

#include <OgreManualObject.h>

#include "../data/boats.h"


void drawSpar(Ogre::ManualObject* manual_planes, int& posCount, Ogre::Vector3 end1, Ogre::Vector3 end2);
void drawSail(Ogre::ManualObject* manual_planes, int& posCount, int relWindDir, Sail &sail);
void drawHull(Ogre::ManualObject* manual_planes, int& posCount, Vessel& boat);


#endif // __GOT_DRAW_THINGS_H_

