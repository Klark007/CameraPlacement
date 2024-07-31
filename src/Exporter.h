#pragma once

#include "engine/Exception.h"
#include "engine/Camera.h"
#include "engine/Helper.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/transform.hpp"

#include  <utility>
#include <iostream>
#include <fstream>
#include <chrono>
#include <format>

#define _USE_MATH_DEFINES
#include <math.h>

// modify as needed
void export_cameras_to_file(const std::vector<std::pair<unsigned int, Camera>>& placed_cameras, const std::string& path) {
    std::ofstream output_file(path);
    if (output_file.is_open()) {
        for (unsigned int i = 0; i < placed_cameras.size(); i++) {
            Camera camera = placed_cameras.at(i).second;

            glm::mat4 view_mat = camera.generate_view_mat();
            // want camera to world transform instead of world to camera that is given by the view matrix
            glm::mat4 cam_to_world = glm::mat4(1); 
            cam_to_world[0] = glm::transpose(view_mat)[0];
            cam_to_world[1] = glm::transpose(view_mat)[1];
            cam_to_world[2] = glm::transpose(view_mat)[2];
            cam_to_world[3] = glm::vec4(glm::mat3(cam_to_world) * -glm::vec3(view_mat[3]), 1);
            cam_to_world = cam_to_world * glm::rotate(glm::mat4(1), (float) M_PI_2, glm::vec3(1, 0, 0));
            
            glm::vec3 rot_vec = rodrigues(glm::mat3(cam_to_world));
            glm::vec3 trans_vec = glm::vec3(cam_to_world[3]);

            output_file << std::scientific; // set scientific notation
            // 0-2 rotation
            output_file << rot_vec.x << "," << -rot_vec.z << "," << rot_vec.y << ",";
            // 3-5 translation
            output_file << trans_vec.x << "," << -trans_vec.z << "," << trans_vec.y << ",";
            // 6-7 fx fy
            output_file << camera.get_focal_len_x() << "," << camera.get_focal_len_y() << ",";
            // 8-9 cx cy
            output_file << camera.get_resolution_x() / 2 << "," << camera.get_resolution_y() / 2 << std::endl;
        }

        output_file.close();
    }
    else {
        throw IOException("Couldn't open file at " + path, __FILE__, __LINE__);
    }
}

void export_cameras(const std::vector<std::pair<unsigned int, Camera>>& placed_cameras, const std::string& output_folder) {
    auto time = std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now());
    auto today = std::chrono::floor<std::chrono::days>(time);

    std::string datetime = std::format("{0:%d-%m-%Y} {1:%H-%M-%S}",
        std::chrono::year_month_day{ today },
        std::chrono::hh_mm_ss{ time - today });

    // outputs both to a dated version and a most recent one
    export_cameras_to_file(placed_cameras, output_folder + "/camera_poses.csv");
    export_cameras_to_file(placed_cameras, output_folder + "/tmp/" + datetime + "_camera_poses.csv");
};