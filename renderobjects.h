#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaders.h"
#include <map>

struct texParam2D
{
	unsigned int wrapS, wrapT, filterMin, filterMag;
	texParam2D(unsigned int wrapS, unsigned int wrapT, unsigned int filterMin, unsigned int filterMag) :
	wrapS(wrapS), wrapT(wrapT), filterMin(filterMin), filterMag(filterMag) {}

};

class texture2D
{
public:
	int width, height, nrChannels;
	unsigned int textureID;
	texture2D(texParam2D params, std::string file);

	~texture2D();
};

bool removeFromRendering(std::string UID, bool del = false);
bool removeFromMemory(std::string UID, bool del = false);

class renderobject
{
private:
	unsigned int VAO, VBO, drawType;
	Shader * shader;
public:
	bool updated = false;
	float * vertices;
	size_t vertCnt;
	size_t stride;
	glm::mat4 trans = glm::mat4(1.0f);
	std::string UID = "";	

	renderobject(float vertices[], size_t vertCnt, size_t stride, unsigned int VAO, unsigned int VBO, Shader * shader, unsigned int drawType = GL_DYNAMIC_DRAW);

	bool addToRendering(std::string UID);

	bool addToMemory(std::string UID);

	bool transferToRendering();

	bool transferToMemory();

	void translate(glm::vec3 transxyz);

	void scale(glm::vec3 scalexyz);

	void rotate(float degrees, glm::vec3 axis);

	void draw();

	~renderobject();
};

std::map<std::string, renderobject*>* getRendering();

std::map<std::string, renderobject*>* getMemory();

