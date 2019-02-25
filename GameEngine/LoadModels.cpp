#include "globalOpenGLStuff.h"
#include "globalStuff.h"	
#include "cAABB.h"// for g_pRogerRabbit

#include "cVAOMeshManager.h"
#include "cGameObject.h"

#include "DebugRenderer/cDebugRenderer.h"
#include "cAnimationState.h"

#include <iostream>


extern cSimpleAssimpSkinnedMesh* g_pRPGSkinnedMesh = NULL;

bool AssimpSM_to_VAO_Converter(cSimpleAssimpSkinnedMesh* pTheAssimpSM,
	unsigned int shaderProgramID)
{
	sModelDrawInfo theSMDrawInfo;

	theSMDrawInfo.meshFileName = pTheAssimpSM->fileName;
	theSMDrawInfo.friendlyName = pTheAssimpSM->friendlyName;

	// Copy the data from assimp format into the sModelDrawInfo format...

	theSMDrawInfo.numberOfVertices = pTheAssimpSM->pScene->mMeshes[0]->mNumVertices;
	theSMDrawInfo.numberOfTriangles = pTheAssimpSM->pScene->mMeshes[0]->mNumFaces;

	// We used the "triangulate" option when loading so all the primitives
	//	will be triangles, but BEWARE!
	theSMDrawInfo.numberOfIndices = theSMDrawInfo.numberOfTriangles * 3;


	// Allocate the vertex array (it's a c-style array)
	theSMDrawInfo.pMeshData = new cMesh();

	theSMDrawInfo.pMeshData->pVertices = new sVertex_xyz_rgba_n_uv2_bt_4Bones[theSMDrawInfo.numberOfVertices];

	// Danger Will Robinson! 
	// You don't really need to do this, but at least it will clear it to zero.
	// (compiler will complain that it's 'not safe', etc.)
	memset(theSMDrawInfo.pMeshData->pVertices, 0,
		sizeof(sVertex_xyz_rgba_n_uv2_bt_4Bones) * theSMDrawInfo.numberOfVertices);


	for (unsigned int vertIndex = 0; vertIndex != theSMDrawInfo.numberOfVertices; vertIndex++)
	{
		theSMDrawInfo.pMeshData->pVertices[vertIndex].x
			= pTheAssimpSM->pScene->mMeshes[0]->mVertices[vertIndex].x;
		theSMDrawInfo.pMeshData->pVertices[vertIndex].y
			= pTheAssimpSM->pScene->mMeshes[0]->mVertices[vertIndex].y;
		theSMDrawInfo.pMeshData->pVertices[vertIndex].z
			= pTheAssimpSM->pScene->mMeshes[0]->mVertices[vertIndex].z;

		// Normals... 
		theSMDrawInfo.pMeshData->pVertices[vertIndex].nx
			= pTheAssimpSM->pScene->mMeshes[0]->mNormals[vertIndex].x;
		theSMDrawInfo.pMeshData->pVertices[vertIndex].ny
			= pTheAssimpSM->pScene->mMeshes[0]->mNormals[vertIndex].y;
		theSMDrawInfo.pMeshData->pVertices[vertIndex].nz
			= pTheAssimpSM->pScene->mMeshes[0]->mNormals[vertIndex].z;

		// Colours... 
		// (If there are no colours, make it hit pink)
		// Note: the method is because you could have more than one set of 
		//	vertex colours in the model (Why? Who the heck knows?) 
		if (pTheAssimpSM->pScene->mMeshes[0]->HasVertexColors(0))
		{
			theSMDrawInfo.pMeshData->pVertices[vertIndex].r
				= pTheAssimpSM->pScene->mMeshes[0]->mColors[vertIndex]->r;
			theSMDrawInfo.pMeshData->pVertices[vertIndex].g
				= pTheAssimpSM->pScene->mMeshes[0]->mColors[vertIndex]->g;
			theSMDrawInfo.pMeshData->pVertices[vertIndex].b
				= pTheAssimpSM->pScene->mMeshes[0]->mColors[vertIndex]->b;
			theSMDrawInfo.pMeshData->pVertices[vertIndex].a
				= pTheAssimpSM->pScene->mMeshes[0]->mColors[vertIndex]->a;
		}
		else
		{	// hotpink	#FF69B4	rgb(255,105,180)
			theSMDrawInfo.pMeshData->pVertices[vertIndex].r = 1.0f;
			theSMDrawInfo.pMeshData->pVertices[vertIndex].g = 105.0f / 255.0f;
			theSMDrawInfo.pMeshData->pVertices[vertIndex].b = 180.0f / 255.0f;
			theSMDrawInfo.pMeshData->pVertices[vertIndex].a = 1.0f;
		}

		//bi-normal  (or bi-tangent)
		theSMDrawInfo.pMeshData->pVertices[vertIndex].bx
			= pTheAssimpSM->pScene->mMeshes[0]->mBitangents[vertIndex].x;
		theSMDrawInfo.pMeshData->pVertices[vertIndex].by
			= pTheAssimpSM->pScene->mMeshes[0]->mBitangents[vertIndex].y;
		theSMDrawInfo.pMeshData->pVertices[vertIndex].bz
			= pTheAssimpSM->pScene->mMeshes[0]->mBitangents[vertIndex].z;

		// Tangent
		theSMDrawInfo.pMeshData->pVertices[vertIndex].tx
			= pTheAssimpSM->pScene->mMeshes[0]->mTangents[vertIndex].x;
		theSMDrawInfo.pMeshData->pVertices[vertIndex].ty
			= pTheAssimpSM->pScene->mMeshes[0]->mTangents[vertIndex].y;
		theSMDrawInfo.pMeshData->pVertices[vertIndex].tz
			= pTheAssimpSM->pScene->mMeshes[0]->mTangents[vertIndex].z;

		// uv2 (which are odd in assimp)
		// Note that there is an array of texture coordinates, 
		// up to 8 (actually). Usually, there's only 1
		if (pTheAssimpSM->pScene->mMeshes[0]->HasTextureCoords(0))	// 1st UV coords
		{
			// Assume there's 1... (at least)
			theSMDrawInfo.pMeshData->pVertices[vertIndex].u0
				= pTheAssimpSM->pScene->mMeshes[0]->mTextureCoords[0][vertIndex].x;
			theSMDrawInfo.pMeshData->pVertices[vertIndex].v0
				= pTheAssimpSM->pScene->mMeshes[0]->mTextureCoords[0][vertIndex].y;
		}
		if (pTheAssimpSM->pScene->mMeshes[0]->HasTextureCoords(1))	// 2nd UV coords
		{
			theSMDrawInfo.pMeshData->pVertices[vertIndex].u0
				= pTheAssimpSM->pScene->mMeshes[0]->mTextureCoords[1][vertIndex].x;
			theSMDrawInfo.pMeshData->pVertices[vertIndex].v0
				= pTheAssimpSM->pScene->mMeshes[0]->mTextureCoords[1][vertIndex].y;
		}
		// TODO: add additional texture coordinates (mTextureCoords[1], etc.)


		// 4Bones: ids and weights
		theSMDrawInfo.pMeshData->pVertices[vertIndex].boneID[0] = pTheAssimpSM->vecVertexBoneData[vertIndex].ids[0];
		theSMDrawInfo.pMeshData->pVertices[vertIndex].boneID[1] = pTheAssimpSM->vecVertexBoneData[vertIndex].ids[1];
		theSMDrawInfo.pMeshData->pVertices[vertIndex].boneID[2] = pTheAssimpSM->vecVertexBoneData[vertIndex].ids[2];
		theSMDrawInfo.pMeshData->pVertices[vertIndex].boneID[3] = pTheAssimpSM->vecVertexBoneData[vertIndex].ids[3];

		theSMDrawInfo.pMeshData->pVertices[vertIndex].boneWeights[0] = pTheAssimpSM->vecVertexBoneData[vertIndex].weights[0];
		theSMDrawInfo.pMeshData->pVertices[vertIndex].boneWeights[1] = pTheAssimpSM->vecVertexBoneData[vertIndex].weights[1];
		theSMDrawInfo.pMeshData->pVertices[vertIndex].boneWeights[2] = pTheAssimpSM->vecVertexBoneData[vertIndex].weights[2];
		theSMDrawInfo.pMeshData->pVertices[vertIndex].boneWeights[3] = pTheAssimpSM->vecVertexBoneData[vertIndex].weights[3];


	}//for ( unsigned int vertIndex = 0;

	// And the triangles

	// Allocate the array to hold the indices (triangle) info


	// Allocate the array for that (indices NOT triangles)
	theSMDrawInfo.pMeshData->pIndices = new unsigned int[theSMDrawInfo.numberOfIndices];

	// Danger Will Robinson! 
	// You don't really need to do this, but at least it will clear it to zero.
	// (compiler will complain that it's 'not safe', etc.)
	memset(theSMDrawInfo.pMeshData->pIndices, 0,
		sizeof(unsigned int) * theSMDrawInfo.numberOfIndices);


	unsigned int numTriangles = pTheAssimpSM->pScene->mMeshes[0]->mNumFaces;
	unsigned int triIndex = 0;		// Steps through the triangles.
	unsigned int indexIndex = 0;	// Setps through the indices (index buffer)
	for (; triIndex != theSMDrawInfo.numberOfTriangles;
		triIndex++, indexIndex += 3)	// Note, every 1 triangle = 3 index steps
	{
		// Get the triangle at this triangle index...
		aiFace* pAIFace = &(pTheAssimpSM->pScene->mMeshes[0]->mFaces[triIndex]);

		theSMDrawInfo.pMeshData->pIndices[indexIndex + 0]				// Offset by 0 (zero)
			= pAIFace->mIndices[0];	// vertex 0

		theSMDrawInfo.pMeshData->pIndices[indexIndex + 1]				// Offset by 1
			= pAIFace->mIndices[1];	// vertex 1

		theSMDrawInfo.pMeshData->pIndices[indexIndex + 2]				// Offset by 2
			= pAIFace->mIndices[2];	// vertex 1
	}//for ( ; triIndex != numVertices; 


	// Calculate the extents on the mesh
	// (Note, because I'm a bone head, this is dupicated...)
	theSMDrawInfo.pMeshData->name = theSMDrawInfo.friendlyName;
	theSMDrawInfo.pMeshData->numberOfIndices = theSMDrawInfo.numberOfIndices;
	theSMDrawInfo.pMeshData->numberOfTriangles = theSMDrawInfo.numberOfTriangles;
	theSMDrawInfo.pMeshData->numberOfVertices = theSMDrawInfo.numberOfVertices;
	theSMDrawInfo.pMeshData->CalculateExtents();

	// ...then pass it to the mesh manager

	::g_pTheVAOMeshManager->LoadModelIntoVAO(theSMDrawInfo, shaderProgramID);
	return true;
}

void LoadSkinnedMeshModel(std::vector<cGameObject*> &vec_pObjectsToDraw,
	GLuint shaderProgramID)
{
//	::g_pRPGSkinnedMesh = new cSimpleAssimpSkinnedMesh();
//
//	//if ( ! ::g_pRPGSkinnedMesh->LoadMeshFromFile( "assets/modelsMD5monsters/hellknight/attack2.md5anim" ) ) 
//	//{
//	//	std::cout << "Didn't load the army pilot" << std::endl;
//	//}
//
//	//if ( ! ::g_pRPGSkinnedMesh->LoadMeshFromFile( "assets/modelsFBX/ArmyPilot(FBX2013).fbx" ) ) 
//	//{
//	//	std::cout << "Didn't load the army pilot" << std::endl;
//	//}
//	//	if ( ! ::g_pSkinnedMesh01->LoadMeshFromFile( "assets/modelsFBX/RPG-Character(FBX2013).FBX" ) ) 
////	if ( ! ::g_pSkinnedMesh01->LoadMeshFromFile( "assets/modelsFBX/RPG-Character_Unarmed-Attack-Kick-L1(FBX2013).FBX" ) ) 
////	if ( ! ::g_pSkinnedMesh01->LoadMeshFromFile( "assets/modelsFBX/RPG-Character_Unarmed-Walk(FBX2013).FBX" ) ) 
////	if ( ! ::g_pSkinnedMesh01->LoadMeshFromFile( "assets/modelsFBX/RPG-Character_Unarmed-Idle(FBX2013).fbx" ) ) 
////	if ( ! ::g_pSkinnedMesh01->LoadMeshFromFile( "assets/modelsMD5/hellknight/attack2.md5anim" ) ) 
////	if ( ! ::g_pSkinnedMesh01->LoadMeshFromFile( "assets/modelsFBX/RPG-Character_Unarmed-Fall(FBX2013).fbx" ) ) 
//
//	//if (!::g_pRPGSkinnedMesh->LoadMeshFromFile("RPG-Character", "assets/modelsFBX/kachujin_g_rosales.fbx"))
//	//{
//	//	std::cout << "Error: problem loading the skinned mesh" << std::endl;
//	//}
//	std::vector<std::string> vecBoneNames;
//	::g_pRPGSkinnedMesh->GetListOfBoneIDandNames(vecBoneNames);
//	//::g_pRPGSkinnedMesh->pScene->mAnimations[0] = NULL;
//
//	//// Now load another animation file... 
//	//::g_pRPGSkinnedMesh->LoadMeshAnimation("Idle", "assets/modelsFBX/test_idle.fbx");
//	//::g_pRPGSkinnedMesh->LoadMeshAnimation("Unarmed-Attack-Kick-L1", "assets/modelsFBX/kick.dae");
//	::g_pRPGSkinnedMesh->LoadMeshAnimation("Unarmed-Walk", "assets/modelsFBX/Walking.fbx");
//	//::g_pRPGSkinnedMesh->LoadMeshAnimation("Unarmed-Fall", "assets/modelsFBX/Arms Hip Hop Dance.fbx");
//	//::g_pRPGSkinnedMesh->LoadMeshAnimation("Roll-Backward", "assets/modelsFBX/RPG-Character_Unarmed-Roll-Backward(FBX2013).fbx");
//	//::g_pRPGSkinnedMesh->LoadMeshAnimation("Roll-Forwards", "assets/modelsFBX/RPG-Character_Unarmed-Roll-Forward(FBX2013).FBX");
//	//::g_pRPGSkinnedMesh->LoadMeshAnimation("Roll-Left", "assets/modelsFBX/RPG-Character_Unarmed-Roll-Left(FBX2013).FBX");
//	//::g_pRPGSkinnedMesh->LoadMeshAnimation("Roll-Right", "assets/modelsFBX/RPG-Character_Unarmed-Roll-Right(FBX2013).FBX");
//	//::g_pRPGSkinnedMesh->LoadMeshAnimation("Unarmed-Jump", "assets/modelsFBX/RPG-Character_Unarmed-Jump(FBX2013).FBX");
//
//	::g_pRPGSkinnedMesh->friendlyName = "RPG-Character";
//
//	cMesh* pTheMesh = ::g_pRPGSkinnedMesh->CreateMeshObjectFromCurrentModel();
//
//	if (pTheMesh)
//	{
//		std::cout << "Mesh got loaded" << std::endl;
//	}
//	else
//	{
//		std::cout << "Didn't load the skinned mesh model" << std::endl;
//	}
//
//	// Copy the mesh information from assimp into our cMesh object, 
//	// then into the sModelDrawInfo thing, and pass to the VAOManager
//
//	if (!AssimpSM_to_VAO_Converter(::g_pRPGSkinnedMesh, shaderProgramID))
//	{
//		std::cout << "Error: Didn't copy the skinned mesh into the VAO format." << std::endl;
//	}
//	else
//	{
//		std::cout << "Copied the skinned mesh into the VAO format" << std::endl;
//
//		// Add this mesh model into the "models to draw" vector
//
//		{	// Bind pose Skinned Mesh object
//			cGameObject* pTestSM = new cGameObject();
//			pTestSM->setDiffuseColour(glm::vec3(0.0f, 0.0f, 0.0f));	// Yellow
//			//pTestSM->setAlphaTransparency(1.0f);
//			//pTestSM->setMeshOrientationEulerAngles(-90.0f, 0.0f, 0.0f, true);
//			pTestSM->setSpecularPower(100.0f);
//			pTestSM->friendlyName = "SM_Object";
//			pTestSM->position = glm::vec3(0.0f, 0.0f, 0.0f);
//			pTestSM->setUniformScale(0.10f);
//			//pTestSM->meshName = "earth.ply";
//
//			// Wireframe, to make it easier to see (for now)
////			pTestSM->bIsWireFrame = true;
////			pTestSM->bDontLight = true;
//
//			// Set this mesh to the skinned mesh object
//			pTestSM->pSimpleSkinnedMesh = ::g_pRPGSkinnedMesh;
//			// HACK
//			//pTestSM->currentAnimation = "assets/modelsFBX/RPG-Character_Unarmed-Idle(FBX2013).fbx";
////			pTestSM->currentAnimation = "Idle";
//
//			cAnimationState* pAniState;
//			pTestSM->pAniState = new cAnimationState();
//
//			pTestSM->pAniState->defaultAnimation.name = "Unarmed-Walk";
//			pTestSM->currentAnimation = "Unarmed-Walk";
//
//			sTextureInfo testObjectTexture;
//			testObjectTexture.name = "metal.bmp";
//			testObjectTexture.strength = 1.0f;
//
//			pTestSM->vecTextures.push_back(sTextureInfo(testObjectTexture));
//
//			vec_pObjectsToDraw.push_back(pTestSM);
//		}
//	}//if ( ! AssimpSM_to_VAO_Converter(
//
//	//if ( pTheMesh )
//	//{
//	//	if ( ! pVAOManager->loadMeshIntoVAO( *pTheMesh, shaderID, false ) )
//	//	{
//	//		std::cout << "Could not load skinned mesh model into new VAO" << std::endl;
//	//	}
//	//}
//	//else
//	//{
//	//	std::cout << "Could not create a cMesh object from skinned mesh file" << std::endl;
//	//}
//	//// Delete temporary mesh if still around
//	//if ( pTheMesh )
//	//{
//	//	delete pTheMesh;
//	//}
//

	return;
}


// Loading models was moved into this function
void LoadModelTypes( cVAOMeshManager* pTheVAOMeshManager, GLuint shaderProgramID )
{

	sModelDrawInfo sphereInfo;
	sphereInfo.meshFileName = "Sphere_320.ply";			// "Sphere_320_faces_xyz.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(sphereInfo, shaderProgramID);

	sModelDrawInfo sphereInvertedNormalsInfo;
	sphereInvertedNormalsInfo.meshFileName = "Sphere_320_faces_xyz_n_GARBAGE_uv_INVERTED_NORMALS.ply";			// "Sphere_320_faces_xyz.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(sphereInvertedNormalsInfo, shaderProgramID);


	sModelDrawInfo mig;
	mig.meshFileName = "mig.ply";			// "Sphere_320_faces_xyz.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(mig, shaderProgramID);



	//for debug draw only ***********************
	sModelDrawInfo cube1x1x1;
	cube1x1x1.meshFileName = "cube_flat_shaded_xyz_n_uv.ply";			// "cube_flat_shaded_xyz.ply";
	pTheVAOMeshManager->LoadModelIntoVAO(cube1x1x1, shaderProgramID);



	// At this point, mesh in in GPU
	std::cout << "Mesh was loaded OK" << std::endl;

	::g_pTheTextureManager->SetBasePath("assets/textures/cubemaps");
	std::string errorString;
	if (::g_pTheTextureManager->CreateCubeTextureFromBMPFiles("CityCubeMap",
		"city_lf.bmp", "city_rt.bmp",				// reverse these
		"city_dn.bmp", "city_up.bmp",				// Rotate the image "right 90 degrees")
		"city_ft.bmp", "city_bk.bmp", true, errorString))
	{
		std::cout << "Loaded the city cube map OK" << std::endl;
	}
	else
	{
		std::cout << "Error: city cube map DIDN't load. On no!" << std::endl;
	}



	return;
}

	
// Loads the models we are drawing into the vector
void LoadModelsIntoScene( std::vector<cGameObject*> &vec_pObjectsToDraw )
{

	//SkyBoxObject
	{	
		cGameObject* pSkyBoxObject = new cGameObject();
		pSkyBoxObject->setDiffuseColour(glm::vec3(1.0f, 105.0f / 255.0f, 180.0f / 255.0f));
		pSkyBoxObject->bUseVertexColour = false;
		pSkyBoxObject->friendlyName = "SkyBoxObject";
		float scale = 5000.0f;
		pSkyBoxObject->nonUniformScale = glm::vec3(scale, scale, scale);
		pSkyBoxObject->meshName = "Sphere_320_faces_xyz_n_GARBAGE_uv_INVERTED_NORMALS.ply";			// "Sphere_320_faces_xyz.ply";
		pSkyBoxObject->bIsDebug = true;
		pSkyBoxObject->bIsVisible = false;
		vec_pObjectsToDraw.push_back(pSkyBoxObject);
	}

	{	
		cGameObject* pDebugSphere = new cGameObject();
		pDebugSphere->setDiffuseColour(glm::vec3(1.0f, 105.0f / 255.0f, 180.0f / 255.0f));
		pDebugSphere->friendlyName = "DebugSphere";
		float scale = 1.0f;
		pDebugSphere->nonUniformScale = glm::vec3(scale, scale, scale);
		pDebugSphere->meshName = "Sphere_320.ply";
		pDebugSphere->bIsWireFrame = true;
		pDebugSphere->position = glm::vec3(-100.0f, 10.0f, 0.0f);
		pDebugSphere->bIsUpdatedByPhysics = true;
		pDebugSphere->bIsDebug = true;
		pDebugSphere->pDebugRenderer = ::g_pDebugRenderer;
		vec_pObjectsToDraw.push_back(pDebugSphere);
	}




	return;
}