//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "AppState.hpp"
#include "../data/data.h"

#include <OgreSubEntity.h>
#include <OgreMaterialManager.h>

//|||||||||||||||||||||||||||||||||||||||||||||||

class GameState : public AppState
{
public:
	GameState();

	DECLARE_APPSTATE_CLASS(GameState)

	void enter();
	void createScene();
	void exit();
	bool pause();
	void resume();

	void moveCamera();
	void getInput();
    void buildGUI();

	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id);

    void itemSelected(OgreBites::SelectMenu* menu);

	void update(double timeSinceLastFrame);

    void initMaterial(void);
    void generateScenery(Ogre::ManualObject* manual_planes, Ogre::ManualObject* manual_lines);
    void drawLine(Ogre::ManualObject* mo, Ogre::Vector3 pointA, Ogre::Vector3 pointB);
    void drawHill(Ogre::ManualObject* manual_planes, Ogre::ManualObject* manual_lines, MapNode* centre);
    Ogre::ColourValue colour(int cornerHeight, int terrain);

    /* Calculate the area of the map currently under the viewport.
     * This assmess the cmaera is pointing straight down.
     */
    void calculateViewedMap(float& lowX, float& lowY, float& highX, float& highY);

private:
	Ogre::SceneNode*			m_pOgreHeadNode;
	Ogre::Entity*				m_pOgreHeadEntity;
	Ogre::MaterialPtr			m_pOgreHeadMat;
	Ogre::MaterialPtr			m_pOgreHeadMatHigh;

    OgreBites::ParamsPanel*		m_pDetailsPanel;
	bool						m_bQuit;

	Ogre::Vector3				m_TranslateVector;
	Ogre::Real					m_MoveSpeed;
	Ogre::Degree				m_RotateSpeed;
	float						m_MoveScale;
	Ogre::Degree				m_RotScale;

	Ogre::RaySceneQuery*		m_pRSQ;
	Ogre::SceneNode*			m_pCurrentObject;
	Ogre::Entity*				m_pCurrentEntity;
	bool						m_bLMouseDown, m_bRMouseDown;
	bool						m_bSettingsMode;

    MapData                     data;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||
