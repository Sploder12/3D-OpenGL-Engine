#include "Render.h"

std::map<std::string, Shader> shaders;

//options
bool doclear = true;

void doClear(bool val)
{
	doclear = val;
}

bool compileShaders()
{
	bool res = true;
	res &= shaders.insert(std::pair<std::string, Shader>("basicEX", Shader("basicEX.vs", "basicEX.fs"))).second;
	res &= shaders.insert(std::pair<std::string, Shader>("basicTexEX", Shader("basicTexEX.vs", "basicTexEX.fs"))).second;
	return res;
}

std::map<std::string, Shader>* getShaders()
{
	return &shaders;
}

void clearScreen(float red, float green, float blue, float alpha)
{
	if (doclear)
	{
		glClearColor(red, green, blue, alpha);
		glClear(GL_COLOR_BUFFER_BIT);
	}
}

void render(GLFWwindow* window)
{
	clearScreen(1.0f, 0.5f, 0.1f, 1.0f);

	std::map<std::string, renderer::Base*>::iterator objIt;
	for (objIt = renderer::getActive()->begin(); objIt != renderer::getActive()->end(); ++objIt)
	{
		objIt->second->draw();
	}
	
}