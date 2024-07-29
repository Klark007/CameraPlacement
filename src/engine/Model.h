#pragma once

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "Mesh.h"
#include "Material.h"
#include "AABB.h"

#include <string>
#include <vector>
#include <unordered_set>
#include  <tuple>

class Model {
public:
	Model(const std::string& file_path);
	void draw(std::shared_ptr<Program> program, const glm::mat4& vp_mat, bool frustum_culling = false, bool draw_aabb = false);
private:
	std::vector<std::tuple<unsigned int, Mesh, AABB>> meshes; // first element is index into materials vector
	std::vector<Material> materials;
	std::vector<std::pair<unsigned int, glm::mat4>> instances; // first index into meshes vector, second transform of the mesh

	std::vector<bool> instance_culled;
	std::vector<unsigned int> idx_range;

	void load_scene(const aiScene* scene, const aiNode* node, glm::mat4 transform);
};