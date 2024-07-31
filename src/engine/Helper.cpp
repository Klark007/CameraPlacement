#include "Helper.h"

#include <iostream>
#include <iomanip>

#ifdef _WIN32
#include <locale>
#include <codecvt>
#endif


void print_vec(const glm::vec3& p) {
	std::cout << "(" << p.x << "," << p.y << "," << p.z << ")" << std::endl;
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
// modules/calib3d/src/calibration.cpp
glm::vec3 rodriguez(glm::mat3 R)
{
	glm::vec3 r;

	r = glm::vec3(R[2][1] - R[1][2], R[0][2] - R[2][0], R[1][0] - R[0][1]);

	double s = std::sqrt((r.x*r.x + r.y*r.y + r.z*r.z) * 0.25);
	double c = (R[0][0] + R[1][1] + R[2][2] - 1) * 0.5;
	c = c > 1.0 ? 1.0 : c < -1.0 ? -1.0 : c;
	double theta = acos(c);

	if (s < 1e-5) {
		if (c > 0) {
			r = glm::vec3(0);
		}
		else {
			double t = (R[0][0] + 1) * 0.5;
			r.x = std::sqrt(std::max(t, 0.0));
			t = (R[1][1] + 1) * 0.5;
			r.y = std::sqrt(std::max(t, 0.0)) * (R[0][1] < 0 ? -1 : 1);
			t = (R[2][2] + 1) * 0.5;
			r.z = std::sqrt(std::max(t, 0.0)) * (R[0][2] < 0 ? -1 : 1);

			if (fabs(r.x) < fabs(r.y) && fabs(r.x) < fabs(r.z) && (R[1][2] > 0) != (r.y * r.z > 0)) {
				r.z = -r.z;
			}
			theta /= glm::length(r);
			r *= theta;
		}
	}
	else {
		double vth = 1 / (2 * s);

		vth *= theta;
		r *= vth;
	}

	return r;
}
