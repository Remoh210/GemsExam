//#include "CameraChangeanimr.h"
//
//#include "globalOpenGLStuff.h"
//
//
//cCameraChangeAnim::cCameraChangeAnim(const glm::mat4& initialTransform, const glm::mat4 finalTransform, float time)
//	:mCurTime(0.f)
//	,mFullTime(time),
//	mIsComplited(false)
//{
//	mFullTime = time;
//	mInetpolateTransform = glm::inverse(finalTransform) * initialTransform;
//}
//
//	void cCameraChangeAnim::Update(float dt)
//	{
//		if (mIsComplited)
//		{
//			return;
//		}
//		mCurTime += dt;
//		if (mCurTime >= mFullTime)
//		{
//			mIsComplited = true;
//			mCurTime = mFullTime;
//			mCurTransform = glm::mat4(1.0f);
//		}
//		else
//		{
//			float tRatio = mCurTime / mFullTime;
//			mCurTransform = glm::interpolate(mInetpolateTransform, glm::mat4(1.0f), tRatio);
//		}
//	}
//	void cCameraChangeAnim::GetCurTransform(glm::mat4& currentTransform) {
//		currentTransform = mCurTransform;
//	}
//	bool cCameraChangeAnim::IsComplete();
//
