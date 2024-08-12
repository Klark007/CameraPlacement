#pragma once

#include "assimp/vector3.h"
#include "assimp/matrix4x4.h"
#include "assimp/vector3.h"

#include <glm/glm.hpp>

#include <filesystem>

// Usefull helper functions for printing etc

void print_vec(const glm::vec3& p);
void print_vec(const glm::vec4& p);
void print_mat(const glm::mat3& m);
void print_mat(const glm::mat4& m);

std::string string_from_path(const std::filesystem::path& path);

glm::vec3 rodrigues(glm::mat3 R); // calls opencv rodrigues function

inline glm::vec3 aiVector3ToGlm(const aiVector3D from) {
    glm::vec3 to;

    to.r = from.x;
    to.g = from.y;
    to.b = from.z;

    return to;
}

inline glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4 from)
{
    glm::mat4 to;

    to[0][0] = (float)from.a1; to[0][1] = (float)from.b1;  to[0][2] = (float)from.c1; to[0][3] = (float)from.d1;
    to[1][0] = (float)from.a2; to[1][1] = (float)from.b2;  to[1][2] = (float)from.c2; to[1][3] = (float)from.d2;
    to[2][0] = (float)from.a3; to[2][1] = (float)from.b3;  to[2][2] = (float)from.c3; to[2][3] = (float)from.d3;
    to[3][0] = (float)from.a4; to[3][1] = (float)from.b4;  to[3][2] = (float)from.c4; to[3][3] = (float)from.d4;

    return to;
}