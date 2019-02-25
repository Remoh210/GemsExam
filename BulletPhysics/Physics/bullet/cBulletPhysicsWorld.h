#pragma once
#include <Interfaces/iPhysicsWorld.h>
#include "btBulletDynamicsCommon.h"
#include <stdio.h>

namespace nPhysics {
	class cBulletPhysicsWorld : public iPhysicsWorld
	{
	public:
		cBulletPhysicsWorld();
		~cBulletPhysicsWorld();
		void SetDebugRenderer(iDebugRenderer* debugRenderer);
		void DrawDebug();

		void SetGravity(const glm::vec3& gravity);
		bool AddBody(iRigidBody* body);
		bool RemoveBody(iRigidBody* body);
		void Update(float dt);

	protected:
		cBulletPhysicsWorld(const cBulletPhysicsWorld& other) {}
		cBulletPhysicsWorld& operator=(const cBulletPhysicsWorld& other) { return *this; }
	private:

		btDefaultCollisionConfiguration* mCollisionConfiguration;

		//use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
		btCollisionDispatcher* mDispatcher;

		//btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
		btBroadphaseInterface* mOverlappingPairCache;

		//the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		btSequentialImpulseConstraintSolver* mSolver = new btSequentialImpulseConstraintSolver;

		btDiscreteDynamicsWorld* mDynamicsWorld;

	};

}