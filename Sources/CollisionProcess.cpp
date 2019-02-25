#include "CollisionProcess.h"
#include "Game.h"

//================================
// Only valid with Bullet Physics.
//================================

// Provides a means to respond to collisions, separations and contact events
// Contact events may not be needed so could be removed


CollisionProcess::CollisionProcess()
{} 


CollisionProcess::~CollisionProcess()
{}


void
CollisionProcess::CheckForCollision(Game* TheGame) {
// Keep a list of the collision pairs
// we find during the current update
    ManifoldPairs pairsThisFrame;
    ManifoldPairs CollisionsThisFrame;

// Go through all the manifolds in the Dispatchers list
    for(int i=0; i<TheGame->Dispatcher->getNumManifolds(); i++) {
        btPersistentManifold* Manifold =
                TheGame->Dispatcher->getManifoldByIndexInternal(i);
// We only care if they have contact points
        if(Manifold->getNumContacts() > 0) {
// getBodyx actually returns btCollisionObject, and we really need
// to get btRigidBody instead for compares
// So we have to use the C++ styly static cast.
// (or plant an idvalue in the rigid body if they are unique)
            const btRigidBody* Body0 =
                    static_cast<const btRigidBody*>(Manifold->getBody0());
            const btRigidBody* Body1 =
                    static_cast<const btRigidBody*>(Manifold->getBody1());
// Since they are pointers, they have numerical value,
// use that to store them lowest value first
            const btRigidBody* FirstRB  = (Body0 > Body1) ? Body1 : Body0;
            const btRigidBody* SecondRB = (Body0 > Body1) ? Body0 : Body1;
// Create the pair
            CollisionObjects NewPair = std::make_pair(FirstRB, SecondRB);
// Insert the pair into the current list
            pairsThisFrame.insert(NewPair);
// This pair definitely are colliding, if they are brand new
// we can safely say its a collsion event
            if(pairsLastFrame.find(NewPair) == pairsLastFrame.end()) {
// Search the old list for this new pair got through to the end...
// it wasn't there so this is a new hit
                CollisionEvent(static_cast<const btRigidBody*>(Body0),
                               static_cast<const btRigidBody*>(Body1));
// Keep these to discount them in the contact list
                CollisionsThisFrame.insert(NewPair);
            }
        } // if(Manifold->getNumContacts() > 0)
    } // for(int i=0; i<TheGame->Dispatcher->getNumManifolds(); i++)

// Create another list for pairs that were removed this update,
// they will be separated events
    ManifoldPairs removedPairs;
    ManifoldPairs ContactPairs;
// Compare the set from last frame with the	set of this
// frame and put the removed one sin the removed Pairs
    std::set_difference(pairsLastFrame.begin(),
                        pairsLastFrame.end(),
                        pairsThisFrame.begin(),
                        pairsThisFrame.end(),
                        std::inserter(removedPairs, removedPairs.begin()));
// Iterate through all of the removed pairs sending separation events for them,
// can't use an index for this so this is a C++11 iterer
    for(ManifoldPairs::const_iterator iter=removedPairs.begin();
        iter!=removedPairs.end();
        ++iter)
    {
        SeparationEvent(static_cast<const btRigidBody*>(iter->first),
                        static_cast<const btRigidBody*>(iter->second));
    }
// Compare the collision with the contact list and we can process
// them as contact pairs too, useful for checking gravity or balance
    std::set_difference(pairsThisFrame.begin(),
                        pairsThisFrame.end(),
                        CollisionsThisFrame.begin(),
                        CollisionsThisFrame.end(),
                        std::inserter(ContactPairs, ContactPairs.begin()));
// All remaining pairs are contact pairs, that have not registered as a collision
    for(ManifoldPairs::const_iterator iter=ContactPairs.begin();
        iter!=ContactPairs.end();
        ++iter)
    {
        ContactEvent(static_cast<const btRigidBody*>(iter->first),
                     static_cast<const btRigidBody*>(iter->second));
    }
// Because the names for sets are pointers, we can simply tell
// the Last pair it is now the current pair
    pairsLastFrame = pairsThisFrame;
}


// We are assuming ObjectModel as a base type, but if you
// have an ID in your ObjectModel, you can cast to other types.
void
CollisionProcess::CollisionEvent(const btRigidBody* RB0, const btRigidBody* RB1) {
// Assumes we set up the user pointer with an ObjectModel* and there
// are handling routines in ObjectModel or overrides in derived
    ObjectModel* First = reinterpret_cast<ObjectModel*>(RB0->getUserPointer());
    ObjectModel* Second = reinterpret_cast<ObjectModel*>(RB1->getUserPointer());
// If either of these is null don't do it but they shoud have values in them
    if(First == nullptr || Second == nullptr)
        return;
    First->HandleCollision(Second);
    Second->HandleCollision(First);
}


void
CollisionProcess::SeparationEvent(const btRigidBody* RB0, const btRigidBody* RB1) {
// Assumes we set up the user pointer with an ObjectModel* and there
// are handling routines in ObjectModel or overrides in derived
    ObjectModel* First = reinterpret_cast<ObjectModel*>(RB0->getUserPointer());
    ObjectModel* Second = reinterpret_cast<ObjectModel*>(RB1->getUserPointer());
    if (First == nullptr || Second == nullptr)
        return;
    First->HandleSeparation(Second);
    Second->HandleSeparation(First);
}


void
CollisionProcess::ContactEvent(const btRigidBody* RB0, const btRigidBody* RB1) {
// This isn't a just hit or a just separated, its an actual contact,
// useful for testing floor.
// This event is done after collision and separations which can be
// useful to test for small bumps

// Assumes we set up the user pointer with an ObjectModel*
    ObjectModel* First = reinterpret_cast<ObjectModel*>(RB0->getUserPointer());
// and there are handling routines in ObjectModel or overrides in derived
    ObjectModel* Second = reinterpret_cast<ObjectModel*>(RB1->getUserPointer());

    if (First == nullptr || Second == nullptr)
        return;
    First->HandleContact(Second);
    Second->HandleContact(First);
}
