//=========================================================
// This is the game class, our project should run from here
//=========================================================

#include "Game.h"
#include "OBJModel.h"
#include "OffsetFollow.h"
#include "Player.h"
#include "SkyBox.h"

#include "Ground.h" // Needed to turn an environment into physics
#include "Font3D.h"

//#define SHOW_TEXT // Define this to see text in the scene

#define ShadowBufferSize 1024*2


//===============================
// Set up the OpenGL window stuff.
//===============================
Game::Game()
  : pMyGraphics(nullptr)
  , pFileHandler(nullptr)
  , pMainModelManager(nullptr)
  , pTheCamera(nullptr)
  , pFont(nullptr)
{
// Set up the OpenGL window
// and create a ShaderManager
    pMyGraphics = new Graphics();
}


Game::~Game() {
    for(uint i=0; i<MyObjects.size(); i++)	{
        if(MyObjects[i]) delete MyObjects[i];
    }
    MyObjects.clear();
    if(pTheCamera)        delete pTheCamera;
    if(pFont)             delete pFont;
    if(pFileHandler)      delete pFileHandler;
    if(pMainModelManager) delete pMainModelManager;
    if(pMyGraphics)       delete pMyGraphics;
}


bool
Game::Init() {
//==========================
// Create a new File Handler
//==========================
    pFileHandler = new MyFiles();

//===========================
// Create a new Model Manager
//===========================
    pMainModelManager = new ModelManager();

//=====================
// Create a new 3D Font
//=====================
    pFont = new Font3D();

//==================
// Not really needed
//==================
    MyObjects.clear();

//=================================
// Create a Player-Following Camera
//=================================
    float ratio = pMyGraphics->state.width/pMyGraphics->state.height;
    pTheCamera = new OffsetFollow(ratio);

//========================================
// Prepare for printing text on the screen
//========================================
    pFont->PO = pMyGraphics->
            OurShaderManager->
            MakeProgramObject("Resources/Shaders/textShader.vsh",
                              "Resources/Shaders/textShader.fsh",
                              pFileHandler);
    pFont->Init(pMyGraphics->state.width,
                pMyGraphics->state.height,
                "Resources/Fonts/atari-font.bmp");

//=======================
// Initialise our physics
//=======================

// btDbvtBroadphase() is a good general purpose broadphase.
// You can also try out btAxis3Sweep.
    BroadPhase             = new btDbvtBroadphase();

// CollisionConfiguration contains default setup for memory, collision setup.
// Advanced users can create their own configuration.
    CollisionConfiguration = new btDefaultCollisionConfiguration();

// Use the default collision dispatcher.
// For parallel processing you can use a diffent dispatcher
// (see Extras/BulletMultiThreaded)
    Dispatcher             = new btCollisionDispatcher(CollisionConfiguration);

// The default constraint solver.
// For parallel processing you can use a different solver
// (see Extras/BulletMultiThreaded)
    ConstraintSolver       = new btSequentialImpulseConstraintSolver();

//==================================================================
// DynamicPhysicsWorld() manages all physics objects and constraints
// It also implements the update of all objects each frame
//==================================================================
    DynamicPhysicsWorld    = new btDiscreteDynamicsWorld(Dispatcher,
                                                         BroadPhase,
                                                         ConstraintSolver,
                                                         CollisionConfiguration);

    DynamicPhysicsWorld->setGravity(btVector3(0.0f, -9.81f, 0.0f));
//=================================
// Done with physics initialization
//=================================

//===========================
// Prepare the wanted objects
//===========================
    InitObjects();

//================================================
// Objects can also cast shadows.
// So lets make sure we have that shader available
//================================================
    pMyGraphics->ShadowShader = pMyGraphics->
            OurShaderManager->
            MakeProgramObject("Resources/Shaders/ShadowMap.vsh",
                              "Resources/Shaders/ShadowMap.fsh",
                              pFileHandler);

//===================================================
// Make our frame buffer: let's make it a simple size
//===================================================
    pMyGraphics->ShadowFB = pMyGraphics->CreateShadowBuffer(ShadowBufferSize,
                                                            ShadowBufferSize);
//===============================
// Set up the debug drawer system
//===============================
    m_pPhysicsDrawer = new PhysicsDraw();
/*  enum	DebugDrawModes
    {
        DBG_NoDebug              = 0,
        DBG_DrawWireframe        = 1,
        DBG_DrawAabb             = 2,
        DBG_DrawFeaturesText     = 4,
        DBG_DrawContactPoints    = 8,
        DBG_NoDeactivation       = 16,
        DBG_NoHelpText           = 32,
        DBG_DrawText             = 64,
        DBG_ProfileTimings       = 128,
        DBG_EnableSatComparison  = 256,
        DBG_DisableBulletLCP     = 512,
        DBG_EnableCCD            = 1024,
        DBG_DrawConstraints      = (1 << 11),
        DBG_DrawConstraintLimits = (1 << 12),
        DBG_FastWireframe        = (1 << 13),
        DBG_DrawNormals          = (1 << 14),
        DBG_MAX_DEBUG_DRAW_MODE
    };*/
    m_pPhysicsDrawer->setDebugMode(8);
    DynamicPhysicsWorld->setDebugDrawer(m_pPhysicsDrawer);

//=========================================================================
// Create the simple shader for lines when we create our 1st physics object
//=========================================================================
    GLuint SimpleShader = pMyGraphics->
            OurShaderManager->
            MakeProgramObject("Resources/Shaders/SimpleTri.vsh",
                              "Resources/Shaders/SimpleTri.fsh",
                              pFileHandler);
    m_pPhysicsDrawer->ProgramObject = SimpleShader;

//=============================================
// World and Player set up, physics initialised
//=============================================
    return true;
}


bool
Game::Update(float deltaTime) {
//==========================================
// Give ojects access to the passage of time
//==========================================
    DeltaTimePassed = deltaTime;

//===============================================
// Get ready to cycle through any objects we made
//===============================================
    pTheCamera->Update(MyObjects[1]);// Player is number 1 Ground is 0
    DynamicPhysicsWorld->stepSimulation(1.0f/60.0f, 5);

// =====================================================================
// If we are going to have shadows, we need to do a pass through all the
// objects and do the draw shadow routines for anything that casts.
// =====================================================================
    glBindFramebuffer(GL_FRAMEBUFFER, pMyGraphics->ShadowFB);
    glViewport(0, 0, ShadowBufferSize, ShadowBufferSize);// Set to buffer size
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(ulong i=0; i<MyObjects.size(); i++) {
        MyObjects[i]->DrawShadow();// Most objects will do a dummy call
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);// Release the shadow framebuffer

//===================================
// Render to the screen buffer again.
// Reset the viewport to screen size
//===================================
    glViewport(0, 0, pMyGraphics->p_state->width, pMyGraphics->p_state->height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(ulong i=0; i<MyObjects.size(); i++) {
        MyObjects[i]->Update();
        MyObjects[i]->Draw();
    }

    Collision.CheckForCollision(this);

/*
    DynamicPhysicsWorld->debugDrawWorld();
// show forward and right for model 1 The Player.
    RayClass.ShowRays(this, MyObjects[1]);
// Do the debug draw
    glm::mat4* Projection = TheCamera->GetProjection();
    glm::mat4* View       = TheCamera->GetView();
    glUseProgram(m_pPhysicsDrawer->ProgramObject);
    GLint MatrixID = glGetUniformLocation(m_pPhysicsDrawer->ProgramObject, "MVP"); // use the MVP in the simple shader
// make the View and Projection matrix
    glm::mat4 VP = *Projection * *View;  // Remember order seems backwards
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &VP[0][0]);
    m_pPhysicsDrawer->DoDebugDraw(); // useful but very slow, take note!
*/

#ifdef SHOW_TEXT
    // Render text as needed
    pFont->RenderText();
#endif

//==================================================
// After our draw we need to swap buffers to display
//==================================================
    glfwSwapBuffers(pMyGraphics->p_state->nativewindow);

//====================================
// And check for keyboard/mouse events
//====================================
    glfwPollEvents();

    if((glfwGetKey(pMyGraphics->state.nativewindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) ||
       (glfwWindowShouldClose(pMyGraphics->state.nativewindow) != 0)) {
        return false;
    }
    return true;
}


//==================================================================
// Here we prepare any objects we want to create in our world
// starting with the world itself
//==================================================================
void
Game::InitObjects() {
    Ground* pRuin;
    pRuin = new Ground(pFileHandler,
                      "Resources/Models/ruin2_walled.obj",
                      pMainModelManager);
    pRuin->Scales = glm::vec3(1.0f);
    pRuin->TheModelManager->GetBoundingBoxes(pRuin);
    pRuin->SetPosition(glm::vec3(0.0f));
    MyObjects.push_back(pRuin);// Now it's on the system for possible updates/draws
    pRuin->StoreGraphicClass(pMyGraphics);
    pRuin->programObject =
            pMyGraphics->
            OurShaderManager->
            MakeProgramObject("Resources/Shaders/ObjectVertWithShadows.vsh",
                              "Resources/Shaders/ObjectFragWithShadows.fsh",
                              pFileHandler);
    pRuin->TheGame = this;
// It's going to be physics based so get his sizes
    pRuin->TheModelManager->GetBoundingBoxes(pRuin);

//===============================================
// Do a ground physics object from the pRuin mesh
//===============================================
    PhysicsObj* playGround = CreatePhysicsObj(pRuin->mesh,
                                              0, // 0 mass means a Static Body
                                              btVector3(0.0f, 0.0f, 0.0f), // position
                                              btQuaternion(0.0f, 0.0f, 0.0f, 1.0f)); // no rotation
// Once the object is created ensure we give it a user pointer.
// It also helps if ObjectModel has a type
    playGround->GetRigidBody()->setUserPointer(static_cast<ObjectModel*>(pRuin));
    pRuin->WhatAmI = ObjectModel::e_GROUND;

//==============================
// Now put our Player on the map
//==============================
    Player* AKnight = new Player(pFileHandler,
                                 "Resources/Models/knight.md2",
                                 pMainModelManager);
    MyObjects.push_back(AKnight); // It's now on the system for possible updates/draws
    AKnight->StoreGraphicClass(pMyGraphics);
    AKnight->TheGame = this;
    AKnight->programObject = pMyGraphics->
                                OurShaderManager->
                                MakeProgramObject("Resources/Shaders/MD2_Vertex.vsh",
                                                  "Resources/Shaders/MD2_Fragment.fsh",
                                                  pFileHandler);
    AKnight->LoadSkin("Resources/Textures/knight.png",
                      pMainModelManager);// He does not auto load a skin
    AKnight->SetPosition(glm::vec3(1.0, 10.0, 0.0f));
    AKnight->Scales = glm::vec3(0.10f);// Original models are quite large
    pMainModelManager->GetBoundingBoxes(AKnight);
    AKnight->MyPhysObj = CreatePhysicsObj(AKnight->CreateMyShape(ObjectModel::CAPSULE),
                                          150, // mass 150kg
                                          btVector3(AKnight->WorldPosition.x,
                                                    AKnight->WorldPosition.y,
                                                    AKnight->WorldPosition.z),
                                          btQuaternion(0, 1, 0, 0));     // Rotation
    AKnight->MyPhysObj->
            GetRigidBody()->
            getCollisionShape()->
            setLocalScaling(btVector3(AKnight->Scales.x,
                                      AKnight->Scales.y,
                                      AKnight->Scales.z));
    AKnight->MyPhysObj->
            GetRigidBody()->
            setUserPointer(static_cast<ObjectModel*>(AKnight));
    AKnight->WhatAmI = ObjectModel::e_PLAYER;

    SkyBox* pSky = new SkyBox(pFileHandler);
    pSky->programObject = pMyGraphics->
                          OurShaderManager->
                          MakeProgramObject("Resources/Shaders/SkyBoxVert.vsh",
                                            "Resources/Shaders/SkyBoxFrag.fsh",
                                            pFileHandler);
    pSky->StoreGraphicClass(pMyGraphics);
    pSky->TheGame = this;
    pSky->Init();
    MyObjects.push_back(pSky);

//========================================================
// Now we have our ground and player, let's create baddies
//========================================================
    pEManager->CreateEnemies(this);
}


PhysicsObj*
Game::CreatePhysicsObj(btCollisionShape* pShape,
                       const float &mass,
                       const btVector3 &aPosition,
                       const btQuaternion &aRotation)
{
// create a new game object
    PhysicsObj* pPhysicsObject = new PhysicsObj(pShape, mass, aPosition, aRotation);
    //    float currentm = pShape->getMargin();
    pShape->setMargin(0.09f);

    if(DynamicPhysicsWorld) {
        DynamicPhysicsWorld->addRigidBody(pPhysicsObject->GetRigidBody());
    }
    else {
        printf("Attempting to add to a Dynamic world that does not exist\n");
    }
// return to place in the objects data or in another list
    return pPhysicsObject;
}

