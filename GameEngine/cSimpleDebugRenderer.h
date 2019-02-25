#pragma once
#include "cGameObject.h"
#include "globalOpenGLStuff.h"
class cSimpleDebugRenderer
{
public:
	cSimpleDebugRenderer(cGameObject* mUnitSphere, GLuint IDprogram);

	void drawSphere(glm::vec3 pos, float rad);
	

private:
	cGameObject* mUnitSphere;
	cGameObject* mUnitPlane;
	GLuint mShaderProgram;
};