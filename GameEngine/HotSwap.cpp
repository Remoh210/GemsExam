#include "globalStuff.h"

void SwapThePhysics()
{
	switch (physics_library)
	{
	case BULLET:
	{
		hGetProckDll = LoadLibraryA("SimplePhysics.dll");
		physics_library = SIMPLE;
		f_createPhysicsFactory CreatePhysicsFactory = (f_createPhysicsFactory)GetProcAddress(hGetProckDll, "CreateFactory");
		gPhysicsFactory = CreatePhysicsFactory();
		gPhysicsWorld = gPhysicsFactory->CreatePhysicsWorld();

		gPhysicsWorld->SetGravity(g_Gravity);
		break;
	}
	case SIMPLE:
	{
		hGetProckDll = LoadLibraryA("BulletPhysics.dll");
		physics_library = BULLET;
		f_createPhysicsFactory CreatePhysicsFactory = (f_createPhysicsFactory)GetProcAddress(hGetProckDll, "CreateFactory");
		gPhysicsFactory = CreatePhysicsFactory();
		gPhysicsWorld = gPhysicsFactory->CreatePhysicsWorld();

		gPhysicsWorld->SetGravity(g_Gravity);
		break;
	}
	default:
		break;
	}
	for (int i = 0; i < vec_pObjectsToDraw.size(); i++)
	{
		cGameObject* CurObj = vec_pObjectsToDraw[i];
		if (CurObj->rigidBody != NULL)
		{
			nPhysics::iShape* CurShape = NULL;
			nPhysics::sRigidBodyDef def;
			def.quatOrientation = CurObj->m_meshQOrientation;
			def.Position = CurObj->position;
			def.AngularVelocity = CurObj->rigidBody->GetAngulatVelocity();
			def.Velocity = CurObj->rigidBody->GetVelocity();

			switch (CurObj->rigidBody->GetShape()->GetShapeType())
			{
			case nPhysics::SHAPE_TYPE_SPHERE:
			{
				float radius = 0.0f;
				CurObj->rigidBody->GetShape()->GetSphereRadius(radius);
				CurShape = gPhysicsFactory->CreateSphereShape(radius);
				def.Mass = CurObj->rigidBody->GetMass();
				break;
			}
			case nPhysics::SHAPE_TYPE_PLANE:
			{
				float planeConst = 0.0f;
				glm::vec3 norm(0.0f);
				CurObj->rigidBody->GetShape()->GetPlaneNormal(norm);
				CurObj->rigidBody->GetShape()->GetPlaneConstant(planeConst);
				CurShape = gPhysicsFactory->CreatePlaneShape(norm, planeConst);
				break;
			}
			default:
				break;
			}

			nPhysics::iRigidBody* rigidBody = gPhysicsFactory->CreateRigidBody(def, CurShape);
			CurObj->rigidBody->~iRigidBody();
			delete CurObj->rigidBody;
			CurObj->rigidBody = rigidBody;
			gPhysicsWorld->AddBody(rigidBody);

		}
	}
	
}