#pragma once

#define GLM_SWIZZLE
#include "glm/glm.hpp"
#include <array>

#include "Mesh.h"

#ifdef DEBUG
#include "Program.h"
#include "Material.h"

#include <memory>
#endif

// Axis aligned bounding box for frustum culling

class AABB {
public:
	AABB(glm::vec3 min, glm::vec3 max);
private:
	std::vector<Vertex>vertices;

	std::array<glm::vec3, 8> homogenous_mult_vertices(const glm::mat4& mvp_mat) const;
#ifdef DEBUG
	Mesh mesh;

	const Material material = Material(
		aiColor4t<float>(0),
		aiColor4t<float>(0),
		aiColor4t<float>(0.75f, 0.9f, 0.2f, 1.0f),
		aiColor4t<float>(0),
		0
	);
public:
	void draw(std::shared_ptr<Program> program) const;
#endif
public:
	bool cull(const glm::mat4& vp_mat, const glm::mat4& m_mat) const;
};	