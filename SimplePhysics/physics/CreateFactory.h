#pragma once
#include <interfaces/iPhysicsFactory.h>

#define EXTERN_DLL_EXPORTS extern "C" __declspec(dllexport)


EXTERN_DLL_EXPORTS nPhysics::iPhysicsFactory* CreateFactory();