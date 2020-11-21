#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

in vec3 aNormal;

out vec3 ourColor;
out vec3 normal;
out vec3 fragPos;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    fragPos = vec3(transform * vec4(aPos, 1.0));
    gl_Position = projection * view * vec4(fragPos, 1.0);
    ourColor = aColor;
    normal = mat3(transpose(inverse(transform))) * aNormal;
}