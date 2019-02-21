
#pragma once

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <sys/time.h>
#include <algorithm>


#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <vector>
// Include GLM
#undef countof
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#undef countof
#include "Camera.h" // a base line camera
#include "Graphics.h"
#include "ObjectModel.h"
#include "MyFiles.h"
#include "ShaderManager.h"
#include "TextureManager.h"

#include "tiny_obj_loader.h"
#include "ModelManager.h"
#include "PhysicsObj.h"
#include "Rays.h"
#include "Font3D.h"
#include "CollisionProcess.h"
#include "EnemyManager.h"


// these are the minimum contents of a game class
class Game
{
public:
	Game();
	~Game();
	
    Graphics*     pMyGraphics;
    MyFiles*      pHandler;
    ModelManager* pMainModelManager; // all models are loaded and info stored here this managter is passed to objects created
    Camera*	      pTheCamera;
    EnemyManager* pEManager;
    Font3D*       pFont;

    Camera* GetCamera() {
        return pTheCamera;
	}
	
    bool Init();
    bool Update(float deltaTime);
    void Draw(Graphics::Target_State *p_state);
	
    std::vector<ObjectModel*> MyObjects;
    uint32_t ScreenX, ScreenY;
    float DeltaTimePassed;
		
// Physics stuff
// We need a simple means to create the physics data
	void ShutDown();
    PhysicsObj* CreatePhysicsObj(btCollisionShape* pShape,
                                 const float &mass,
                                 const btVector3 &Position = btVector3(0, 0, 0),
                                 const btQuaternion &Rotation = btQuaternion(0, 0, 0, 1));// default no rotation using w 1
// Pointers for the world and other important things
	btBroadphaseInterface* BroadPhase;
	btDefaultCollisionConfiguration* CollisionConfiguration;
	btCollisionDispatcher* Dispatcher;
	btSequentialImpulseConstraintSolver* ConstraintSolver;
	btDiscreteDynamicsWorld* DynamicPhysicsWorld;
	PhysicsDraw* m_pPhysicsDrawer;
// We don't want to have multiple collision shapes defined
// for the same objects, so we will associate their file
// names and if already created we won't make another;
	std::map<std::string, btCollisionShape*> ExistingColShapes;
	CollisionProcess Collision;

protected:
    Rays RayClass;
};




