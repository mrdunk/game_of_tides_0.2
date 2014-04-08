//|||||||||||||||||||||||||||||||||||||||||||||||
#include <OgreManualObject.h>
#include <OgreMesh.h>
#include <OgreSceneNode.h>
#include <OgreStaticGeometry.h>
#include <OgreTechnique.h>
#include <OgreMeshManager.h>

#include "DebugState.hpp"
#include <utility>      // std::pair, std::make_pair
#include "../data/boats.h"
#include "DrawThings.hpp"


#define MAP_TILES           32
#define MAP_TILE_SIZE       MAP_SIZE / MAP_TILES
#define WATERLEVEL          10.0 / MAP_MIN_RES
#define VISIBILITY          0.2
//|||||||||||||||||||||||||||||||||||||||||||||||

using namespace Ogre;

//|||||||||||||||||||||||||||||||||||||||||||||||

DebugState::DebugState()
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

void DebugState::enter()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Entering DebugState...");

    zoom = 1;

    m_pSceneMgr = OgreFramework::getSingletonPtr()->m_pRoot->createSceneManager(ST_GENERIC, "GameSceneMgr");
    //m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

    m_pSceneMgr->addRenderQueueListener(OgreFramework::getSingletonPtr()->m_pOverlaySystem);

    m_pCamera = m_pSceneMgr->createCamera("GameCamera");
    //m_pCamera->setPosition(Vector3(5, 60, 60));
    //m_pCamera->lookAt(Vector3(5, 20, 0));
    m_pCamera->setNearClipDistance(MAP_SIZE * 0.75);
    m_pCamera->setFarClipDistance(2 * MAP_SIZE);

    m_pCamera->setPosition(MAP_SIZE/2, MAP_SIZE, MAP_SIZE/2);
    m_pCamera->lookAt(MAP_SIZE/2, 0, MAP_SIZE/2);

    Ogre::Quaternion rot1;
    rot1.FromAngleAxis(Ogre::Degree(-90), Ogre::Vector3::UNIT_X);

    Ogre::Quaternion rot2;
    rot2.FromAngleAxis(Ogre::Degree(-90), Ogre::Vector3::UNIT_Z);

    m_pCamera->setOrientation(rot1 * rot2);


    m_pCamera->setAspectRatio(Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth()) /
        Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight()));

    OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);
    m_pCurrentObject = 0;

    Ogre::ColourValue fadeColour(0.9, 0.9, 0.9);
    OgreFramework::getSingletonPtr()->m_pViewport->setBackgroundColour(fadeColour);
    //m_pSceneMgr->setFog(Ogre::FOG_LINEAR, fadeColour, 0.0, 0, 250);
    //m_pSceneMgr->setFog(Ogre::FOG_EXP, fadeColour, 0.005);
    //m_pSceneMgr->setFog(Ogre::FOG_EXP2, fadeColour, 0.003);
    
    buildGUI();

    boats.push_back(testBoat());

    createScene();
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool DebugState::pause()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Pausing DebugState...");

    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void DebugState::resume()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Resuming DebugState...");

    buildGUI();

    OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);
    m_bQuit = false;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void DebugState::exit()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Leaving DebugState...");

    m_pSceneMgr->destroyCamera(m_pCamera);
    if(m_pSceneMgr)
        OgreFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void DebugState::viewBox(ManualObject* manual_lines){
    float lowX, lowY, highX, highY;
    calculateViewedMap(lowX, lowY, highX, highY);
    //cout << lowX << "," << lowY << "\t" << highX << "," << highY << "\n";
    drawLine(manual_lines, ColourValue(0,1,1), Ogre::Vector3(lowX, 10, lowY), Ogre::Vector3(lowX, 10, highY));
    drawLine(manual_lines, ColourValue(0,1,1), Ogre::Vector3(lowX, 10, highY), Ogre::Vector3(highX, 10, highY));
    drawLine(manual_lines, ColourValue(0,1,1), Ogre::Vector3(highX, 10, highY), Ogre::Vector3(highX, 10, lowY));
    drawLine(manual_lines, ColourValue(0,1,1), Ogre::Vector3(highX, 10, lowY), Ogre::Vector3(lowX, 10, lowY));
    drawLine(manual_lines, ColourValue(0,1,1), Ogre::Vector3(lowX, 10, lowY), Ogre::Vector3(highX, 10, highY));
    drawLine(manual_lines, ColourValue(0,1,1), Ogre::Vector3(lowX, 10, highY), Ogre::Vector3(highX, 10, lowY));

    boats.front().setPos((lowX + highX) / 2, (lowY + highY) /2, 0.0);
    displayBoats();
}

void DebugState::drawUniqueLine(Ogre::ManualObject* mo, Ogre::ColourValue colour, Ogre::Vector3 pointA, Ogre::Vector3 pointB){
    if(pointA.x < pointB.x) return;
    if(pointA.x == pointB.x and pointA.y < pointB.y) return;
    drawLine(mo, colour, pointA, pointB);
}

void DebugState::viewCell(ManualObject* manual_lines){
    cout << "DebugState::viewCell....\n";
    
    const int recursion = data.maxRecursion;
    Vector3 site0, site1;
    for(auto it = data.MapContainer.begin(); it != data.MapContainer.end(); ++it){
        if(it->second.type == TYPE_SITE and it->second.minRecursion <= recursion){
            //cout << it->second.numSite(recursion) << "\n";
            site0 = Vector3((float)it->first.x() / MAP_MIN_RES, (float)it->second.getHeight() / MAP_MIN_RES +1, (float)it->first.y() / MAP_MIN_RES);
            if(it->second.numCorner(recursion) > 0){
                for(auto itSite = it->second.beginSite(recursion); itSite != it->second.endSite(recursion); ++itSite){
                    site1 = Vector3((float)itSite->x() / MAP_MIN_RES, (float)data.find(*itSite)->second.getHeight() / MAP_MIN_RES +1, (float)itSite->y() / MAP_MIN_RES);
                    drawUniqueLine(manual_lines, ColourValue(0.1,0.1,0), site0, site1);
                }
            }
        }
    }
    cout << "...done\n";
}

void DebugState::viewCell2(ManualObject* manual_lines){
    cout << "DebugState::viewCell2....\n";

    if(debugRecursion < 0) debugRecursion = 0;
    if(debugRecursion > data.maxRecursion) debugRecursion = 0;
    const int recursion = debugRecursion;
    
    Vector3 corner, previousCorner, firstCorner;
    bool started;
    for(auto it = data.MapContainer.begin(); it != data.MapContainer.end(); ++it){
        if(it->second.type == TYPE_SITE and it->second.minRecursion <= recursion){
            if(it->second.numCorner(recursion) > 0){
                started = false;
                for(auto itCorner = it->second.beginCorner(recursion); itCorner != it->second.endCorner(recursion); ++itCorner){
                    corner = Vector3((float)itCorner->x() / MAP_MIN_RES, ((float)data.find(*itCorner)->second.getHeight() / MAP_MIN_RES) +10, 
                                     (float)itCorner->y() / MAP_MIN_RES);
                    if(!started){
                        started = true;
                        firstCorner = corner;
                    } else {
                        drawUniqueLine(manual_lines, ColourValue(1,0,0), previousCorner, corner);
                    }
                    previousCorner = corner;
                }
                drawUniqueLine(manual_lines, ColourValue(1,0,0), previousCorner, firstCorner);
            }
        }
    }
    cout << "...done\n";
}


ManualObject* DebugState::addLines(const String name, void(DebugState::*p_function)(ManualObject*)){
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

void DebugState::addPlanes(const String name, const int recursion, Point bl, Point tr){
    int vertexCount = 0;
    cout << "maxRecursion: " << recursion << "\n";
    ManualObject* manual_planes;
    try{
        manual_planes = m_pSceneMgr->getManualObject(name);
        m_pSceneMgr->destroyManualObject(manual_planes);
    } catch (Ogre::Exception ex) {
    }
    manual_planes = m_pSceneMgr->createManualObject(name);

    Vector3 site, corner, normal;
    manual_planes->begin("SolidColour", Ogre::RenderOperation::OT_TRIANGLE_LIST);
    int cornerCount;

    //TODO: replace corners list with an itterator.
    unordered_set<Point, pairHash> corners = data.cornersInBox(recursion, bl, tr);

    bool land;
    for(auto it = corners.begin(); it != corners.end(); ++it){
        if(data.find(*it)->second.numCorner(recursion) > 0){
            land = true;
            if(data.find(*it)->second.getHeight() == 0){
                // Since centre of tile is above 0, check corners too.
                land = false;
                for(auto itCorner = data.find(*it)->second.beginCorner(recursion); itCorner != data.find(*it)->second.endCorner(recursion); ++itCorner){
                    if(data.find(*itCorner)->second.getHeight() > 0){
                        // At least one corner is above 0 so treat as land.
                        land = true;
                        break;
                    }
                }
            }
            if(land){
                site = Vector3((float)it->x() / MAP_MIN_RES, (float)data.find(*it)->second.getHeight() / MAP_MIN_RES, (float)it->y() / MAP_MIN_RES);
                
                manual_planes->position(site);
                manual_planes->colour(landColour(site.y, site.y, 1));
                manual_planes->normal(0,1,0);
                cornerCount = 0;
                for(auto itCorner = data.find(*it)->second.beginCorner(recursion); itCorner != data.find(*it)->second.endCorner(recursion); ++itCorner){
                    corner = Vector3((float)itCorner->x() / MAP_MIN_RES, (float)data.find(*itCorner)->second.getHeight() / MAP_MIN_RES, 
                            (float)itCorner->y() / MAP_MIN_RES);

                    // calculate normal for this point.
                    Ogre::Vector3 dir0 = site - corner;
                    Ogre::Vector3 dir0_flat(dir0.x, 0, dir0.z);
                    Ogre::Vector3 dir1 = Quaternion(Degree(90), Vector3::UNIT_Y) * dir0_flat;
                    normal = dir0.crossProduct(dir1).normalisedCopy();

                    manual_planes->position(corner);
                    manual_planes->colour(landColour(site.y, corner.y, normal.y));
                    manual_planes->normal(normal);
                    if(cornerCount > 0){
                        manual_planes->triangle(vertexCount, vertexCount + cornerCount +1, vertexCount + cornerCount);
                    }
                    ++cornerCount;
                }
                manual_planes->triangle(vertexCount, vertexCount +1, vertexCount + cornerCount);
                vertexCount += cornerCount +1;
            }
        }
    }

    manual_planes->end();

    m_pSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(manual_planes);

}

Ogre::ColourValue DebugState::landColour(float siteHeight, float height, float gradient){
    //if(siteHeight == 0){
    //if(height == 0){
    //    return Ogre::ColourValue(0, 0.2, 0.6);
    //}
    float red = height *4;
    float green = 0.8 / (height *4 +1);
    float blue = 1 - gradient;
    return Ogre::ColourValue(red, green, blue);
}

void DebugState::createScene(){
    Ogre::Light* directionalLight = m_pSceneMgr->createLight("Light");
    directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
    directionalLight->setDiffuseColour(Ogre::ColourValue(.8, .8, .8));
    directionalLight->setSpecularColour(Ogre::ColourValue(.8, .8, .8));
    directionalLight->setDirection(Ogre::Vector3( 0, -1, 0));


    initMaterial();

    //addLines("screenOutline", &DebugState::viewBox);
    //addLines("cells", &DebugState::viewCell);
    //addLines("cells2", &DebugState::viewCell2);
    //addPlanes("planes", data.maxRecursion);

    // Set this so scenery re-generates.
    regenScene = true;
    debugRecursion = -1;

    // sea surface
    for(int x = 0; x < MAP_SIZE; x += MAP_SIZE / MAP_TILES){
        for(int y = 0; y < MAP_SIZE; y += MAP_SIZE / MAP_TILES){

            std::string name("waterSurface");
            name += std::to_string(x);
            name += "_";
            name += std::to_string(y);

            Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);

            Ogre::MeshManager::getSingleton().createPlane(name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                    plane, MAP_SIZE/MAP_TILES, MAP_SIZE/MAP_TILES, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);

            Ogre::Entity* entWater = m_pSceneMgr->createEntity(name + "Entity", name);
            //entWater->setRenderQueueGroup( RENDER_QUEUE_WORLD_GEOMETRY_2 );             // Render sea after land.
            Ogre::SceneNode* nodeWater = m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
            nodeWater->attachObject(entWater);

            entWater->setMaterialName("Examples/Rockwall");
            entWater->setCastShadows(false);
            nodeWater->setPosition(x + MAP_SIZE/2/MAP_TILES, WATERLEVEL, y + MAP_SIZE/2/MAP_TILES);
        }
    }
}

void DebugState::initMaterial(void){
    static int init=false;
    if(init)
        return;
    else
        init=true;

    Ogre::ColourValue val2 = Ogre::ColourValue(0.2,0.2,0.2,1);
    Ogre::MaterialPtr matptr = Ogre::MaterialManager::getSingleton().create("SolidColour", "General"); 
    matptr->setReceiveShadows(false); 
    matptr->getTechnique(0)->setLightingEnabled(true);
    matptr->getTechnique(0)->getPass(0)->setColourWriteEnabled(true);
    matptr->getTechnique(0)->getPass(0)->setDiffuse(val2); 
    matptr->getTechnique(0)->getPass(0)->setAmbient(val2);
    matptr->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    matptr->getTechnique(0)->getPass(0)->setVertexColourTracking(Ogre::TVC_DIFFUSE);   
}


void DebugState::drawLine(Ogre::ManualObject* mo, Ogre::ColourValue colour, Ogre::Vector3 pointA, Ogre::Vector3 pointB){
    mo->colour(colour);
    mo->normal(0,1,0);
    mo->position(Ogre::Vector3(pointA.x, pointA.y, pointA.z));
    mo->position(Ogre::Vector3(pointB.x, pointB.y, pointB.z));
}


void DebugState::calculateViewedMap(float& lowX, float& lowY, float& highX, float& highY){
    Plane worldPlane(Ogre::Vector3::UNIT_Y, Ogre::Vector3(0, 0, 0));

//    Ogre::Ray rayCentre;
    Ogre::Ray rayTL, rayBR;
//    m_pCamera->getCameraToViewportRay(0, 0, &rayCentre);
    m_pCamera->getCameraToViewportRay(0, 1, &rayTL);
    m_pCamera->getCameraToViewportRay(1, 0, &rayBR);

    std::pair<bool, Ogre::Real > resultCentre, resultTL, resultBR;
//    resultCentre = rayCentre.intersects(worldPlane);
    resultTL = rayTL.intersects(worldPlane);
    resultBR = rayBR.intersects(worldPlane);

    //if(resultCentre.first and resultTL.first and resultBR.first){
        //we intersect with the plane
//        Ogre::Vector3 pCentre = rayCentre.getPoint(resultCentre.second);
        Ogre::Vector3 pTL = rayTL.getPoint(resultTL.second);
        Ogre::Vector3 pBR = rayBR.getPoint(resultBR.second);
        lowX = pTL.x;
        lowY = pTL.z;
        highX = pBR.x;
        highY = pBR.z;
    //}

}


//|||||||||||||||||||||||||||||||||||||||||||||||

bool DebugState::keyPressed(const OIS::KeyEvent &keyEventRef)
{

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
        ManualObject* lines;

        try {
            lines = m_pSceneMgr->getManualObject("screenOutline");
            lines->setVisible(!lines->getVisible());
        } catch (Ogre::Exception ex) {
            lines = addLines("screenOutline", &DebugState::viewBox);
        }

        try {
            lines = m_pSceneMgr->getManualObject("cells");
            lines->setVisible(!lines->getVisible());
        } catch (Ogre::Exception ex) {
            lines = addLines("cells", &DebugState::viewCell);
        }

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
        //float lowX, lowY, highX, highY;
        //calculateViewedMap(lowX, lowY, highX, highY);
        addLines("screenOutline", &DebugState::viewBox);
    }

    if(!m_bSettingsMode || (m_bSettingsMode && !OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_O)))
        OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);

    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool DebugState::keyReleased(const OIS::KeyEvent &keyEventRef)
{
    OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);
    return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool DebugState::mouseMoved(const OIS::MouseEvent &evt)
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

bool DebugState::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
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

bool DebugState::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
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

void DebugState::moveCamera()
{
    static Ogre::ColourValue fadeColour(0.9, 0.9, 0.9);
    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_LSHIFT))
        m_pCamera->moveRelative(m_TranslateVector);
    m_pCamera->moveRelative(m_TranslateVector / 10);

    if(m_pCamera->getPosition().y > MAP_SIZE / 2){
        m_pCamera->setFarClipDistance(2 * MAP_SIZE);
        m_pCamera->setNearClipDistance(m_pCamera->getPosition().y / 2);
        m_pSceneMgr->setFog(Ogre::FOG_EXP, fadeColour, 0);
    } else if(m_pCamera->getPosition().y > 1){
        m_pCamera->setNearClipDistance(m_pCamera->getPosition().y / 2);
        m_pCamera->setFarClipDistance(MAP_SIZE);
        m_pSceneMgr->setFog(Ogre::FOG_EXP, fadeColour, VISIBILITY / m_pCamera->getPosition().y);
    } else {
        float height = m_pCamera->getPosition().y - WATERLEVEL;
        if(height < 0.01) height = 0.01;
        m_pCamera->setNearClipDistance(height / 5);
        m_pCamera->setFarClipDistance(MAP_SIZE / height);
        m_pSceneMgr->setFog(Ogre::FOG_EXP, fadeColour, VISIBILITY);
    }
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void DebugState::getInput()
{
    if(m_bSettingsMode == false)
    {
        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_A)){
            m_TranslateVector.x = -m_MoveScale;
            regenScene = true;
        }

        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_D)){
            m_TranslateVector.x = m_MoveScale;
            regenScene = true;
        }

        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_E)){
            m_TranslateVector.y = -m_MoveScale;
            regenScene = true;
        }

        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_Q)){
            m_TranslateVector.y = m_MoveScale;
            regenScene = true;
        }

        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_W)){
            m_TranslateVector.z = -m_MoveScale;
            regenScene = true;
        }

        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_S)){
            m_TranslateVector.z = m_MoveScale;
            regenScene = true;
        }

    }
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void DebugState::displayBoats(){

    for(auto it = boats.begin(); it != boats.end(); ++it){
        cout << "boat: " << it->description << "\t" << it->relWindDir << "\n";
        ManualObject* manual_planes;
        int posCount = 0;

        try{
            manual_planes = m_pSceneMgr->getManualObject(it->description);
            manual_planes->clear();
            
            SceneNode* m_pSceneNode = m_pSceneMgr->getSceneNode("mySceneNode");

            m_pSceneNode->setPosition(it->x, WATERLEVEL, it->y);
        } catch (Ogre::Exception ex) {
            manual_planes = m_pSceneMgr->createManualObject(it->description);
            
            SceneNode* m_pSceneNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("mySceneNode");
            m_pSceneNode->attachObject(manual_planes);
            
            m_pSceneNode->scale(1.0/MAP_MIN_RES, 1.0/MAP_MIN_RES, 1.0/MAP_MIN_RES);
            m_pSceneNode->setPosition(it->x, WATERLEVEL, it->y);
        }

        manual_planes->begin("SolidColour", Ogre::RenderOperation::OT_TRIANGLE_LIST);

        drawHull(manual_planes, posCount, *it);

        manual_planes->end();

    }

}

void DebugState::update(double timeSinceLastFrame)
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

    //m_MoveScale = m_MoveSpeed   * timeSinceLastFrame * (float)MAP_SIZE / 1000.0;
    m_MoveScale = m_MoveSpeed   * timeSinceLastFrame * (float)m_pCamera->getPosition().y / 10.0;
    m_RotScale  = m_RotateSpeed * timeSinceLastFrame;

    m_TranslateVector = Vector3::ZERO;

    getInput();
    generateScenery();
    //displayBoats();
    moveCamera();
}


void DebugState::generateScenery(){
    //static struct tile tiles[MAP_TILES * MAP_TILES];
    static std::unordered_set<int> todoTiles;
    static std::unordered_set<int> doneTiles;

    if(regenScene){
        regenScene = false;
        todoTiles.clear();
        if(m_pCamera->getDerivedPosition().y >= 1){
            int newZoom = MAP_SIZE / (int)m_pCamera->getDerivedPosition().y;
            if(newZoom < 0) newZoom = 0;
            if(newZoom > data.maxRecursion) newZoom = data.maxRecursion;
            if(newZoom != zoom){
                zoom = newZoom;
                doneTiles.clear();
            }
        }
        cout << zoom << "\n";

        float lowX, lowY, highX, highY;
        calculateViewedMap(lowX, lowY, highX, highY);

       for(int xTile = 0; xTile < MAP_TILES; ++xTile){
           for(int yTile = 0; yTile < MAP_TILES; ++yTile){
               if(lowX <= (xTile +1) * MAP_TILE_SIZE and highX > xTile * MAP_TILE_SIZE and
                        lowY <= (yTile +1) * MAP_TILE_SIZE and highY > yTile * MAP_TILE_SIZE and
                        doneTiles.count(yTile * MAP_TILES + xTile) == 0){
                   todoTiles.insert(yTile * MAP_TILES + xTile);
                   cout << xTile << "," << yTile << "\t";
               }
           }
           cout << "\n";
       }
       cout << "\n";
    }

    //while(todoTiles.size() > 0){
    if(todoTiles.size() > 0){
        int working = *(todoTiles.begin());
        todoTiles.erase(working);
        addPlanes("tile" + std::to_string(working), zoom, Point((working % MAP_TILES) * MAP_TILE_SIZE * MAP_MIN_RES, (working / MAP_TILES) * MAP_TILE_SIZE * MAP_MIN_RES),
                          Point((working % MAP_TILES +1) * MAP_TILE_SIZE * MAP_MIN_RES, (working / MAP_TILES +1) * MAP_TILE_SIZE * MAP_MIN_RES));
        doneTiles.insert(working);
        //cout << (working % MAP_TILES) * MAP_TILE_SIZE * MAP_MIN_RES << "," << (working / MAP_TILES) * MAP_TILE_SIZE * MAP_MIN_RES << "\t" 
        //    << (working % MAP_TILES  +1) * MAP_TILE_SIZE * MAP_MIN_RES << "," << (working / MAP_TILES +1) * MAP_TILE_SIZE * MAP_MIN_RES << "\n";
    }
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void DebugState::buildGUI()
{
    OgreFramework::getSingletonPtr()->m_pTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
    OgreFramework::getSingletonPtr()->m_pTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
    //OgreFramework::getSingletonPtr()->m_pTrayMgr->createLabel(OgreBites::TL_TOP, "GameLbl", "Game mode", 250);
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
    displayModes.push_back("Hidden");
    for(int r = 0; r <= data.maxRecursion; ++r){
        displayModes.push_back("Resolution " + std::to_string(r));
    }
    OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSelectMenu(OgreBites::TL_TOPRIGHT, "DisplayModeSelMenu", "Display Mode", 200, data.maxRecursion +2, displayModes);

    OgreFramework::getSingletonPtr()->m_pTrayMgr->createThickSlider(OgreBites::TL_TOPRIGHT, "SailAngle", "SailAngle", 200, 80, -90, 90, 181);
    OgreFramework::getSingletonPtr()->m_pTrayMgr->createThickSlider(OgreBites::TL_TOPRIGHT, "WindDir", "WindDir", 200, 80, 0, 359, 360);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void DebugState::itemSelected(OgreBites::SelectMenu* menu)
{
    ManualObject* lines;
    switch(menu->getSelectionIndex())
    {
        case 0:
            debugRecursion = -1;
            regenScene = true;
            lines = m_pSceneMgr->getManualObject("cells2");
            lines->setVisible(false);
            break;
        default:
            debugRecursion = menu->getSelectionIndex() -1;
            regenScene = true;
            lines = addLines("cells2", &DebugState::viewCell2);
            lines->setVisible(true);
            break;
    }
}

void DebugState::sliderMoved(OgreBites::Slider* slider)
{
    static int SailAngle = -90;
    static int WindDir = 0;

    if(slider->getName() == "SailAngle"){
        SailAngle = slider->getValue();
    }
    else if(slider->getName() == "WindDir"){
        WindDir = slider->getValue();
        if(WindDir >= 180) WindDir -= 360;
    }

    boats.front().update(WindDir, 0, 0, SailAngle);  // TODO: affect selected boat rather than boats.front().
    displayBoats();
}

//|||||||||||||||||||||||||||||||||||||||||||||||
