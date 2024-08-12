#include "Exporter.h"

#include "opencv2/calib3d/calib3d.hpp"

// modify as needed
void export_cameras_to_file(const std::vector<std::shared_ptr<CameraPreview>>& placed_cameras, const std::string& path, ExportSettings settings) {
    std::ofstream output_file(path);
    if (output_file.is_open()) {
        for (unsigned int i = 0; i < placed_cameras.size(); i++) {
            Camera camera = placed_cameras.at(i)->get_camera();

            glm::mat4 view_mat = camera.generate_view_mat();
            
            // might want camera to world transform instead of world to camera that is given by the view matrix
            glm::mat4 cam_to_world = glm::mat4(1);
            cam_to_world[0] = glm::transpose(view_mat)[0];
            cam_to_world[1] = glm::transpose(view_mat)[1];
            cam_to_world[2] = glm::transpose(view_mat)[2];
            cam_to_world[3] = glm::vec4(glm::mat3(cam_to_world) * -glm::vec3(view_mat[3]), 1);

            glm::vec3 rot_vec = settings.camera_to_world ? rodrigues(glm::mat3(cam_to_world)) : rodrigues(glm::mat3(view_mat));
            glm::vec3 trans_vec = settings.camera_to_world ? glm::vec3(cam_to_world[3]) : glm::vec3(view_mat[3]);

            output_file << std::scientific; // set scientific notation
            if (settings.cs == Z_UP) {
                // 0-2 rotation
                output_file << rot_vec.x << "," << -rot_vec.z << "," << rot_vec.y << ",";
                // 3-5 translation
                output_file << trans_vec.x << "," << -trans_vec.z << "," << trans_vec.y << ",";
            }
            else if (settings.cs == Y_UP) {
                // 0-2 rotation
                output_file << rot_vec.x << "," << rot_vec.y << "," << rot_vec.z << ",";
                // 3-5 translation
                output_file << trans_vec.x << "," << trans_vec.y << "," << trans_vec.z << ",";
            }
            else if (settings.cs == Synthetic) {
                glm::mat4 view_mat_LHS = camera.generate_view_mat_LHS();

                glm::mat4 cam_to_world = glm::mat4(1);
                cam_to_world[0] = glm::transpose(view_mat_LHS)[0];
                cam_to_world[1] = glm::transpose(view_mat_LHS)[1];
                cam_to_world[2] = glm::transpose(view_mat_LHS)[2];
                cam_to_world[3] = glm::vec4(glm::mat3(cam_to_world) * -glm::vec3(view_mat_LHS[3]), 1);

                // change to z up
                glm::mat4 rotation = glm::rotate(glm::mat4(1), (float)M_PI_2, glm::vec3(1, 0, 0)) * cam_to_world;
                glm::vec3 translation = glm::vec3(cam_to_world[3]);
                translation = glm::vec3(translation.x, -translation.z, translation.y);

                // want camera to world transform instead of world to camera that is given by the view matrix
                glm::mat4 world_to_cam = glm::mat4(1);
                world_to_cam[0] = glm::transpose(rotation)[0];
                world_to_cam[1] = glm::transpose(rotation)[1];
                world_to_cam[2] = glm::transpose(rotation)[2];
                world_to_cam[3] = glm::vec4(glm::mat3(world_to_cam) * -translation, 1);
                
                rot_vec = rodrigues(world_to_cam);
                trans_vec = glm::vec3(world_to_cam[3]);

                // 0-2 rotation
                output_file << rot_vec.x << "," << rot_vec.y << "," << rot_vec.z << ",";
                // 3-5 translation
                output_file << trans_vec.x << "," << trans_vec.y << "," << trans_vec.z << ",";

            }
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