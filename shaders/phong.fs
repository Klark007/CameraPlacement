#version 450 core

struct Material {
	vec3 diffuse;
	vec3 specular;
	vec3 ambient;
	vec3 emissive;
	float shininess;
};
uniform Material mat;

uniform mat4 view;

struct Light {
	vec3 position;
};
Light light = Light(vec3(0.5));

in vec3 w_position;
in vec3 w_normal;
in vec2 tex_coord;

out vec4 FragColor;

void main()
{
	vec3 light_dir = normalize(light.position - w_position);
	vec3 cam_pos = view[3].xyz;

	vec3 diffuse_color = max(dot(w_normal, light_dir), 0.0) * mat.diffuse;
	vec3 col  = diffuse_color + mat.ambient;
	FragColor = vec4(col,1.0);
}