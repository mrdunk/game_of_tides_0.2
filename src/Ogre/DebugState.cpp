//|||||||||||||||||||||||||||||||||||||||||||||||
#include <OgreManualObject.h>
#include <OgreMesh.h>
#include <OgreSceneNode.h>
#include <OgreStaticGeometry.h>
#include <OgreTechnique.h>

#include "DebugState.hpp"
#include <utility>      // std::pair, std::make_pair

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

    m_pSceneMgr = OgreFramework::getSingletonPtr()->m_pRoot->createSceneManager(ST_GENERIC, "GameSceneMgr");
    //m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

    m_pSceneMgr->addRenderQueueListener(OgreFramework::getSingletonPtr()->m_pOverlaySystem);

    m_pCamera = m_pSceneMgr->createCamera("GameCamera");
    //m_pCamera->setPosition(Vector3(5, 60, 60));
    //m_pCamera->lookAt(Vector3(5, 20, 0));
    m_pCamera->setNearClipDistance(1);

    m_pCamera->setPosition(MAP_SIZE/2, MAP_SIZE, MAP_SIZE/2);
    m_pCamera->lookAt(MAP_SIZE/2, 0, MAP_SIZE/2);
    m_pCamera->setFarClipDistance(MAP_SIZE*4);

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
            site0 = Vector3((float)it->first.x() / MAP_MIN_RES, (float)it->second.getHeight(recursion) / MAP_MIN_RES +1, (float)it->first.y() / MAP_MIN_RES);
            if(it->second.numCorner(recursion) > 0){
                for(auto itSite = it->second.beginSite(recursion); itSite != it->second.endSite(recursion); ++itSite){
                    site1 = Vector3((float)itSite->x() / MAP_MIN_RES, (float)data.find(*itSite)->second.getHeight(recursion) / MAP_MIN_RES +1, (float)itSite->y() / MAP_MIN_RES);
                    drawUniqueLine(manual_lines, ColourValue(0.1,0.1,0), site0, site1);
                }
            }
        }
    }
    cout << "...done\n";
}

void DebugState::viewCell2(ManualObject* manual_lines){
    cout << "DebugState::viewCell2....\n";

    const int recursion = 0;
    Vector3 corner, previousCorner, firstCorner;
    bool started;
    for(auto it = data.MapContainer.begin(); it != data.MapContainer.end(); ++it){
        if(it->second.type == TYPE_SITE and it->second.minRecursion <= recursion){
            if(it->second.numCorner(recursion) > 0){
                started = false;
                for(auto itCorner = it->second.beginCorner(recursion); itCorner != it->second.endCorner(recursion); ++itCorner){
                    corner = Vector3((float)itCorner->x() / MAP_MIN_RES, 1, (float)itCorner->y() / MAP_MIN_RES);
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


void DebugState::addLines(const String name, void(DebugState::*p_function)(ManualObject*)){
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
}

void DebugState::addPlanes(const String name, Point bl, Point tr){
    int vertexCount = 0;
    const int recursion = data.maxRecursion;
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
    for(auto it = data.MapContainer.begin(); it != data.MapContainer.end(); ++it){
        if(it->second.type == TYPE_SITE and it->second.minRecursion <= recursion and 
                it->first.x() >= bl.x() and it->first.x() < tr.x() and it->first.y() >= bl.y() and it->first.y() < tr.y()){
            if(it->second.numCorner(recursion) > 0){
                site = Vector3((float)it->first.x() / MAP_MIN_RES, (float)it->second.getHeight(recursion) / MAP_MIN_RES, (float)it->first.y() / MAP_MIN_RES);
                manual_planes->position(site);
                manual_planes->colour(landColour(site.y, site.y, 1));
                manual_planes->normal(0,1,0);
                cornerCount = 0;
                for(auto itCorner = it->second.beginCorner(recursion); itCorner != it->second.endCorner(recursion); ++itCorner){
                    corner = Vector3((float)itCorner->x() / MAP_MIN_RES, (float)data.find(*itCorner)->second.getHeight(recursion) / MAP_MIN_RES, 
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

    /*manual_planes->convertToMesh("test.mesh.planes");
    Entity *ent1 = m_pSceneMgr->createEntity("planes", "test.mesh.planes");
    m_pSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent1);
    
    //m_pSceneMgr->destroyAllStaticGeometry();
    Ogre::StaticGeometry *sg = m_pSceneMgr->createStaticGeometry("WorldMapArea");

    sg->setRegionDimensions(Ogre::Vector3(MAP_SIZE, MAP_SIZE, MAP_SIZE));
    sg->setOrigin(Ogre::Vector3(MAP_SIZE/2, 0, MAP_SIZE/2));

    Ogre::Quaternion rot0;
    rot0.FromAngleAxis(Ogre::Degree(0), Ogre::Vector3::UNIT_Y);
    sg->addEntity(ent1, Ogre::Vector3(0,0,0), rot0, Ogre::Vector3(1,1,1));

    sg->build();*/
}

Ogre::ColourValue DebugState::landColour(float siteHeight, float height, float gradient){
    if(siteHeight == 0){
        return Ogre::ColourValue(0, 0.2, 0.6);
    }
    float red = height / 8;
    float green = 0.8 / (height +1);
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

    addLines("screenOutline", &DebugState::viewBox);
    addLines("cells", &DebugState::viewCell);
    addLines("cells2", &DebugState::viewCell2);
    //addPlanes("planes");
    //addPlanes("planes");
    

    return;
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
        //m_bSettingsMode = !m_bSettingsMode;
        //return true;
        //Entity* line = m_pSceneMgr->getRootSceneNode()->getEntity("line");
        ManualObject* lines = m_pSceneMgr->getManualObject("screenOutline");
        lines->setVisible(!lines->getVisible());
        lines = m_pSceneMgr->getManualObject("cells");
        lines->setVisible(!lines->getVisible());
        lines = m_pSceneMgr->getManualObject("cells2");
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
    if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_LSHIFT))
        m_pCamera->moveRelative(m_TranslateVector);
    m_pCamera->moveRelative(m_TranslateVector / 10);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void DebugState::getInput()
{
    if(m_bSettingsMode == false)
    {
        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_A)){
            m_TranslateVector.x = -m_MoveScale;
            hasMoved = true;
        }

        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_D)){
            m_TranslateVector.x = m_MoveScale;
            hasMoved = true;
        }

        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_E)){
            m_TranslateVector.y = -m_MoveScale;
            hasMoved = true;
        }

        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_Q)){
            m_TranslateVector.y = m_MoveScale;
            hasMoved = true;
        }

        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_W)){
            m_TranslateVector.z = -m_MoveScale;
            hasMoved = true;
        }

        if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_S)){
            m_TranslateVector.z = m_MoveScale;
            hasMoved = true;
        }

    }
}

//|||||||||||||||||||||||||||||||||||||||||||||||

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

    m_MoveScale = m_MoveSpeed   * timeSinceLastFrame * (float)MAP_SIZE / 1000.0;
    m_RotScale  = m_RotateSpeed * timeSinceLastFrame;

    m_TranslateVector = Vector3::ZERO;

    getInput();
    generateScenery();
    moveCamera();
}

//struct tile{
//    int     index;
//    int     recursion;
//    int     size;    
//}

#define MAP_TILES           16
#define MAP_TILE_SIZE       MAP_SIZE / MAP_TILES

void DebugState::generateScenery(){
    //static struct tile tiles[MAP_TILES * MAP_TILES];
    static std::unordered_set<int> todoTiles;
    if(hasMoved){
        hasMoved = false;
        int zoom;
        if(m_pCamera->getDerivedPosition().y >= 1){
            zoom = MAP_SIZE / (int)m_pCamera->getDerivedPosition().y;
        }
        if(zoom <= 0) zoom = 1;
        cout << zoom << "\n";

        float lowX, lowY, highX, highY;
        calculateViewedMap(lowX, lowY, highX, highY);

       for(int xTile = 0; xTile < MAP_TILES; ++xTile){
           for(int yTile = 0; yTile < MAP_TILES; ++yTile){
               if(lowX <= (xTile +1) * MAP_TILE_SIZE and highX > xTile * MAP_TILE_SIZE and
                        lowY <= (yTile +1) * MAP_TILE_SIZE and highY > yTile * MAP_TILE_SIZE){
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
        addPlanes("tile" + std::to_string(working), Point((working % MAP_TILES) * MAP_TILE_SIZE * MAP_MIN_RES, (working / MAP_TILES) * MAP_TILE_SIZE * MAP_MIN_RES),
                          Point((working % MAP_TILES +1) * MAP_TILE_SIZE * MAP_MIN_RES, (working / MAP_TILES +1) * MAP_TILE_SIZE * MAP_MIN_RES));
        //cout << (working % MAP_TILES) * MAP_TILE_SIZE * MAP_MIN_RES << "," << (working / MAP_TILES) * MAP_TILE_SIZE * MAP_MIN_RES << "\t" 
        //    << (working % MAP_TILES  +1) * MAP_TILE_SIZE * MAP_MIN_RES << "," << (working / MAP_TILES +1) * MAP_TILE_SIZE * MAP_MIN_RES << "\n";
    }
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void DebugState::buildGUI()
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

void DebugState::itemSelected(OgreBites::SelectMenu* menu)
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
