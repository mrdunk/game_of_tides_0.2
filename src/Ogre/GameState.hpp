//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "AppState.hpp"
#include "../data/data.h"
#include "../data/boats.h"

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

    void sliderMoved(OgreBites::Slider* slider);

	void update(double timeSinceLastFrame);

    void initMaterial(void);
    void viewBox(Ogre::ManualObject* manual_lines);
    void viewBoatLines(Ogre::ManualObject* manual_lines);
    void viewBoatPlanes(Ogre::ManualObject* manual_planes);
    Ogre::ManualObject* addLines(const Ogre::String name, void(GameState::*p_function)(Ogre::ManualObject*));
    Ogre::ManualObject* addPlanes(const Ogre::String name, void(GameState::*p_function)(Ogre::ManualObject*));
    void drawLine(Ogre::ManualObject* mo, Ogre::ColourValue colour, Ogre::Vector3 pointA, Ogre::Vector3 pointB);
    void drawBoat(void);

    Vessel boat;

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
