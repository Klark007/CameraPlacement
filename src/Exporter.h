#pragma once

#include "CameraPreview.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <format>

#define _USE_MATH_DEFINES
#include <math.h>

// Used to export the current placed cameras into csv files

enum CoordinateSystem {
    Z_UP,
    Y_UP,
    Synthetic // LHS plus y down see: https://github.com/robinjacobs/SyntheticCheckerboardTrajectories
};

struct ExportSettings {
    bool camera_to_world;
    CoordinateSystem cs;
};

void export_cameras_to_file(const std::vector<std::shared_ptr<CameraPreview>>& placed_cameras, const std::string& path, ExportSettings settings);

inline void export_cameras(const std::vector<std::shared_ptr<CameraPreview>>& placed_cameras, const std::string& output_folder) {
    auto time = std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now());
    auto today = std::chrono::floor<std::chrono::days>(time);

    std::string datetime = std::format("{0:%d-%m-%Y} {1:%H-%M-%S}",
        std::chrono::year_month_day{ today },
        std::chrono::hh_mm_ss{ time - today });

    // outputs both to a dated version and a most recent one
    ExportSettings blender_settings{ true, Z_UP };
    ExportSettings opencv_settings{ false, Synthetic };
    
    std::cout << "Exporting " << placed_cameras.size() << " cameras" << std::endl;

    export_cameras_to_file(placed_cameras, output_folder + "/camera_poses.csv", blender_settings);
    export_cameras_to_file(placed_cameras, output_folder + "/camera_poses_cv2.csv", opencv_settings);
    export_cameras_to_file(placed_cameras, output_folder + "/tmp/" + datetime + "_camera_poses.csv", blender_settings);
    export_cameras_to_file(placed_cameras, output_folder + "/tmp/" + datetime + "_camera_poses_cv2.csv", opencv_settings);
};