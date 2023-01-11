#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aNormal;

out vec3 Normal;
out vec3 Position;

uniform mat4 transform;

uniform mat4 model;

void main()
{
    Normal = mat3(transpose(inverse(model))) * aNormal;
    Position = vec3(model * vec4(aPos, 1.0));
    gl_Position = transform * vec4(aPos, 1.0);
}  