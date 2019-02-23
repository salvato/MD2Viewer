// Simple access to the Physics info
#include "PhysicsObj.h"


PhysicsObj::PhysicsObj (btCollisionShape* Shape,
	float mass,
    const btVector3    &Position,// These have defaults
    const btQuaternion &Rotation)// if needed
{
// We have to create a startup transform
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(Position);
	transform.setRotation(Rotation);
// This new info needs to create the Game Values
	m_Status = new GameValues(transform);
	m_Shape = Shape;
// Set up inertia, start with 0, and test if we have an infinite mass(0.0),
// if so, no inertia is possible
	btVector3 Inertia(0, 0, 0);
	if (mass != 0.0f)
		m_Shape->calculateLocalInertia(mass, Inertia);
// Make a new Rigid Body with this info
	btRigidBody::btRigidBodyConstructionInfo RBinfo(mass,m_Status,m_Shape,Inertia);
	m_Body = new btRigidBody(RBinfo);
// And were done, there's no error reporting in Bullet
// so we just have to assume it's ok
}


PhysicsObj::~PhysicsObj() {
// Remove the things we made
	delete m_Body;
	delete m_Shape;
	delete m_Status;
}



