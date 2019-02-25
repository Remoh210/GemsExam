#include "cSimplePhysicsFactory.h"
#include "cSimpleRigidBody.h"
//#include "interfaces/shapes.h"
#include "shape.h"
#include "cSimplePhysicsWorld.h"

namespace nPhysics
{
	cPhysicsFactory::~cPhysicsFactory() {}
	iPhysicsWorld* cPhysicsFactory::CreatePhysicsWorld()
	{
		return new cSimplePhysicsWorld();
	}

	iRigidBody* cPhysicsFactory::CreateRigidBody(const sRigidBodyDef& desc, iShape* shape)
	{
		return new cSimpleRigidBody(desc, shape);
	}

	iSphereShape* cPhysicsFactory::CreateSphereShape(float radius)
	{
		return new cSphereShape(radius);
	}
	iPlaneShape* cPhysicsFactory:: CreatePlaneShape(const glm::vec3& normal, float constant)
	{
		return new cPlaneShape(normal, constant);
	}

}