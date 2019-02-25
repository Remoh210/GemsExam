//#include "stdafx.h"
#include "shape.h"

namespace nPhysics
{
	cSphereShape::cSphereShape(float radius)
		: iSphereShape()
		, mRadius(radius)
	{

	}
	cSphereShape::cSphereShape()
		: iSphereShape()
	{

	}

	cSphereShape::cSphereShape(const cSphereShape& other)
		: iSphereShape()
	{

	}
	cSphereShape& cSphereShape::operator=(const cSphereShape& other)
	{
		return *this;
	}
	cSphereShape::~cSphereShape()
	{

	}
	//float cSphereShape::GetRadius()
	//{
	//	return mRadius;
	//}






	cPlaneShape::cPlaneShape(const glm::vec3& normal, float planeConst)
		: iPlaneShape()
		, mNormal(normal)
		, mPlaneConst(planeConst)
	{

	}
	cPlaneShape::cPlaneShape()
		: iPlaneShape()
	{

	}

	cPlaneShape::cPlaneShape(const cPlaneShape& other)
		: iPlaneShape()
	{

	}
	cPlaneShape& cPlaneShape::operator=(const cPlaneShape& other)
	{
		return *this;
	}
	cPlaneShape::~cPlaneShape()
	{

	}

	//float cPlaneShape::GetPlaneConstant()
	//{
	//	return this->mPlaneConst;
	//}

	//glm::vec3 cPlaneShape::GetPlaneNormal()
	//{
	//	return this->mNormal;
	//}
	//bool cPlaneShape::getPlaneType(PlaneType& type)
	//{
	//	type = this->type;
	//	return true;
	//}
	//bool cPlaneShape::setGameType(const PlaneType& type)
	//{
	//	this->type = type;
	//	return true;
	//}
	//bool cPlaneShape::GetPlaneNormal(glm::vec3& normalOut)
	//{
	//	normalOut = mNormal;
	//	return true;
	//}
	//bool cPlaneShape::GetPlaneConst(float& planeConstOut)
	//{
	//	planeConstOut = mPlaneConst;
	//	return true;
	//}
}