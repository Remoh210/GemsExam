#include "cBulletRigidBody.h"
#include "nConvert.h"
#include "bullet_shapes.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp>

namespace nPhysics {

	cBulletRigidBody::cBulletRigidBody(const sRigidBodyDef& def, iShape* shape)
		:mShape(shape)
	{

		switch (shape->GetShapeType())
		{
		case nPhysics::SHAPE_TYPE_PLANE:
		{
			btCollisionShape* colShape = dynamic_cast<cBulletPlaneShape*>(shape)->GetBulletShape();
			btTransform startTransform;
			startTransform.setIdentity();
			mMass = def.Mass;
			btScalar mass(mMass);
			bool isDynamic = (mass != 0.f);
			btVector3 localInertia(0, 0, 0);

			if (isDynamic)
			{
				colShape->calculateLocalInertia(mass, localInertia);
			}

			btRigidBody::btRigidBodyConstructionInfo rbInfo(0, 0, colShape, localInertia);
			rbInfo.m_restitution = 0.9;
			rbInfo.m_friction = 10.0;
			mBody = new btRigidBody(rbInfo);
			


			break;
		}
		case nPhysics::SHAPE_TYPE_SPHERE:
		{
			btCollisionShape* colShape = dynamic_cast<cBulletSphereShape*>(shape)->GetBulletShape();
			btTransform startTransform;
			startTransform.setIdentity();
			
			mMass = def.Mass;
			btScalar mass(mMass);
			bool isDynamic = (mass != 0.f);
			btVector3 localInertia(0, 0, 0);

			if (isDynamic)
			{
				colShape->calculateLocalInertia(mass, localInertia);
			}
			startTransform.setOrigin(nConvert::ToBullet(def.Position));
			startTransform.setRotation(nConvert::ToBullet(def.quatOrientation));
			mMotionState = new btDefaultMotionState(startTransform);
			btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, mMotionState, colShape, localInertia);
			//just for now (for character collision)
			rbInfo.m_restitution = 0.0;
			rbInfo.m_friction = 0.0;
			mBody = new btRigidBody(rbInfo);
			mBody->setLinearVelocity(nConvert::ToBullet(def.Velocity));
			mBody->setAngularVelocity(nConvert::ToBullet(def.AngularVelocity));
			//mBody->setAngularFactor(btVector3(0.0f, 1.0f, 0.0f));
			mBody->setSleepingThresholds(0.0f, 0.0f);

			break;
		}
		default:
		{

			break;
		}


		}
	}

	cBulletRigidBody::~cBulletRigidBody()
	{
		delete mBody;
		mBody = 0;
		delete mMotionState;
		mMotionState = 0;
	}

	iShape * cBulletRigidBody::GetShape()
	{
		return mShape;
	}

	glm::mat4 cBulletRigidBody::GetTransform()
	{	
		btTransform tranform;
		mMotionState->getWorldTransform(tranform);	
		return nConvert::ToSimple(tranform);
	}

	glm::vec3 cBulletRigidBody::GetPosition()
	{

		btVector3 btVec = this->mBody->getCenterOfMassPosition();
		return nConvert::ToSimple(btVec);
	}

	glm::vec3 cBulletRigidBody::GetEulerRotation()
	{
		return glm::vec3();
	}

	glm::mat4 cBulletRigidBody::GetMatRotation()
	{
		btQuaternion btQuat = this->mBody->getOrientation();

		glm::quat quatRot = nConvert::ToSimple(btQuat);
		return glm::toMat4(quatRot);
	}

	float cBulletRigidBody::GetMass()
	{
		return this->mMass;
	}

	glm::vec3 cBulletRigidBody::GetVelocity()
	{
		btVector3 vel = this->mBody->getLinearVelocity();
		return nConvert::ToSimple(vel);
	}

	glm::vec3 cBulletRigidBody::GetAccel()
	{
		return glm::vec3();
	}

	glm::vec3 cBulletRigidBody::GetAngulatVelocity()
	{
		return nConvert::ToSimple(mBody->getAngularVelocity());
	}

	void cBulletRigidBody::SetPosition(glm::vec3 position)
	{
		//btVector3 vel = this->mBody->setMotionState()
	}

	void cBulletRigidBody::SetEulerRotation(glm::vec3 rotation)
	{
	}

	void cBulletRigidBody::SetMatRotation(glm::mat4 rotation)
	{
		//btMotionState* state = this->mBody->getMotionState();
		//btTransform startTransform;
		//state->getWorldTransform(startTransform);
		//startTransform.getRotation().m;
		//this->mBody->setMotionState()
	}

	void cBulletRigidBody::SetMass(float mass)
	{
	}

	void cBulletRigidBody::SetVelocity(glm::vec3 velocity)
	{
		//this->mBody->activate();
		this->mBody->setLinearVelocity(nConvert::ToBullet(velocity));
	}
	void cBulletRigidBody::SettAccel(glm::vec3 accel)
	{
	}
}