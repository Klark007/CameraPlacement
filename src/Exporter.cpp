#include "Exporter.h"

#include "opencv2/calib3d/calib3d.hpp"

// modify as needed
void export_cameras_to_file(const std::vector<std::pair<unsigned int, Camera>>& placed_cameras, const std::string& path, ExportSettings settings) {
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
            //cam_to_world = cam_to_world * glm::rotate(glm::mat4(1), (float) M_PI_2, glm::vec3(1, 0, 0));


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
                /*
                glm::mat4 rotation = cam_to_world;
                glm::vec3 translation = glm::vec3(cam_to_world[3]);

                //rotation = rotation * glm::rotate(glm::mat4(1), (float)M_PI, glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1), 0*-(float)M_PI_2, glm::vec3(1, 0, 0)); // so that y points down
                glm::mat4 view_mat_mod = glm::mat4(1);
                view_mat_mod[0] = glm::transpose(rotation)[0];
                view_mat_mod[1] = glm::transpose(rotation)[1];
                view_mat_mod[2] = glm::transpose(rotation)[2];
                view_mat_mod[3] = glm::vec4(glm::mat3(rotation) * -translation, 1);

                rot_vec = rodrigues(glm::mat3(view_mat_mod));
                trans_vec = glm::vec3(view_mat_mod[3]);

                // switch at export
                // 0-2 rotation
                output_file << rot_vec.x << "," << -rot_vec.z << "," << rot_vec.y << ",";
                // 3-5 translation
                output_file << trans_vec.x << "," << -trans_vec.z << "," << trans_vec.y << ",";
                */

                glm::mat4 view_mat_LHS = camera.generate_view_mat_LHS();

                glm::mat4 cam_to_world = glm::mat4(1);
                cam_to_world[0] = glm::transpose(view_mat_LHS)[0];
                cam_to_world[1] = glm::transpose(view_mat_LHS)[1];
                cam_to_world[2] = glm::transpose(view_mat_LHS)[2];
                cam_to_world[3] = glm::vec4(glm::mat3(cam_to_world) * -glm::vec3(view_mat_LHS[3]), 1);

                // rotate camera around future z by 180
                // cam_to_world glm::rotate(glm::mat4(1), (float)M_PI, glm::vec3(0, 0, 1)) *

                glm::mat4 rotation = glm::rotate(glm::mat4(1), (float)M_PI_2, glm::vec3(1, 0, 0)) * cam_to_world;
                glm::vec3 translation = glm::vec3(cam_to_world[3]);
                translation = glm::vec3(translation.x, -translation.z, translation.y);

                // want camera to world transform instead of world to camera that is given by the view matrix
                glm::mat4 cam_to_world_mod = glm::mat4(1);
                cam_to_world_mod[0] = glm::transpose(rotation)[0];
                cam_to_world_mod[1] = glm::transpose(rotation)[1];
                cam_to_world_mod[2] = glm::transpose(rotation)[2];
                cam_to_world_mod[3] = glm::vec4(glm::mat3(cam_to_world_mod) * -translation, 1);

                std::cout << i << "," << std::endl;

                print_mat(view_mat_LHS);
                print_mat(cam_to_world);
                print_mat(cam_to_world_mod);

                glm::mat3 rot_3 = glm::mat3(cam_to_world_mod);
                print_mat(glm::mat4(glm::transpose(rot_3) * rot_3));
                std::cout << glm::determinant(rot_3) << std::endl << std::endl;

                /*
                std::cout << "Pos should be (opengl, opencv):";
                print_vec(camera.get_pos());
                print_vec(glm::vec3(camera.get_pos().x, -camera.get_pos().z, camera.get_pos().y));
                print_vec(glm::transpose(glm::mat3(cam_to_world_mod)) * -cam_to_world_mod[3]);

                std::cout << "Last should be (opengl, opencv):";
                print_vec(camera.get_dir());
                print_vec(glm::vec3(camera.get_dir().x, -camera.get_dir().z, camera.get_dir().y));

                std::cout << std::endl;

                print_vec(glm::mat3(cam_to_world) * glm::vec3(1, 0, 0));
                print_vec(glm::mat3(cam_to_world) * glm::vec3(0, 1, 0));
                print_vec(glm::mat3(cam_to_world) * glm::vec3(0, 0, 1));

                std::cout << std::endl;

                print_vec(glm::mat3(cam_to_world_mod) * glm::vec3(1, 0, 0));
                print_vec(glm::mat3(cam_to_world_mod) * glm::vec3(0, 1, 0));
                print_vec(glm::mat3(cam_to_world_mod) * glm::vec3(0, 0, 1));

                std::cout << std::endl;

                std::cout << "E" << std::endl;
                */
                
                cv::Mat cv_cam_to_world_mod = cv::Mat::zeros(3, 3, CV_32F);
                for (unsigned int i = 0; i < 3; i++) {
                    for (unsigned int j = 0; j < 3; j++) {
                        cv_cam_to_world_mod.at<float>(i, j) = cam_to_world_mod[j][i];
                    }
                }

                cv::Mat cv_rot_vec = cv::Mat::zeros(3, 1, CV_32F);
                cv::Rodrigues(cv_cam_to_world_mod, cv_rot_vec);
                // std::cout << cv_rot_vec << std::endl;

                //rot_vec = rodrigues(glm::mat3(cam_to_world_mod));
                //print_vec(rot_vec);

                trans_vec = glm::vec3(cam_to_world_mod[3]);

                // switch at export
                // 0-2 rotation
                //output_file << rot_vec.x << "," << rot_vec.y << "," << rot_vec.z << ",";
                output_file << cv_rot_vec.at<float>(0) << "," << cv_rot_vec.at<float>(1) << "," << cv_rot_vec.at<float>(2) << ",";
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