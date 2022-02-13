#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 textCoords;

uniform mat4 transform;

out vec2 texture_coordinates;

void main()
{
    texture_coordinates = textCoords;
    gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
