//     ___                 ___ _     
//    / _ \ _ __  ___ _ _ / __| |    
//   | (_) | '_ \/ -_) ' \ (_ | |__  
//    \___/| .__/\___|_||_\___|____| 
//         |_|                       
//
#include "globalOpenGLStuff.h"
#include "globalStuff.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr
#include <rapidjson/filereadstream.h>
#include <rapidjson/document.h>
#include "Camera.h"
#include <stdlib.h>
#include <stdio.h>		// printf();
#include <iostream>		// cout (console out)

#include <vector>		// "smart array" dynamic array

#include "cShaderManager.h"
#include "cGameObject.h"
#include "cVAOMeshManager.h"
#include <algorithm>
#include <windows.h>


#include "DebugRenderer/cDebugRenderer.h"
#include "cLightHelper.h"


//Dll 
HINSTANCE hGetProckDll = 0;
//typedef nPhysics::iPhysicsFactory*(*f_createPhysicsFactory)();
ePhysics physics_library = UNKNOWN;

nPhysics::iPhysicsFactory* gPhysicsFactory = NULL;
nPhysics::iPhysicsWorld* gPhysicsWorld = NULL;

glm::vec3 g_Gravity = glm::vec3(0.0f, -1.0f, 0.0f);

GLuint program;
cDebugRenderer* g_pDebugRendererACTUAL = NULL;
iDebugRenderer* g_pDebugRenderer = NULL;
cSimpleDebugRenderer* g_simpleDubugRenderer = NULL;
cLuaBrain* p_LuaScripts = NULL;
cTextRend* g_textRenderer = NULL;
//cCommandGroup sceneCommandGroup;
int cou;
int nbFrames = 0;
int FPS = 0;
std::vector<cAABB::sAABB_Triangle> vec_cur_AABB_tris;
void UpdateWindowTitle(void);
double currentTime = 0;
double deltaTime = 0;
double FPS_last_Time = 0;
bool bIsDebugMode = false;

std::vector< cGameObject* > vec_pObjectsToDraw;
std::vector< cGameObject* > vec_pSpheres;

// To the right, up 4.0 units, along the x axis


unsigned int numberOfObjectsToDraw = 0;

unsigned int SCR_WIDTH = 1000;
unsigned int SCR_HEIGHT = 800;
std::string title = "Default";
std::string scene = "Scene1.json";

Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));

bool distToCam(cGameObject* leftObj, cGameObject* rightObj) {
	return glm::distance(leftObj->position, camera.Position) > glm::distance(rightObj->position, camera.Position); // here go your sort conditions
}

std::vector <cGameObject*> vec_sorted_drawObj;

glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 g_CameraEye = glm::vec3(0.0, 0.0, 250.0f);

//glm::vec3 g_CameraAt = glm::vec3(g_CameraEye, g_CameraEye.z + cameraFront.z, cameraUp.y);
//glm::vec3 g_CameraAt = glm::vec3( 0.0, 0.0, 0.0f );


cShaderManager* pTheShaderManager = NULL;		// "Heap" variable
cVAOMeshManager* g_pTheVAOMeshManager = NULL;
cSceneManager* g_pSceneManager = NULL;
//cTextRend g_textRend;
cLightManager* LightManager = NULL;

std::vector<cGameObject*> vec_transObj;
std::vector<cGameObject*> vec_non_transObj;

cBasicTextureManager* g_pTheTextureManager = NULL;

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

cAABBHierarchy* g_pTheTerrain = new cAABBHierarchy();

bool loadConfig();
cFBO* g_pFBOMain;

// For now, I'm doing this here, but you might want to do this
//  in the object, in the "phsyics" thing, or wherever. 
//  Or leave it here!!

// Set up the off screen textures to draw to
GLuint g_FBO = 0;
GLuint g_FBO_colourTexture = 0;
GLuint g_FBO_depthTexture = 0;
GLint g_FBO_SizeInPixes = 512;		// = 512 the WIDTH of the framebuffer, in pixels;

int main(void)
{
	loadConfig();

	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title.c_str(), NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}


	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);


	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);


	// Create the shader manager...
	//cShaderManager TheShaderManager;	
	//cShaderManager* pTheShaderManager;		
	pTheShaderManager = new cShaderManager();
	pTheShaderManager->setBasePath("assets/shaders/");

	cShaderManager::cShader vertexShader;
	cShaderManager::cShader fragmentShader;

	vertexShader.fileName = "vertex01.glsl";
	vertexShader.shaderType = cShaderManager::cShader::VERTEX_SHADER;

	fragmentShader.fileName = "fragment01.glsl";
	fragmentShader.shaderType = cShaderManager::cShader::FRAGMENT_SHADER;

	if (pTheShaderManager->createProgramFromFile("BasicUberShader",
		vertexShader,
		fragmentShader))
	{		// Shaders are OK
		std::cout << "Compiled shaders OK." << std::endl;
	}
	else
	{
		std::cout << "OH NO! Compile error" << std::endl;
		std::cout << pTheShaderManager->getLastError() << std::endl;
	}


	// Load the uniform location values (some of them, anyway)
	cShaderManager::cShaderProgram* pSP = ::pTheShaderManager->pGetShaderProgramFromFriendlyName("BasicUberShader");
	pSP->LoadUniformLocation("texture00");
	pSP->LoadUniformLocation("texture01");
	pSP->LoadUniformLocation("texture02");
	pSP->LoadUniformLocation("texture03");
	pSP->LoadUniformLocation("texture04");
	pSP->LoadUniformLocation("texture05");
	pSP->LoadUniformLocation("texture06");
	pSP->LoadUniformLocation("texture07");
	pSP->LoadUniformLocation("texBlendWeights[0]");
	pSP->LoadUniformLocation("texBlendWeights[1]");




	program = pTheShaderManager->getIDFromFriendlyName("BasicUberShader");


	::g_pTheVAOMeshManager = new cVAOMeshManager();
	::g_pTheVAOMeshManager->SetBasePath("assets/models");

	::g_pTheTextureManager = new cBasicTextureManager();

	::g_textRenderer = new cTextRend();
	//Create Scene Manager
	::g_pSceneManager = new cSceneManager();
	::g_pSceneManager->setBasePath("scenes");

	::LightManager = new cLightManager();

	::g_pFBOMain = new cFBO();

	//
	//	if ( FBOStatus == GL_FRAMEBUFFER_COMPLETE )
	std::string FBOErrorString;
	if (::g_pFBOMain->init(1024, 1024, FBOErrorString))
	{
		std::cout << "Framebuffer is good to go!" << std::endl;
	}
	else
	{
		std::cout << "Framebuffer is NOT complete" << std::endl;
	}

	// Point back to default frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	// Loading the uniform variables here (rather than the inner draw loop)
	GLint objectColour_UniLoc = glGetUniformLocation(program, "objectColour");
	//uniform vec3 lightPos;
	//uniform float lightAtten;


//	GLint lightPos_UniLoc = glGetUniformLocation(program, "lightPos");
//	GLint lightBrightness_UniLoc = glGetUniformLocation(program, "lightBrightness");

	//	// uniform mat4 MVP;	THIS ONE IS NO LONGER USED	
	//uniform mat4 matModel;	// M
	//uniform mat4 matView;		// V
	//uniform mat4 matProj;		// P
	//GLint mvp_location = glGetUniformLocation(program, "MVP");
	GLint matModel_location = glGetUniformLocation(program, "matModel");
	GLint matView_location = glGetUniformLocation(program, "matView");
	GLint matProj_location = glGetUniformLocation(program, "matProj");
	GLint eyeLocation_location = glGetUniformLocation(program, "eyeLocation");

	// Note that this point is to the +interface+ but we're creating the actual object
	::g_pDebugRendererACTUAL = new cDebugRenderer();
	::g_pDebugRenderer = (iDebugRenderer*)::g_pDebugRendererACTUAL;

	if (!::g_pDebugRendererACTUAL->initialize())
	{
		std::cout << "Warning: couldn't init the debug renderer." << std::endl;
		std::cout << "\t" << ::g_pDebugRendererACTUAL->getLastError() << std::endl;
	}
	else
	{
		std::cout << "Debug renderer is OK" << std::endl;
	}


	//PhysicsInit
	hGetProckDll = LoadLibraryA("BulletPhysics.dll");
	physics_library = BULLET;
	f_createPhysicsFactory CreatePhysicsFactory = (f_createPhysicsFactory)GetProcAddress(hGetProckDll, "CreateFactory");
	gPhysicsFactory = CreatePhysicsFactory();
	gPhysicsWorld = gPhysicsFactory->CreatePhysicsWorld();

	gPhysicsWorld->SetGravity(g_Gravity);

	//nPhysics::iShape* plane = gPhysicsFactory->CreatePlaneShape(glm::vec3(0.0f), 0.0f);
	//nPhysics::sRigidBodyDef def;
	//def.Position = glm::vec3(0.0f, 0.0f, 10.0f);
	//nPhysics::iRigidBody* rigidBody = gPhysicsFactory->CreateRigidBody(def, plane);
	//gPhysicsWorld->AddBody(rigidBody);



	LoadSkinnedMeshModel(::vec_pObjectsToDraw, program);

	LoadModelTypes(::g_pTheVAOMeshManager, program);
	::g_pSceneManager->loadScene(scene);
	::LightManager->LoadUniformLocations(program);

	LoadModelsIntoScene(::vec_pObjectsToDraw);
	g_simpleDubugRenderer = new cSimpleDebugRenderer(findObjectByFriendlyName("DebugSphere"), program);

	//vec_sorted_drawObj = vec_pObjectsToDraw;


	for (unsigned int objIndex = 0;
		objIndex != (unsigned int)vec_pObjectsToDraw.size();
		objIndex++)
	{
		cGameObject* pCurrentMesh = vec_pObjectsToDraw[objIndex];
		if (pCurrentMesh->materialDiffuse.a < 1.0f) { vec_transObj.push_back(pCurrentMesh); }
		else { vec_non_transObj.push_back(pCurrentMesh); }

	}//for ( unsigned int objIndex = 0; 


	// Get the current time to start with
	double lastTime = glfwGetTime();



	cLightHelper* pLightHelper = new cLightHelper();




	//::p_LuaScripts = new cLuaBrain();
	//::p_LuaScripts->SetObjectVector(&(::vec_pObjectsToDraw));

	//::p_LuaScripts->LoadScriptFile("example.lua");
	//FBO
	int renderPassNumber = 1;
	// 1 = 1st pass (the actual scene)
	// 2 = 2nd pass (rendering what we drew to the output)
	GLint renderPassNumber_UniLoc = glGetUniformLocation(program, "renderPassNumber");

	//std::cout << renderPassNumber_UniLoc << std::endl;
	//*****************************************************************

	//Copy All Spheres to new Vec to manipulate them later
	for (int i = 0; i < vec_pObjectsToDraw.size(); i++)
	{
		if (vec_pObjectsToDraw[i]->rigidBody != NULL) {
			if (vec_pObjectsToDraw[i]->rigidBody->GetShape()->GetShapeType() == nPhysics::SHAPE_TYPE_SPHERE) {
				vec_pSpheres.push_back(vec_pObjectsToDraw[i]);
			}
		}
	}

	//AddSomeVel



	// Draw the "scene" (run the program)
	while (!glfwWindowShouldClose(window))
	{

		// Switch to the shader we want
		::pTheShaderManager->useShaderProgram("BasicUberShader");


		//// Set for the 1st pass
		////glBindFramebuffer(GL_FRAMEBUFFER, g_FBO);		// Point output to FBO
		glBindFramebuffer(GL_FRAMEBUFFER, ::g_pFBOMain->ID);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//**********************************************************
		// Clear the offscreen frame buffer
		//glViewport(0, 0, g_FBO_SizeInPixes, g_FBO_SizeInPixes);
		//GLfloat	zero = 0.0f;
		//GLfloat one = 1.0f;
		//glClearBufferfv(GL_COLOR, 0, &zero);
		//glClearBufferfv(GL_DEPTH, 0, &one);
		//**********************************************************


		::g_pFBOMain->clearBuffers(true, true);


		glUniform1f(renderPassNumber_UniLoc, 1.0f);	// Tell shader it's the 1st pass

		float ratio;
		int width, height;




		glm::mat4x4 matProjection = glm::mat4(1.0f);
		glm::mat4x4	matView = glm::mat4(1.0f);


		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);

		// These things will impact ANY framebuffer
		// (controls state of the rendering, so doesn't matter where
		//  the output goes to, right?)
		glEnable(GL_DEPTH);		// Enables the KEEPING of the depth information
		glEnable(GL_DEPTH_TEST);	// When drawing, checked the existing depth
		glEnable(GL_CULL_FACE);	// Discared "back facing" triangles

		// Colour and depth buffers are TWO DIFF THINGS.
		// Note that this is clearing the main framebuffer
		// (Which will do NOTHING to the offscreen buffer)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
		matProjection = glm::perspective(1.0f,			// FOV
			ratio,		// Aspect ratio
			0.1f,			// Near clipping plane
			10000.0f);	// Far clipping plane


//glm::vec3 migpos = findObjectByFriendlyName("mig")->position;
//matView = glm::lookAt(camera.Position, migpos, camera.WorldUp);

		matView = glm::lookAt(glm::vec3(20.0f, 20.0f, 40.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glUniform3f(eyeLocation_location, camera.Position.x, camera.Position.y, camera.Position.z);

		//matView = glm::lookAt( g_CameraEye,	// Eye
		//	                    g_CameraAt,		// At
		//	                    glm::vec3( 0.0f, 1.0f, 0.0f ) );// Up
		glUniformMatrix4fv(matView_location, 1, GL_FALSE, glm::value_ptr(matView));
		glUniformMatrix4fv(matProj_location, 1, GL_FALSE, glm::value_ptr(matProjection));
		// Do all this ONCE per frame
		LightManager->CopyLightValuesToShader();

		std::sort(vec_transObj.begin(), vec_transObj.end(), distToCam);

	





		double FPS_currentTime = glfwGetTime();
		nbFrames++;
		if (FPS_currentTime - FPS_last_Time >= 1.0) { // If last prinf() was more than 1 sec ago
			// printf and reset timer
			//printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			FPS = nbFrames * 1;
			nbFrames = 0;
			FPS_last_Time += 1.0;
		}
		g_textRenderer->drawText(width, height, ("FPS: " + std::to_string(FPS)).c_str());

		switch (physics_library)
		{
		case SIMPLE:
			g_textRenderer->drawText(width, height, ("Physics: My crappy physics"), 100.0f);
			break;
		case BULLET:
			g_textRenderer->drawText(width, height, ("Physics: Bullet"), 100.0f);
			break;
		case UNKNOWN:
			break;
		default:
			break;
		}
		g_textRenderer->drawText(width, height, ("Gravity: " + std::to_string((int)g_Gravity.y)).c_str(), 150.0f);

		//float sx = 2.0f / width;
		//float sy = 2.0f / height;
		//GLfloat yoffset = 50.0f;
		//GLfloat xoffset = 8 * sx;

		//g_textRend.renderText("ololosadasdasdassdasdasdasd", -1 + xoffset, 1 - yoffset * sy, sx, sy);
		//yoffset += 50.0f;



		//REFLECTION

		//{
		//	GLint bAddReflect_UniLoc = glGetUniformLocation(program, "bAddReflect");
		//	//			glUniform1f( bAddReflect_UniLoc, (float)GL_TRUE );

		//	GLint bAddRefract_UniLoc = glGetUniformLocation(program, "bAddRefract");
		//	glUniform1f(bAddRefract_UniLoc, (float)GL_TRUE);

		//	cGameObject* pBunny = findObjectByFriendlyName("Ufo2UVb");

		//	glm::vec3 oldPos = pBunny->position;
		//	glm::vec3 oldScale = pBunny->nonUniformScale;
		//	glm::quat oldOrientation = pBunny->getQOrientation();

		//	pBunny->position = glm::vec3(0.0f, 25.0f, 0.0f);
		//	pBunny->setUniformScale(100.0f);
		//	pBunny->setMeshOrientationEulerAngles(0.0f, 0.0f, 0.0f, true);

		//	glm::mat4x4 matModel = glm::mat4(1.0f);			// mat4x4 m, p, mvp;

		//	DrawObject(pBunny, matModel, program);

		//	pBunny->position = oldPos;
		//	pBunny->nonUniformScale = oldScale;
		//	pBunny->setQOrientation(oldOrientation);

		//	glUniform1f(bAddReflect_UniLoc, (float)GL_FALSE);
		//	glUniform1f(bAddRefract_UniLoc, (float)GL_FALSE);
		//}





		// High res timer (likely in ms or ns)
		currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;



		double MAX_DELTA_TIME = 0.1;	// 100 ms
		if (deltaTime > MAX_DELTA_TIME)
		{
			deltaTime = MAX_DELTA_TIME;
		}
		// update the "last time"
		lastTime = currentTime;

		for (unsigned int objIndex = 0;
			objIndex != (unsigned int)vec_pObjectsToDraw.size();
			objIndex++)
		{
			cGameObject* pCurrentMesh = vec_pObjectsToDraw[objIndex];

			pCurrentMesh->Update(deltaTime);

		}//for ( unsigned int objIndex = 0; 

		//sceneCommandGroup.Update(deltaTime);



		// The physics update loop

		//New Dll physics
		gPhysicsWorld->Update(deltaTime);

		for (int i = 0; i < vec_pObjectsToDraw.size(); i++)
		{
			cGameObject* curMesh = vec_pObjectsToDraw[i];
			if (curMesh->rigidBody != NULL && curMesh->rigidBody->GetShape()->GetShapeType() != nPhysics::SHAPE_TYPE_PLANE) {

				float rad;
				curMesh->rigidBody->GetShape()->GetSphereRadius(rad);
				if (curMesh->friendlyName == "chan") {
					curMesh->position = curMesh->rigidBody->GetPosition();
					curMesh->position.y = curMesh->rigidBody->GetPosition().y - rad;
				}
				else { curMesh->position = curMesh->rigidBody->GetPosition(); }
				curMesh->m_meshQOrientation = glm::mat4(curMesh->rigidBody->GetMatRotation());
				//HACK CHARACTER

			}
		}

		if (bIsDebugMode) {
			// Call the debug renderer
			for (int i = 0; i < vec_pObjectsToDraw.size(); i++) {
				cGameObject* curObj = vec_pObjectsToDraw[i];
				//curObj->bIsVisible = false;
				curObj->bDontLight = true;
				if (curObj->rigidBody != NULL) {
					if (curObj->rigidBody->GetShape()->GetShapeType() == nPhysics::SHAPE_TYPE_SPHERE) {

						float rad;
						curObj->rigidBody->GetShape()->GetSphereRadius(rad);
						g_simpleDubugRenderer->drawSphere(curObj->rigidBody->GetPosition(), rad);
					}
					if (curObj->rigidBody->GetShape()->GetShapeType() == nPhysics::SHAPE_TYPE_PLANE) {
						//curObj->bIsWireFrame = true;
						curObj->bIsVisible = true;
						glm::mat4 matIden = glm::mat4(1.0f);
						DrawObject(curObj, matIden, program);
					}
				}
			}
		}
		::g_pDebugRendererACTUAL->RenderDebugObjects(matView, matProjection, deltaTime);

		//::p_LuaScripts->UpdateCG(deltaTime);
		//::p_LuaScripts->Update(deltaTime);

		for (std::vector<sLight*>::iterator it = LightManager->vecLights.begin(); it != LightManager->vecLights.end(); ++it)
		{

			sLight* CurLight = *it;
			if (CurLight->AtenSphere == true)
			{


				cGameObject* pDebugSphere = findObjectByFriendlyName("DebugSphere");
				pDebugSphere->bIsVisible = true;
				pDebugSphere->bDontLight = true;
				glm::vec4 oldDiffuse = pDebugSphere->materialDiffuse;
				glm::vec3 oldScale = pDebugSphere->nonUniformScale;
				pDebugSphere->setDiffuseColour(glm::vec3(255.0f / 255.0f, 105.0f / 255.0f, 180.0f / 255.0f));
				pDebugSphere->bUseVertexColour = false;
				pDebugSphere->position = glm::vec3(CurLight->position);
				glm::mat4 matBall(1.0f);


				pDebugSphere->materialDiffuse = oldDiffuse;
				pDebugSphere->setUniformScale(0.1f);			// Position
				DrawObject(pDebugSphere, matBall, program);

				const float ACCURACY_OF_DISTANCE = 0.0001f;
				const float INFINITE_DISTANCE = 10000.0f;

				float distance90Percent =
					pLightHelper->calcApproxDistFromAtten(0.90f, ACCURACY_OF_DISTANCE,
						INFINITE_DISTANCE,
						CurLight->atten.x,
						CurLight->atten.y,
						CurLight->atten.z);

				pDebugSphere->setUniformScale(distance90Percent);			// 90% brightness
				//pDebugSphere->objColour = glm::vec3(1.0f,1.0f,0.0f);
				pDebugSphere->setDiffuseColour(glm::vec3(1.0f, 1.0f, 0.0f));
				DrawObject(pDebugSphere, matBall, program);

				//			pDebugSphere->objColour = glm::vec3(0.0f,1.0f,0.0f);	// 50% brightness
				pDebugSphere->setDiffuseColour(glm::vec3(0.0f, 1.0f, 0.0f));
				float distance50Percent =
					pLightHelper->calcApproxDistFromAtten(0.50f, ACCURACY_OF_DISTANCE,
						INFINITE_DISTANCE,
						CurLight->atten.x,
						CurLight->atten.y,
						CurLight->atten.z);
				pDebugSphere->setUniformScale(distance50Percent);
				DrawObject(pDebugSphere, matBall, program);

				//			pDebugSphere->objColour = glm::vec3(1.0f,0.0f,0.0f);	// 25% brightness
				pDebugSphere->setDiffuseColour(glm::vec3(1.0f, 0.0f, 0.0f));
				float distance25Percent =
					pLightHelper->calcApproxDistFromAtten(0.25f, ACCURACY_OF_DISTANCE,
						INFINITE_DISTANCE,
						CurLight->atten.x,
						CurLight->atten.y,
						CurLight->atten.z);
				pDebugSphere->setUniformScale(distance25Percent);
				DrawObject(pDebugSphere, matBall, program);

				float distance1Percent =
					pLightHelper->calcApproxDistFromAtten(0.01f, ACCURACY_OF_DISTANCE,
						INFINITE_DISTANCE,
						CurLight->atten.x,
						CurLight->atten.y,
						CurLight->atten.z);
				//			pDebugSphere->objColour = glm::vec3(0.0f,0.0f,1.0f);	// 1% brightness
				pDebugSphere->setDiffuseColour(glm::vec3(0.0f, 0.0f, 1.0f));
				pDebugSphere->setUniformScale(distance1Percent);
				DrawObject(pDebugSphere, matBall, program);

				//			pDebugSphere->objColour = oldColour;
				pDebugSphere->materialDiffuse = oldDiffuse;
				pDebugSphere->nonUniformScale = oldScale;
				pDebugSphere->bIsVisible = false;
			}
		}



		cGameObject* pSkyBox = findObjectByFriendlyName("SkyBoxObject");
		// Place skybox object at camera location
		pSkyBox->position = camera.Position;
		pSkyBox->bIsVisible = true;
		pSkyBox->bIsWireFrame = false;

		glDisable(GL_CULL_FACE);		// Force drawing the sphere
//		                                // Could also invert the normals
		// Draw the BACK facing (because the normals of the sphere face OUT and we 
		//  are inside the centre of the sphere..
		glCullFace(GL_FRONT);

		// Bind the cube map texture to the cube map in the shader
		GLuint cityTextureUNIT_ID = 30;			// Texture unit go from 0 to 79
		glActiveTexture(cityTextureUNIT_ID + GL_TEXTURE0);	// GL_TEXTURE0 = 33984

		int cubeMapTextureID = ::g_pTheTextureManager->getTextureIDFromName("CityCubeMap");

		// Cube map is now bound to texture unit 30
		//		glBindTexture( GL_TEXTURE_2D, cubeMapTextureID );
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTextureID);

		//uniform samplerCube textureSkyBox;
		GLint skyBoxCubeMap_UniLoc = glGetUniformLocation(program, "textureSkyBox");
		glUniform1i(skyBoxCubeMap_UniLoc, cityTextureUNIT_ID);

		//uniform bool useSkyBoxTexture;
		GLint useSkyBoxTexture_UniLoc = glGetUniformLocation(program, "useSkyBoxTexture");
		glUniform1f(useSkyBoxTexture_UniLoc, (float)GL_TRUE);

		glm::mat4 matIdentity = glm::mat4(1.0f);
		DrawObject(pSkyBox, matIdentity, program);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		pSkyBox->bIsVisible = false;
		glUniform1f(useSkyBoxTexture_UniLoc, (float)GL_FALSE);


		// Draw all the objects in the "scene"
		for (unsigned int objIndex = 0;
			objIndex != (unsigned int)vec_non_transObj.size();
			objIndex++)
		{
			cGameObject* pCurrentMesh = vec_non_transObj[objIndex];

			glm::mat4x4 matModel = glm::mat4(1.0f);			// mat4x4 m, p, mvp;

			DrawObject(pCurrentMesh, matModel, program);

		}//for ( unsigned int objIndex = 0; 

		for (unsigned int objIndex = 0;
			objIndex != (unsigned int)vec_transObj.size();
			objIndex++)
		{
			cGameObject* pCurrentMesh = vec_transObj[objIndex];

			glm::mat4x4 matModel = glm::mat4(1.0f);			// mat4x4 m, p, mvp;

			DrawObject(pCurrentMesh, matModel, program);

		}//for ( unsigned int objIndex = 0; 

		// *****************************************
	// 2nd pass
	// *****************************************
	//	glUniform1f(renderPassNumber_UniLoc, 2.0f);	// Tell shader it's the 1st pass
	// 1. Set the Framebuffer output to the main framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);		// Points to the "regular" frame buffer

													// Get the size of the actual (screen) frame buffer
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);

		glEnable(GL_DEPTH);		// Enables the KEEPING of the depth information
		glEnable(GL_DEPTH_TEST);	// When drawing, checked the existing depth
		glEnable(GL_CULL_FACE);	// Discared "back facing" triangles

		// 2. Clear everything **ON THE MAIN FRAME BUFFER** 
		//     (NOT the offscreen buffer)
		// This clears the ACTUAL screen framebuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		// 3. Bind 1 texture (what we drew)
		cGameObject* p2SidedQuad = findObjectByFriendlyName("plane_tv");
		p2SidedQuad->bIsVisible = true;
		p2SidedQuad->b_HACK_UsesOffscreenFBO = true;
		p2SidedQuad->bDontLight = true;
		p2SidedQuad->bUseVertexColour = false;
		//p2SidedQuad->materialDiffuse = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		p2SidedQuad->materialDiffuse = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		//p2SidedQuad->bIsWireFrame = true;
		// Rotate it so it's "up and down"
		p2SidedQuad->setMeshOrientationEulerAngles(90.0f, 0.0f, 90.0f, true);
		p2SidedQuad->position = glm::vec3(0.0f, 100.0f, 50.0f); ;
		p2SidedQuad->setUniformScale(50.0f);


		//  (for now: soon there will be multiple textures)
		glUniform1f(renderPassNumber_UniLoc, 2.0f);	// Tell shader it's the 1st pass

		matView = camera.GetViewMatrix();

		glUniformMatrix4fv(matView_location, 1, GL_FALSE, glm::value_ptr(matView));
		glUniformMatrix4fv(matProj_location, 1, GL_FALSE, glm::value_ptr(matProjection));

		// 4. Draw a single quad		
		glm::mat4 matModel = glm::mat4(1.0f);	// identity
		DrawObject(p2SidedQuad, matModel, program);

		p2SidedQuad->bIsVisible = false;






		glUniform1f(renderPassNumber_UniLoc, 1.0f);	// Tell shader it's the 1st pass
		for (unsigned int objIndex = 0;
			objIndex != (unsigned int)vec_non_transObj.size();
			objIndex++)
		{
			cGameObject* pCurrentMesh = vec_non_transObj[objIndex];

			glm::mat4x4 matModel = glm::mat4(1.0f);			// mat4x4 m, p, mvp;

			DrawObject(pCurrentMesh, matModel, program);

		}//for ( unsigned int objIndex = 0; 

		for (unsigned int objIndex = 0;
			objIndex != (unsigned int)vec_transObj.size();
			objIndex++)
		{
			cGameObject* pCurrentMesh = vec_transObj[objIndex];

			glm::mat4x4 matModel = glm::mat4(1.0f);			// mat4x4 m, p, mvp;

			DrawObject(pCurrentMesh, matModel, program);

		}//for ( unsigned int objIndex = 0; 


		UpdateWindowTitle();

		glfwSwapBuffers(window);		// Shows what we drew

		glfwPollEvents();
		ProcessAsynKeys(window);




	}//while (!glfwWindowShouldClose(window))

	// Delete stuff
	delete pTheShaderManager;
	delete ::g_pTheVAOMeshManager;
	delete ::g_pTheTextureManager;

	// 
	delete ::g_pDebugRenderer;

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}





void UpdateWindowTitle(void)
{



	return;
}

cGameObject* findObjectByFriendlyName(std::string theNameToFind)
{
	for (unsigned int index = 0; index != vec_pObjectsToDraw.size(); index++)
	{
		// Is this it? 500K - 1M
		// CPU limited Memory delay = 0
		// CPU over powered (x100 x1000) Memory is REAAAAALLY SLOW
		if (vec_pObjectsToDraw[index]->friendlyName == theNameToFind)
		{
			return vec_pObjectsToDraw[index];
		}
	}

	// Didn't find it.
	return NULL;	// 0 or nullptr
}


cGameObject* findObjectByUniqueID(unsigned int ID_to_find)
{
	for (unsigned int index = 0; index != vec_pObjectsToDraw.size(); index++)
	{
		if (vec_pObjectsToDraw[index]->getUniqueID() == ID_to_find)
		{
			return vec_pObjectsToDraw[index];
		}
	}

	// Didn't find it.
	return NULL;	// 0 or nullptr
}

bool loadConfig()
{
	rapidjson::Document doc;
	FILE* fp = fopen("config/config.json", "rb"); // non-Windows use "r"
	char readBuffer[65536];
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	doc.ParseStream(is);
	fclose(fp);
	rapidjson::Value Window(rapidjson::kObjectType);
	Window = doc["Window"];

	SCR_WIDTH = Window["Width"].GetInt();
	SCR_HEIGHT = Window["Height"].GetInt();

	title = Window["Title"].GetString();
	if (doc.HasMember("Scene")) {
		scene = doc["Scene"].GetString();
	}
	if (doc.HasMember("Gravity")) {
		const rapidjson::Value& grArray = doc["Gravity"];
		for (int i = 0; i < 3; i++) {
			g_Gravity[i] = grArray[i].GetFloat();
		}

	}

	return true;

	//std::string language = doc["Language"].GetString();
	//ASSERT_NE(language, "");
	//if (language == "English") { TextRend.setLang(ENGLISH); }
	//else if (language == "Spanish") { TextRend.setLang(SPANISH); }
	//else if (language == "Japanese") { TextRend.setLang(JAPANESE); }
	//else if (language == "Ukrainian") { TextRend.setLang(UKRAINAN); }
	//else if (language == "Polish") { TextRend.setLang(POLSKA); }

}



