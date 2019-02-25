#include "cBulletPhysicsWorld.h"
#include "nConvert.h"
#include "cBulletRigidBody.h"


nPhysics::cBulletPhysicsWorld::cBulletPhysicsWorld()
	: iPhysicsWorld()
{
	mCollisionConfiguration = new btDefaultCollisionConfiguration();

	//use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);

	//btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	mOverlappingPairCache = new btDbvtBroadphase();

	//the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	mSolver = new btSequentialImpulseConstraintSolver;

	mDynamicsWorld = new btDiscreteDynamicsWorld(mDispatcher, mOverlappingPairCache, mSolver, mCollisionConfiguration);
	mDynamicsWorld->setGravity(btVector3(0, -10, 0));
}

nPhysics::cBulletPhysicsWorld::~cBulletPhysicsWorld()
{
	if(mDynamicsWorld)
	{
		delete mDynamicsWorld;
		mDynamicsWorld = 0;
	}
	if (mSolver)
	{
		delete mSolver;
		mSolver = 0;
	}
	if (mOverlappingPairCache)
	{
		delete mOverlappingPairCache;
		mOverlappingPairCache = 0;
	}
	if (mDispatcher)
	{
		delete mDispatcher;
		mDispatcher = 0;
	}
	if (mCollisionConfiguration)
	{
		delete mCollisionConfiguration;
		mCollisionConfiguration = 0;
	}
}

void nPhysics::cBulletPhysicsWorld::SetDebugRenderer(iDebugRenderer * debugRenderer)
{
}

void nPhysics::cBulletPhysicsWorld::DrawDebug()
{
}

void nPhysics::cBulletPhysicsWorld::SetGravity(const glm::vec3& gravity)
{
	mDynamicsWorld->setGravity(nConvert::ToBullet(gravity));
}

bool nPhysics::cBulletPhysicsWorld::AddBody(iRigidBody* body)
{
	cBulletRigidBody* bulletRigidBody = dynamic_cast<cBulletRigidBody*>(body);
	if (!bulletRigidBody)
	{
		return false;
	}
	mDynamicsWorld->addRigidBody(bulletRigidBody->GetBulletBody());

	return true;

}

bool nPhysics::cBulletPhysicsWorld::RemoveBody(iRigidBody * body)
{
	cBulletRigidBody* bulletRigidBody = dynamic_cast<cBulletRigidBody*>(body);
	if (!bulletRigidBody)
	{
		return false;
	}
	btRigidBody* BulletBtBody = bulletRigidBody->GetBulletBody();
	mDynamicsWorld->removeRigidBody(BulletBtBody);
	return true;

}

void nPhysics::cBulletPhysicsWorld::Update(float dt)
{
	mDynamicsWorld->stepSimulation(dt, 10);
}
