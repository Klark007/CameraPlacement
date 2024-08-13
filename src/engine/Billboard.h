#pragma once

#include "glm/glm.hpp"

#include "Mesh.h"
#include "Program.h"
#include "Texture.h"
#include "Material.h"

#include <memory>

class Billboard {
public:
	Billboard(glm::vec3 pos, float scale, std::shared_ptr<Texture> texture);
private:
	glm::vec3 position;
	glm::vec3 direction;
	float scale;

	const Material material = Material(
		aiColor4t<float>(0),
		aiColor4t<float>(0),
		aiColor4t<float>(1.0, 1.0, 1.0, 1.0),
		aiColor4t<float>(0),
		0
	);

	Mesh mesh;
	std::shared_ptr<Texture> icon;
public:
	inline void set_dir(glm::vec3 dir) { direction = dir; }; // called once per frame to rotate plane to face camera
	void draw(std::shared_ptr<Program> program) const;
};