#pragma once

#include "assimp/color4.h"

#include "Program.h"

#include <memory>

class Material {
public:
	Material(aiColor4t<float> diffuse, aiColor4t<float> specular, aiColor4t<float> ambient, aiColor4t<float> emissive, float shininess);

	aiColor4t<float> diffuse;
	aiColor4t<float> specular;
	aiColor4t<float> ambient;
	aiColor4t<float> emissive;
	float shininess;

	inline void bind(std::shared_ptr<Program> program) const; 
};

inline void Material::bind(std::shared_ptr<Program> program) const
{
	program->set_vec3f("mat.diffuse", diffuse.r, diffuse.g, diffuse.b);
	program->set_vec3f("mat.specular", specular.r, specular.g, specular.b);
	program->set_vec3f("mat.ambient", ambient.r, ambient.g, ambient.b);
	program->set_vec3f("mat.emissive", emissive.r, emissive.g, emissive.b);
	program->set1f("mat.shininess", shininess);
}
