#include "cDalek.h"
#include <iostream>
#include "globalStuff.h"
#include <Windows.h>		
#include <process.h>






DWORD WINAPI UpdateDalekPosition(void* pInitialData)
{

	cDalek* pDalek = (cDalek*)pInitialData;



	while (true)
	{
		
		pDalek->update(pDalek->dt);

	}
	

	return 0;
}




CRITICAL_SECTION CR_POSITION;

cDalek::cDalek(cGameObject * dalekOb, std::vector<glm::vec3> vecInBlocks, glm::vec3 MF)
{
	this->dalekObj = dalekOb;
	this->modelForward = MF;
	this->dalekForward = getDalekForward(modelForward);
	this->vecBlocks = vecInBlocks;
	this->dt = 0.f;

	LPDWORD phThread = 0;	// Clear to zero
	DWORD hThread = 0;
	HANDLE hThreadHandle = 0;
	InitializeCriticalSection(&CR_POSITION);

	// Pass a pointer to this instance 
	// Recal that the "this" pointer is the pointer to
	//	this particular instance of the object
	void* pDalek = (void*)(this);


	hThreadHandle = CreateThread(NULL,	// Default security
		0,		// Stack size - default - win32 = 1 Mbyte
		&UpdateDalekPosition, // Pointer to the thread's function
		pDalek,		// The value (parameter) we pass to the thread
			// This is a pointer to void... more on this evil thing later...
		0,  // CREATE_SUSPENDED or 0 for threads...
		(DWORD*)&phThread);		// pointer or ref to variable that will get loaded with threadID


	





}

void cDalek::update(float deltaTime)
{
	//this->dt = deltaTime;
	this->dalekForward = getDalekForward(modelForward);

	if (moveOrChangeDir(dalekObj, vecBlocks, dalekForward))
	{
		updatePosition(deltaTime);
		//Sleep(1000);
		
	}
	else
	{
		float r = ((float)rand() / (RAND_MAX));
		std::cout << "random: " << r << std::endl;
		if (r > 0.5)
		{
			dalekObj->adjMeshOrientationEulerAngles(glm::vec3(0.0f, -90.0f, 0.0f), true);
			std::cout << "rotating: -90" << r << std::endl;
		}
		else
		{
			dalekObj->adjMeshOrientationEulerAngles(glm::vec3(0.0f, 90.0f, 0.0f), true);
			//std::cout << "rotating: 90" << r << std::endl;
		}
		Sleep(1000);
	}


}



bool cDalek::checkCube(glm::vec3 pos, std::vector<glm::vec3> vec_pos)
{
	for (int i = 0; i < vec_pos.size(); i++)
	{
		float dist = glm::distance(pos, vec_pos[i]);
		if (dist < 10.0f)
		{
			return true;
		}
	}
	return false;
}

bool cDalek::moveOrChangeDir(cGameObject* pDalek, std::vector<glm::vec3> vec_blcok_pos, glm::vec3 dalekforward)
{
	float cubeSize = 20.0f;
	glm::vec3 forwad_1_unit = pDalek->position + dalekforward * cubeSize / 2.0f;
	if (cDalek::checkCube(forwad_1_unit, vec_blcok_pos))
	{
		return false;
	}
	else
	{
		return true;
	}
}

glm::vec3 cDalek::getDalekForward(glm::vec3 MF)
{
	glm::vec3 DalekForward = glm::vec3(0.0f);
	glm::vec4 vecForwardDirection_ModelSpace = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

	glm::quat charkRotation = this->dalekObj->getQOrientation();
	glm::mat4 matCharkRotation = glm::mat4(charkRotation);
	DalekForward = matCharkRotation * vecForwardDirection_ModelSpace;

	return glm::vec3(DalekForward);

}

void cDalek::updatePosition(float dt)
{
	float step = 0.1f * dt;
	//EnterCriticalSection(&CR_POSITION);
	dalekObj->position += dalekForward * step;
	LeaveCriticalSection(&CR_POSITION);
}
