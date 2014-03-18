//|||||||||||||||||||||||||||||||||||||||||||||||
#include <OgreManualObject.h>
#include <OgreMesh.h>
#include <OgreSceneNode.h>
#include <OgreStaticGeometry.h>
#include <OgreTechnique.h>

#include "GameState.hpp"
#include <utility>      // std::pair, std::make_pair
#include "../data/boats.h"
#include "../data/boats.cpp"

//|||||||||||||||||||||||||||||||||||||||||||||||

using namespace Ogre;

//|||||||||||||||||||||||||||||||||||||||||||||||

GameState::GameState()
{
    m_MoveSpeed			= 0.1f;
    m_RotateSpeed		= 0.3f;

    m_bLMouseDown       = false;
    m_bRMouseDown       = false;
    m_bQuit             = false;
    m_bSettingsMode     = false;

    m_pDetailsPanel		= 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

#define BOAT_VIEW 1.0f
void GameState::enter()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Entering GameState...");

    m_pSceneMgr = OgreFramework::getSingletonPtr()->m_pRoot->createSceneManager(ST_GENERIC, "GameSceneMgr");

    m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.3f, 0.3f, 0.3f));

    Ogre::Light* directionalLight = m_pSceneMgr->createLight("Light");
    directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
    directionalLight->setDiffuseColour(Ogre::ColourValue(.3, .3, .3));
    //directionalLight->setDiffuseColour(Ogre::ColourValue(1,1,1));
    directionalLight->setSpecularColour(Ogre::ColourValue(.3, .3, .3));
    directionalLight->setDirection(Ogre::Vector3(0, -1, 0));

    // This light source is to give some texture to underside of objects.
    Ogre::Light* directionalLightReverse = m_pSceneMgr->createLight("LightReverse");
    directionalLightReverse->setType(Ogre::Light::LT_DIRECTIONAL);
    directionalLightReverse->setDiffuseColour(Ogre::ColourValue(0.1, 0.1, 0.1));
    directionalLightReverse->setSpecularColour(Ogre::ColourValue(0.1, 0.1, 0.1));
    directionalLightReverse->setDirection(Ogre::Vector3(0, 1, 0));


    m_pSceneMgr->addRenderQueueListener(OgreFramework::getSingletonPtr()->m_pOverlaySystem);

    m_pCamera = m_pSceneMgr->createCamera("GameCamera");
    //m_pCamera->setPosition(Vector3(5, 60, 60));
    //m_pCamera->lookAt(Vector3(5, 20, 0));
    m_pCamera->setNearClipDistance(1);

    m_pCamera->setPosition(BOAT_VIEW/2, BOAT_VIEW, BOAT_VIEW/2);
    m_pCamera->lookAt(BOAT_VIEW/2, 0, BOAT_VIEW/2);
    //m_pCamera->setFarClipDistance(BOAT_VIEW*4);

    Ogre::Quaternion rot1;
    rot1.FromAngleAxis(Ogre::Degree(-90), Ogre::Vector3::UNIT_X);

    Ogre::Quaternion rot2;
    rot2.FromAngleAxis(Ogre::Degree(-90), Ogre::Vector3::UNIT_Z);

    m_pCamera->setOrientation(rot1 * rot2);


    m_pCamera->setAspectRatio(Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth()) /
        Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight()));

    OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);
    m_pCurrentObject = 0;

    buildGUI();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::pause()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Pausing GameState...");

    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::resume()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Resuming GameState...");

    buildGUI();

    OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);
    m_bQuit = false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::exit()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Leaving GameState...");

    m_pSceneMgr->destroyCamera(m_pCamera);
    if(m_pSceneMgr)
        OgreFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);
}

//|||||||||||||||||||||||||||||||||||||||||||||||


void GameState::initMaterial(void){
    static int init=false;
    if(init)
        return;
    else
        init=true;

//    Ogre::ColourValue val2 = Ogre::ColourValue(0.2, 0.2, 0.2, 1);
    Ogre::MaterialPtr matptr = Ogre::MaterialManager::getSingleton().create("SolidColour", "General"); 
    matptr->setReceiveShadows(false); 
    matptr->getTechnique(0)->setLightingEnabled(true);
    matptr->getTechnique(0)->getPass(0)->setColourWriteEnabled(true);
//    matptr->getTechnique(0)->getPass(0)->setDiffuse(val2); 
//    matptr->getTechnique(0)->getPass(0)->setAmbient(val2);
    matptr->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    matptr->getTechnique(0)->getPass(0)->setVertexColourTracking(Ogre::TVC_DIFFUSE);
    //matptr->getTechnique(0)->getPass(0)->setCullingMode(CullingMode::CULL_NONE);          // Display both sides of polygons
}

void GameState::drawLine(Ogre::ManualObject* mo, Ogre::ColourValue colour, Ogre::Vector3 pointA, Ogre::Vector3 pointB){
    mo->colour(colour);
    mo->normal(0,1,0);
    mo->position(Ogre::Vector3(pointA.x, pointA.y, pointA.z));
    mo->position(Ogre::Vector3(pointB.x, pointB.y, pointB.z));
}

void GameState::viewBox(Ogre::ManualObject* manual_lines){
    drawLine(manual_lines, ColourValue(0,1,1), Ogre::Vector3(-BOAT_VIEW, 0, -BOAT_VIEW), Ogre::Vector3(BOAT_VIEW, 0, -BOAT_VIEW));
    drawLine(manual_lines, ColourValue(0,1,1), Ogre::Vector3(BOAT_VIEW, 0, -BOAT_VIEW), Ogre::Vector3(BOAT_VIEW, 0, BOAT_VIEW));
    drawLine(manual_lines, ColourValue(0,1,1), Ogre::Vector3(BOAT_VIEW, 0, BOAT_VIEW), Ogre::Vector3(-BOAT_VIEW, 0, BOAT_VIEW));
    drawLine(manual_lines, ColourValue(0,1,1), Ogre::Vector3(-BOAT_VIEW, 0, BOAT_VIEW), Ogre::Vector3(-BOAT_VIEW, 0, -BOAT_VIEW));
}

void GameState::viewBoatLines(Ogre::ManualObject* manual_lines){
    Vessel boat = testBoat();
    cout << boat.description << "\n";

    HullSection* lastSection;
    for(auto itSection = boat.sections.begin(); itSection != boat.sections.end(); ++itSection){
        std::pair<float,float>* previousPanel = &(*(itSection->widthHeight.begin()));
        for(auto itPanel = itSection->widthHeight.begin();  itPanel != itSection->widthHeight.end(); ++itPanel){
            if(*previousPanel != *itPanel){
                drawLine(manual_lines, ColourValue(0.1,0.1,0.1), 
                         Ogre::Vector3(previousPanel->second, previousPanel->first, itSection->position),
                         Ogre::Vector3(itPanel->second, itPanel->first, itSection->position));
                drawLine(manual_lines, ColourValue(0.1,0.1,0.1),
                        Ogre::Vector3(-previousPanel->second, previousPanel->first, itSection->position),
                        Ogre::Vector3(-itPanel->second, itPanel->first, itSection->position));
            }
            previousPanel = &(*itPanel);
        }

        if(itSection != boat.sections.begin()){
            std::pair<float,float>* previousPanelFore = &(*(lastSection->widthHeight.begin()));
            std::pair<float,float>* previousPanelAft = &(*(itSection->widthHeight.begin()));
            auto itPanelFore = lastSection->widthHeight.begin();
            auto itPanelAft = itSection->widthHeight.begin();
            while(itPanelAft != itSection->widthHeight.end()){
                drawLine(manual_lines, ColourValue(1,0.1,0.1),
                        Ogre::Vector3(previousPanelFore->second, previousPanelFore->first, lastSection->position),
                        Ogre::Vector3(itPanelAft->second, itPanelAft->first, itSection->position));
                drawLine(manual_lines, ColourValue(1,0.1,0.1),
                        Ogre::Vector3(previousPanelAft->second, previousPanelAft->first, itSection->position),
                        Ogre::Vector3(itPanelFore->second, itPanelFore->first, lastSection->position));
                drawLine(manual_lines, ColourValue(1,0.1,0.1),
                        Ogre::Vector3(-previousPanelFore->second, previousPanelFore->first, lastSection->position),
                        Ogre::Vector3(-itPanelAft->second, itPanelAft->first, itSection->position));
                drawLine(manual_lines, ColourValue(1,0.1,0.1),
                        Ogre::Vector3(-previousPanelAft->second, previousPanelAft->first, itSection->position),
                        Ogre::Vector3(-itPanelFore->second, itPanelFore->first, lastSection->position));
                previousPanelFore = &(*itPanelFore);
                previousPanelAft = &(*itPanelAft);
                ++itPanelFore; ++itPanelAft;
            }
        }

        lastSection = &(*itSection);
    }
}

void GameState::viewBoatPlanes(Ogre::ManualObject* manual_planes){
    Vessel boat = testBoat();
    cout << boat.description << "\n";

    int posCount = 0;
    HullSection* lastSection;
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
                    manual_planes->colour(ColourValue(0.6 + 0.2 * itPanelAft->first, 0.4, 0.1));
                    manual_planes->normal(normal);
                    manual_planes->position(c2);
                    manual_planes->colour(ColourValue(0.6 + 0.2 * itPanelAft->first, 0.4, 0.1));
                    manual_planes->normal(normal);
                    manual_planes->position(c3);
                    manual_planes->colour(ColourValue(0.6 + 0.2 * itPanelAft->first, 0.4, 0.1));
                    manual_planes->normal(normal);
                    manual_planes->position(c4);
                    manual_planes->colour(ColourValue(0.6 + 0.2 * itPanelAft->first, 0.4, 0.1));
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
                    manual_planes->colour(ColourValue(0.5, 1, 1));
                    manual_planes->normal(normal);
                    manual_planes->position(c2);
                    manual_planes->colour(ColourValue(0.5, 1, 1));
                    manual_planes->normal(normal);
                    manual_planes->position(c3);
                    manual_planes->colour(ColourValue(0.5, 1, 1));
                    manual_planes->normal(normal);
                    manual_planes->position(c4);
                    manual_planes->colour(ColourValue(0.5, 1, 1));
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
                    manual_planes->colour(ColourValue(0.5, 1, 1));
                    manual_planes->normal(normal);
                    manual_planes->position(c2);
                    manual_planes->colour(ColourValue(0.5, 1, 1));
                    manual_planes->normal(normal);
                    manual_planes->position(c3);
                    manual_planes->colour(ColourValue(0.5, 1, 1));
                    manual_planes->normal(normal);
                    manual_planes->position(c4);
                    manual_planes->colour(ColourValue(0.5, 1, 1));
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
                    manual_planes->colour(ColourValue(0.5, 0.5, 1));
                    manual_planes->normal(normalOutsideStarboard);

                    manual_planes->position(Ogre::Vector3(c3));
                    manual_planes->colour(ColourValue(0.5, 0.5, 1));
                    manual_planes->normal(normalOutsideStarboard);

                    manual_planes->position(Ogre::Vector3(c2));
                    manual_planes->colour(ColourValue(0.5, 0.5, 1));
                    manual_planes->normal(normalOutsideStarboard);

                    manual_planes->position(Ogre::Vector3(c4));
                    manual_planes->colour(ColourValue(0.5, 0.5, 1));
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
                    manual_planes->colour(ColourValue(0.5, 0.5, 1));
                    manual_planes->normal(normalOutsidePort);                           

                    manual_planes->position(Ogre::Vector3(c3));
                    manual_planes->colour(ColourValue(0.5, 0.5, 1));
                    manual_planes->normal(normalOutsidePort);                                                                   

                    manual_planes->position(Ogre::Vector3(c2));
                    manual_planes->colour(ColourValue(0.5, 0.5, 1));                                                                                            
                    manual_planes->normal(normalOutsidePort);

                    manual_planes->position(Ogre::Vector3(c4));
                    manual_planes->colour(ColourValue(0.5, 0.5, 1));
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
        cout << "m\n";
        float thicknes = itMast->height / 100;
        c1 = Ogre::Vector3(thicknes, 0, itMast->position - thicknes);
        c2 = Ogre::Vector3(thicknes, itMast->height, itMast->position - thicknes);
        c3 = Ogre::Vector3(-thicknes, 0, itMast->position - thicknes);
        c4 = Ogre::Vector3(-thicknes, itMast->height, itMast->position - thicknes);
        c5 = Ogre::Vector3(thicknes, 0, itMast->position + thicknes);
        c6 = Ogre::Vector3(thicknes, itMast->height, itMast->position + thicknes);
        c7 = Ogre::Vector3(-thicknes, 0, itMast->position + thicknes);
        c8 = Ogre::Vector3(-thicknes, itMast->height, itMast->position + thicknes);

        dir0 = c3 - c5;
        dir1 = c3 - c4;
        normal = dir1.crossProduct(dir0).normalisedCopy();

        manual_planes->position(Ogre::Vector3(c1));
        manual_planes->colour(ColourValue(0.5, 0.5, 1));
        manual_planes->normal(normal);

        manual_planes->position(Ogre::Vector3(c2));
        manual_planes->colour(ColourValue(0.5, 0.5, 1));
        manual_planes->normal(normal);

        dir0 = c7 - c1;
        dir1 = c7 - c8;
        normal = dir1.crossProduct(dir0).normalisedCopy();

        manual_planes->position(Ogre::Vector3(c3));
        manual_planes->colour(ColourValue(0.5, 0.5, 1));
        manual_planes->normal(normal);

        manual_planes->position(Ogre::Vector3(c4));
        manual_planes->colour(ColourValue(0.5, 0.5, 1));
        manual_planes->normal(normal);

        dir0 = c1 - c7;
        dir1 = c1 - c2;
        normal = dir1.crossProduct(dir0).normalisedCopy();

        manual_planes->position(Ogre::Vector3(c5));
        manual_planes->colour(ColourValue(0.5, 0.5, 1));
        manual_planes->normal(normal);

        manual_planes->position(Ogre::Vector3(c6));
        manual_planes->colour(ColourValue(0.5, 0.5, 1));
        manual_planes->normal(normal);

        dir0 = c5 - c3;
        dir1 = c5 - c6;
        normal = dir1.crossProduct(dir0).normalisedCopy();

        manual_planes->position(Ogre::Vector3(c7));
        manual_planes->colour(ColourValue(0.5, 0.5, 1));
        manual_planes->normal(normal);

        manual_planes->position(Ogre::Vector3(c8));
        manual_planes->colour(ColourValue(0.5, 0.5, 1));
        manual_planes->normal(normal);

        posCount += 8;

        manual_planes->triangle(posCount -6, posCount -7, posCount -8);
        manual_planes->triangle(posCount -7, posCount -6, posCount -5);
        manual_planes->triangle(posCount -2, posCount -5, posCount -6);
        manual_planes->triangle(posCount -1, posCount -5, posCount -2);
        manual_planes->triangle(posCount -2, posCount -4, posCount -3);
        manual_planes->triangle(posCount -1, posCount -2, posCount -3);
        manual_planes->triangle(posCount -4, posCount -8, posCount -7);
        manual_planes->triangle(posCount -4, posCount -7, posCount -3);
    }

}

ManualObject* GameState::addLines(const String name, void(GameState::*p_function)(ManualObject*)){
    ManualObject* manual_lines;
    try{
        manual_lines = m_pSceneMgr->getManualObject(name);
        SceneNode* parent = manual_lines->getParentSceneNode();
        parent->detachObject(manual_lines);
        m_pSceneMgr->destroySceneNode(parent->getName());
        manual_lines->clear();
    } catch (Ogre::Exception ex) {
        manual_lines = m_pSceneMgr->createManualObject(name);
    }

    manual_lines->begin("SolidColour", Ogre::RenderOperation::OT_LINE_LIST);

    (this->*p_function)(manual_lines);

    manual_lines->end();

    m_pSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(manual_lines);

    return manual_lines;
}

ManualObject* GameState::addPlanes(const String name, void(GameState::*p_function)(ManualObject*)){
    ManualObject* manual_planes;
    try{
        manual_planes = m_pSceneMgr->getManualObject(name);
        SceneNode* parent = manual_planes->getParentSceneNode();
        parent->detachObject(manual_planes);
        m_pSceneMgr->destroySceneNode(parent->getName());
        manual_planes->clear();
    } catch (Ogre::Exception ex) {
        manual_planes = m_pSceneMgr->createManualObject(name);
    }

    manual_planes->begin("SolidColour", Ogre::RenderOperation::OT_TRIANGLE_LIST);

    (this->*p_function)(manual_planes);

    manual_planes->end();

    m_pSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(manual_planes);

    return manual_planes;
}

void GameState::drawBoat(void){
    static bool initialised = false;

    if(!initialised){
        cout << "drawBoat...\n";
        initialised = true;

        initMaterial();

        //addLines("debug", &GameState::viewBox);
        //addLines("boatLines", &GameState::viewBoatLines);
        addPlanes("boatPlanes", &GameState::viewBoatPlanes);

        cout << "...done\n";
    }
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::keyPressed(const OIS::KeyEvent &keyEventRef)
{
    if(m_bSettingsMode == true)
    {
        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_S))
        {
            OgreBites::SelectMenu* pMenu = (OgreBites::SelectMenu*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("DisplayModeSelMenu");
            if(pMenu->getSelectionIndex() + 1 < (int)pMenu->getNumItems())
                pMenu->selectItem(pMenu->getSelectionIndex() + 1);
        }

        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_W))
        {
            OgreBites::SelectMenu* pMenu = (OgreBites::SelectMenu*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("DisplayModeSelMenu");
            if(pMenu->getSelectionIndex() - 1 >= 0)
                pMenu->selectItem(pMenu->getSelectionIndex() - 1);
        }
    }

    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
    {
        pushAppState(findByName("PauseState"));
        return true;
    }

    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_I))
    {
        if(m_pDetailsPanel->getTrayLocation() == OgreBites::TL_NONE)
        {
            OgreFramework::getSingletonPtr()->m_pTrayMgr->moveWidgetToTray(m_pDetailsPanel, OgreBites::TL_TOPLEFT, 0);
            m_pDetailsPanel->show();
        }
        else
        {
            OgreFramework::getSingletonPtr()->m_pTrayMgr->removeWidgetFromTray(m_pDetailsPanel);
            m_pDetailsPanel->hide();
        }
    }

    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_TAB))
    {
        //m_bSettingsMode = !m_bSettingsMode;
        //return true;
        //Entity* line = m_pSceneMgr->getRootSceneNode()->getEntity("line");
        Entity* lines = m_pSceneMgr->getEntity("lines");
        lines->setVisible(!lines->getVisible());
    }

    if((m_bSettingsMode && OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_RETURN)) ||
            OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_NUMPADENTER))
    {
    }

    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_SPACE))
    {
        // Point camera straight down.
        Ogre::Quaternion rot1;
        rot1.FromAngleAxis(Ogre::Degree(-90), Ogre::Vector3::UNIT_X);

        Ogre::Quaternion rot2;
        rot2.FromAngleAxis(Ogre::Degree(-90), Ogre::Vector3::UNIT_Z);

        m_pCamera->setOrientation(rot1 * rot2);
    }

    if(!m_bSettingsMode || (m_bSettingsMode && !OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_O)))
        OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);

    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::keyReleased(const OIS::KeyEvent &keyEventRef)
{
    OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::mouseMoved(const OIS::MouseEvent &evt)
{
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseMove(evt)) return true;

    if(m_bRMouseDown)
    {
        m_pCamera->yaw(Degree(evt.state.X.rel * -0.1f));
        m_pCamera->pitch(Degree(evt.state.Y.rel * -0.1f));
    }

    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseDown(evt, id)) return true;

    if(id == OIS::MB_Left)
    {
        m_bLMouseDown = true;
    }
    else if(id == OIS::MB_Right)
    {
        m_bRMouseDown = true;
    }

    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool GameState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
    if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseUp(evt, id)) return true;

    if(id == OIS::MB_Left)
    {
        m_bLMouseDown = false;
    }
    else if(id == OIS::MB_Right)
    {
        m_bRMouseDown = false;
    }

    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::moveCamera()
{
    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_LSHIFT))
        m_pCamera->moveRelative(m_TranslateVector);
    m_pCamera->moveRelative(m_TranslateVector / 10);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::getInput()
{
    if(m_bSettingsMode == false)
    {
        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_A))
            m_TranslateVector.x = -m_MoveScale;

        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_D))
            m_TranslateVector.x = m_MoveScale;

        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_E))
            m_TranslateVector.y = -m_MoveScale;

        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_Q))
            m_TranslateVector.y = m_MoveScale;

        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_W))
            m_TranslateVector.z = -m_MoveScale;

        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_S))
            m_TranslateVector.z = m_MoveScale;

    }
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::update(double timeSinceLastFrame)
{
    m_FrameEvent.timeSinceLastFrame = timeSinceLastFrame;
    OgreFramework::getSingletonPtr()->m_pTrayMgr->frameRenderingQueued(m_FrameEvent);

    if(m_bQuit == true)
    {
        popAppState();
        return;
    }

    if(!OgreFramework::getSingletonPtr()->m_pTrayMgr->isDialogVisible())
    {
        if(m_pDetailsPanel->isVisible())
        {
            m_pDetailsPanel->setParamValue(0, Ogre::StringConverter::toString(m_pCamera->getDerivedPosition().x));
            m_pDetailsPanel->setParamValue(1, Ogre::StringConverter::toString(m_pCamera->getDerivedPosition().y));
            m_pDetailsPanel->setParamValue(2, Ogre::StringConverter::toString(m_pCamera->getDerivedPosition().z));
            m_pDetailsPanel->setParamValue(3, Ogre::StringConverter::toString(m_pCamera->getDerivedOrientation().w));
            m_pDetailsPanel->setParamValue(4, Ogre::StringConverter::toString(m_pCamera->getDerivedOrientation().x));
            m_pDetailsPanel->setParamValue(5, Ogre::StringConverter::toString(m_pCamera->getDerivedOrientation().y));
            m_pDetailsPanel->setParamValue(6, Ogre::StringConverter::toString(m_pCamera->getDerivedOrientation().z));
            if(m_bSettingsMode)
                m_pDetailsPanel->setParamValue(7, "Buffered Input");
            else
                m_pDetailsPanel->setParamValue(7, "Un-Buffered Input");
        }
    }

    m_MoveScale = m_MoveSpeed   * timeSinceLastFrame * MAP_SIZE / 1000;
    m_RotScale  = m_RotateSpeed * timeSinceLastFrame;

    m_TranslateVector = Vector3::ZERO;

    getInput();
    drawBoat();
    moveCamera();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::buildGUI()
{
    OgreFramework::getSingletonPtr()->m_pTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
    OgreFramework::getSingletonPtr()->m_pTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
    OgreFramework::getSingletonPtr()->m_pTrayMgr->createLabel(OgreBites::TL_TOP, "GameLbl", "Game mode", 250);
    OgreFramework::getSingletonPtr()->m_pTrayMgr->showCursor();

    Ogre::StringVector items;
    items.push_back("cam.pX");
    items.push_back("cam.pY");
    items.push_back("cam.pZ");
    items.push_back("cam.oW");
    items.push_back("cam.oX");
    items.push_back("cam.oY");
    items.push_back("cam.oZ");
    items.push_back("Mode");

    m_pDetailsPanel = OgreFramework::getSingletonPtr()->m_pTrayMgr->createParamsPanel(OgreBites::TL_TOPLEFT, "DetailsPanel", 200, items);
    m_pDetailsPanel->show();

    Ogre::StringVector displayModes;
    displayModes.push_back("Solid mode");
    displayModes.push_back("Wireframe mode");
    displayModes.push_back("Point mode");
    OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSelectMenu(OgreBites::TL_TOPRIGHT, "DisplayModeSelMenu", "Display Mode", 200, 3, displayModes);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::itemSelected(OgreBites::SelectMenu* menu)
{
    switch(menu->getSelectionIndex())
    {
        case 0:
            m_pCamera->setPolygonMode(Ogre::PM_SOLID);break;
        case 1:
            m_pCamera->setPolygonMode(Ogre::PM_WIREFRAME);break;
        case 2:
            m_pCamera->setPolygonMode(Ogre::PM_POINTS);break;
    }
}

//|||||||||||||||||||||||||||||||||||||||||||||||
