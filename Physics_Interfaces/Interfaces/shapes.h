#pragma once
#include "iShape.h"
#include <glm/glm.hpp>

namespace nPhysics
{
	class iSphereShape : public iShape
	{
	public:
		virtual ~iSphereShape() {}
		virtual bool GetSphereRadius(float& radiusOut) = 0;

	protected:
		iSphereShape() : iShape(SHAPE_TYPE_SPHERE) {}
		iSphereShape(const iSphereShape& other) : iShape(other) {}
		iSphereShape& operator=(const iSphereShape& other) { return *this; }
	};

	class iPlaneShape : public iShape
	{
	public:
		virtual ~iPlaneShape() {}
		virtual bool GetPlaneConstant(float& planeConstantOut) = 0;
		virtual bool GetPlaneNormal(glm::vec3& planeNormalOut) = 0;

	protected:
		iPlaneShape() : iShape(SHAPE_TYPE_PLANE) {}
		iPlaneShape(const iPlaneShape& other) : iShape(other) {}
		iPlaneShape& operator=(const iPlaneShape& other) { return *this; }
	};
}