#include "Mesh.h"

#include <GL/glew.h>

// find next multiple of alignment starting from size
GLsizei align(GLsizei size, GLsizei alignment) {
	// compute the size / alignment rounded up and multiply by alignment
	return (size + alignment - 1) / alignment * alignment;
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<Index> indices, unsigned int vertices_per_face)
	: vertices{ vertices }, indices{ indices }, vertices_per_face{ vertices_per_face }
{
	// model with 0 indices can be created due to Model.cpp ignoring lines but them still existing in the model offset
	if (indices.size() == 0) {
		return;
	}
	unsigned int vbo, ibo;
	
	glCreateVertexArrays(1, &vao);

	// vertices
	glCreateBuffers(1, &vbo);
	glNamedBufferStorage(vbo, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_STORAGE_BIT);

	// indices
	glCreateBuffers(1, &ibo);
	glNamedBufferStorage(ibo, indices.size() * sizeof(Index), indices.data(), GL_DYNAMIC_STORAGE_BIT);


	glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));
	glVertexArrayElementBuffer(vao, ibo);

	// vertex attributes
	glEnableVertexArrayAttrib(vao, 0);
	glEnableVertexArrayAttrib(vao, 1);
	glEnableVertexArrayAttrib(vao, 2);

	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
	glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
	glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texture_coord));
	
	glVertexArrayAttribBinding(vao, 0, 0);
	glVertexArrayAttribBinding(vao, 1, 0);
	glVertexArrayAttribBinding(vao, 2, 0);
}

void Mesh::draw() const
{
	glBindVertexArray(vao);

	GLenum mode = GL_POINTS;
	if (vertices_per_face == 3) {
		mode = GL_TRIANGLES;
	}
	else if (vertices_per_face == 2) {
		mode = GL_LINES;
	}
	glDrawElements(mode, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
