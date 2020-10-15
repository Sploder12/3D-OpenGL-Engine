#include "Render.h"

void clearScreen(float red, float green, float blue, float alpha)
{
	glClearColor(red, green, blue, alpha);
	glClear(GL_COLOR_BUFFER_BIT);
}

void render(GLFWwindow* window)
{


	clearScreen(1.0f, 0.5f, 0.1f, 1.0f);
}