#pragma once
#include <glm\vec3.hpp>
#include <Interfaces\shapes.h>

namespace nPhysics
{
	class cSphereShape : public iSphereShape
	{
	public:
		cSphereShape(float radius);

		virtual ~cSphereShape();

		virtual bool GetSphereRadius(float& radiusOut) { radiusOut = this->mRadius; return true; }

	private:
		cSphereShape();
		cSphereShape(const cSphereShape& other);
		cSphereShape& operator=(const cSphereShape& other);

		float mRadius;
	};

	class cPlaneShape : public iPlaneShape
	{
	public:

		cPlaneShape(const glm::vec3& normal, float planeConst);

		virtual ~cPlaneShape();

		virtual bool GetPlaneConstant(float& planeConstantOut) { planeConstantOut = this->mPlaneConst; return true; };
		virtual bool GetPlaneNormal(glm::vec3& planeNormalOut) { planeNormalOut = this->mNormal; return true; };

	private:
		cPlaneShape();
		cPlaneShape(const cPlaneShape& other);
		cPlaneShape& operator=(const cPlaneShape& other);

		glm::vec3 mNormal;
		float mPlaneConst;
		//PlaneType type;
	};
}
