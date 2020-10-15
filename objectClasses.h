#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class shape
{
	float * vertices;
	size_t vertCnt;

	shape(float vertices[], size_t vertCnt);

	void translate(float x, float y, float z);

	void scale(float x, float y, float z);

	void rotate(float pitch, float roll, float yaw);
};