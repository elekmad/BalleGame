/*
 * BallGame.h
 *
 *  Created on: 15 nov. 2020
 *      Author: damien
 */

#ifndef BALLGAME_H_
#define BALLGAME_H_

#include <Ogre.h>
#include <OgreRTShaderSystem.h>
#include <OgreMath.h>
#include <iostream>
#include "BaseApplication.h"


#include <Newton.h>
#include <toolbox_stdafx.h>
#include <dHighResolutionTimer.h>
#include <DebugDisplay.h>

#include <dList.h>
#include <dMatrix.h>

//because of Xlib defines (True False Bool None) which must be undef
#undef True
#undef False
#undef None
#undef Bool
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/Ogre/Renderer.h>

#define LEVELS_FOLDER "./Levels/"
#define LEVELS_EXTENSION "json"
#define STATES_EXTENSION "state"

using namespace Ogre;
using namespace OgreBites;

#define MAX_PHYSICS_FPS				240.0f
#define MAX_PHYSICS_SUB_STEPS		50
//#define PROJECTILE_INITIAL_SPEED	20.0f

#ifdef OGRE_STATIC_LIB
#  define OGRE_STATIC_GL
#  if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#    define OGRE_STATIC_Direct3D9
// dx10 will only work on vista, so be careful about statically linking
#    if OGRE_USE_D3D10
#      define OGRE_STATIC_Direct3D10
#    endif
#  endif
#  define OGRE_STATIC_BSPSceneManager
#  define OGRE_STATIC_ParticleFX
#  define OGRE_STATIC_CgProgramManager
#  ifdef OGRE_USE_PCZ
#    define OGRE_STATIC_PCZSceneManager
#    define OGRE_STATIC_OctreeZone
#  else
#    define OGRE_STATIC_OctreeSceneManager
#  endif
#  include "OgreStaticPluginLoader.h"
#endif

class BallGame;
class GroupEntity;

enum BallGameEntityType
{
	Case,
	Ball
};

class BallGameEntity
{
	public :

	BallGameEntity(const dMatrix& matrix);
	BallGameEntity();
	~BallGameEntity(){}
	void Finalize(void);
    static void TransformCallback(const NewtonBody* body, const dFloat* matrix, int threadIndex);
    void ExportToJson(rapidjson::Value &v, rapidjson::Document::AllocatorType& allocator);
    void ImportFromJson(rapidjson::Value &v, Ogre::SceneManager* mSceneMgr, Node *parent = NULL);
    void setOgreNode(SceneNode *node);
    void setNewtonBody(NewtonBody *body);
    const NewtonBody *getNewtonBody(void) const { return Body; }
    dMatrix *PrepareNewtonBody(dVector &NewtonBodyLocation, dVector &NewtonBodySize);
    void DisplaySelectedBox(bool display);
    const Ogre::Vector3 &getInitialPosition(void) const { return InitialPos; }
    const Ogre::Vector3 &getRelaticePosition(void) const { return OgreEntity->getPosition(); }
    const Ogre::Vector3 &getAbsolutePosition(void) const { return OgreEntity->_getDerivedPosition(); }
    void setInitialPosition(const Ogre::Vector3 &NewPosition) { InitialPos = NewPosition; }
    void setRelaticePosition(const Ogre::Vector3 &NewPosition) { OgreEntity->setPosition(NewPosition); }
    void setAbsolutePosition(const Ogre::Vector3 &NewPosition) { OgreEntity->_setDerivedPosition(NewPosition); }
    const Ogre::Quaternion &getInitialOrientation(void) const { return InitialOrientation; }
    const Ogre::Quaternion &getRelaticeOrientation(void) const { return OgreEntity->getOrientation(); }
    const Ogre::Quaternion &getAbsoluteOrientation(void) const { return OgreEntity->_getDerivedOrientation(); }
    void setInitialOrientation(const Ogre::Quaternion &NewOrient) { InitialOrientation = NewOrient; }
    void setRelaticeOrientation(const Ogre::Quaternion &NewOrient) { OgreEntity->setOrientation(NewOrient); }
    void setAbsoluteOrientation(const Ogre::Quaternion &NewOrient) { OgreEntity->_setDerivedOrientation(NewOrient); }
    const Ogre::Vector3 &getInitialScale(void) const { return InitialScale; }
    const Ogre::Vector3 &getRelaticeScale(void) const { return OgreEntity->getScale(); }
    const Ogre::Vector3 &getAbsoluteScale(void) const { return OgreEntity->_getDerivedScale(); }
    void setInitialScale(const Ogre::Vector3 &NewScale) { InitialScale = NewScale; }
    void setRelaticeScale(const Ogre::Vector3 &NewScale) { OgreEntity->setScale(NewScale); }
    const Ogre::String &getName(void) const { return OgreEntity->getName(); }
    void Move(float x, float y, float z);
    void Rotate(float x, float y, float z);
    void Scale(float x, float y, float z);
    void getVelocity(dFloat *Velocity) { NewtonBodyGetVelocity(Body, Velocity); }
    enum BallGameEntityType getType(void) { return type; }
    GroupEntity *getGroup(void) { return Group; }

	protected :

    enum BallGameEntityType type;
	//mutable dMatrix m_matrix;			// interpolated matrix
	dVector m_curPosition;				// position one physics simulation step in the future
	dVector m_nextPosition;             // position at the current physics simulation step
	dQuaternion m_curRotation;          // rotation one physics simulation step in the future
	dQuaternion m_nextRotation;         // rotation at the current physics simulation step
	SceneNode *OgreEntity;
	NewtonBody *Body;
	GroupEntity *Group;
	Ogre::Vector3 InitialPos;
	Ogre::Vector3 InitialScale;
	Ogre::Quaternion InitialOrientation;

    void SetMatrixUsafe(const dQuaternion& rotation, const dVector& position);

	friend class GroupEntity;
};

class BallEntity : public BallGameEntity
{
	public :

	BallEntity(const dMatrix& matrix);
	BallEntity();
	void CreateFromJson(rapidjson::Value &v, Ogre::SceneManager* mSceneMgr, NewtonWorld *m_world, Node *parent = NULL);
	void AddForceVector(dVector *force);
    void ExportToJson(rapidjson::Value &v, rapidjson::Document::AllocatorType& allocator);
    void ImportFromJson(rapidjson::Value &v, Ogre::SceneManager* mSceneMgr, Node *parent = NULL);
	dVector *GetForceVector();
	void CleanupForces(void);
    void CreateNewtonBody(NewtonWorld *m_world);
    dFloat getMass(void);
    void setMass(dFloat newMass);
    float getInitialMass(void) { return InitialMass; }
    void setInitialMass(float newInitialMass) { InitialMass = newInitialMass; }

	protected :

	dList<dVector*> Forces;
	float InitialMass;
};

class CaseEntity : public BallGameEntity
{
	public :

	enum CaseType
	{
		typeBox = 0,
		typeRamp = 1
	};
	enum CaseType type;
	CaseEntity(const dMatrix& matrix, enum CaseType _type = typeBox);
	CaseEntity(enum CaseType _type = typeBox);
	void CreateFromJson(rapidjson::Value &v, Ogre::SceneManager* mSceneMgr, NewtonWorld *m_world, Node *parent = NULL);
//	void AddBallColliding(NewtonBody *ball);
//	bool CheckIfAlreadyColliding(NewtonBody *ball);
	void SetForceToApply(float force, dVector *direction);
	void ApplyForceOnBall(BallEntity *ball);
    void ExportToJson(rapidjson::Value &v, rapidjson::Document::AllocatorType& allocator);
    void ImportFromJson(rapidjson::Value &v, Ogre::SceneManager* mSceneMgr, Node *parent = NULL);
    void CreateNewtonBody(NewtonWorld *m_world);
    float getForce(void) { return force_to_apply; }
    const dVector *getForceDirection(void) { return force_direction; }
    void AddBallColliding(BallEntity *ball);
	void ApplyForceOnCollidingBalls(void);

	protected :

    bool CheckIfAlreadyColliding(BallEntity *ball);

	std::list<BallEntity*> BallsUnderCollide;

	float force_to_apply;
	dVector *force_direction;
};

class GroupEntity
{
	public :

	GroupEntity(String &name, Ogre::SceneManager* mSceneMgr);
	~GroupEntity(){};
	void Finalize(void);
	void AddChild(BallGameEntity* child);
	bool DelChild(BallGameEntity* child);
	void ComputeChilds(void);
	void FillListWithChilds(std::list<BallGameEntity*> &list);
    void Move(float x, float y, float z);
    void Rotate(float x, float y, float z);
    void Scale(float x, float y, float z);

	private :

	SceneNode *OgreEntity;
	std::list<BallGameEntity*> childs;
};

class BallGame : public BaseApplication
{
    public :

    BallGame();
    ~BallGame();

    private :

    class EntityType
	{
	public :

    	EntityType(){ InitialMass = 0.0; Type= Case; }
    	~EntityType(){}
		String Name;
		enum BallGameEntityType Type;
		String MeshName;
		Vector3 InitialPos;
		Vector3 InitialScale;
		Quaternion InitialOrientation;
		float InitialMass;
	};

    std::list<class EntityType*> EntityTypes;

    void LoadBallGameEntityTypes(void);

    void Append(BallGameEntity *entity);
    void _StartPhysic(void);
    void _StopPhysic(void);
    void SwitchEditMode(void);
    void DeleteCase(CaseEntity *Entity, std::list<CaseEntity*>::iterator *iter = NULL);
    void RemoveCase(CaseEntity *Entity, std::list<CaseEntity*>::iterator *iter = NULL);
    void DeleteBall(BallEntity *Entity, std::list<BallEntity*>::iterator *iter = NULL);
    void RemoveBall(BallEntity *Entity, std::list<BallEntity*>::iterator *iter = NULL);
    void DeleteGroup(GroupEntity *Entity, std::list<GroupEntity*>::iterator *iter = NULL);
    void RemoveGroup(GroupEntity *Entity, std::list<GroupEntity*>::iterator *iter = NULL);
    void LoadStatesList(void);
    void EmptyLevel(void);//Clean all BallGame, Newton and Ogre entities to start with new level.
    void ChangeLevel(void);
    void ImportLevelFromJson(Node *parent = NULL);

    String Level;
    void SetLevel(String &level_name);

    //Place New Element
    enum PlacementModes
	{
    	PlaceMove,
		PlaceRotate,
		PlaceScale,
    	EditMove,
		EditRotate,
		EditScale,
		Delete
	}PlacementMode;

	EntityType *ToBePlacedEntityType;
	SceneNode *ogreThumbnailNode;
    BallGameEntity *ToBePlacedEntity;
    BallGameEntity *LastPlacedEntity;
    BallGameEntity *ToBeDeletedEntity;
    void PlaceNewElement(void);
    void PlaceUnderEditElement(void);
    void PlaceElement(BallGameEntity *ToBePlacedEntity);
    void PrepareNewElement(void);
    inline void UnprepareNewElement(void);
    void DeleteElement(void);
    inline void PrepareDeleteElement(BallGameEntity *Entity);
    inline void UnprepareDeleteElement(void);

    //Edit Entities
    bool MultiSelectionMode;
    void MoveEntities(float x, float y, float z);
    void RotateEntities(float x, float y, float z);
    void ScaleEntities(float x, float y, float z);
    void MultiSelectionSetEmpty(void);
    bool ManageMultiSelectionSet(BallGameEntity *entity);
    std::list<class BallGameEntity*> UnderEditEntites;
    //Edit Ball
    void EditBall(BallEntity *Entity);
    BallEntity *UnderEditBall;
    dFloat UnderEditBallMass;

    //Edit Case
    bool MouseOverButton;
    void EditCase(CaseEntity *Entity);
	CaseEntity *UnderEditCase;
	bool CaseHasForce;
	float UnderEditCaseForce;
	bool force_directed;
	dVector force_direction;
    void UpdateEditButtons(void);

    enum RunningMode
    {
		Running,
		Editing
    }mode;
    /////////////////////  NEWTON ///////////////////
    public :

    NewtonWorld* m_world;
    static void PostUpdateCallback(const NewtonWorld* const world, dFloat timestep);
    static void OnContactCollision (const NewtonJoint* contactJoint, dFloat timestep, int threadIndex);
    void UpdatePhysics(dFloat timestep);

    private :

    void SetupNewton(void);
    NewtonWorld* GetNewton(void);
    static void BodySerialization (NewtonBody* const body, void* const bodyUserData, NewtonSerializeCallback serializeCallback, void* const serializeHandle);
    static void BodyDeserialization (NewtonBody* const body, void* const bodyUserData, NewtonDeserializeCallback deserializecallback, void* const serializeHandle);
    void SerializedPhysicScene(const String* const name);
    void DeserializedPhysicScene(const String* const name);
    BallGameEntity *GetEntity(char *name);
    bool CheckIfAlreadyColliding(CaseEntity *ToCheck);
    void AddCaseColliding(CaseEntity *ToAdd);

    bool m_suspendPhysicsUpdate;
    unsigned64 m_microsecunds;
    int m_physicsFramesCount;
    dFloat m_mainThreadPhysicsTime;
    dFloat m_mainThreadPhysicsTimeAcc;
    bool m_asynchronousPhysicsUpdate;

    long nb_entities;
    std::list<CaseEntity*> Cases;
    std::list<BallEntity*> Balls;
    std::list<GroupEntity*> Groups;
	std::list<CaseEntity*> CasesUnderCollide;


    void CheckforCollides(void);
    void AddCase(CaseEntity *Entity);
    void AddBall(BallEntity *Entity);
    void AddGroup(GroupEntity *Entity);

    /////////////////////////////////////////////////


    ////////////////  Ogre ////////////////////////////
    public :

    bool keyPressed( const OIS::KeyEvent &arg );
    bool keyReleased( const OIS::KeyEvent &arg );
	bool mouseMoved( const OIS::MouseEvent &arg );
    bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

    private :

	void createScene(void);
    void SetupGame(void);

    //Mouse picking
	BallGameEntity *LastHighligted;
    //////////////////////////////////////////////////


	/////////////////// CE GUI ////////////////////////

    private :

    void SetupGUI(void);
    bool EnteringArea(const CEGUI::EventArgs &e);
    bool LeavingArea(const CEGUI::EventArgs &e);
    template<typename T> T*CreateNewGUIComponent(std::string &TypeName, std::string &Name = "");
    template<typename T> T*CreateNewGUIComponent(const char *TypeName, const char *Name = "");

    CEGUI::OgreRenderer* mRenderer;
    CEGUI::Window *ThumbnailWindow;

    CEGUI::LayoutContainer* MainLayout;

    CEGUI::PushButton *StopPhysicPushB;
    bool StopPhysicPushBCallback(const CEGUI::EventArgs &e);
    CEGUI::PushButton *EditModePushB;
    bool EditModePushBCallback(const CEGUI::EventArgs &e);
    CEGUI::PushButton *StatesModePushB;
    bool StatesModePushBCallback(const CEGUI::EventArgs &e);
    CEGUI::Combobox *ChooseLevelComboB;
    bool ChooseLevelComboBCallback(const CEGUI::EventArgs &e);
    void CreateThumbnail(String meshname);
    CEGUI::Editbox *NewLevelEditB;
//    bool NewLevelEditBCallback(const CEGUI::EventArgs &e);
    CEGUI::PushButton *NewLevelCreateB;
    bool NewLevelCreateBCallback(const CEGUI::EventArgs &e);
    CEGUI::PushButton *SaveLevelPushB;
    bool SaveLevelPushBCallback(const CEGUI::EventArgs &e);
    CEGUI::PushButton *QuitPushB;
    bool QuitPushBCallback(const CEGUI::EventArgs &e);

    //Edit buttons

    CEGUI::Titlebar *EditingModeTitleBanner;
    CEGUI::Titlebar *LevelNameBanner;
    CEGUI::Titlebar *StatesBanner;
    CEGUI::PushButton *SaveStatePushB;
    bool SaveStatePushBCallback(const CEGUI::EventArgs &e);
    CEGUI::Combobox *ChooseStateToLoadB;
    bool ChooseStateToLoadBCallback(const CEGUI::EventArgs &e);
    CEGUI::PushButton *LoadStatePushB;
    bool LoadStatePushBCallback(const CEGUI::EventArgs &e);
    CEGUI::PushButton *DelStatePushB;
    bool DelStatePushBCallback(const CEGUI::EventArgs &e);

    //Add new elements Buttons & Callbacks
    CEGUI::Titlebar *AddElementTitleBanner;
    CEGUI::Combobox *ChooseTypeOfElementToAddB;
    bool ChooseTypeOfElementToAddBCallback(const CEGUI::EventArgs &e);
    CEGUI::PushButton *PlaceNewElementB;
    bool PlaceNewElementBCallback(const CEGUI::EventArgs &e);
    CEGUI::PushButton *EditElementB;
    bool EditElementBCallback(const CEGUI::EventArgs &e);
    CEGUI::PushButton *DeleteElementB;
    bool DeleteElementBCallback(const CEGUI::EventArgs &e);
    CEGUI::PushButton *MoveElementB;
    bool MoveElementBCallback(const CEGUI::EventArgs &e);
    void SetMoveNewElement(void);
    void SetMoveElement(void);
    CEGUI::PushButton *RotateElementB;
    bool RotateElementBCallback(const CEGUI::EventArgs &e);
    void SetRotateNewElement(void);
    void SetRotateElement(void);
    CEGUI::PushButton *ScaleElementB;
    bool ScaleElementBCallback(const CEGUI::EventArgs &e);
    void SetScaleNewElement(void);
    void SetScaleElement(void);
    CEGUI::ToggleButton *GroupElementsB;
    bool GroupElementsBCallback(const CEGUI::EventArgs &e);


    //Edit Ball Buttons & Callbacks
    CEGUI::Editbox *BallMassValueEditB;
    CEGUI::PushButton *ApplyMassChangesToBallPushB;
    bool ApplyMassChangesToBallPushBCallback(const CEGUI::EventArgs &event);

    //Edit Case Buttons & Callbacks
    CEGUI::ToggleButton *CaseHasForceToggleB;
    CEGUI::Editbox *CaseForceValueEditB;
    inline void CaseForceValueEditBSetText(float value);
    inline void CaseForceValueEditBSetText(double value);
    bool CaseForceValueEditBCallback(const CEGUI::EventArgs &event);
    CEGUI::ToggleButton *CaseHasForceDirectionToggleB;
    bool ToggleForceCallback(const CEGUI::EventArgs &e);
    bool ToggleForceDirectedCallback(const CEGUI::EventArgs &event);
    CEGUI::Editbox *CaseForceDirectionXValueEditB;
    CEGUI::Editbox *CaseForceDirectionYValueEditB;
    CEGUI::Editbox *CaseForceDirectionZValueEditB;
    CEGUI::PushButton *NormalizeCaseForceDirectionPushB;
    bool NormalizeCaseForceDirectionPushBCallback(const CEGUI::EventArgs &e);
    CEGUI::PushButton *ApplyForceChangesToCasePushB;
    bool ApplyForceChangesToCasePushBCallback(const CEGUI::EventArgs &event);


	//////////////////////////////////////////////////

    /////////////////// RapidJson ////////////////////

    void ExportLevelIntoJson(String &export_str);

    //////////////////////////////////////////////////

    virtual bool frameEnded(const Ogre::FrameEvent& fe);
    void SetCam(float x, float y, float z);
    void MoveCam(float x, float y, float z);
};


#endif /* BALLGAME_H_ */
