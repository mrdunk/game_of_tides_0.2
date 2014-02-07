//|||||||||||||||||||||||||||||||||||||||||||||||

#include "GameState.hpp"

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
    m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.7f, 0.7f, 0.7f));

    m_pSceneMgr->addRenderQueueListener(OgreFramework::getSingletonPtr()->m_pOverlaySystem);

    m_pRSQ = m_pSceneMgr->createRayQuery(Ray());
    m_pRSQ->setQueryMask(OGRE_HEAD_MASK);

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

    Data data;

    createScene(data);
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
    m_pSceneMgr->destroyQuery(m_pRSQ);
    if(m_pSceneMgr)
        OgreFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void GameState::createScene(Data& data)
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
    generateScenery(data, manual_planes, manual_lines);
   
    manual_planes->convertToMesh("test.mesh.planes");
    Entity *ent1 = m_pSceneMgr->createEntity("planes", "test.mesh.planes");
//    m_pSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent1);

    manual_lines->convertToMesh("test.mesh.lines");
    Entity *ent2 = m_pSceneMgr->createEntity("lines", "test.mesh.lines");
    m_pSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent2);

    ent2->setVisible(false);


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

void GameState::generateScenery(Data& data, ManualObject* manual_planes, ManualObject* manual_lines){
    manual_planes->begin("SolidColour", RenderOperation::OT_TRIANGLE_LIST);
    manual_lines->begin("SolidColour", Ogre::RenderOperation::OT_LINE_LIST);

    for (auto it = data.MapContainer.begin(); it != data.MapContainer.end(); ++it){
        if(it->second.type == TYPE_SITE){
            // Only do this stuff for sites. (We'd get it twice if we did it for corners as well.)

            drawHill(data, manual_planes, manual_lines, &(it->second));
        }
    }
    std::cout << "done drawHill\n";
    manual_planes->end();
    manual_lines->end();
}


Ogre::ColourValue GameState::colour(int height, int terrain){
    //if(height >= 0){
    if( terrain > TERRAIN_SEA){
        height = height * 1000 / MAP_SIZE;
        return Ogre::ColourValue((float)height / 50.0, 0.8 / (height +1), 0);  // green
    }
    return Ogre::ColourValue(0, 0.1, 0.4);  // blue
}


void GameState::drawHill(Data& data, Ogre::ManualObject* manual_planes, Ogre::ManualObject* manual_lines, MapSite* centre){
    if(centre->x < 0 or centre->y < 0 or centre->x > MAP_SIZE or centre->y > MAP_SIZE) return;
    static int vertexCount = 0;
    //if(vertexCount > 0) return;

    MapSite current_corner = {};
    Ogre::Vector3 current_corner_v(0,0,0);
    Ogre::Vector3 previous_corner_v(0,0,0);
    Ogre::Vector3 first_corner_v(0,0,0);
    Ogre::Vector3 normal;

    // http://www.ogre3d.org/forums/viewtopic.php?t=43030
    Ogre::Vector3 edge_a(centre->x, centre->height, centre->y);
    if(centre->height < 0) edge_a.y = -1;
    manual_planes->position(edge_a);
    manual_planes->colour(colour(centre->height, centre->terrain));
    manual_planes->normal(0,1,0);
    
    int cornerCount = 0;
    int recursion = 1;
    //if(centre->terrain == TERRAIN_SHORE or centre->recDepth == 1) recursion = 1;
    for (auto corner_it = centre->corner_begin(recursion); corner_it != centre->corner_end(recursion); ++corner_it){
        current_corner = data.MapContainer[*corner_it];
        current_corner_v = Ogre::Vector3(current_corner.x, 0, current_corner.y);

        if(current_corner_v.x > 0 and current_corner_v.z > 0 and current_corner_v.x < MAP_SIZE and current_corner_v.z < MAP_SIZE){

            // height is the average of all adgacent sites.
            int height = 0, heightCount = 0;
            for (auto adgSite_it = current_corner.site_begin(recursion); adgSite_it != current_corner.site_end(recursion); ++adgSite_it){
                height += data.MapContainer[*adgSite_it].height;
                ++heightCount;
            }
            height /= heightCount;
            if(height >= 0) current_corner_v.y = height;
            else current_corner_v.y = -1;

            // calculate normal for this point.
            Ogre::Vector3 dir0 = edge_a - current_corner_v;
            Ogre::Vector3 dir0_flat(dir0.x, 0, dir0.z);
            Ogre::Vector3 dir1 = Quaternion(Degree(90), Vector3::UNIT_Y) * dir0_flat;
            normal = dir0.crossProduct(dir1).normalisedCopy();

            manual_planes->position(current_corner_v);
            manual_planes->colour(colour(current_corner_v.y, centre->terrain));
            manual_planes->normal(normal);

            if(cornerCount > 0){
                manual_planes->triangle(vertexCount, vertexCount + cornerCount, vertexCount + cornerCount +1);
                drawLine(manual_lines, previous_corner_v, current_corner_v);
            }
            if(first_corner_v == Ogre::Vector3(0,0,0)){
                first_corner_v = current_corner_v;
            }

            previous_corner_v = current_corner_v;
            ++cornerCount;
        }
    }
    if(cornerCount > 1){
        manual_planes->triangle(vertexCount, vertexCount + cornerCount, vertexCount + 1);
    }

    vertexCount += cornerCount +1;
}


void GameState::drawLine(Ogre::ManualObject* mo, Ogre::Vector3 pointA, Ogre::Vector3 pointB){
    mo->position(Ogre::Vector3(pointA.x, 1000, pointA.z));
    mo->position(Ogre::Vector3(pointB.x, 1000, pointB.z));
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
