#pragma once
#include "renderobjects.h"

void doClear(bool val);

bool compileShaders();

std::map<std::string, Shader>* getShaders();

void clearScreen(float red, float green, float blue, float alpha = 1.0f);

void render(GLFWwindow* window);