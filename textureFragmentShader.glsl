#version 330 core

out vec4 FragColor;

in vec2 texture_coordinates;

uniform bool use_color;
uniform vec4 set_color;


uniform sampler2D a_texture;

void main()
{
    if (use_color)
    {
    FragColor = set_color;
    return;
    }
    FragColor = texture(a_texture, texture_coordinates);

    
}