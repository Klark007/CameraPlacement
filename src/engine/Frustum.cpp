#include "Frustum.h"

#include <iostream>
#include "Helper.h"

Frustum::Frustum(std::shared_ptr<Camera> camera, float far_plane_scale)
{
	std::vector<Vertex> vertices;

	glm::mat4 inv_vp = glm::inverse(camera->generate_projection_mat() * camera->generate_view_mat());

	for (int i_z = 0; i_z <= 1; i_z += 1) {
		for (int i_y = -1; i_y <= 1; i_y += 2) {
			for (int i_x = -1; i_x <= 1; i_x += 2) {
				glm::vec4 point = glm::vec4(
					i_x,
					i_y,
					(i_z == 0) ? -1 : far_plane_scale,
					1
				);
				glm::vec4 backprojected_point = inv_vp * point;

				backprojected_point /= backprojected_point.w;

				Vertex v{ glm::vec3(backprojected_point), glm::vec3(1), glm::vec2(0) };
				vertices.emplace_back(v);
			}
		}
	}
	std::vector<Index> indices = {
		// Near
		0,1,
		0,2,
		1,3,
		2,3,

		// Far
		4,5,
		4,6,
		5,7,
		6,7,

		0,4,
		1,5,
		2,6,
		3,7
	};

	mesh = Mesh(vertices, indices, 2);


	// Point at camera position
	std::vector<Vertex> pv{ { camera->get_pos(), glm::vec3(1), glm::vec2(0) } };
	std::vector<Index> pi = { 0 };

	point = Mesh(pv, pi, 1);
}

void Frustum::draw(std::shared_ptr<Program> program) const
{
	material.bind(program);
	mesh.draw();
	point.draw();
}
