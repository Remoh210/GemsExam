#pragma once
#include "physics_interfaces.h"
#include <vector>
#include "cSimpleRigidBody.h"


namespace nPhysics
{
	class cSimplePhysicsWorld : public iPhysicsWorld {
	public:



		struct RK4_Derivative
		{
			RK4_Derivative() :
				dx(0.0f), dv(0.0f)
			{
			}
			glm::vec3 dx; // dx/dt = velocity
			glm::vec3 dv; // dv/dt = acceleration
		};

		struct RK4_State
		{
			glm::vec3 x;      // position
			glm::vec3 v;      // velocity
		};

		RK4_Derivative evaluate(const RK4_State& initial, float dt, const RK4_Derivative& d);
		void integrate(glm::vec3& pos, glm::vec3& vel, glm::vec3 accel, float dt);

		cSimplePhysicsWorld();
		virtual ~cSimplePhysicsWorld();
		virtual void SetDebugRenderer(iDebugRenderer* debugRenderer);
		virtual void DrawDebug();
		virtual void SetGravity(const glm::vec3& gravity);
		virtual bool AddBody(iRigidBody* body);
		virtual bool RemoveBody(iRigidBody* body);
		virtual void Update(float dt);

	private:
		iDebugRenderer* mDebugRenderer;
		bool CollisionTest(cSimpleRigidBody* bodyA, cSimpleRigidBody* bodyB);
		glm::vec3 mGravity;
		std::vector<cSimpleRigidBody*> mBodies;

	};
}