#pragma once

#include "glm/glm.hpp"

#include "Mesh.h"
#include "Program.h"
#include "Material.h"

#include "Camera.h"

#include <memory>


class Frustum {
public:
	Frustum(std::shared_ptr<Camera> camera, float far_plane_scale);
private:
	Mesh mesh;
	Mesh point;

	const Material material = Material(
		aiColor4t<float>(0),
		aiColor4t<float>(0),
		aiColor4t<float>(0.2, 0.2, 0.2, 1.0),
		aiColor4t<float>(0),
		0
	);
public:
	void draw(std::shared_ptr<Program> program) const;
};