#pragma once

//============================
// This only works with Bullet
//============================

// These are systems to determine if collision has occured,
// or there is a separation or a contact.
// The default is to use the class ObjectModel as our base type
// and call the virtual handling routines it has which currently do nothing.
// Derived classes should override them and take whatever action
// the game needs to respond appropriatly.
// The Game Class should itself contain an instance of this class, and pass
// itself so that Check for collion has access to the Dispatcher.

#include <set>
#include <iterator>
#include <bullet/btBulletDynamicsCommon.h>

class Game;


class CollisionProcess
{
public:
    CollisionProcess();
    ~CollisionProcess();

public:
// game class should call this after physics processing
    void CheckForCollision(Game* TheGame);
// will check for any events and call a processing routing at ObjectModel
    void SeparationEvent(const btRigidBody *RB0, const btRigidBody *RB1);
    void CollisionEvent(const btRigidBody *RB0, const btRigidBody *RB1);
    void ContactEvent(const btRigidBody *RB0, const btRigidBody *RB1);

public:
    typedef std::pair<const btRigidBody*, const btRigidBody*> CollisionObjects;
    typedef std::set<CollisionObjects> ManifoldPairs;
    ManifoldPairs pairsLastFrame; // the pairs from last frame need to be storeed
};
