#include "cGameObject.h"

cGameObject::cGameObject()
{
	this->position = glm::vec3(0.0f);
	this->nonUniformScale = glm::vec3(1.0f);

	//	this->preRotation = glm::vec3(0.0f);
	//	this->postRotation = glm::vec3(0.0f);
	this->setMeshOrientationEulerAngles(glm::vec3(0.0f, 0.0f, 0.0f));

	//	this->m_meshQOrientation = glm::quat( glm::vec3(0.0f,0.0f,0.0f));

		// 
	this->bIsDebug = false;
	this->bIsVisible = true;
	this->bIsWireFrame = false;
	// HACK: See class definition for why this is a hack
	this->b_HACK_UsesOffscreenFBO = false;

	// Set unique ID
	this->m_uniqueID = cGameObject::m_NextID;
	// Increment
	cGameObject::m_NextID++;	// 32 bit - 4 billion

//	this->objColour = glm::vec3(1.0f, 1.0f, 1.0f);	// white by default

	this->materialDiffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	this->materialSpecular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	this->bUseVertexColour = false;

	this->bDontLight = false;

	this->velocity = glm::vec3(0.0f);
	this->accel = glm::vec3(0.0f);
	this->bIsUpdatedByPhysics = false;	// physics ignores by default
	this->bIsUpdatedByPhysics = false;
//	this->radius = 0.0f;

	this->pDebugRenderer = NULL;

	// Set shape to NULL
	this->pTheShape = NULL;
	this->shapeType = cGameObject::UNKOWN_SHAPE;
	this->rigidBody = NULL;



	// Skinned mesh and animations:
	this->pSimpleSkinnedMesh = NULL;
	this->pAniState = NULL;

	return;
}

void cGameObject::setDiffuseColour(glm::vec3 newDiffuse)
{
	this->materialDiffuse = glm::vec4(newDiffuse, this->materialDiffuse.a);
	return;
}

void cGameObject::setAlphaTransparency(float newAlpha)
{
	this->materialDiffuse.a = newAlpha;
	return;
}

void cGameObject::setSpecularColour(glm::vec3 colourRGB)
{
	this->materialSpecular = glm::vec4(colourRGB, this->materialSpecular.a);
	return;
}

void cGameObject::setSpecularPower(float specPower)
{
	this->materialSpecular.a = specPower;
	return;
}


void cGameObject::setUniformScale(float scale)
{
	this->nonUniformScale = glm::vec3(scale, scale, scale);
	return;
}

//static unsigned int m_NextID; //= 0;

//static 
unsigned int cGameObject::m_NextID = cGameObject::STARTING_ID_VALUE;


void cGameObject::Update(double deltaTime)
{
	if (this->bIsUpdatedByPhysics)
	{
		// Figure out a line showing the velocityz
		glm::vec3 vVel = this->velocity + this->position;
		glm::vec3 vAcc = this->accel + this->position;

		if (this->pDebugRenderer)		// != NULL
		{
			this->pDebugRenderer->addLine(this->position, vVel,
				glm::vec3(0.0f, 1.0f, 0.0f), 0.0f);

			this->pDebugRenderer->addLine(this->position, vAcc,
				glm::vec3(0.0f, 1.0f, 1.0f), 0.0f);
		}//if ( this->pDebugRenderer )	
	}//if ( this->bIsUpdatedByPhysics )

	return;
}
