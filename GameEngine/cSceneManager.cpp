#include "cSceneManager.h"
#include "globalStuff.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include "cAnimationState.h"

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include <rapidjson/writer.h>
#include <cstdio>


cSceneManager::cSceneManager() {};

bool AssimpSM_to_VAO_Converter(cSimpleAssimpSkinnedMesh* pTheAssimpSM,
	unsigned int shaderProgramID);

void cSceneManager::setBasePath(std::string basepath)
{
	this->m_basePath = basepath;
	return;
}


bool cSceneManager::saveScene(std::string filename) {

	rapidjson::Document doc;
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
	doc.SetObject();
	rapidjson::Value MeshArray(rapidjson::kArrayType);
	
	//Camera Output
	rapidjson::Value CameraObj(rapidjson::kObjectType);
	rapidjson::Value CameraSpeed(camera.MovementSpeed);
	rapidjson::Value CameraPosArray(rapidjson::kArrayType);

	for (int i = 0; i < 3; i++)
	{
		rapidjson::Value temp(camera.Position[i]);
		CameraPosArray.PushBack(temp, allocator);
	}
	CameraObj.AddMember("Speed", CameraSpeed, allocator);
	CameraObj.AddMember("Position", CameraPosArray, allocator);


	//GameObjects
	/*or rapidjson::Value myArray; ;
	     myArray.SetArray() */
	for (std::vector<cGameObject*>::iterator it = vec_pObjectsToDraw.begin(); it != vec_pObjectsToDraw.end(); ++it) {
		cGameObject* CurModel = *it;
		if (!CurModel->bIsDebug) {
			rapidjson::Value ObjValue(rapidjson::kObjectType);
			rapidjson::Value FriendlyName(CurModel->friendlyName.c_str(), allocator);
			rapidjson::Value MeshName(CurModel->meshName.c_str(), allocator);
			rapidjson::Value PositionArray(rapidjson::kArrayType);
			rapidjson::Value DiffuseRGBArray(rapidjson::kArrayType);
			rapidjson::Value SpecularRGBArray(rapidjson::kArrayType);
			rapidjson::Value Rotation(rapidjson::kArrayType);
			rapidjson::Value Scale(rapidjson::kArrayType);
			rapidjson::Value Visible(CurModel->bIsVisible);
			rapidjson::Value UsePhysics(CurModel->bIsUpdatedByPhysics);
			rapidjson::Value WireFrame(CurModel->bIsWireFrame);



			for (int i = 0; i < 3; i++) {
				rapidjson::Value temp(CurModel->position[i]);
				PositionArray.PushBack(temp, allocator);
			}

			for (int i = 0; i < 4; i++) {
				rapidjson::Value temp(CurModel->materialDiffuse[i]);
				DiffuseRGBArray.PushBack(temp, allocator);
			}

			for (int i = 0; i < 4; i++) {
				rapidjson::Value temp(CurModel->materialSpecular[i]);
				SpecularRGBArray.PushBack(temp, allocator);
			}

			for (int i = 0; i < 3; i++) {
				glm::vec3 rot = CurModel->getMeshOrientationEulerAngles(true);
				rapidjson::Value temp(rot[i]);
				Rotation.PushBack(temp, allocator);
			}
			for (int i = 0; i < 3; i++) {
				rapidjson::Value temp(CurModel->nonUniformScale[i]);
				Scale.PushBack(temp, allocator);
			}

			ObjValue.AddMember("Name", FriendlyName, allocator);
			ObjValue.AddMember("Mesh", MeshName, allocator);
			ObjValue.AddMember("Visible", Visible, allocator);
			ObjValue.AddMember("Use_Physics", UsePhysics, allocator);
			ObjValue.AddMember("Wireframe", WireFrame, allocator);
			ObjValue.AddMember("Position", PositionArray, allocator);
			ObjValue.AddMember("DiffuseRGB_Alpha", DiffuseRGBArray, allocator);
			ObjValue.AddMember("SpecularRGB_Alpha", SpecularRGBArray, allocator);
			ObjValue.AddMember("Rotation", Rotation, allocator);
			ObjValue.AddMember("Scale", Scale, allocator);

			//Textures
			if (CurModel->vecTextures.size() > 0) {
				rapidjson::Value TexArray(rapidjson::kArrayType);
				for (int i = 0; i < CurModel->vecTextures.size(); i++) {
					rapidjson::Value TexObjValue(rapidjson::kObjectType);
					rapidjson::Value TexName(CurModel->vecTextures[i].name.c_str(), allocator);
					rapidjson::Value TexStrength(CurModel->vecTextures[i].strength);
					TexObjValue.AddMember("Name", TexName, allocator);
					TexObjValue.AddMember("Strength", TexStrength, allocator);
					TexArray.PushBack(TexObjValue, allocator);
				}
				ObjValue.AddMember("Textures", TexArray, allocator);




				if (CurModel->rigidBody != NULL) {

					rapidjson::Value PhysObjValue(rapidjson::kObjectType);
					switch (CurModel->rigidBody->GetShape()->GetShapeType())
					{
					case nPhysics::SHAPE_TYPE_SPHERE:
					{
						PhysObjValue.AddMember("Type", "SPHERE", allocator);
						float r = 0.0f;
						CurModel->rigidBody->GetShape()->GetSphereRadius(r);
						PhysObjValue.AddMember("Radius", r, allocator);
					}
						break;
					case nPhysics::SHAPE_TYPE_PLANE:
					{

						PhysObjValue.AddMember("Type", "PLANE", allocator);
						float planeConst = 0.0f;
						CurModel->rigidBody->GetShape()->GetPlaneConstant(planeConst);
						PhysObjValue.AddMember("Constant", planeConst, allocator);

						rapidjson::Value NormalArray(rapidjson::kArrayType);
						glm::vec3 planeNormal = glm::vec3(0.0f);
						CurModel->rigidBody->GetShape()->GetPlaneNormal(planeNormal);

						for (int i = 0; i < 3; i++) {
							rapidjson::Value temp(planeNormal[i]);
							NormalArray.PushBack(temp, allocator);
						}
						PhysObjValue.AddMember("Normal", NormalArray, allocator);
					}
					default:
						break;
					}


					PhysObjValue.AddMember("Mass", CurModel->rigidBody->GetMass(), allocator);
					ObjValue.AddMember("RigidBody", PhysObjValue, allocator);
				
				}


				


			}



			MeshArray.PushBack(ObjValue, allocator);

		}
	}



	//Lights
	rapidjson::Value LightsArray(rapidjson::kArrayType);
	for (std::vector<sLight*>::iterator it = LightManager->vecLights.begin(); it != LightManager->vecLights.end(); ++it) {
		sLight* CurLight = *it;
		rapidjson::Value ObjValue(rapidjson::kObjectType);
		rapidjson::Value FriendlyName(CurLight->lightName.c_str(), allocator);
		rapidjson::Value LightType(CurLight->GetLightType_str().c_str(), allocator);
		rapidjson::Value PositionArray(rapidjson::kArrayType);
		rapidjson::Value AttenArray(rapidjson::kArrayType);
		rapidjson::Value DiffuseArray(rapidjson::kArrayType);
		rapidjson::Value DirectionArray(rapidjson::kArrayType);
		rapidjson::Value Turned("OFF");
		
	

		//cone Angles
		

		//light position

		for (int i = 0; i < 3; i++) {
			rapidjson::Value temp(CurLight->position[i]);
			PositionArray.PushBack(temp, allocator);
		}
		for (int i = 0; i < 4; i++) {
			rapidjson::Value temp(CurLight->diffuse[i]);
			DiffuseArray.PushBack(temp, allocator);
		}
		for (int i = 0; i < 4; i++) {
			rapidjson::Value temp(CurLight->atten[i]);
			AttenArray.PushBack(temp, allocator);
		}
		

		if (CurLight->param2.x == 1.0f) {
			rapidjson::Value temp("ON");
			Turned = temp;
		}
	

		ObjValue.AddMember("Name", FriendlyName, allocator);
		ObjValue.AddMember("Type", LightType, allocator);

		if (CurLight->GetLightType_str() != "POINT_LIGHT") {
			rapidjson::Value AnglesArray(rapidjson::kArrayType);
			for (int i = 1; i < 3; i++) {
				rapidjson::Value temp(CurLight->param1[i]);
				AnglesArray.PushBack(temp, allocator);
			}
			ObjValue.AddMember("Angles", AnglesArray, allocator);

			// Direction Or LookAt object
			if (CurLight->ObjectLookAt == NULL ) {
				for (int i = 0; i < 3; i++) {
					rapidjson::Value temp(CurLight->direction[i]);
					DirectionArray.PushBack(temp, allocator);
				}
				ObjValue.AddMember("Direction", DirectionArray, allocator);
			}
			else {
				rapidjson::Value temp(CurLight->ObjectLookAt->friendlyName.c_str(), allocator);
				ObjValue.AddMember("ObjectLookAt", temp, allocator);
			}
		}
		ObjValue.AddMember("Position", PositionArray, allocator);
		ObjValue.AddMember("Attenuation", AttenArray, allocator);
		ObjValue.AddMember("DiffuseRGB_Alpha", DiffuseArray, allocator);
		ObjValue.AddMember("Turned", Turned, allocator);

		LightsArray.PushBack(ObjValue, allocator);

	}











	doc.AddMember("Camera", CameraObj, allocator);
	doc.AddMember("GameObjects", MeshArray, allocator);
	doc.AddMember("Lights", LightsArray, allocator);

	
	
	std::string fileToLoadFullPath = this->m_basePath + "/" + filename;

	FILE* fp = fopen(fileToLoadFullPath.c_str(), "wb"); // non-Windows use "w"
	char writeBuffer[65536];
	rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
	rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);
	doc.Accept(writer);
	fclose(fp);
	return true;
}




bool cSceneManager::loadScene(std::string filename) {


	std::string fileToLoadFullPath = this->m_basePath + "/" + filename;

	//vec_pObjectsToDraw.clear();
	//LightManager->vecLights.clear();
	::g_pTheTextureManager->SetBasePath("assets/textures");
	rapidjson::Document doc;
	FILE* fp = fopen(fileToLoadFullPath.c_str(), "rb"); // non-Windows use "r"
	char readBuffer[65536];
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	doc.ParseStream(is);
	fclose(fp);


	//Camera
	float Speed;
	glm::vec3 CameraPos;
	rapidjson::Value& CameraPosArray = doc["Camera"]["Position"];

	camera.MovementSpeed = doc["Camera"]["Speed"].GetFloat();
	for (rapidjson::SizeType i = 0; i < CameraPosArray.Size(); i++) {
		CameraPos[i] = CameraPosArray[i].GetFloat();
		std::cout << "CameraPos: [ " << CameraPos[i] << " ]" << std::endl;
	}
	camera.Position = CameraPos;

	

	//Game Objects
	const rapidjson::Value& GameObject = doc["GameObjects"];
	const rapidjson::Value& LightObject = doc["Lights"];


	for (rapidjson::SizeType i = 0; i < GameObject.Size(); i++) {

		cGameObject *CurModel = new cGameObject();
		sModelDrawInfo curModelInfo;

		CurModel->friendlyName = GameObject[i]["Name"].GetString();	
		CurModel->bIsVisible = GameObject[i]["Visible"].GetBool();
		CurModel->bIsUpdatedByPhysics = GameObject[i]["Use_Physics"].GetBool();
		CurModel->bIsWireFrame = GameObject[i]["Wireframe"].GetBool();

		//if ply;
		if(GameObject[i].HasMember("Mesh"))
		{
			CurModel->meshName = GameObject[i]["Mesh"].GetString();
			curModelInfo.meshFileName = GameObject[i]["Mesh"].GetString();
			g_pTheVAOMeshManager->LoadModelIntoVAO(curModelInfo, program);
		}
		//if Skinned Mesh;
		if (GameObject[i].HasMember("SkinnedMesh"))
		{
			//curModelInfo.meshFileName = GameObject[i]["SkinnedMesh"].GetString();
			CurModel->meshName = GameObject[i]["SkinnedMesh"].GetString();
			cSimpleAssimpSkinnedMesh* curSkinnedMesh = new cSimpleAssimpSkinnedMesh();
			if (!curSkinnedMesh->LoadMeshFromFile(CurModel->friendlyName, CurModel->meshName))
			{
				std::cout << "Error: problem loading the skinned mesh" << std::endl;
			}
			if (GameObject[i].HasMember("Animation"))
			{
				std::string Idle = (GameObject[i]["Animation"].HasMember("Idle")) ? GameObject[i]["Animation"]["Idle"].GetString() : "";
				if (Idle != "") {
					curSkinnedMesh->LoadMeshAnimation("Idle", Idle);
				}

				std::string Forward = (GameObject[i]["Animation"].HasMember("Walk-forward")) ? GameObject[i]["Animation"]["Walk-forward"].GetString() : "";
				if (Forward != "") {
					curSkinnedMesh->LoadMeshAnimation("Walk-forward", Forward);
				}

				std::string Run = (GameObject[i]["Animation"].HasMember("Run-forward")) ? GameObject[i]["Animation"]["Run-forward"].GetString() : "";
				if (Run != "") {
					curSkinnedMesh->LoadMeshAnimation("Run-forward", Run);
				}

				std::string Backward = (GameObject[i]["Animation"].HasMember("Walk-backward")) ? GameObject[i]["Animation"]["Walk-backward"].GetString() : "";
				if (Backward != "") {
					curSkinnedMesh->LoadMeshAnimation("Walk-backward", Backward);
				}

				std::string StrafeL = (GameObject[i]["Animation"].HasMember("Strafe-Left")) ? GameObject[i]["Animation"]["Strafe-Left"].GetString() : "";
				if (StrafeL != "") {
					curSkinnedMesh->LoadMeshAnimation("Strafe-Left", StrafeL);
				}

				std::string StrafeR = (GameObject[i]["Animation"].HasMember("Strafe-Right")) ? GameObject[i]["Animation"]["Strafe-Right"].GetString() : "";
				if (StrafeR != "") {
					curSkinnedMesh->LoadMeshAnimation("Strafe-Right", StrafeR);
				}

				std::string TurnL = (GameObject[i]["Animation"].HasMember("Turn-Left")) ? GameObject[i]["Animation"]["Turn-Left"].GetString() : "";
				if (TurnL != "") {
					curSkinnedMesh->LoadMeshAnimation("Turn-Left", TurnL);
				}

				std::string TurnR = (GameObject[i]["Animation"].HasMember("Turn-Right")) ? GameObject[i]["Animation"]["Turn-Right"].GetString() : "";
				if (TurnR != "") {
					curSkinnedMesh->LoadMeshAnimation("Turn-Right", TurnR);
				}

				std::string Jump1 = (GameObject[i]["Animation"].HasMember("Jump1")) ? GameObject[i]["Animation"]["Jump1"].GetString() : "";
				if (Jump1 != "") {
					curSkinnedMesh->LoadMeshAnimation("Jump1", Jump1);
				}

				std::string Jump2 = (GameObject[i]["Animation"].HasMember("Jump2")) ? GameObject[i]["Animation"]["Jump2"].GetString() : "";
				if (Jump2 != "") {
					curSkinnedMesh->LoadMeshAnimation("Jump2", Jump2);
				}

				std::string Action1 = (GameObject[i]["Animation"].HasMember("Action1")) ? GameObject[i]["Animation"]["Action1"].GetString() : "";
				if (Action1 != "") {
					curSkinnedMesh->LoadMeshAnimation("Action1", Action1);
				}

				std::string Action2 = (GameObject[i]["Animation"].HasMember("Action2")) ? GameObject[i]["Animation"]["Action2"].GetString() : "";
				if (Action2 != "") {
					curSkinnedMesh->LoadMeshAnimation("Action2", Action2);
				}

				std::string Action3 = (GameObject[i]["Animation"].HasMember("Action3")) ? GameObject[i]["Animation"]["Action3"].GetString() : "";
				if (Action3 != "") {
					curSkinnedMesh->LoadMeshAnimation("Action3", Action3);
				}

				std::string Action4 = (GameObject[i]["Animation"].HasMember("Action4")) ? GameObject[i]["Animation"]["Action4"].GetString() : "";
				if (Action4 != "") {
					curSkinnedMesh->LoadMeshAnimation("Action4", Action4);
				}

				std::string Action5 = (GameObject[i]["Animation"].HasMember("Action5")) ? GameObject[i]["Animation"]["Action5"].GetString() : "";
				if (Action5 != "") {
					curSkinnedMesh->LoadMeshAnimation("Action5", Action5);
				}


				//std::vector<std::string> vec_anim_name;
				//vec_anim_name.push_back((GameObject[i]["Animation"].HasMember("Idle")) ? GameObject[i]["Animation"]["Idle"].GetString() : "");
				//vec_anim_name.push_back((GameObject[i]["Animation"].HasMember("Walk-forward")) ? GameObject[i]["Animation"]["Walk-forward"].GetString() : "");
				//vec_anim_name.push_back((GameObject[i]["Animation"].HasMember("Walk-backwards")) ? GameObject[i]["Animation"]["Idle"].GetString() : "");
				//vec_anim_name.push_back((GameObject[i]["Animation"].HasMember("Idle")) ? GameObject[i]["Animation"]["Idle"].GetString() : "");
				//vec_anim_name.push_back((GameObject[i]["Animation"].HasMember("Idle")) ? GameObject[i]["Animation"]["Idle"].GetString() : "");
				//vec_anim_name.push_back((GameObject[i]["Animation"].HasMember("Idle")) ? GameObject[i]["Animation"]["Idle"].GetString() : "");
				//vec_anim_name.push_back((GameObject[i]["Animation"].HasMember("Idle")) ? GameObject[i]["Animation"]["Idle"].GetString() : "");
				//vec_anim_name.push_back((GameObject[i]["Animation"].HasMember("Idle")) ? GameObject[i]["Animation"]["Idle"].GetString() : "");
				//vec_anim_name.push_back((GameObject[i]["Animation"].HasMember("Idle")) ? GameObject[i]["Animation"]["Idle"].GetString() : "");
				//vec_anim_name.push_back((GameObject[i]["Animation"].HasMember("Idle")) ? GameObject[i]["Animation"]["Idle"].GetString() : "");
				//vec_anim_name.push_back((GameObject[i]["Animation"].HasMember("Idle")) ? GameObject[i]["Animation"]["Idle"].GetString() : "");
				//vec_anim_name.push_back((GameObject[i]["Animation"].HasMember("Idle")) ? GameObject[i]["Animation"]["Idle"].GetString() : "");

				//for (int i = 0; i < vec_anim_name.size(); i++) {
				//	std::string animName = vec_anim_name[i];
				//	if (animName != "") {
				//		curSkinnedMesh->LoadMeshAnimation(animName, GameObject[i]["Animation"][animName.c_str()].GetString());
				//	}
				//}

				//cAnimationState* pAniState;
				CurModel->pSimpleSkinnedMesh = curSkinnedMesh;
				CurModel->pAniState = new cAnimationState();
				CurModel->pAniState->defaultAnimation.name = "Idle";
				CurModel->currentAnimation = "Idle";

			}
			if (!AssimpSM_to_VAO_Converter(curSkinnedMesh, program))
			{
				std::cout << "Error: Didn't copy the skinned mesh into the VAO format." << std::endl;
			}

		}

		
		
		

		const rapidjson::Value& PositionArray = GameObject[i]["Position"];
		for (rapidjson::SizeType i = 0; i < PositionArray.Size(); i++) {

			CurModel->position[i] = PositionArray[i].GetFloat();
			
		}

		const rapidjson::Value& DiffuseArray = GameObject[i]["DiffuseRGB_Alpha"];
		for (rapidjson::SizeType i = 0; i < DiffuseArray.Size(); i++) {

			CurModel->materialDiffuse[i] = DiffuseArray[i].GetFloat();

		}

		const rapidjson::Value& SpecularArray = GameObject[i]["SpecularRGB_Alpha"];
		for (rapidjson::SizeType i = 0; i < SpecularArray.Size(); i++) {

			CurModel->materialSpecular[i] = SpecularArray[i].GetFloat();

		}

		const rapidjson::Value& RotationArray = GameObject[i]["Rotation"];
		//for (rapidjson::SizeType i = 0; i < RotationArray.Size(); i++) {
			//In Degrees
	    glm::vec3 rot(RotationArray[0].GetFloat(), RotationArray[1].GetFloat(), RotationArray[2].GetFloat());
		CurModel->setMeshOrientationEulerAngles(rot, true);

			//CurModel->m_meshQOrientation[i] = RotationArray[i].GetFloat();

		//}

		const rapidjson::Value& ScaleArray = GameObject[i]["Scale"];
		for (rapidjson::SizeType i = 0; i < ScaleArray.Size(); i++) {

			CurModel->nonUniformScale[i] = ScaleArray[i].GetFloat();

		}

		if (GameObject[i].HasMember("Textures")) {
			const rapidjson::Value& TexArray = GameObject[i]["Textures"];
			for (rapidjson::SizeType i = 0; i < TexArray.Size(); i++)
			{
				sTextureInfo CurModelTex;
				CurModelTex.name = TexArray[i]["Name"].GetString();
				CurModelTex.strength = TexArray[i]["Strength"].GetFloat();
				CurModel->vecTextures.push_back(CurModelTex);
				//Creating Texture even if there is alreade same textue NEED FIX
				::g_pTheTextureManager->Create2DTextureFromBMPFile(CurModelTex.name, true);
			}
		}


		if(GameObject[i].HasMember("RigidBody"))
		{
			nPhysics::iShape* CurShape = NULL;
			nPhysics::sRigidBodyDef def;
			std::string type = GameObject[i]["RigidBody"]["Type"].GetString();

			//in Radians
			def.Orientation = CurModel->getMeshOrientationEulerAngles();
			def.Position = CurModel->position;

			
			if (type == "SPHERE")
			{
				float radius = GameObject[i]["RigidBody"]["Radius"].GetFloat();
				CurShape = gPhysicsFactory->CreateSphereShape(radius);
				def.Mass = GameObject[i]["RigidBody"]["Mass"].GetFloat();
				
			}
			else if (type == "PLANE")
			{
				


				float planeConst = GameObject[i]["RigidBody"]["Constant"].GetFloat();

				const rapidjson::Value& NormArray = GameObject[i]["RigidBody"]["Normal"];
				glm::vec3 norm;
				for (int i = 0; i < 3; i++)
				{
					norm[i] = NormArray[i].GetFloat();
				}
				CurShape = gPhysicsFactory->CreatePlaneShape(norm, planeConst);

			}
			nPhysics::iRigidBody* rigidBody = gPhysicsFactory->CreateRigidBody(def, CurShape);
			CurModel->rigidBody = rigidBody;
			gPhysicsWorld->AddBody(rigidBody);
		}

		vec_pObjectsToDraw.push_back(CurModel);

	
	}



	for (rapidjson::SizeType i = 0; i < LightObject.Size(); i++) {

		sLight* CurLight = new sLight();

		CurLight->lightName = LightObject[i]["Name"].GetString();
		CurLight->SetLightType(LightObject[i]["Type"].GetString());
		if (LightObject[i].HasMember("Angles"))	{
			const rapidjson::Value& AngleArray = LightObject[i]["Angles"];
			CurLight->SetSpotConeAngles(AngleArray[0].GetFloat(), AngleArray[1].GetFloat());
		}

		if (LightObject[i].HasMember("Direction")) {
			const rapidjson::Value& DirectionArray = LightObject[i]["Direction"];
			for (rapidjson::SizeType i = 0; i < DirectionArray.Size(); i++) {
				CurLight->direction[i] = DirectionArray[i].GetFloat();
			}
		}

		
		const rapidjson::Value& PositionArray = LightObject[i]["Position"];
		for (rapidjson::SizeType i = 0; i < PositionArray.Size(); i++) {
			CurLight->position[i] = PositionArray[i].GetFloat();
		}
		if (LightObject[i].HasMember("ObjectLookAt")) {
			cGameObject* LookAtObj = findObjectByFriendlyName(LightObject[i]["ObjectLookAt"].GetString());
			if (LookAtObj != NULL) {
				CurLight->SetRelativeDirectionByLookAt(LookAtObj);
			}
			else {
				std::cout << "LookAt obj not found: " << LightObject[i]["ObjectLookAt"].GetString() << std::endl;
			}
		}
		const rapidjson::Value& AttenArray = LightObject[i]["Attenuation"];
		for (rapidjson::SizeType i = 0; i < AttenArray.Size(); i++) {
			CurLight->atten[i] = AttenArray[i].GetFloat();
		}
		const rapidjson::Value& DiffuseArray = LightObject[i]["DiffuseRGB_Alpha"];
		for (rapidjson::SizeType i = 0; i < DiffuseArray.Size(); i++) {
			CurLight->diffuse[i] = DiffuseArray[i].GetFloat();
		}
		std::string turn = LightObject[i]["Turned"].GetString();
		if (turn == "ON") {
			CurLight->param2.x = 1.0f;
		}
		else{
			CurLight->param2.x = 0.0f;
		}

		LightManager->vecLights.push_back(CurLight);
		
	}

	


	return true;
}