#include "CreateFactory.h"
#include "bullet/cBulletPhysicsFactory.h"

EXTERN_DLL_EXPORTS nPhysics::iPhysicsFactory* CreateFactory()
{
	return new nPhysics::cBulletPhysicsFactory();
}