#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 textCoords;

uniform mat4 projection, view, model, transform;

out vec2 texture_coordinates;

void main()
{
    texture_coordinates = textCoords;
    gl_Position = projection * view * model * transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
}
