#include "AABB.h"

AABB::AABB(glm::vec3 min, glm::vec3 max)
{
	// hard coded vertices
	vertices = {
		{glm::vec3(min.x,min.y,min.z),glm::vec3(1), glm::vec2(0)}, // 0
		{glm::vec3(max.x,min.y,min.z),glm::vec3(1), glm::vec2(0)}, // 1
		{glm::vec3(min.x,max.y,min.z),glm::vec3(1), glm::vec2(0)}, // 2
		{glm::vec3(min.x,min.y,max.z),glm::vec3(1), glm::vec2(0)}, // 3
		{glm::vec3(max.x,max.y,min.z),glm::vec3(1), glm::vec2(0)}, // 4
		{glm::vec3(max.x,min.y,max.z),glm::vec3(1), glm::vec2(0)}, // 5
		{glm::vec3(min.x,max.y,max.z),glm::vec3(1), glm::vec2(0)}, // 6
		{glm::vec3(max.x,max.y,max.z),glm::vec3(1), glm::vec2(0)}, // 7
	};

	// AABB drawing only enabled in debug mode
#ifdef DEBUG
	std::vector<Index> indices = {
		// left face
		0,1,
		0,3,
		1,5,
		3,5,

		// back face
		0,2,
		3,6,
		2,6,

		// bottom face
		1,4,
		2,4,

		// right face
		4,7,
		6,7,

		// front face
		5,7
	};

	mesh = Mesh(vertices, indices, 2);
#endif
}

std::array<glm::vec3, 8> AABB::homogenous_mult_vertices(const glm::mat4& mvp_mat) const
{
	std::array<glm::vec3, 8> proj_vertices{};

	for (unsigned int i = 0; i < 8; i++) {
		glm::vec4 proj_p = mvp_mat * glm::vec4(vertices[i].position, 1);
		proj_vertices[i] = proj_p.xyz() / proj_p.w;
	}

	return proj_vertices;
}

#ifdef DEBUG
void AABB::draw(std::shared_ptr<Program> program) const
{
	material.bind(program);
	mesh.draw();
}
#endif

inline int check_boundary(glm::vec3 p, int idx) {
	if (p[idx] < -1) {
		return -1;
	}
	else if (p[idx] > 1) {
		return 1;
	}
	else {
		return 0;
	}
}

bool AABB::cull(const glm::mat4& vp_mat, const glm::mat4& m_mat) const
{	
	std::array<glm::vec3, 8> projected_corners = homogenous_mult_vertices(vp_mat * m_mat);

	// could be improved by testing the obb against the extracted planes
	for (unsigned int i = 0; i < 3; i++) {
		bool sucess = true;
		int side = check_boundary(projected_corners[0], i);
		for (unsigned int j = 1; j < 8; j++) {
			sucess = sucess && (side == check_boundary(projected_corners[j], i));
		}

		if (sucess && side != 0) {
			return true;
		}
	}

	return false;
}