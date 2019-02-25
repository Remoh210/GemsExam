#pragma once
#include "iRigidBody.h"
#include "iDebugRenderer.h"
namespace nPhysics {
	class iPhysicsWorld
	{
	public:
		virtual ~iPhysicsWorld() {};
		virtual void SetDebugRenderer(iDebugRenderer* debugRenderer) = 0;
		virtual void DrawDebug() = 0;

		virtual void SetGravity(const glm::vec3& gravity) = 0;
		virtual bool AddBody(iRigidBody* body) = 0;
		virtual bool RemoveBody(iRigidBody* body) = 0;
		virtual void Update(float dt) = 0;

	protected:
		iPhysicsWorld() {}
		iPhysicsWorld(const iPhysicsWorld& other) {}
		iPhysicsWorld& operator=(const iPhysicsWorld& other) { return *this; }
	};

}