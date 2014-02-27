//|||||||||||||||||||||||||||||||||||||||||||||||
#include <OgreManualObject.h>
#include <OgreMesh.h>
#include <OgreSceneNode.h>
#include <OgreStaticGeometry.h>
#include <OgreTechnique.h>

#include "GameState.hpp"
#include <utility>      // std::pair, std::make_pair

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

void GameState::enter()
{
    OgreFramework::getSingletonPtr()->m_pLog->logMessage("Entering GameState...");

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

void GameState::createScene()
{
    Ogre::Light* directionalLight = m_pSceneMgr->createLight("Light");
    directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
    directionalLight->setDiffuseColour(Ogre::ColourValue(.8, .8, .8));
    directionalLight->setSpecularColour(Ogre::ColourValue(.8, .8, .8));

    directionalLight->setDirection(Ogre::Vector3( 0, -1, 0));

    //m_pSceneMgr->createLight("Light")->setPosition(75,75,75);


    initMaterial();
    
    ManualObject* manual_planes = m_pSceneMgr->createManualObject("MapPoly");
    ManualObject* manual_lines = m_pSceneMgr->createManualObject("MapLine");
    generateScenery(manual_planes, manual_lines);
   
    manual_planes->convertToMesh("test.mesh.planes");
    Entity *ent1 = m_pSceneMgr->createEntity("planes", "test.mesh.planes");
    cout << "manual_planes size: " << ent1->getMesh()->getSize() << "\n";
//    m_pSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent1);

    if(manual_lines->getNumSections() > 0){
        manual_lines->convertToMesh("test.mesh.lines");
        Entity *ent2 = m_pSceneMgr->createEntity("lines", "test.mesh.lines");
        m_pSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent2);
        ent2->setVisible(false);
        cout << "manual_lines size: " << ent2->getMesh()->getSize() << "\n";
    }


//    return;


    // Create Static Geometry.
    m_pSceneMgr->destroyAllStaticGeometry();
    Ogre::StaticGeometry *sg = m_pSceneMgr->createStaticGeometry("WorldMapArea");

    sg->setRegionDimensions(Ogre::Vector3(MAP_SIZE, MAP_SIZE, MAP_SIZE));
    sg->setOrigin(Ogre::Vector3(MAP_SIZE/2, 0, MAP_SIZE/2));

    Ogre::Quaternion rot0;
    rot0.FromAngleAxis(Ogre::Degree(0), Ogre::Vector3::UNIT_Y);
    sg->addEntity(ent1, Ogre::Vector3(0,0,0), rot0, Ogre::Vector3(1,1,1));

    sg->build();

}

void GameState::initMaterial(void){
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

void GameState::generateScenery(ManualObject* manual_planes, ManualObject* manual_lines){
    manual_planes->begin("SolidColour", RenderOperation::OT_TRIANGLE_LIST);
    manual_lines->begin("SolidColour", Ogre::RenderOperation::OT_LINE_LIST);

    /*int count = 0;
    for (auto it = data.MapContainer.begin(); it != data.MapContainer.end(); ++it){
        if(it->second.type == TYPE_SITE){
            // Only do this stuff for sites. (We'd get it twice if we did it for corners as well.)
            ++count;
            if(count > 500000){
                cout << "clearing\n";
                count = 0;
                manual_lines->end();
                manual_lines->clear();
                manual_lines->begin("SolidColour", Ogre::RenderOperation::OT_LINE_LIST);

                //break;
            }
            drawHill(manual_planes, manual_lines, &(it->second));
        }
    }*/
    std::cout << "done drawHill\n";
    manual_planes->end();
    manual_lines->end();
    //std::cout << count << " hills.\n";
    std::cout << manual_lines->getNumSections() << " manual_lines\n";
    std::cout << manual_planes->getNumSections() << " manual_planes\n";
}


Ogre::ColourValue GameState::colour(int cornerHeight, int terrain){
    //if(terrain >= TERRAIN_LAND or terrain == TERRAIN_UNDEFINED or terrain == TERRAIN_SHORE){
        cornerHeight = cornerHeight * 4000 / MAP_SIZE;
        if(cornerHeight <= 0) cornerHeight = 1;
        return Ogre::ColourValue((float)cornerHeight / 50.0, 0.8 / cornerHeight, 0);  // green
    //} else if(terrain == TERRAIN_SHORE){
    //    cornerHeight = cornerHeight * 1000 / MAP_SIZE;
    //    return Ogre::ColourValue(0.5, 0, 0);
    //}
    return Ogre::ColourValue(0, 0.1, 0.4);  // blue
}


void GameState::drawHill(Ogre::ManualObject* manual_planes, Ogre::ManualObject* manual_lines, MapNode* centre){
}

// comprison for drawline().
// http://ideone.com/AQfmO1
bool operator < (const std::pair<Ogre::Vector3,Ogre::Vector3> &l, const std::pair<Ogre::Vector3,Ogre::Vector3> &r){
    if(max(l.first.x, l.second.x) == max(r.first.x, r.second.x)){
        if(min(l.first.x, l.second.x) == min(r.first.x, r.second.x)){
            if(max(l.first.y, l.second.y) == max(r.first.y, r.second.y)){
                return min(l.first.y, l.second.y) > min(r.first.y, r.second.y);
            } else {
                return max(l.first.y, l.second.y) > max(r.first.y, r.second.y);
            }
        } else {
            return min(l.first.x, l.second.x) > min(r.first.x, r.second.x);
        }
    } else {
        return max(l.first.x, l.second.x) > max(r.first.x, r.second.x);
    }
}

void GameState::drawLine(Ogre::ManualObject* mo, Ogre::Vector3 pointA, Ogre::Vector3 pointB){
    mo->position(Ogre::Vector3(pointA.x, pointA.y + 10, pointA.z));
    mo->position(Ogre::Vector3(pointB.x, pointB.y + 10, pointB.z));
}


void GameState::calculateViewedMap(float& lowX, float& lowY, float& highX, float& highY){
    // This plane sits 1/10th of the way from the camera to the surface because the full distance is out of range of the getCameraToViewportRay calculations.
    Plane worldPlane(Ogre::Vector3::UNIT_Y, 900 * Ogre::Vector3(0,m_pCamera->getPosition().y / 1000 ,0));

    Ogre::Ray rayCentre, rayTL, rayBR;
    m_pCamera->getCameraToViewportRay(0.5, 0.5, &rayCentre);
    m_pCamera->getCameraToViewportRay(0, 1, &rayTL);
    m_pCamera->getCameraToViewportRay(1, 0, &rayBR);

    std::pair<bool, Ogre::Real > resultCentre, resultTL, resultBR;
    resultCentre = rayCentre.intersects(worldPlane);
    resultTL = rayTL.intersects(worldPlane);
    resultBR = rayBR.intersects(worldPlane);


    if(resultCentre.first and resultTL.first and resultBR.first){
        //we intersect with the plane
        Ogre::Vector3 pCentre = rayCentre.getPoint(resultCentre.second);
        Ogre::Vector3 pTL = rayTL.getPoint(resultTL.second);
        Ogre::Vector3 pBR = rayBR.getPoint(resultBR.second);
        lowX = pCentre.x + ((pTL.x - pCentre.x) *10);
        lowY = pCentre.z + ((pTL.z - pCentre.z) *10);
        highX = pCentre.x + ((pBR.x - pCentre.x) *10);
        highY = pCentre.z + ((pBR.z - pCentre.z) *10);
        std::cout << "centre:\n";
        std::cout << pCentre.x << "\t\t" << pCentre.z << "\n";
        //std::cout << (int)lowX << "\t\t" << (int)lowY << "\n";
        //std::cout << (int)highX << "\t\t" << (int)highY << "\n\n";
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
        float lowX, lowY, highX, highY;
        calculateViewedMap(lowX, lowY, highX, highY);
        //data.Section(lowX, lowY, highX, highY);
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
