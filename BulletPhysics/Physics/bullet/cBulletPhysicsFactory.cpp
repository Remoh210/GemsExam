#include "cBulletPhysicsFactory.h"
#include "cBulletRigidBody.h"
#include "bullet_shapes.h"
#include "cBulletPhysicsWorld.h"

namespace nPhysics
{
	cBulletPhysicsFactory::~cBulletPhysicsFactory() {}

	iPhysicsWorld* cBulletPhysicsFactory::CreatePhysicsWorld()
	{
		return new cBulletPhysicsWorld();
	}

	iRigidBody* cBulletPhysicsFactory::CreateRigidBody(const sRigidBodyDef & def, iShape* shape)
	{
		return new cBulletRigidBody(def, shape);
	}

	iSphereShape* cBulletPhysicsFactory::CreateSphereShape(float radius)
	{
		return new cBulletSphereShape(radius);
	}

	iPlaneShape* cBulletPhysicsFactory::CreatePlaneShape(const glm::vec3 & normal, float constant)
	{
		return new cBulletPlaneShape(normal, constant);
	}

}