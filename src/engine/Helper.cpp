#include "Helper.h"

#include "opencv2/calib3d/calib3d.hpp"

#include <iostream>
#include <iomanip>

#ifdef _WIN32
#include <locale>
#include <codecvt>
#endif


void print_vec(const glm::vec3& p) {
	std::cout << "(" << p.x << "," << p.y << "," << p.z << ")" << std::endl;
}

void print_vec(const glm::vec4& p) {
	std::cout << "(" << p.x << "," << p.y << "," << p.z << "," << p.w << ")" << std::endl;
}

void print_mat(const glm::mat3& m) {
	// glm is column major
	// mat is a vector of columns 
	std::cout << "(" << std::fixed << std::setprecision(3) << std::endl;
	for (unsigned int i = 0; i < 3; i++) {
		for (unsigned int j = 0; j < 3; j++) {
			std::cout << std::setfill(' ') << std::setw(6) << m[j][i] << ",";
		}
		std::cout << std::endl;
	}
	std::cout << ")" << std::defaultfloat << std::setprecision(6) << std::endl;
}

void print_mat(const glm::mat4& m) {
	// glm is column major
	// mat is a vector of columns 
	std::cout << "(" << std::fixed <<std::setprecision(3) << std::endl;
	for (unsigned int i = 0; i < 4; i++) {
		for (unsigned int j = 0; j < 4; j++) {
			std::cout << std::setfill(' ') << std::setw(6) << m[j][i] << ",";
		}
		std::cout << std::endl;
	}
	std::cout << ")" << std::defaultfloat << std::setprecision(6) << std::endl;
}

std::string string_from_path(const std::filesystem::path& path)
{
	std::string str;
#if defined(_WIN32)
	std::basic_string<wchar_t> w_path = path.native();
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
	str = converter.to_bytes(w_path);
#elif defined(__linux__)
	str = path.native();
#endif
	return str;
}

// opencv2 implementation using glm
glm::vec3 rodrigues(glm::mat3 R)
{
	cv::Mat cv_R = cv::Mat::zeros(3, 3, CV_32F);
	for (unsigned int i = 0; i < 3; i++) {
		for (unsigned int j = 0; j < 3; j++) {
			cv_R.at<float>(i, j) = R[j][i];
		}
	}

	cv::Mat cv_rot_vec = cv::Mat::zeros(3, 1, CV_32F);
	cv::Rodrigues(cv_R, cv_rot_vec);

	return glm::vec3(
		cv_rot_vec.at<float>(0),
		cv_rot_vec.at<float>(1),
		cv_rot_vec.at<float>(2)
	);
}
