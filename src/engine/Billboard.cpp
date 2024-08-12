#include "Billboard.h"

Billboard::Billboard(glm::vec3 pos, float scale, std::shared_ptr<Texture> texture)
	: position {pos}, direction {glm::vec3(1,0,0)}, scale {scale}, icon {texture}
{
	std::vector<Vertex> vertices = {
		{glm::vec3(-1.0,-1.0,0.0), glm::vec3(0.0,1.0,0.0), glm::vec2(0.0,1.0)},
		{glm::vec3(-1.0,1.0,0.0), glm::vec3(0.0,1.0,0.0), glm::vec2(0.0,0.0)},
		{glm::vec3(1.0,-1.0,0.0), glm::vec3(0.0,1.0,0.0), glm::vec2(1.0,1.0)},
		{glm::vec3(1.0,1.0,0.0), glm::vec3(0.0,1.0,0.0), glm::vec2(1.0,0.0)}
	};
	std::vector<Index> indices = { 0,1,2,1,3,2 };
	mesh = { vertices, indices };
}

void Billboard::draw(std::shared_ptr<Program> program) const
{
	glm::mat4 model_inv = glm::lookAt(position + direction * 1e-3f, position + direction, glm::vec3(0, 1, 0));
	glm::mat4 model = glm::mat4(1);
	model[0] = glm::transpose(model_inv)[0] * scale;
	model[1] = glm::transpose(model_inv)[1] * scale;
	model[2] = glm::transpose(model_inv)[2] * scale;
	model[3] = glm::vec4(glm::mat3(model) / scale * -glm::vec3(model_inv[3]), 1);

	program->set_mat4f("model",model);

	material.bind(program);
	icon->bind(); // binds to texture unit 0

	mesh.draw();
}
