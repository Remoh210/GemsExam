#include "cLightManager.h"



cLightManager::cLightManager()
{
	//intilization stufff
}

void cLightManager::TurnOnLight(int index)
{


}

sLight::sLight() {

	ObjectRelativeTo = NULL;
	ObjectLookAt = NULL;

}

void sLight::SetLightType(std::string typeAsString)
{
	if (typeAsString == "POINT_LIGHT")
	{
		this->param1.x = 0.0f;		// Point
		m_lightType = POINT_LIGHT;
	}
	else if (typeAsString == "SPOT_LIGHT")
	{
		this->param1.x = 1.0f;		// Point
		m_lightType = SPOT_LIGHT;
	}
	else if (typeAsString == "DIRECTIONAL_LIGHT")
	{
		this->param1.x = 2.0f;		// Point
		m_lightType = DIRECTIONAL_LIGHT;
	}
	else 
	{
		this->param1.x = 0.0f;		// Point
		m_lightType = POINT_LIGHT;
	}
	

	return;
}

sLight::eLightType sLight::GetLightType_enum(void)
{
	
	return m_lightType;
}

std::string sLight::GetLightType_str(void)
{

	switch (this->m_lightType)
	{
	case sLight::POINT_LIGHT:
		return "POINT_LIGHT";
	case sLight::SPOT_LIGHT:
		return "SPOT_LIGHT";
	case sLight::DIRECTIONAL_LIGHT:
		return "DIRECTIONAL_LIGHT";
	default:
		return "POINT_LIGHT";
	};
}


void sLight::SetLightType(sLight::eLightType lightType)
{
	switch (lightType)
	{
	case sLight::POINT_LIGHT:
		this->param1.x = 0.0f;		// Point
		m_lightType = POINT_LIGHT;
		break;
	case sLight::SPOT_LIGHT:
		this->param1.x = 1.0f;		// Spot
		m_lightType = SPOT_LIGHT;
		break;
	case sLight::DIRECTIONAL_LIGHT:
		this->param1.x = 2.0f;		// Directional
		m_lightType = DIRECTIONAL_LIGHT;
		break;
	default:
		// Make point if we don't know
		// (shouldn't happen)
		m_lightType = POINT_LIGHT;
		this->param1.x = 0.0f;		// Point
		break;
	};

	return;
}

void sLight::SetSpotConeAngles(float innerAngleDegrees, float outerAngleDegrees)
{
	//if ( outerAngleDegrees > innerAngleDegrees )
	//{	// Switch them
	//	float temp = innerAngleDegrees;
	//	innerAngleDegrees = outerAngleDegrees;
	//	outerAngleDegrees = temp;
	//}

	//	glm::vec4 param1;		// x = lightType, y = inner angle, z = outer angle, w = TBD
	this->param1.y = innerAngleDegrees;
	this->param1.z = outerAngleDegrees;

	return;
}

void sLight::SetRelativeDirection(glm::vec3 relDirection)
{
	// To set the vec4 that's being passed
	this->direction = glm::vec4(relDirection, 1.0f);
	return;
}

void sLight::SetRelativeDirectionByEulerAngles(glm::vec3 directionAngle)
{
	// Take the angles and make a quaternion out of them
	// Use a mat4x4 x vec4 tranformation (just like the shader or in physics)
	// Use the final xyz location to send to SetRelativeDirectionByLookAt()

	// Ta-Da!

	return;
}

void sLight::SetRelativeDirectionByLookAt(glm::vec3 pointInWorldXYZ)
{
	// The vector from what I'm looking at to where I am, then normalize

	glm::vec3 lookVector = pointInWorldXYZ - glm::vec3(this->position);

	lookVector = glm::normalize(lookVector);

	this->SetRelativeDirection(lookVector);

	return;
}

void sLight::SetRelativeDirectionByLookAt(cGameObject* lookAtObj)
{
	// The vector from what I'm looking at to where I am, then normalize
	this->ObjectLookAt = lookAtObj;
	glm::vec3 lookVector = ObjectLookAt->position - glm::vec3(this->position);

	lookVector = glm::normalize(lookVector);

	this->SetRelativeDirection(lookVector);

	return;
}


void cLightManager::LoadUniformLocations(int shaderID)
{
	int i = 0;

	for (std::vector<sLight*>::iterator it = vecLights.begin(); it != vecLights.end(); ++it)
	{
		vecLights[i]->position_UniLoc = glGetUniformLocation(shaderID, ("theLights[" + std::to_string(i) + "].position").c_str());
		vecLights[i]->diffuse_UniLoc = glGetUniformLocation(shaderID, ("theLights[" + std::to_string(i) + "].diffuse").c_str());
		vecLights[i]->specular_UniLoc = glGetUniformLocation(shaderID, ("theLights[" + std::to_string(i) + "].specular").c_str());
		vecLights[i]->atten_UniLoc = glGetUniformLocation(shaderID, ("theLights[" + std::to_string(i) + "].atten").c_str());
		vecLights[i]->direction_UniLoc = glGetUniformLocation(shaderID, ("theLights[" + std::to_string(i) + "].direction").c_str());
		vecLights[i]->param1_UniLoc = glGetUniformLocation(shaderID, ("theLights[" + std::to_string(i) + "].param1").c_str());
		vecLights[i]->param2_UniLoc = glGetUniformLocation(shaderID, ("theLights[" + std::to_string(i) + "].param2").c_str());

		i++;
	}
}

void cLightManager::CopyLightValuesToShader(void)
{
	for (std::vector<sLight*>::iterator it = vecLights.begin(); it != vecLights.end(); ++it)
	{
		sLight* CurLight = *it;
		glUniform4f(CurLight->position_UniLoc,
			CurLight->position.x, CurLight->position.y, CurLight->position.z, 1.0f);
		glUniform4f(CurLight->diffuse_UniLoc,
			CurLight->diffuse.x, CurLight->diffuse.y, CurLight->diffuse.z, 1.0f);
		glUniform4f(CurLight->direction_UniLoc, CurLight->direction.x, CurLight->direction.y,
			CurLight->direction.z, CurLight->direction.w);
		glUniform4f(CurLight->param1_UniLoc, CurLight->param1.x, CurLight->param1.y, CurLight->param1.z, CurLight->param1.w);
		glUniform4f(CurLight->param2_UniLoc, CurLight->param2.x, 0.0f, 0.0f, 0.0f);	// Turns it "on")
		glUniform4f(CurLight->atten_UniLoc,
			CurLight->atten.x, CurLight->atten.y, CurLight->atten.z, CurLight->atten.w);
	}
}


