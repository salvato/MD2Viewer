#include "Rays.h"
#include "Game.h"
#include "Camera.h"


// a pick is nothing more than a fancy raycast using the camera
// as its start going through the xy of the screen mouse
RayReturn Rays::Pick(Game* TheGame) {
    RayReturn ReturnValues;
    ReturnValues.WhoWeHit = nullptr; // ensure a negitive response is default
    btVector3 CameraStart = btVector3(TheGame->GetCamera()->CameraPos.x,
                                      TheGame->GetCamera()->CameraPos.y,
                                      TheGame->GetCamera()->CameraPos.z);
    btVector3 ThePickRay = GetPickingRay(TheGame);

    if(Raycast(TheGame->DynamicPhysicsWorld, CameraStart, ThePickRay, ReturnValues) == true) {
        // don't have to put any code here, the Return value will have an RB in it
        // if there's a hit but we can add some extra checks here if we need to.
    }
    return ReturnValues;
}


// 
btVector3
Rays::GetPickingRay(Game* TheGame) {
    double xpos, ypos;
    glfwPollEvents();
    glfwGetCursorPos(TheGame->pMyGraphics->state.nativewindow,
                     &xpos,
                     &ypos);
    int x = int(floor(xpos));
    int y = int(floor(ypos));
    // our camera data is usually held as glm, so theres some annoying
    // conversions needed...whats a good way to stop that?
    // calculate the field-of-view
    float tanFov = 1.0f / TheGame->GetCamera()->NearPlane ;
    float fov = btScalar(2.0) * btAtan(tanFov);
    (void)fov;
    btVector3 RayStart = btVector3(TheGame->GetCamera()->CameraPos.x,
                                   TheGame->GetCamera()->CameraPos.y,
                                   TheGame->GetCamera()->CameraPos.z
                                   ); // stored as a glm so needs converting
    // we need to work out the forward direction of the ray	which is in glm format.
    glm::vec3 GLMRaysForwardDir = TheGame->GetCamera()->Target - TheGame->GetCamera()->CameraPos;
    // convert the result to btvector3
    btVector3 RaysForwardDir = btVector3(GLMRaysForwardDir.x, GLMRaysForwardDir.y, GLMRaysForwardDir.z);
    RaysForwardDir.normalize();
    RaysForwardDir *= TheGame->GetCamera()->FarPlane; // extend it to the far plane
    // Need the horizontal and vertical vectors of the camera
    btVector3 vertical = btVector3(TheGame->GetCamera()->UpVector.x,
                                   TheGame->GetCamera()->UpVector.y,
                                   TheGame->GetCamera()->UpVector.z
                                   ); // its stored as glm so we have to convert it
    btVector3 horizontal = RaysForwardDir.cross(vertical);
    horizontal.normalize();
    vertical = horizontal.cross(RaysForwardDir);
    vertical.normalize();
    horizontal *= (2.0f * (TheGame->GetCamera()->FarPlane * tanFov));
    vertical *= (2.0f * (TheGame->GetCamera()->FarPlane * tanFov));
    // now calculate the aspect ratio
    btScalar aspect = TheGame->ScreenX / TheGame->ScreenY ;
    // now use the x and y coordinats to move the ray
    horizontal *= aspect;
    btVector3 rayToCenter = RayStart + RaysForwardDir;
    btVector3 dHor = horizontal * (1.0f / float(TheGame->ScreenX));
    btVector3 dVert = vertical * (1.0f / float(TheGame->ScreenY));

    btVector3 rayTo = rayToCenter - (0.5f * horizontal) + (0.5f * vertical);
    rayTo += float(x) * dHor;
    rayTo -= float(y) * dVert;

    // return the direction of the picking ray.
    return rayTo;
}


bool
Rays::Raycast(btDiscreteDynamicsWorld* TheWorld, btVector3 &start, btVector3 &to, RayReturn &returnstruct) {
    btVector3 rayTo = to;
    btVector3 rayStart = start; // where does it start

    // create a raycast callback object and give it the info it needs
    btCollisionWorld::ClosestRayResultCallback rayCallback(rayStart, rayTo);

    // let Bullet do its thing
    TheWorld->rayTest(rayStart, rayTo, rayCallback);

    // callback will return an indicate if we hit
    if (rayCallback.hasHit()) {
        // it retuns rigid bodies
        const btRigidBody* RB = reinterpret_cast<const btRigidBody*>(btRigidBody::upcast(rayCallback.m_collisionObject));
        // ok this seems valid, so extract the useful info

        // The raycast does not actually know the difference between a static terrain or a game object, usually we don't
        // really want to hit the floor, but we might want to hit a wall, so returning a flag to indicate it was a static
        // object. allows the calling code to decide if the returned info is useful

        returnstruct.WasItStatic  = RB->isStaticObject();
        returnstruct.WhoWeHit = RB;
        returnstruct.WhereWeHit = rayCallback.m_hitPointWorld;
        btVector3 T = returnstruct.WhereWeHit - rayStart;
        returnstruct.Distance = T.length();

        if(returnstruct.Distance == 0.0f) {
            ObjectModel::ObjectType T = static_cast<ObjectModel*>(RB->getUserPointer())->WhatAmI;
            (void)T;
            return false;
        }
        return true;
    }
    // we didn't hit anything
    return false;
}


//show the relevent rays
void
Rays::ShowRays(Game* TheGame, ObjectModel* Model) {
    // players pos as a btVector3
    btVector3 Pos = btVector3(Model->WorldPosition.x,
                              Model->WorldPosition.y,
                              Model->WorldPosition.z);

    TheGame->m_pPhysicsDrawer->drawLine(Pos, Pos+(Model->Forward*10), btVector3(1, 1, 1));
    TheGame->m_pPhysicsDrawer->drawLine(Pos, Pos+(Model->Right * 10), btVector3(1, 1, 1));
}

