#include "Player.h"
#include "Game.h"
#include "Ground.h"


Player::Player(MyFiles* FH, const char* FN, ModelManager* MM)
    : MD2Model(FH, FN, MM)
{
    pAnimation->SetSequence("stand", this);
    Forward = btVector3(0, 0, 0); // make sure we have an initital vector
}


bool
Player::Update() {
    MyPhysObj->GetRigidBody()->setActivationState(DISABLE_DEACTIVATION);
    bool trigger = false; // assume a false trigger
// Override the X and Z angular velocties to prevent toppling.
    btVector3 angl = btVector3(0, 0, 0);
    MyPhysObj->GetRigidBody()->setAngularFactor(angl);
    MyPhysObj->GetRigidBody()->setFriction(10.0f);
    MyPhysObj->GetRigidBody()->setGravity(btVector3(0, -9.81f/Scales.y, 0));

    sprintf(buffer, "Forward * 1.5 = : %f,%f,%f",
            double(Forward.getX()*1.5f),
            double(Forward.getY()*1.5f),
            double(Forward.getZ()*1.5f));
    TheGame->pFont->PrintText(buffer, 20, 140);

    btVector3 LinVer = MyPhysObj->GetRigidBody()->getLinearVelocity();
    sprintf(buffer, "Linear Vel = : %f,%f,%f",
            double(LinVer.getX()),
            double(LinVer.getY()),
            double(LinVer.getZ()));
    TheGame->pFont->PrintText(buffer, 20, 156);

    btVector3 Pos = MyPhysObj->GetRigidBody()->getCenterOfMassPosition();
    sprintf(buffer, "Centre of Mass pos = : %f,%f,%f",
            double(Pos.getX()),
            double(Pos.getY()),
            double(Pos.getZ()));
    TheGame->pFont->PrintText(buffer, 20, 156+16);

    btScalar PMargin = MyPhysObj->GetShape()->getMargin();
    Ground* TheG = static_cast<Ground*>(TheGame->MyObjects[0]);
    btScalar GMargin = 	TheG->mesh->getMargin();
    sprintf(buffer, "Player Margin:-%f  / Ground Margin:- %f",
            double(PMargin),
            double(GMargin));
    TheGame->pFont->PrintText(buffer, 20, 156 + (16*2));

    btQuaternion Current = this->MyPhysObj->GetRigidBody()->getOrientation();
    int	 numManifolds = this->TheGame->Dispatcher->getNumManifolds();

//===============================
// do what ever logic needs doing
//===============================

    glfwPollEvents();
#ifdef DEBUG
// only use the debug draws in debug mode
    if(glfwGetKey(TheGraphics->state.nativewindow, GLFW_KEY_W) == GLFW_PRESS) {
        TheGame->m_pPhysicsDrawer->ToggleDebugFlag(1 + 2 + 8);
        while(glfwGetKey(TheGraphics->state.nativewindow, GLFW_KEY_W) == GLFW_PRESS) {
            glfwPollEvents();
        } // acts as a debounce
    }
    glfwPollEvents();
#endif // DEBUG

// Make the MVP, but use the lights position
    TheGraphics->LightTarget = WorldPosition;
    WorldPosition = WorldPosition + glm::vec3(-30, 50, 0);
// Set lights up based on our mans positon, not ideal!!
    TheGraphics->LightPos = WorldPosition;

    if((glfwGetKey(TheGraphics->state.nativewindow, GLFW_KEY_UP) == GLFW_PRESS) &&
        numManifolds != 0)
    {
        pAnimation->SetSequence("run",
                                this,
                                MD2Anim::TRIGGER,
                                1.0f/60,
                                "stand");
        btVector3 vel =	MyPhysObj->GetRigidBody()->getLinearVelocity();
        vel.setX(0);
        vel.setZ(0);
        MyPhysObj->GetRigidBody()->setLinearVelocity(vel+(Forward*1.5));
        MyPhysObj->GetRigidBody()->setRestitution(0);
        MyPhysObj->GetRigidBody()->setFriction(1.5f);
        trigger = true;
    }

    if(glfwGetKey(TheGraphics->state.nativewindow, GLFW_KEY_DOWN) == GLFW_PRESS) {
        pAnimation->SetSequence("crwalk",
                                this,
                                MD2Anim::TRIGGER,
                                1.0f/60.0f,
                                "stand");
        MyPhysObj->GetRigidBody()->setLinearVelocity(Forward * -0.6f);
        MyPhysObj->GetRigidBody()->setFriction(0.5f);
        trigger = true;
    }

// Not very physics based but lets orient
    if(glfwGetKey(TheGraphics->state.nativewindow, GLFW_KEY_LEFT) == GLFW_PRESS) {
        btQuaternion AddVal = btQuaternion(btVector3(0, 1, 0), glm::radians(0.25f));
        MyPhysObj->SetOrientation(Current*AddVal);
        trigger = true;
    }

    if(glfwGetKey(TheGraphics->state.nativewindow, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        btQuaternion AddVal = btQuaternion(btVector3(0, 1, 0), glm::radians(-0.25f));
        MyPhysObj->SetOrientation(Current*AddVal);
        trigger = true;
    }
    MD2Model::Update();
// Then do the update for matrices and animation
    pAnimation->Update(TheGame->DeltaTimePassed, trigger);
//	Animation.UpdateSlave();
    return true;
}


/*******************************************************************************************
Collision handlers, all types that need collision responses need this.
How we handle collision and separation will depend on what we collide
with and sometimes at what angle!
Remember these only handle actual collision and seperation
*******************************************************************************************/
void
Player::HandleCollision(const ObjectModel *WhatDidIHit) {
    (void)WhatDidIHit;
        printf("I hit some something\n");
        if(WhatDidIHit->WhatAmI == ObjectModel::e_GROUND)
            printf("I hit da ground dude\n");
}


// Separation can sometimes happen as a result of bumpy ground
void
Player::HandleSeparation(const ObjectModel* WhatDidIHit) {
    (void)WhatDidIHit;
        printf("I have separated from something\n");
        if (WhatDidIHit->WhatAmI == ObjectModel::e_GROUND)
            printf("I have separated from ground dude\n");
}


// This will happen after collision and/or seperation,
void
Player::HandleContact(const ObjectModel *WhatDidIHit) {
    (void)WhatDidIHit;
//        if (WhatDidIHit->WhatAmI == ObjectModel::e_GROUND)
//            printf("I am in contact with the ground\n");
}
