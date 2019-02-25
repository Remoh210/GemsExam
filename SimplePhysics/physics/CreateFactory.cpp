#include "CreateFactory.h"
#include "simple/cSimplePhysicsFactory.h"

EXTERN_DLL_EXPORTS nPhysics::iPhysicsFactory* CreateFactory()
{
	return new nPhysics::cPhysicsFactory();
}