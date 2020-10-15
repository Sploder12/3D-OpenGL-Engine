#pragma once
#include "renderobjects.h"

bool compileShaders();

std::map<std::string, Shader>* getShaders();

void clearScreen(float red, float green, float blue, float alpha = 1.0f);

void render(GLFWwindow* window, unsigned int VAO, unsigned int VBO);