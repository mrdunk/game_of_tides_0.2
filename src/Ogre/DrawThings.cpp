#include <cmath>        // std::abs

#include "DrawThings.hpp"

void drawHull(Ogre::ManualObject* manual_planes, int& posCount, Vessel& boat){
    
    HullSection* lastSection = &(boat.sections.back());
    Ogre::Vector3 c1, c2, c3, c4, dir0, dir1, normalInsideStarboard, normalOutsideStarboard, normalInsidePort, normalOutsidePort, normal;
    for(auto itSection = boat.sections.begin(); itSection != boat.sections.end(); ++itSection){
            std::pair<float,float>* previousPanelFore = &(*(lastSection->widthHeight.begin()));
            std::pair<float,float>* previousPanelAft = &(*(itSection->widthHeight.begin()));
            auto itPanelFore = lastSection->widthHeight.begin();
            auto itPanelAft = itSection->widthHeight.begin();
            while(itPanelAft != itSection->widthHeight.end()){
                if(itSection != boat.sections.begin() and itPanelFore == lastSection->widthHeight.begin()){
                    // deck
                    c1 = Ogre::Vector3(itPanelAft->second, itPanelAft->first, itSection->position);
                    c2 = Ogre::Vector3(-itPanelAft->second, itPanelAft->first, itSection->position);
                    c3 = Ogre::Vector3(itPanelFore->second, itPanelFore->first, lastSection->position);
                    c4 = Ogre::Vector3(-itPanelFore->second, itPanelFore->first, lastSection->position);
                    if(c1 != c2 and c1 != c4){
                        dir0 = c1 - c2;
                        dir1 = c1 - c4;
                    } else {
                        dir0 = c1 - c2;
                        dir1 = c1 - c3;
                    }
                    normal = dir1.crossProduct(dir0).normalisedCopy();

                    manual_planes->position(c1);
                    manual_planes->colour(Ogre::ColourValue(0.6 + 0.2 * itPanelAft->first, 0.4, 0.1));
                    manual_planes->normal(normal);
                    manual_planes->position(c2);
                    manual_planes->colour(Ogre::ColourValue(0.6 + 0.2 * itPanelAft->first, 0.4, 0.1));
                    manual_planes->normal(normal);
                    manual_planes->position(c3);
                    manual_planes->colour(Ogre::ColourValue(0.6 + 0.2 * itPanelAft->first, 0.4, 0.1));
                    manual_planes->normal(normal);
                    manual_planes->position(c4);
                    manual_planes->colour(Ogre::ColourValue(0.6 + 0.2 * itPanelAft->first, 0.4, 0.1));
                    manual_planes->normal(normal);

                    posCount += 4;

                    manual_planes->triangle(posCount -4, posCount -2, posCount -3);
                    manual_planes->triangle(posCount -3, posCount -2, posCount -1);
                }
                if((itSection == boat.sections.begin() or &(*itSection) == &(boat.sections.back())) and previousPanelAft->second != 0 and itPanelAft->second != 0){
                    // bow and stern bulkhead.
                    c1 = Ogre::Vector3(previousPanelAft->second, previousPanelAft->first, itSection->position);
                    c2 = Ogre::Vector3(-previousPanelAft->second, previousPanelAft->first, itSection->position);
                    c3 = Ogre::Vector3(itPanelAft->second, itPanelAft->first, itSection->position);
                    c4 = Ogre::Vector3(-itPanelAft->second, itPanelAft->first, itSection->position);
                    if(c1 != c2 and c1 != c4){
                        dir0 = c1 - c2;
                        dir1 = c1 - c4;
                    } else {
                        dir0 = c1 - c2;
                        dir1 = c1 - c3;
                    }
                    normal = dir1.crossProduct(dir0).normalisedCopy();

                    manual_planes->position(c1);
                    manual_planes->colour(Ogre::ColourValue(0.5, 1, 1));
                    manual_planes->normal(normal);
                    manual_planes->position(c2);
                    manual_planes->colour(Ogre::ColourValue(0.5, 1, 1));
                    manual_planes->normal(normal);
                    manual_planes->position(c3);
                    manual_planes->colour(Ogre::ColourValue(0.5, 1, 1));
                    manual_planes->normal(normal);
                    manual_planes->position(c4);
                    manual_planes->colour(Ogre::ColourValue(0.5, 1, 1));
                    manual_planes->normal(normal);

                    posCount += 4;

                    if(itSection == boat.sections.begin()){
                        // bow
                        manual_planes->triangle(posCount -4, posCount -2, posCount -3);
                        manual_planes->triangle(posCount -3, posCount -2, posCount -1);
                    } else {
                        // stern
                        manual_planes->triangle(posCount -4, posCount -3, posCount -2);
                        manual_planes->triangle(posCount -3, posCount -1, posCount -2);
                    }
                }
                if((itSection->position - lastSection->position == 0 and itPanelFore->first != itPanelAft->first)){
                    // Bulkheads.
                    c1 = Ogre::Vector3(itPanelFore->second, itPanelFore->first, lastSection->position);
                    c2 = Ogre::Vector3(itPanelAft->second, itPanelAft->first, itSection->position);
                    c3 = Ogre::Vector3(-itPanelFore->second, itPanelFore->first, lastSection->position);;
                    c4 = Ogre::Vector3(-itPanelAft->second, itPanelAft->first, itSection->position);
                    if(c1 != c2 and c1 != c4){
                        dir0 = c1 - c2;
                        dir1 = c1 - c4;
                    } else {
                        dir0 = c1 - c2;
                        dir1 = c1 - c3;
                    }
                    normal = dir1.crossProduct(dir0).normalisedCopy();

                    manual_planes->position(c1);
                    manual_planes->colour(Ogre::ColourValue(0.5, 1, 1));
                    manual_planes->normal(normal);
                    manual_planes->position(c2);
                    manual_planes->colour(Ogre::ColourValue(0.5, 1, 1));
                    manual_planes->normal(normal);
                    manual_planes->position(c3);
                    manual_planes->colour(Ogre::ColourValue(0.5, 1, 1));
                    manual_planes->normal(normal);
                    manual_planes->position(c4);
                    manual_planes->colour(Ogre::ColourValue(0.5, 1, 1));
                    manual_planes->normal(normal);

                    posCount += 4;

                    manual_planes->triangle(posCount -4, posCount -2, posCount -3);
                    manual_planes->triangle(posCount -3, posCount -2, posCount -1);
                }
                if(itSection != boat.sections.begin() and itPanelFore != lastSection->widthHeight.begin() and itSection->position - lastSection->position != 0){
                    // Sides of boat
                    c1 = Ogre::Vector3(previousPanelFore->second, previousPanelFore->first, lastSection->position);
                    c2 = Ogre::Vector3(previousPanelAft->second, previousPanelAft->first, itSection->position);
                    c3 = Ogre::Vector3(itPanelFore->second, itPanelFore->first, lastSection->position);
                    c4 = Ogre::Vector3(itPanelAft->second, itPanelAft->first, itSection->position);
                    if(c1 != c2 and c1 != c4){
                        dir0 = c1 - c2;
                        dir1 = c1 - c4;
                    } else {
                        dir0 = c1 - c2;
                        dir1 = c1 - c3;
                    }
                    normalOutsideStarboard = dir0.crossProduct(dir1).normalisedCopy();

                    // Outside starboard hull.
                    manual_planes->position(Ogre::Vector3(c1));
                    manual_planes->colour(Ogre::ColourValue(0.5, 0.5, 1));
                    manual_planes->normal(normalOutsideStarboard);

                    manual_planes->position(Ogre::Vector3(c3));
                    manual_planes->colour(Ogre::ColourValue(0.5, 0.5, 1));
                    manual_planes->normal(normalOutsideStarboard);

                    manual_planes->position(Ogre::Vector3(c2));
                    manual_planes->colour(Ogre::ColourValue(0.5, 0.5, 1));
                    manual_planes->normal(normalOutsideStarboard);

                    manual_planes->position(Ogre::Vector3(c4));
                    manual_planes->colour(Ogre::ColourValue(0.5, 0.5, 1));
                    manual_planes->normal(normalOutsideStarboard);


                    c1 = Ogre::Vector3(-previousPanelFore->second, previousPanelFore->first, lastSection->position);
                    c2 = Ogre::Vector3(-previousPanelAft->second, previousPanelAft->first, itSection->position);
                    c3 = Ogre::Vector3(-itPanelFore->second, itPanelFore->first, lastSection->position);
                    c4 = Ogre::Vector3(-itPanelAft->second, itPanelAft->first, itSection->position);
                    if(c1 != c2 and c1 != c4){
                        dir0 = c1 - c2;
                        dir1 = c1 - c4;
                    } else {
                        dir0 = c1 - c2;
                        dir1 = c1 - c3;
                    }
                    normalOutsidePort = dir1.crossProduct(dir0).normalisedCopy();
                    // Outside port hull.
                    manual_planes->position(Ogre::Vector3(c1));
                    manual_planes->colour(Ogre::ColourValue(0.5, 0.5, 1));
                    manual_planes->normal(normalOutsidePort);

                    manual_planes->position(Ogre::Vector3(c3));
                    manual_planes->colour(Ogre::ColourValue(0.5, 0.5, 1));
                    manual_planes->normal(normalOutsidePort);

                    manual_planes->position(Ogre::Vector3(c2));
                    manual_planes->colour(Ogre::ColourValue(0.5, 0.5, 1));
                    manual_planes->normal(normalOutsidePort);

                    manual_planes->position(Ogre::Vector3(c4));
                    manual_planes->colour(Ogre::ColourValue(0.5, 0.5, 1));
                    manual_planes->normal(normalOutsidePort);

                    posCount += 8;

                    manual_planes->triangle(posCount -8, posCount -6, posCount -7);
                    manual_planes->triangle(posCount -5, posCount -7, posCount -6);

                    manual_planes->triangle(posCount -4, posCount -3, posCount -2);
                    manual_planes->triangle(posCount -1, posCount -2, posCount -3);
                }

                previousPanelFore = &(*itPanelFore);
                previousPanelAft = &(*itPanelAft);
                ++itPanelFore; ++itPanelAft;
            }

        lastSection = &(*itSection);
    }

    Ogre::Vector3 c5, c6, c7, c8;
    for(auto itMast = boat.masts.begin(); itMast != boat.masts.end(); ++itMast){
        std::cout << "m\n";
        drawSpar(manual_planes, posCount, Ogre::Vector3(0.0, 0.0, itMast->position), Ogre::Vector3(0.0, itMast->height, itMast->position));

        Ogre::Vector3 tl, tr, bl, br;
        for(auto itSail = itMast->sails.begin(); itSail != itMast->sails.end(); ++itSail){
            std::cout << " s\n";
            drawSail(manual_planes, posCount, boat.relWindDir, *itSail);
        }
    }
}


void drawSpar(Ogre::ManualObject* manual_planes, int& posCount, Ogre::Vector3 end1, Ogre::Vector3 end2){
    Ogre::Real len = end1.distance(end2);
    float thickness = len / 100;

    Ogre::Vector3 offset, offset90;
    if(end1.x == end2.x){
        offset = Ogre::Vector3(1,0,0) * thickness;
        offset90 = Ogre::Vector3(0,0,-1) * thickness;
    } else if(end1.y == end2.y){
        offset = Ogre::Vector3(0,1,0) * thickness;
        offset90 = Ogre::Vector3(-1,0,0) * thickness;
    } else {
        offset = Ogre::Vector3(end2 - end1).normalisedCopy().crossProduct(Ogre::Vector3::UNIT_Y).normalisedCopy() * thickness;
        offset90 = Ogre::Quaternion(Ogre::Degree(90), (end2 - end1).normalisedCopy()) * offset;
    }
    Ogre::Vector3 c1 = end1 - offset - offset90;
    Ogre::Vector3 c2 = end2 - offset - offset90;
    Ogre::Vector3 c3 = end1 + offset - offset90;
    Ogre::Vector3 c4 = end2 + offset - offset90;

    Ogre::Vector3 c5 = end1 - offset + offset90;
    Ogre::Vector3 c6 = end2 - offset + offset90;
    Ogre::Vector3 c7 = end1 + offset + offset90;
    Ogre::Vector3 c8 = end2 + offset + offset90;

    Ogre::Vector3 dir0 = c3 - c5;
    Ogre::Vector3 dir1 = c3 - c4;
    Ogre::Vector3 normal = dir1.crossProduct(dir0).normalisedCopy();

    manual_planes->position(Ogre::Vector3(c1));
    manual_planes->colour(Ogre::ColourValue(0.5, 0.5, 1));
    manual_planes->normal(normal);

    manual_planes->position(Ogre::Vector3(c2));
    manual_planes->colour(Ogre::ColourValue(0.5, 0.5, 1));
    manual_planes->normal(normal);

    dir0 = c7 - c1;
    dir1 = c7 - c8;
    normal = dir1.crossProduct(dir0).normalisedCopy();

    manual_planes->position(Ogre::Vector3(c3));
    manual_planes->colour(Ogre::ColourValue(0.5, 0.5, 1));
    manual_planes->normal(normal);

    manual_planes->position(Ogre::Vector3(c4));
    manual_planes->colour(Ogre::ColourValue(0.5, 0.5, 1));
    manual_planes->normal(normal);

    dir0 = c1 - c7;
    dir1 = c1 - c2;
    normal = dir1.crossProduct(dir0).normalisedCopy();

    manual_planes->position(Ogre::Vector3(c5));
    manual_planes->colour(Ogre::ColourValue(0.5, 0.5, 1));
    manual_planes->normal(normal);

    manual_planes->position(Ogre::Vector3(c6));
    manual_planes->colour(Ogre::ColourValue(0.5, 0.5, 1));
    manual_planes->normal(normal);

    dir0 = c5 - c3;
    dir1 = c5 - c6;
    normal = dir1.crossProduct(dir0).normalisedCopy();

    manual_planes->position(Ogre::Vector3(c7));
    manual_planes->colour(Ogre::ColourValue(0.5, 0.5, 1));
    manual_planes->normal(normal);

    manual_planes->position(Ogre::Vector3(c8));
    manual_planes->colour(Ogre::ColourValue(0.5, 0.5, 1));
    manual_planes->normal(normal);

    posCount += 8;

    manual_planes->triangle(posCount -4, posCount -6, posCount -8);
    manual_planes->triangle(posCount -2, posCount -6, posCount -4);
    manual_planes->triangle(posCount -3, posCount -7, posCount -5);
    manual_planes->triangle(posCount -1, posCount -3, posCount -5);

    manual_planes->triangle(posCount -6, posCount -7, posCount -8);
    manual_planes->triangle(posCount -7, posCount -6, posCount -5);
    manual_planes->triangle(posCount -2, posCount -5, posCount -6);
    manual_planes->triangle(posCount -1, posCount -5, posCount -2);
    manual_planes->triangle(posCount -2, posCount -4, posCount -3);
    manual_planes->triangle(posCount -1, posCount -2, posCount -3);
    manual_planes->triangle(posCount -4, posCount -8, posCount -7);
    manual_planes->triangle(posCount -4, posCount -7, posCount -3);
}

void drawSail(Ogre::ManualObject* manual_planes, int& posCount, int relWindDir, Sail &sail){

    Ogre::Quaternion rotation(Ogre::Degree(sail.sailAngle), Ogre::Vector3::UNIT_Y);
    
    int aoa = -relWindDir - sail.sailAngle;
    
    if(aoa > 90) aoa = 180 - aoa;
    else if(aoa <= -90) aoa = -180 - aoa;
    std::cout << "aoa: " << aoa << "\n";

    if(sail.bl != sail.br){
        if(sail.footSpar == 1){
            Ogre::Vector3 bl = sail.bl;
            Ogre::Vector3 br = sail.br;
            bl = (rotation * (bl - sail.footRotatePos)) + sail.footRotatePos;
            br = (rotation * (br - sail.footRotatePos)) + sail.footRotatePos;
            drawSpar(manual_planes, posCount, bl, br);
        }
    }
    if(sail.tl != sail.tr){
        if(sail.headSpar == 1){
            Ogre::Vector3 tl = sail.tl;
            Ogre::Vector3 tr = sail.tr;
            tl = (rotation * (tl - sail.headRotatePos)) + sail.headRotatePos;
            tr = (rotation * (tr - sail.headRotatePos)) + sail.headRotatePos;
            drawSpar(manual_planes, posCount, tl, tr);
        }
    }
    if(sail.bl != sail.br){
        if(sail.footSpar == 1){
            float curveSize = (sail.footLength + sail.headHeight - sail.footHeight) * aoa / 2000;
            Ogre::Vector3 footBag = Ogre::Vector3(1, 0, 0) * curveSize;
            Ogre::Vector3 footCurve = Ogre::Vector3(0, 1, 0) * std::abs(curveSize);
            
            Ogre::Vector3 b0 = sail.bl;
            Ogre::Vector3 b1 = (3 * sail.bl / 4 + 1 * sail.br / 4) + footBag + footCurve;
            Ogre::Vector3 b2 = (2 * sail.bl / 4 + 2 * sail.br / 4) + footBag + footCurve;
            Ogre::Vector3 b3 = sail.br;

            b0 = (rotation * (b0 - sail.footRotatePos)) + sail.footRotatePos;
            b1 = (rotation * (b1 - sail.footRotatePos)) + sail.footRotatePos;
            b2 = (rotation * (b2 - sail.footRotatePos)) + sail.footRotatePos;
            b3 = (rotation * (b3 - sail.footRotatePos)) + sail.footRotatePos;

            manual_planes->position(b0);
            manual_planes->colour(Ogre::ColourValue(1, 1, 1));
            manual_planes->normal(0,1,0);

            manual_planes->position(b1);
            manual_planes->colour(Ogre::ColourValue(.9, .9, .9));
            manual_planes->normal(0,1,0);

            manual_planes->position(b2);
            manual_planes->colour(Ogre::ColourValue(.8, .8, .8));
            manual_planes->normal(0,1,0);

            manual_planes->position(b3);
            manual_planes->colour(Ogre::ColourValue(.7, .7, .7));
            manual_planes->normal(0,1,0);

            posCount += 4;

            manual_planes->triangle(posCount -4, posCount -3, posCount -1);
            manual_planes->triangle(posCount -3, posCount -4, posCount -1);
            manual_planes->triangle(posCount -3, posCount -2, posCount -1);
            manual_planes->triangle(posCount -2, posCount -3, posCount -1);
        } else {
            float curveSize = sail.footLength * aoa / 500;
            Ogre::Vector3 footBag = Ogre::Vector3(1, 0, 0) * curveSize;
            Ogre::Vector3 footShorten = Ogre::Vector3(0, 0, 1) * std::abs(curveSize) /3;

            Ogre::Vector3 b0 = sail.bl;
            Ogre::Vector3 b1 = (3 * sail.bl / 4 + 1 * sail.br / 4) + footBag - footShorten;
            Ogre::Vector3 b2 = (2 * sail.bl / 4 + 2 * sail.br / 4) + footBag - 2* footShorten;
            Ogre::Vector3 b3 = sail.br - 3* footShorten;

            b0 = (rotation * (b0 - sail.footRotatePos)) + sail.footRotatePos;
            b1 = (rotation * (b1 - sail.footRotatePos)) + sail.footRotatePos;
            b2 = (rotation * (b2 - sail.footRotatePos)) + sail.footRotatePos;
            b3 = (rotation * (b3 - sail.footRotatePos)) + sail.footRotatePos;

            manual_planes->position(b0);
            manual_planes->colour(Ogre::ColourValue(1, 1, 1));
            manual_planes->normal(0,1,0);

            manual_planes->position(b1);
            manual_planes->colour(Ogre::ColourValue(.9, .9, .9));
            manual_planes->normal(0,1,0); 

            manual_planes->position(b2);
            manual_planes->colour(Ogre::ColourValue(.8, .8, .8));
            manual_planes->normal(0,1,0); 

            manual_planes->position(b3);
            manual_planes->colour(Ogre::ColourValue(.7, .7, .7));
            manual_planes->normal(0,1,0); 

            posCount += 4;
        }
    }
    if(sail.tl != sail.tr){
        if(sail.headSpar == 1){
            float curveSize = (sail.headLength + sail.headHeight - sail.headHeight) * aoa / 2000;
            Ogre::Vector3 headBag = Ogre::Vector3(1, 0, 0) * curveSize;
            Ogre::Vector3 headCurve = Ogre::Vector3(0, 1, 0) * std::abs(curveSize);

            Ogre::Vector3 t0 = sail.tl;
            Ogre::Vector3 t1 = 3 * sail.tl / 4 + 1 * sail.tr / 4 + headBag - headCurve;
            Ogre::Vector3 t2 = 2 * sail.tl / 4 + 2 * sail.tr / 4 + headBag - headCurve;
            Ogre::Vector3 t3 = sail.tr;

            t0 = (rotation * (t0 - sail.headRotatePos)) + sail.headRotatePos;
            t1 = (rotation * (t1 - sail.headRotatePos)) + sail.headRotatePos;
            t2 = (rotation * (t2 - sail.headRotatePos)) + sail.headRotatePos;
            t3 = (rotation * (t3 - sail.headRotatePos)) + sail.headRotatePos;

            manual_planes->position(t0);
            manual_planes->colour(Ogre::ColourValue(1, 1, 1));
            manual_planes->normal(0,1,0);

            manual_planes->position(t1);
            manual_planes->colour(Ogre::ColourValue(.9, .9, .9));
            manual_planes->normal(0,1,0);

            manual_planes->position(t2);
            manual_planes->colour(Ogre::ColourValue(.8, .8, .8));
            manual_planes->normal(0,1,0);

            manual_planes->position(t3);
            manual_planes->colour(Ogre::ColourValue(.7, .7, .7));
            manual_planes->normal(0,1,0);

            posCount += 4;

            manual_planes->triangle(posCount -4, posCount -3, posCount -1);
            manual_planes->triangle(posCount -3, posCount -4, posCount -1);
            manual_planes->triangle(posCount -3, posCount -2, posCount -1);
            manual_planes->triangle(posCount -2, posCount -3, posCount -1);
        } else {
            float curveSize = sail.headLength * aoa / 500;
            Ogre::Vector3 headBag = Ogre::Vector3(1, 0, 0) * curveSize;
            Ogre::Vector3 headShorten = Ogre::Vector3(0, 0, 1) * std::abs(curveSize) /3;

            Ogre::Vector3 t0 = sail.tl;
            Ogre::Vector3 t1 = 3 * sail.tl / 4 + 1 * sail.tr / 4 + headBag - headShorten;
            Ogre::Vector3 t2 = 2 * sail.tl / 4 + 2 * sail.tr / 4 + headBag - 2* headShorten;
            Ogre::Vector3 t3 = sail.tr - 3* headShorten;

            t0 = (rotation * (t0 - sail.headRotatePos)) + sail.headRotatePos;
            t1 = (rotation * (t1 - sail.headRotatePos)) + sail.headRotatePos;
            t2 = (rotation * (t2 - sail.headRotatePos)) + sail.headRotatePos;
            t3 = (rotation * (t3 - sail.headRotatePos)) + sail.headRotatePos;

            manual_planes->position(t0);
            manual_planes->colour(Ogre::ColourValue(1, 1, 1));
            manual_planes->normal(0,1,0);   

            manual_planes->position(t1);
            manual_planes->colour(Ogre::ColourValue(.9, .9, .9));
            manual_planes->normal(0,1,0); 

            manual_planes->position(t2);
            manual_planes->colour(Ogre::ColourValue(.8, .8, .8));
            manual_planes->normal(0,1,0);

            manual_planes->position(t3);
            manual_planes->colour(Ogre::ColourValue(.7, .7, .7));
            manual_planes->normal(0,1,0); 

            posCount += 4;
        }
    }

    if(sail.bl != sail.br and sail.tl != sail.tr){
        std::cout << "  both\n";

        manual_planes->triangle(posCount -4, posCount -3, posCount -7);
        manual_planes->triangle(posCount -3, posCount -4, posCount -7);
        manual_planes->triangle(posCount -3, posCount -2, posCount -6);
        manual_planes->triangle(posCount -2, posCount -3, posCount -6);
        manual_planes->triangle(posCount -2, posCount -1, posCount -5);
        manual_planes->triangle(posCount -1, posCount -2, posCount -5);

        manual_planes->triangle(posCount -8, posCount -7, posCount -4);
        manual_planes->triangle(posCount -7, posCount -8, posCount -4);
        manual_planes->triangle(posCount -7, posCount -6, posCount -3);
        manual_planes->triangle(posCount -6, posCount -7, posCount -3);
        manual_planes->triangle(posCount -6, posCount -5, posCount -2);
        manual_planes->triangle(posCount -5, posCount -6, posCount -2);

    } else if(sail.tl == sail.tr){
        // pointy at top
        manual_planes->position(sail.tl);
        manual_planes->colour(Ogre::ColourValue(1, 1, 1));
        manual_planes->normal(0,1,0);
        
        posCount += 1;

        manual_planes->triangle(posCount -5, posCount -4, posCount -1);
        manual_planes->triangle(posCount -4, posCount -5, posCount -1);
        manual_planes->triangle(posCount -4, posCount -3, posCount -1);
        manual_planes->triangle(posCount -3, posCount -4, posCount -1);
        manual_planes->triangle(posCount -3, posCount -2, posCount -1);
        manual_planes->triangle(posCount -2, posCount -3, posCount -1);
    } else if(sail.bl == sail.br){

    }
}
