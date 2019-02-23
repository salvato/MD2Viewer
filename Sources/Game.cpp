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

// Set size for your shadow buffer, it does have a speed impact
// though if you set above 1024 and a resolution impact if you
// set below 512
// Consider that the texture represents the entire visible area
// were light is cast and has to expand pixels to suit, it might
// we wise to set up several lights and swap them in and out
// as needed depending on the size of your terrain.
#define ShadowBufferSize 2048

//#define SHOW_TEXT

// Don't do anything special here, leave it to the init.
// But do set up the OpenGL window stuff.
Game::Game() {
    pMyGraphics = new Graphics();// Set up the OpenGL window and create a ShaderManager
    pHandler = new MyFiles();
    pMainModelManager = new ModelManager();
    pFont = new Font3D();
}


Game::~Game() {
    for(uint i=0; i<MyObjects.size(); i++)	{
        delete MyObjects[i];
    }
    MyObjects.clear();
    delete pTheCamera;
    delete pFont;
    delete pHandler;
    delete pMainModelManager;
    delete pMyGraphics;
}


bool
Game::Init() {
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
                              pHandler);
    pFont->Init(pMyGraphics->state.width,
                pMyGraphics->state.height,
                "Resources/Fonts/atari-font.bmp");

//=======================
// Initialise our physics
//=======================
    BroadPhase             = new btDbvtBroadphase();
    CollisionConfiguration = new btDefaultCollisionConfiguration();
    Dispatcher             = new btCollisionDispatcher(CollisionConfiguration);
    ConstraintSolver       = new btSequentialImpulseConstraintSolver();

// DynamicPhysicsWorld() manages all physics objects and constraints.
// It also implements the update of all objects each frame.
    DynamicPhysicsWorld    = new btDiscreteDynamicsWorld(Dispatcher,
                                                         BroadPhase,
                                                         ConstraintSolver,
                                                         CollisionConfiguration);

    DynamicPhysicsWorld->setGravity(btVector3(0.0f, -9.81f, 0.0f));
//============================
// Done physics initialization
//============================

//==================================================================
// Here we would prepare any objects we want to create in our world
// starting with the world
//==================================================================
    glm::vec3 Pos;
    Ground* pRuin;
    pRuin = new Ground(pHandler,
                      "Resources/Models/ruin2_walled.obj",
                      pMainModelManager);
    pRuin->Scales = glm::vec3(1.0f);
    pRuin->TheModelManager->GetBoundingBoxes(pRuin);
    Pos = glm::vec3(0.0, 0.0, 0.0f);
    pRuin->SetPosition(Pos);
    MyObjects.push_back(pRuin);// Now it's on the system for possible updates/draws
    pRuin->StoreGraphicClass(pMyGraphics);
    pRuin->programObject =
            pMyGraphics->
            OurShaderManager->
            MakeProgramObject("Resources/Shaders/ObjectVertWithShadows.vsh",
                              "Resources/Shaders/ObjectFragWithShadows.fsh",
                              pHandler);
    pRuin->TheGame = this;
// It's going to be physics based so get his sizes
    pRuin->TheModelManager->GetBoundingBoxes(pRuin);

//===================
// Do a ground object
//===================
    PhysicsObj* Ground = CreatePhysicsObj(pRuin->mesh,
                                          0, // mass
                                          btVector3(0.0f, 0.0f, 0.0f), // position
                                          btQuaternion(0.0f, 0.0f, 0.0f, 1.0f)); // no rotation
// Once the object is created ensure we give it a user pointer.
// It also helps if ObjectModel has a type
    Ground->GetRigidBody()->setUserPointer(static_cast<ObjectModel*>(pRuin));
    pRuin->WhatAmI = ObjectModel::e_GROUND;

//==============================
// Now put our Player on the map
//==============================
    Pos = glm::vec3(1.0, 10.0, 0.0f);
    Player* AKnight = new Player(pHandler,
                                 "Resources/Models/knight.md2",
                                 pMainModelManager);
    MyObjects.push_back(AKnight); // It's now on the system for possible updates/draws
    AKnight->StoreGraphicClass(pMyGraphics);
    AKnight->TheGame = this;
    AKnight->programObject = pMyGraphics->
            OurShaderManager->
            MakeProgramObject("Resources/Shaders/MD2_Vertex.vsh",
                              "Resources/Shaders/MD2_Fragment.fsh",
                              pHandler);
    AKnight->LoadSkin("Resources/Textures/knight.png",
                      pMainModelManager);// he does not auto load a skin
    AKnight->SetPosition(Pos);
    AKnight->Scales = glm::vec3(0.10f);  // original models are quite large
    pMainModelManager->GetBoundingBoxes(AKnight);
    AKnight->MyPhysObj = CreatePhysicsObj(AKnight->CreateMyShape(ObjectModel::CAPSULE),
                                          150, // mass 150kg
                                          btVector3(Pos.x, Pos.y, Pos.z),// Position}
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

    SkyBox* pSky = new SkyBox(pHandler);
    pSky->programObject = pMyGraphics->
                          OurShaderManager->
                          MakeProgramObject("Resources/Shaders/SkyBoxVert.vsh",
                                            "Resources/Shaders/SkyBoxFrag.fsh",
                                            pHandler);
    pSky->StoreGraphicClass(pMyGraphics);
    pSky->TheGame = this;
    pSky->Init();
    MyObjects.push_back(pSky);

//========================================================
// Now we have our ground and player, let's create baddies
//========================================================
    pEManager->CreateEnemies(this);

// Clear up the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//================================================
// Objects can also cast shadows.
// So lets make sure we have that shader available
//================================================
    pMyGraphics->ShadowShader = pMyGraphics->
            OurShaderManager->
            MakeProgramObject("Resources/Shaders/ShadowMap.vsh",
                              "Resources/Shaders/ShadowMap.fsh",
                              pHandler);
// Make our frame buffer: let's make it a simple size
    pMyGraphics->ShadowFB = pMyGraphics->CreateShadowBuffer(ShadowBufferSize,
                                                          ShadowBufferSize);
//=======================
// Set up the draw system
//=======================
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
                              pHandler);
    m_pPhysicsDrawer->ProgramObject = SimpleShader;

//=============================================
// World and Player set up, physics initialised
//=============================================
    return true;
}


bool
Game::Update(float deltaTime) {
// Give ojects access to the passage of time
    DeltaTimePassed = deltaTime;

// Get ready to cycle through any objects we made
    pTheCamera->Update(MyObjects[1]); // Player is number 1 Ground is 0
    DynamicPhysicsWorld->stepSimulation(1.0f/60.0f, 5);

// If we are going to have shadows, we need to do a pass through all the
// objects and do the draw shadow routines for anything that casts.
    glBindFramebuffer(GL_FRAMEBUFFER, pMyGraphics->ShadowFB);
    glViewport(0, 0, ShadowBufferSize, ShadowBufferSize);  // set to buffer size
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(ulong i=0; i<MyObjects.size(); i++) {
        MyObjects[i]->DrawShadow(); // most objects will do a dummy call
    }
// Release the shadow framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

// Render to the screen buffer again.
// Reset the viewport to screen size
    glViewport(0, 0, pMyGraphics->p_state->width, pMyGraphics->p_state->height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for(ulong i=0; i<MyObjects.size(); i++) {
        MyObjects[i]->Update();
        MyObjects[i]->Draw();
    } // for(ulong i=0; i<MyObjects.size(); i++)
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
// Render text as needed
#ifdef SHOW_TEXT
    pFont->RenderText();
#endif

// After our draw we need to swap buffers to display
    glfwSwapBuffers(pMyGraphics->p_state->nativewindow);
    glfwPollEvents();
    if((glfwGetKey(pMyGraphics->state.nativewindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) ||
       (glfwWindowShouldClose(pMyGraphics->state.nativewindow) != 0)) {
        return false;
    }
    return true;
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

