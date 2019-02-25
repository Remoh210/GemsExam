#pragma once
#include <Interfaces/iRigidBody.h>
#include <Interfaces/sRigidBodyDef.h>
#include <btBulletDynamicsCommon.h>


namespace nPhysics
{
	class cBulletRigidBody : public iRigidBody
	{

	public:
		cBulletRigidBody(const sRigidBodyDef& def, iShape* shape);
		~cBulletRigidBody();

		inline btRigidBody* GetBulletBody() { return mBody; }

		virtual iShape* GetShape();
		virtual glm::mat4 GetTransform();
		virtual glm::vec3 GetPosition();
		virtual glm::vec3 GetEulerRotation();
		virtual glm::mat4 GetMatRotation();
		virtual float GetMass();
		virtual glm::vec3 GetVelocity();
		virtual glm::vec3 GetAccel();
		virtual glm::vec3 GetAngulatVelocity();
		virtual void SetPosition(glm::vec3 position);
		virtual void SetEulerRotation(glm::vec3 rotation);
		virtual void SetMatRotation(glm::mat4 rotation);
		virtual void SetMass(float mass);
		virtual void SetVelocity(glm::vec3 velocity);
		virtual void SettAccel(glm::vec3 accel);


		//void SettAccel(glm::vec3 accel);
	private:
		btDefaultMotionState* mMotionState;
		btRigidBody* mBody;
		iShape* mShape;
		float mMass;
	};

}