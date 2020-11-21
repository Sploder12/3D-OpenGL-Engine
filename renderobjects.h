#pragma once
#include "camera.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "stb_image.h"

#ifndef GLM_H
#define GLM_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#endif

#include "shaders.h"
#include <map>

namespace renderer
{
	#define SWIDTH 800
	#define SHEIGHT 600

	camera* getCamera();
	void setCamera(camera* cam);

	glm::vec3 faceNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
	float* genFaceNormals(float* verticies, unsigned int faceCnt);

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
		texture2D(texParam2D* params, std::string file, bool flip = false);

		~texture2D();
	};

	
	bool removeFromActive(std::string UID, bool del = false);
	bool removeFromMemory(std::string UID, bool del = false);

	struct Base
	{
		Shader* shader;
		unsigned int VAO, VBO, drawType;
		float* faceNormals;
		bool updated = false;
		glm::mat4 trans = glm::mat4(1.0f);
		std::string UID = "";

		Base(Shader* shader, unsigned int drawType = GL_STATIC_DRAW);

		bool addToActive(std::string UID);

		bool addToMemory(std::string UID);

		bool transferToActive();

		bool transferToMemory();

		void translate(glm::vec3 transxyz);

		void scale(glm::vec3 scalexyz);

		void rotate(float degrees, glm::vec3 axis);

		virtual void draw() = 0;
	};

	struct Basic : Base
	{
		float* vertices;
		float* colors;
		GLsizei vertCnt;

		Basic(float vertices[], float colors[], GLsizei vertCnt, Shader* shader, unsigned int drawType = GL_DYNAMIC_DRAW);

		void draw();
	};

	struct BasicTextured : Base
	{
		float* vertices;
		float* colors;
		float* texturecords;
		GLsizei vertCnt;
		texture2D* texture;

		BasicTextured(float vertices[], float colors[], float texturecords[], GLsizei vertCnt, Shader* shader, texture2D* texture, unsigned int drawType = GL_DYNAMIC_DRAW);

		void draw();
	};

	struct PointLight : Base
	{
		float* vertices;
		GLsizei vertCnt;
		glm::vec3 lightpos;
		glm::vec3 lightcolor;

		PointLight(float vertices[], GLsizei vertCnt, Shader* shader, glm::vec3 lightcolor, glm::vec3 lightpos, unsigned int drawType = GL_DYNAMIC_DRAW);

		void translate(glm::vec3 transxyz);

		void draw();
	};

	std::map<std::string, Base*>* getActive();
	std::map<std::string, Base*>* getMemory();
}

