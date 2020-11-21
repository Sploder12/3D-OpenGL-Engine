#include "renderobjects.h"

camera* curCam;

camera* renderer::getCamera() { return curCam; }
void renderer::setCamera(camera* cam) { curCam = cam; }

glm::vec3 renderer::faceNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
	glm::vec3 v1 = p2 - p1;
	glm::vec3 v2 = p3 - p1;

	return glm::cross(v1, v2);
}

float* renderer::genFaceNormals(float* verticies, unsigned int faceCnt)
{
	float * out = new float[faceCnt * 9];
	for (unsigned int i = 0; i < faceCnt; i++)
	{
		glm::vec3 p1 = glm::vec3(verticies[i * 9], verticies[i * 9 + 1], verticies[i * 9 + 2]);
		glm::vec3 p2 = glm::vec3(verticies[i * 9 + 3], verticies[i * 9 + 4], verticies[i * 9 + 5]);
		glm::vec3 p3 = glm::vec3(verticies[i * 9 + 6], verticies[i * 9 + 7], verticies[i * 9 + 8]);
		glm::vec3 norm = renderer::faceNormal(p1, p2, p3);

		if (i < 2) norm.z = -norm.z;
		if (i > 5) norm.x = -norm.x;
		if (i > 9) norm.y = -norm.y;
		out[i * 9] = norm.x;
		out[i * 9 + 1] = norm.y;
		out[i * 9 + 2] = norm.z;
		out[i * 9 + 3] = norm.x;
		out[i * 9 + 4] = norm.y;
		out[i * 9 + 5] = norm.z;
		out[i * 9 + 6] = norm.x;
		out[i * 9 + 7] = norm.y;
		out[i * 9 + 8] = norm.z;
	}
	return out;
}

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

renderer::Basic::Basic(float vertices[], float colors[], GLsizei vertCnt, Shader* shader, unsigned int drawType) :
	renderer::Base(shader, drawType), vertices(vertices), colors(colors), vertCnt(vertCnt)
{
	faceNormals = genFaceNormals(vertices, vertCnt/3);
	//verts
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(12 * vertCnt));
	glEnableVertexAttribArray(1);
	//facenormals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(24 * vertCnt));
	glEnableVertexAttribArray(2);
	//data
	glBufferData(GL_ARRAY_BUFFER, 36 * vertCnt, NULL, drawType);

	glBufferSubData(GL_ARRAY_BUFFER, 0, 12 * vertCnt, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 12 * vertCnt, 12 * vertCnt, colors);
	glBufferSubData(GL_ARRAY_BUFFER, 24 * vertCnt, 12 * vertCnt, faceNormals);
}

void renderer::Basic::draw()
{
	camera* cam = getCamera();

	shader->use();
	glBindVertexArray(VAO);

	unsigned int transformLoc = glGetUniformLocation(shader->ID, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
	
	glm::mat4 projection = glm::perspective(float(glm::radians(cam->getFov())), (float)SWIDTH / (float)SHEIGHT, 0.1f, 100.0f);
	shader->setMat4("projection", projection);

	// camera/view transformation
	glm::mat4 view = glm::lookAt(cam->pos, cam->pos + cam->front, cam->up);
	shader->setMat4("view", view);

	shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);

	shader->setVec3("lightPos", glm::vec3(1.1f, 0.0f, 1.1f));
	shader->setVec3("viewPos", cam->pos);

	glDrawArrays(GL_TRIANGLES, 0, vertCnt);

	this->updated = false;
}

renderer::BasicTextured::BasicTextured(float vertices[], float colors[], float texturecords[], GLsizei vertCnt, Shader* shader, texture2D* texture, unsigned int drawType):
	renderer::Base(shader, drawType), vertices(vertices), colors(colors), texturecords(texturecords), vertCnt(vertCnt), texture(texture)
{
	faceNormals = genFaceNormals(vertices, vertCnt / 3);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(12*vertCnt));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(24*vertCnt));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(32*vertCnt));
	glEnableVertexAttribArray(3);
	
	glBufferData(GL_ARRAY_BUFFER, 44*vertCnt, NULL, drawType);

	glBufferSubData(GL_ARRAY_BUFFER, 0, 12 * vertCnt, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 12 * vertCnt, 12 * vertCnt, colors);
	glBufferSubData(GL_ARRAY_BUFFER, 24 * vertCnt, 8 * vertCnt, texturecords);
	glBufferSubData(GL_ARRAY_BUFFER, 32 * vertCnt, 12 * vertCnt, faceNormals);
}

void renderer::BasicTextured::draw()
{
	camera* cam = getCamera();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture->textureID);
	
	shader->use();
	glBindVertexArray(VAO);
	unsigned int uniformLoc = glGetUniformLocation(shader->ID, "transform");
	glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(trans));

	glm::mat4 projection = glm::perspective(float(glm::radians(cam->getFov())), (float)SWIDTH / (float)SHEIGHT, 0.1f, 100.0f);
	shader->setMat4("projection", projection);

	// camera/view transformation
	glm::mat4 view = glm::lookAt(cam->pos, cam->pos + cam->front, cam->up);
	shader->setMat4("view", view);

	shader->setVec3("lightColor", 1.0f, 0.9f, 0.0f);

	shader->setVec3("lightPos", glm::vec3(1.1f, 0.0f, 1.1f));
	shader->setVec3("viewPos", cam->pos);

	glDrawArrays(GL_TRIANGLES, 0, vertCnt);

	this->updated = false;
}

renderer::PointLight::PointLight(float vertices[], GLsizei vertCnt, Shader* shader, glm::vec3 lightcolor, glm::vec3 lightpos, unsigned int drawType):
	renderer::Base(shader, drawType), vertices(vertices), vertCnt(vertCnt),  lightpos(lightpos), lightcolor(lightcolor)
{

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBufferData(GL_ARRAY_BUFFER, 12*vertCnt, vertices, drawType);

}

void renderer::PointLight::translate(glm::vec3 transxyz)
{
	trans = glm::translate(trans, transxyz);
	lightpos += transxyz;
	this->updated = true;
}

void renderer::PointLight::draw()
{
	camera* cam = getCamera();
	shader->use();
	glBindVertexArray(VAO);
	unsigned int uniformLoc = glGetUniformLocation(shader->ID, "transform");
	glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(trans));

	glm::mat4 projection = glm::perspective(float(glm::radians(cam->getFov())), (float)SWIDTH / (float)SHEIGHT, 0.1f, 100.0f);
	shader->setMat4("projection", projection);

	// camera/view transformation
	glm::mat4 view = glm::lookAt(cam->pos, cam->pos + cam->front, cam->up);
	shader->setMat4("view", view);

	shader->setVec3("lightcolor", this->lightcolor);

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
