#include "Model.h"

#include <GL/glew.h>

#include "Helper.h"
#include "Exception.h"

#include <iostream>
#include <array>
#include <numeric>
#include <execution>

#define PARALLEL_CULLING false // set for parallel cpu execution on all instances
#define THREAD_WORK_AMOUNT 1

Model::Model(const std::string& file_path)
    : meshes{ std::vector<std::tuple<unsigned int, Mesh, AABB>>() }, 
      materials{ std::vector <Material>() }, 
      instances{ std::vector<std::pair<unsigned int, glm::mat4>>()}
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(file_path,
        (aiProcessPreset_TargetRealtime_MaxQuality ^ aiProcess_SplitLargeMeshes) | // disable split large meshes
        aiProcess_FlipWindingOrder | // for culling
        aiProcess_GenBoundingBoxes   // create AABB
    );
    // output winding order is counter clockwise

    if (scene == nullptr) {
        throw IOException("Model not found at " + file_path, __FILE__, __LINE__);
    }

    // store materials
    for (unsigned int material_idx = 0; material_idx < scene->mNumMaterials; material_idx++) {
        aiMaterial* material = scene->mMaterials[material_idx];
        aiColor4t<float> diffuse, specular, ambient, emissive;
        float shininess;

        material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
        material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
        material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
        material->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
        material->Get(AI_MATKEY_SHININESS, shininess);

        materials.emplace_back(Material(diffuse, specular, ambient, emissive, shininess));
    }

    // store meshes
    for (unsigned int mesh_idx = 0; mesh_idx < scene->mNumMeshes; mesh_idx++) {
        // create mesh
        aiMesh* mesh = scene->mMeshes[mesh_idx];

        // check that all data needed is present
#ifdef DEBUG
        if (!mesh->HasPositions()) {
            throw IOException("Model " + file_path + " Mesh " + std::to_string(mesh_idx) + +" does not contain position ", __FILE__, __LINE__);
        }
        if (!mesh->HasNormals()) {
            throw IOException("Model " + file_path + " Mesh " + std::to_string(mesh_idx) + +" does not contain normals ", __FILE__, __LINE__);
        }
#endif
        
        std::vector<Vertex> vertices{};
        std::vector<Index> indices{};

        for (unsigned int vertex_idx = 0; vertex_idx < mesh->mNumVertices; vertex_idx++) {
            aiVector3D pos = mesh->mVertices[vertex_idx];
            aiVector3D n = mesh->mNormals[vertex_idx];
            aiVector3D uv = (mesh->HasTextureCoords(0)) ? (mesh->mTextureCoords[0])[vertex_idx] : aiVector3D(0,0,0);
            vertices.emplace_back(Vertex{
                glm::vec3(pos.x, pos.y, pos.z),
                glm::vec3(n.x, n.y, n.z),
                glm::vec2(uv.x, uv.y)
            });
        }

        unsigned int nr_vertices_per_face = 0;
        if (mesh->mNumFaces > 0) {
            nr_vertices_per_face = mesh->mFaces[0].mNumIndices;
        }

#ifdef DEBUG
        // use only triangles, lines and points; model should be triagulated by assimp
        if (nr_vertices_per_face > 3) {
            throw IOException("Model " + file_path + " Mesh " + std::to_string(mesh_idx) + +" failed triangulation", __FILE__, __LINE__);
        }
#endif

        for (unsigned int face_idx = 0; face_idx < mesh->mNumFaces; face_idx++) {
            aiFace face = mesh->mFaces[face_idx];
            
#ifdef DEBUG
            if (face.mNumIndices != nr_vertices_per_face) {
                throw IOException("Model " + file_path + " Mesh " + std::to_string(mesh_idx) + +" contains faces with different nr of vertices", __FILE__, __LINE__);
            }
#endif
            indices.emplace_back(face.mIndices[0]);
            indices.emplace_back(face.mIndices[1]);
            if (nr_vertices_per_face == 3) {
                indices.emplace_back(face.mIndices[2]);
            }
        }

        
        unsigned int material_idx = mesh->mMaterialIndex;
        
        meshes.emplace_back(std::tuple<unsigned int, Mesh, AABB>{ 
            material_idx, 
            Mesh{ vertices, indices, nr_vertices_per_face }, 
            AABB{ aiVector3ToGlm(mesh->mAABB.mMin), aiVector3ToGlm(mesh->mAABB.mMax) }
        });
    }

    // load instances of meshes
    load_scene(scene, scene->mRootNode, glm::mat4(1));

    std::cout << file_path << ":" << instances.size() << std::endl;

    // init for culling
    instance_culled = std::vector<bool>(instances.size(), false);
    idx_range = std::vector<unsigned int>((size_t) std::ceil((double) instances.size() / THREAD_WORK_AMOUNT));
    std::iota(idx_range.begin(), idx_range.end(), 0);
}

void Model::draw(std::shared_ptr<Program> program, const glm::mat4& vp_mat, bool frustum_culling, bool draw_aabb)
{
    if (frustum_culling) {
        // cull aabb's
        auto cull_lambda = [&](unsigned int i) {
            unsigned int size_lim = std::min(THREAD_WORK_AMOUNT * (i + 1), (unsigned int) instance_culled.size());
            for (unsigned int j = THREAD_WORK_AMOUNT * i; j < size_lim; j++) {
                const auto& [mesh_idx, transform] = instances.at(j);
                const auto& [material_idx, mesh, aabb] = meshes.at(mesh_idx);

                instance_culled.at(j) = aabb.cull(vp_mat, transform);
            }
        };

        std::for_each(
#if PARALLEL_CULLING
            std::execution::par,
#endif
            idx_range.begin(),
            idx_range.end(),
            cull_lambda
        );
    }
    
    // draw meshes
    for (unsigned int i = 0; i < instances.size(); i++) {
        const auto& [mesh_idx, transform] = instances.at(i);
        const auto& [material_idx, mesh, aabb] = meshes.at(mesh_idx);

#ifndef DEBUG
        if (instance_culled.at(i) && frustum_culling) {
            continue;
        }
#endif


        Material material = materials[material_idx];

        material.bind(program);
        program->set_mat4f("model", transform);
        
#ifdef DEBUG
        if (!instance_culled.at(i) || !frustum_culling) {
            mesh.draw();
        }

        if (draw_aabb) {
            aabb.draw(program);
        }
#endif
#ifndef DEBUG
        mesh.draw();
#endif

    }
}



void Model::load_scene(const aiScene* scene, const aiNode* node, glm::mat4 transform)
{
    glm::mat4 new_transform = transform * aiMatrix4x4ToGlm(node->mTransformation);
    
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        instances.emplace_back(std::pair<unsigned int, glm::mat4>(node->mMeshes[i], new_transform));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        load_scene(scene, node->mChildren[i], new_transform);
    }
}
