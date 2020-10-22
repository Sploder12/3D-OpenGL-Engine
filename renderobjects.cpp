#include "renderobjects.h"

renderer::texture2D::texture2D(texParam2D* params, std::string file, bool flip)
{
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params->wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params->wrapT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params->filterMin);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params->filterMag);

	stbi_set_flip_vertically_on_load(!flip);
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

renderer::texture2D::~texture2D()
{

}

std::map<std::string, renderer::Base*> activeObjects; //These are actively rendered
std::map<std::string, renderer::Base*> memoryObjects; //These are not actively rendered but stored for later

bool renderer::removeFromActive(std::string UID, bool del)
{
	if (del)
	{
		delete activeObjects.at(UID);
	}
	return (activeObjects.erase(UID) > 0) ? false : true;
}

bool renderer::removeFromMemory(std::string UID, bool del)
{
	if (del)
	{
		delete activeObjects.at(UID);
	}
	return (memoryObjects.erase(UID) > 0)? false: true;
}

//Base Object
renderer::Base::Base(Shader * shader, unsigned int drawType) :
	shader(shader), drawType(drawType)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
}

bool renderer::Base::addToActive(std::string UID)
{
	this->UID = UID;
	return activeObjects.insert(std::pair<std::string, renderer::Base*>(UID, this)).second;
}

bool renderer::Base::addToMemory(std::string UID)
{
	this->UID = UID;
	return memoryObjects.insert(std::pair<std::string, renderer::Base*>(UID, this)).second;
}

bool renderer::Base::transferToActive()
{
	bool tmp = removeFromMemory(this->UID);
	return this->addToActive(this->UID) && tmp;
}

bool renderer::Base::transferToMemory()
{
	bool tmp = removeFromActive(this->UID);
	return this->addToMemory(this->UID) && tmp;
}

void renderer::Base::translate(glm::vec3 transxyz)
{
	trans = glm::translate(trans, transxyz);
	this->updated = true;
}

void renderer::Base::scale(glm::vec3 transxyz)
{
	trans = glm::scale(trans, transxyz);
	this->updated = true;
}

void renderer::Base::rotate(float degrees, glm::vec3 axis)
{
	trans = glm::rotate(trans, glm::radians(degrees), axis);
	this->updated = true;
}

renderer::Basic::Basic(float vertices[], GLsizei vertCnt, GLsizei stride, Shader* shader, unsigned int drawType) :
	renderer::Base(shader, drawType), vertices(vertices), vertCnt(vertCnt)
{
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBufferData(GL_ARRAY_BUFFER, 4*vertCnt*stride, vertices, drawType);
}

void renderer::Basic::draw()
{
	shader->use();
	glBindVertexArray(VAO);

	unsigned int transformLoc = glGetUniformLocation(shader->ID, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

	glDrawArrays(GL_TRIANGLES, 0, vertCnt);

	this->updated = false;
}

renderer::BasicTextured::BasicTextured(float vertices[], GLsizei vertCnt, GLsizei stride, Shader* shader, texture2D* texture, unsigned int drawType):
	renderer::Base(shader, drawType), vertices(vertices), vertCnt(vertCnt), texture(texture)
{
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBufferData(GL_ARRAY_BUFFER, 4 * vertCnt * stride, vertices, drawType);
}

void renderer::BasicTextured::draw()
{

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture->textureID);
	
	shader->use();
	glBindVertexArray(VAO);
	unsigned int uniformLoc = glGetUniformLocation(shader->ID, "transform");
	glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(trans));

	glDrawArrays(GL_TRIANGLES, 0, vertCnt);

	this->updated = false;
}


//has to be defined after object
std::map<std::string, renderer::Base*>* renderer::getActive()
{
	return &activeObjects;
}

std::map<std::string, renderer::Base*>* renderer::getMemory()
{
	return &memoryObjects;
}
