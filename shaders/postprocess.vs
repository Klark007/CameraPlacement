#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 2) in vec2 uv;

out vec2 tex_coord;

void main() 
{
	vec2 d_pos = vec2(pos.x*2 - 1, pos.z*2 - 1); // screen position between -1 and 1
	gl_Position = vec4(d_pos.x, d_pos.y, 0.0, 1.0); 
	tex_coord = uv;
}