#pragma once

#ifndef GLM_H
#define GLM_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif

class camera
{
private:
	double fov;
	double pitch, yaw;
public:
	
	double maxFov = 45;
	double maxPitch = 89.0, maxYaw = -1000.2;

	double sensitivity, speed;

	glm::vec3 pos;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;

	glm::vec3 worldUp;
	
	camera(glm::vec3 position = glm::vec3(0.0, 0.0, 0.0), glm::vec3 up = glm::vec3(0.0, 1.0, 0.0), double fov = 45.0, double yaw = -90.0, double pitch = 0.0);
	
	camera(double posX = 0.0, double posY = 0.0, double posZ = 0.0, double upX = 0.0, double upY = 1.0, double upZ = 0.0, double fov = 45.0, double yaw = -90.0, double pitch = 0.0);

	void updateCameraVectors();

	glm::mat4 GetViewMatrix();

	void moveFov(double amnt);
	void setFov(double fov);
	double getFov();

	void movePitch(double amnt);
	void setPitch(double pitch);
	double getPitch();

	void moveYaw(double amnt);
	void setYaw(double yaw);
	double getYaw();


};