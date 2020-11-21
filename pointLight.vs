#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightcolor;

out vec3 color;

void main()
{
	color = lightcolor;
	gl_Position = projection * view * transform * vec4(aPos, 1.0);
}