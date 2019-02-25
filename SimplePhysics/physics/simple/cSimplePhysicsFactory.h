#pragma once
#include "physics_interfaces.h"
namespace nPhysics
{
	class cPhysicsFactory : public iPhysicsFactory
	{
	public:
		virtual ~cPhysicsFactory();

		virtual iPhysicsWorld* CreatePhysicsWorld();

		virtual iRigidBody* CreateRigidBody(const sRigidBodyDef& def, iShape* shape);

		virtual iSphereShape* CreateSphereShape(float radius);
		virtual iPlaneShape* CreatePlaneShape(const glm::vec3& normal, float constant);
	};
}


