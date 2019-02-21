#pragma once
#include <bullet/btBulletDynamicsCommon.h>
#include "PhysicsDraw.h"

// struct to store our raycasting results

typedef struct {
    const btRigidBody* WhoWeHit;
    btVector3 WhereWeHit;
    float Distance;
    bool WasItStatic;
} RayReturn;

class Game;
class ObjectModel;


class Rays
{
public:
    Rays() {}
    ~Rays() {}

public:
    bool Raycast(btDiscreteDynamicsWorld* theWorld, btVector3 &startPosition, btVector3 &direction, RayReturn &output);
    btVector3 GetPickingRay(Game* TheGame);
    RayReturn Pick(Game* TheGame);
    // like debug draw we are going to allow ourseves to draw some rays	this is game specific stuff
    void ShowRays(Game* TheGame, ObjectModel* Model);

private:	
    btVector3 StartVector;
};

