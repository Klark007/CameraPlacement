#pragma once

#include <glm/glm.hpp>

#include <vector>

// Mesh handles sending vertices and indices to the gpu

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texture_coord;
};

typedef unsigned int Index;

class Mesh {
public:
	Mesh() = default;
	Mesh(std::vector<Vertex> vertices, std::vector<Index> indices, unsigned int vertices_per_face = 3);

	void draw() const;
private:
	unsigned int vao;

	unsigned int vertices_per_face;

	std::vector<Vertex> vertices;
	std::vector<Index> indices;
};