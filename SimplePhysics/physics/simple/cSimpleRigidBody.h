#pragma once
#include <Interfaces/iRigidBody.h>
#include <Interfaces/sRigidBodyDef.h>
#include <Interfaces/iShape.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#define GLM_ENABLE_EXPERIMENTAL		
#include <glm/gtx/quaternion.hpp>

	

namespace nPhysics
{
	class cSimpleRigidBody : public iRigidBody
	{
	public:
		cSimpleRigidBody(const sRigidBodyDef& def, iShape* shape);
		virtual ~cSimpleRigidBody();

		virtual iShape* GetShape();

		virtual glm::mat4 GetTransform();
		virtual glm::vec3 GetPosition();
		virtual glm::vec3 GetEulerRotation();
		virtual glm::mat4 GetMatRotation();
		virtual float GetMass();
		virtual glm::vec3 GetVelocity();
		virtual glm::vec3 GetAccel();
		virtual glm::vec3 GetAngulatVelocity();

		//virtual void SetTransform(glm::mat4 transform);
		//virtual void ApplyForce(const glm::vec3 force);
		virtual void SetPosition(glm::vec3 position);
		virtual void SetEulerRotation(glm::vec3 rotation);
		virtual void SetMatRotation(glm::mat4 rotation);
		virtual void SetMass(float mass);
		virtual void SetVelocity(glm::vec3 velocity);
		virtual void SettAccel(glm::vec3 accel);

		
		

	protected:
		//cSimpleRigidBody(const sRigidBodyDef& def, iShape* shape);
		cSimpleRigidBody();
		cSimpleRigidBody(const cSimpleRigidBody& other);
		cSimpleRigidBody& operator = (const cSimpleRigidBody& other);
	private:
		friend class cSimplePhysicsWorld;
		iShape* mShape;
		glm::vec3 mPosition;
		glm::vec3 mVelocity;
		glm::mat4 mRotation;
		glm::vec3 mAcceleration;
		glm::vec3 mLastPos;
		float mMass;
		glm::vec3 mAnguralVel;
		//float mInverseMass;
	};
}