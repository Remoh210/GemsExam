#pragma once
#include<glm/glm.hpp>
class cCameraChangeAnim
{
public:
	cCameraChangeAnim(const glm::mat4& initialTransform, const glm::mat4 finalTransform, float time);

	void Update(float dt);
	void GetCurTransform(glm::mat4& currentTransform);
	bool IsComplete();

private:
	float mFullTime;
	float mCurTime;
	bool mIsComplited;
	//this is what we travel from, towards to identity mat
	glm::mat4 mInetpolateTransform;
	glm::mat4 mCurTransform;
};