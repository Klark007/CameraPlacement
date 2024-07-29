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
	
	std::cout << "(" << std::setprecision(3) << std::endl;
	for (unsigned int i = 0; i < 4; i++) {
		for (unsigned int j = 0; j < 4; j++) {
			std::cout << std::setfill(' ') << std::setw(5) << m[i][j] << ",";
		}
		std::cout << std::endl;
	}
	std::cout << ")" << std::setprecision(6) << std::endl;
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
