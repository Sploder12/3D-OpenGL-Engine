#include "renderobjects.h"

texture2D::texture2D(texParam2D params, std::string file)
{
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.filterMin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.filterMag);
	// load and generate the texture
	unsigned char* data = stbi_load(file.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

texture2D::~texture2D() 
{

}

std::map<std::string, renderobject*> renderObjects; //These are actively rendered
std::map<std::string, renderobject*> memoryObjects; //These are not actively rendered but stored for later

bool removeFromRendering(std::string UID, bool del)
{
	if (del)
	{
		delete renderObjects.at(UID);
	}
	return (renderObjects.erase(UID) > 0) ? false : true;
}

bool removeFromMemory(std::string UID, bool del)
{
	if (del)
	{
		delete renderObjects.at(UID);
	}
	return (memoryObjects.erase(UID) > 0)? false: true;
}

//Base Object
renderobject::renderobject(float vertices[], size_t vertCnt, size_t stride, unsigned int VAO, unsigned int VBO, Shader * shader, unsigned int drawType) :
	vertices(vertices), vertCnt(vertCnt), stride(stride), shader(shader), drawType(drawType)
{
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 4*stride*vertCnt, vertices, drawType);
}

bool renderobject::addToRendering(std::string UID)
{
	this->UID = UID;
	return renderObjects.insert(std::pair<std::string, renderobject*>(UID, this)).second;
}

bool renderobject::addToMemory(std::string UID)
{
	this->UID = UID;
	return memoryObjects.insert(std::pair<std::string, renderobject*>(UID, this)).second;
}

bool renderobject::transferToRendering()
{
	bool tmp = removeFromMemory(this->UID);
	return this->addToRendering(this->UID) && tmp;
}

bool renderobject::transferToMemory()
{
	bool tmp = removeFromRendering(this->UID);
	return this->addToMemory(this->UID) && tmp;
}

void renderobject::translate(glm::vec3 transxyz)
{
	trans = glm::translate(trans, transxyz);
	this->updated = true;
}

void renderobject::scale(glm::vec3 transxyz)
{
	trans = glm::scale(trans, transxyz);
	this->updated = true;
}

void renderobject::rotate(float degrees, glm::vec3 axis)
{
	trans = glm::rotate(trans, glm::radians(degrees), axis);
	this->updated = true;
}

void renderobject::draw()
{
	shader->use();
	unsigned int transformLoc = glGetUniformLocation(shader->ID, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

	//glBindVertexArray(this->VAO);
	//glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		
	glDrawArrays(GL_TRIANGLES, 0, 3);
	this->updated = false;
}

renderobject::~renderobject()
{

}

//has to be defined after object
std::map<std::string, renderobject*>* getRendering()
{
	return &renderObjects;
}

std::map<std::string, renderobject*>* getMemory()
{
	return &memoryObjects;
}
