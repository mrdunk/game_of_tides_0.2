//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef DEBUG_STATE_HPP
#define DEBUG_STATE_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "AppState.hpp"
#include "../data/data.h"

#include <OgreSubEntity.h>
#include <OgreMaterialManager.h>

//|||||||||||||||||||||||||||||||||||||||||||||||

class DebugState : public AppState{
  public:
	DebugState();

	DECLARE_APPSTATE_CLASS(DebugState)

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
    void drawLine(Ogre::ManualObject* mo, Ogre::ColourValue colour, Ogre::Vector3 pointA, Ogre::Vector3 pointB);
    void drawUniqueLine(Ogre::ManualObject* mo, Ogre::ColourValue colour, Ogre::Vector3 pointA, Ogre::Vector3 pointB);
    void drawHill(Ogre::ManualObject* manual_planes, Ogre::ManualObject* manual_lines, MapNode* centre);
    Ogre::ColourValue colour(int cornerHeight, int terrain);

    void addLines(const Ogre::String name, void(DebugState::*p_function)(Ogre::ManualObject*));
    void viewBox(Ogre::ManualObject* manual_lines);
    void viewCell(Ogre::ManualObject* manual_lines);
    void viewCell2(Ogre::ManualObject* manual_lines);

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
