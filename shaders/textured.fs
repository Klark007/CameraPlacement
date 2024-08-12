#version 450 core

uniform sampler2D texture_color;

in vec3 w_position;
in vec3 w_normal;
in vec2 tex_coord;

out vec4 FragColor;

void main()
{
	FragColor = texture(texture_color, tex_coord);
}