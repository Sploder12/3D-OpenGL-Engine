#include "Render.h"

std::map<std::string, Shader> shaders;

bool compileShaders()
{
	bool res = true;
	res &= shaders.insert(std::pair<std::string, Shader>("example", Shader("example.vs", "example.fs"))).second;
	return res;
}

std::map<std::string, Shader>* getShaders()
{
	return &shaders;
}

void clearScreen(float red, float green, float blue, float alpha)
{
	glClearColor(red, green, blue, alpha);
	glClear(GL_COLOR_BUFFER_BIT);
}

void render(GLFWwindow* window, unsigned int VAO, unsigned int VBO)
{
	clearScreen(1.0f, 0.5f, 0.1f, 1.0f);

	std::map<std::string, renderobject*>::iterator objIt;
	for (objIt = getRendering()->begin(); objIt != getRendering()->end(); ++objIt)
	{
		objIt->second->draw();
	}
	
}