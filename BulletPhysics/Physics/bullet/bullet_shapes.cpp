#include "bullet_shapes.h"

namespace nPhysics
{


	cBulletSphereShape::cBulletSphereShape(float radius)
		: iSphereShape()
		, mRadius(radius)
		
	{
		mBulletShape = new btSphereShape(btScalar(mRadius));
	}
	cBulletSphereShape::~cBulletSphereShape()
	{
		delete mBulletShape;
	}




	cBulletPlaneShape::cBulletPlaneShape(const glm::vec3& normal, float planeConst)
		: iPlaneShape()
		, mNormal(normal)
		, mCnonst(planeConst)

	{
		this->mBulletShape = new btStaticPlaneShape(btVector3(normal.x, normal.y, normal.z), planeConst);
	}
	cBulletPlaneShape::~cBulletPlaneShape()
	{
		delete mBulletShape;
	}


	bool cBulletPlaneShape::GetPlaneNormal(glm::vec3& planeNormalOut)
	{
		planeNormalOut = mNormal;
		return true;
	}
	bool cBulletPlaneShape::GetPlaneConstant(float& planeConstantOut)
	{
		planeConstantOut = mCnonst;
		return true;
	}



}
