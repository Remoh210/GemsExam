#pragma once
#include <Interfaces/iShape.h>
#include <Interfaces/shapes.h>
#include "btBulletCollisionCommon.h"

namespace nPhysics
{
	
	class cBulletSphereShape : public iSphereShape
	{
	public:
		cBulletSphereShape(float radius);
		virtual ~cBulletSphereShape();
		virtual bool GetSphereRadius(float& radiusOut) { radiusOut = this->mRadius; return true; }
		inline btCollisionShape* GetBulletShape()
		{
			return mBulletShape;
		}
	protected:
		float mRadius;
		cBulletSphereShape(const cBulletSphereShape& other) : iSphereShape(other) {}
		cBulletSphereShape& operator=(const cBulletSphereShape& other) { return *this; }
		btCollisionShape* mBulletShape;

	};

	class cBulletPlaneShape :  public iPlaneShape
	{
	public:
		cBulletPlaneShape(const glm::vec3& normal, float planeConst);
		virtual ~cBulletPlaneShape();
		virtual bool GetPlaneNormal(glm::vec3& planeNormalOut);
		virtual bool GetPlaneConstant(float& planeConstantOut);
		inline btCollisionShape* GetBulletShape()
		{
			return mBulletShape;
		}

	protected:
		glm::vec3 mNormal;
		float mCnonst;
		//iShape
		cBulletPlaneShape(const cBulletPlaneShape& other) : iPlaneShape(other) {}
		cBulletPlaneShape& operator=(const cBulletPlaneShape& other) { return *this; }
		btCollisionShape* mBulletShape;

	};

}