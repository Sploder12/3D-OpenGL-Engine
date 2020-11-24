#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

in vec3 aNormal;

out vec3 ourColor;
out vec2 TexCoord;
out vec3 fragPos;
out vec3 normal;

uniform mat4 transform;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    fragPos = vec3(transform * vec4(aPos, 1.0));
    normal = mat3(transpose(inverse(transform))) * aNormal;  
    ourColor = aColor;
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
    gl_Position = projection * view * vec4(fragPos, 1.0);
}