#pragma once
#include <Windows.h>		
#include <process.h>
#include "cGameObject.h" 
#include <vector>




class cDalek
{
public:
	cDalek(cGameObject* dalekOb, std::vector<glm::vec3> vecInBlocks, glm::vec3 vecMS);
	cGameObject* dalekObj;
	glm::vec3 dalekForward;
	float dt;
	void update(float deltaTime);
	CRITICAL_SECTION CR_POSITION;
	
private:
	
	std::vector<glm::vec3> vecBlocks;

	bool checkCube(glm::vec3 pos, std::vector<glm::vec3> vec_pos);
	bool moveOrChangeDir(cGameObject* pDalek, std::vector<glm::vec3> vec_blcok_pos, glm::vec3 dalekforward);
	glm::vec3 getDalekForward(glm::vec3 MF);
	

	glm::vec3 modelForward;
	
	void updatePosition(float dt);
	

};