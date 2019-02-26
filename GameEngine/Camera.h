#ifndef CAMERA_H
#define CAMERA_H
#include "cGameObject.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>


#include <vector>




enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};


enum Camera_Type {
	FOLLOW,
	FLY
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


class Camera
{
public:

	Camera_Type Type;
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	float Yaw;
	float Pitch;
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;
	bool b_controlledByScript;
	glm::mat4 newViewMat;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
		b_controlledByScript = false;
		glm::mat4 newViewMat = glm::mat4(0.0f);
		Type = FLY;
	}
	// Constructor with scalar values
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = glm::vec3(posX, posY, posZ);
		WorldUp = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
		b_controlledByScript = false;
		glm::mat4 newViewMat = glm::mat4(0.0f);
		Type = FLY;
	}

	//FollowCamera const
	Camera(cGameObject* Targetobj, glm::vec3 idealpos, float minDist, float maxSpeedDist, float maxSpd, float time = 0.0f, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
		b_controlledByScript = true;
		glm::mat4 newViewMat = glm::mat4(0.0f);
		Type = FOLLOW;
		b_Started = false;
		m_bIsDone = false;
		MovementSpeed = maxSpd * 1.2f;
		SetFollowCamera(Targetobj, idealpos, minDist, maxSpeedDist, maxSpd, time);

	}



	



	//For Command
	void SetViewMatrix(glm::mat4 newView) { this->newViewMat = newView; }

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		if (!this->b_controlledByScript) { return glm::lookAt(Position, Position + Front, Up);}
		else { return newViewMat; }
	}

	// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		if (Type == FOLLOW) 
		{
			float velocity = MovementSpeed * deltaTime;

			if (direction == FORWARD) 
			{

				idealRelPosition.x +=  velocity;
			}
			if (direction == BACKWARD)
			{

				idealRelPosition.x -= velocity;
			}
			if (direction == LEFT)
				idealRelPosition.z -=  velocity;
			if (direction == RIGHT)
				idealRelPosition.z +=  velocity;
			if (direction == UP)
				idealRelPosition += WorldUp * velocity;
			if (direction == DOWN)
				idealRelPosition -= WorldUp * velocity;
		}
		else {

			float velocity = MovementSpeed * deltaTime;
			if (direction == FORWARD)
				Position += Front * velocity;
			if (direction == BACKWARD)
				Position -= Front * velocity;
			if (direction == LEFT)
				Position -= Right * velocity;
			if (direction == RIGHT)
				Position += Right * velocity;
			if (direction == UP)
				Position += WorldUp * velocity;
			if (direction == DOWN)
				Position -= WorldUp * velocity;
		}
	}

	// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		// Update Front, Right and Up Vectors using the updated Euler angles
		updateCameraVectors();
	}


	//void ProcessMouseScroll(float yoffset)
	//{
	//	if (Zoom >= 1.0f && Zoom <= 45.0f)
	//		Zoom -= yoffset;
	//	if (Zoom <= 1.0f)
	//		Zoom = 1.0f;
	//	if (Zoom >= 45.0f)
	//		Zoom = 45.0f;
	//}



	void UpdateCamera(float dt)
	{
		if (Type == FOLLOW && !m_bIsDone) {

			if (!this->b_Started)
			{
				this->b_Started = true;
				this->initialTime = glfwGetTime();
				this->initPosition = Position;
			}

			this->elapsedTime = glfwGetTime() - this->initialTime;

			this->finalPosition = this->targetObj->position;

			this->idealPosition = this->finalPosition + this->idealRelPosition;

			glm::vec3 direction = this->idealPosition - Position;
			float distance = glm::length(direction);

			glm::vec3 directionNormal = glm::normalize(direction);





			float speedRatio = glm::smoothstep(this->minDistance,
				this->maxSpeedDistance,
				distance);

			float scaledMaxSpeed = speedRatio * this->maxSpeed;

			glm::vec3 vecMaxSpeed = glm::vec3(scaledMaxSpeed, scaledMaxSpeed, scaledMaxSpeed);

			glm::vec3 velocity = directionNormal * vecMaxSpeed;


			glm::vec3 deltaPosition = (float)dt * velocity;

			Position += deltaPosition;

			newViewMat = glm::lookAt(Position, targetObj->position, glm::vec3(0.0f, 1.0f, 0.0f));
			b_controlledByScript = true;



			if (this->elapsedTime > time && this->time != 0.0f)
			{
				this->m_bIsDone = true;
			}

		}



		
	
	}






private:




	glm::vec3 initPosition;
	glm::vec3 finalPosition;
	glm::vec3 idealPosition;
	glm::vec3 idealRelPosition;


	bool b_Started;
	bool m_bIsDone;

	double initialTime;
	double elapsedTime;
	
	float minDistance;
	float maxSpeedDistance;
	float maxSpeed;
	cGameObject* targetObj;
	float time;



	void SetFollowCamera(cGameObject* Targetobj, glm::vec3 idealpos, float minDist, float maxSpeedDist, float maxSpd, float time = 0.0f)
	{
		
		//this->theObj = obj;
		this->idealRelPosition = idealpos;
		this->minDistance = minDist;
		this->maxSpeedDistance = maxSpeedDist;
		this->maxSpeed = maxSpd;
		this->targetObj = Targetobj;
		this->time = time;
		this->b_Started = false;

	}
	
	void updateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		Right = glm::normalize(glm::cross(Front, WorldUp));  
		Up = glm::normalize(glm::cross(Right, Front));

	}
};


extern glm::vec3 cameraPos;
extern glm::vec3 cameraFront;
extern glm::vec3 cameraUp;

//camera control 
extern Camera camera;

#endif