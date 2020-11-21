#include "camera.h"

camera::camera(glm::vec3 position, glm::vec3 up, double fov, double yaw, double pitch):
	fov(fov), yaw(yaw), pitch(pitch), sensitivity(0.1), speed(2.5),
	pos(position), front(glm::vec3(0.0f, 0.0f, -1.0f)), worldUp(up)
{
	updateCameraVectors();
}

camera::camera(double posX, double posY, double posZ, double upX, double upY, double upZ, double fov, double yaw, double pitch):
	fov(fov), yaw(yaw), pitch(pitch), sensitivity(0.1), speed(2.5),
	pos(glm::vec3(posX, posY, posZ)), front(glm::vec3(0.0f, 0.0f, -1.0f)), worldUp(glm::vec3(upX, upY, upZ))
{
	updateCameraVectors();
}

void camera::updateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);
	// also re-calculate the Right and Up vector
	right = glm::normalize(glm::cross(front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	up = glm::normalize(glm::cross(right, front));
}

glm::mat4 camera::GetViewMatrix()
{
	return glm::lookAt(pos, pos + front, up);
}

void camera::moveFov(double amnt)
{
	amnt += this->fov;
	if (amnt < 1.0)
	{
		this->fov = 1.0;
		return;
	}
	if (amnt > this->maxFov)
	{
		this->fov = this->maxFov;
		return;
	}
	this->fov = amnt;
}

void camera::setFov(double fov) { moveFov(fov - this->fov); }

double camera::getFov() { return this->fov; }

void camera::movePitch(double amnt)
{
	amnt += this->pitch;
	if (this->maxPitch != -1000.2)
	{
		if (amnt < -maxPitch)
		{
			this->pitch = -maxPitch;
			return;
		}
		if (amnt > this->maxPitch)
		{
			this->pitch = this->maxPitch;
			return;
		}
	}
	this->pitch = amnt;
	updateCameraVectors();
}

void camera::setPitch(double pitch) { moveFov(pitch - this->pitch); }

double camera::getPitch() { return this->pitch; }

void camera::moveYaw(double amnt)
{
	amnt += this->yaw;
	if (this->maxYaw != -1000.2)
	{
		if (amnt < -maxYaw)
		{
			this->yaw = -maxYaw;
			return;
		}
		if (amnt > this->maxYaw)
		{
			this->yaw = this->maxYaw;
			return;
		}
	}
	this->yaw = amnt;
	updateCameraVectors();
}

void camera::setYaw(double yaw) { moveFov(yaw - this->yaw); }

double camera::getYaw() { return this->yaw; }