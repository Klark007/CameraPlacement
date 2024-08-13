#include "Camera.h"


Camera::Camera(glm::vec3 pos, glm::vec3 des, glm::vec3 up, unsigned int res_x, unsigned int res_y, float focx, float focy, float near_plane, float far_plane)
	: Camera(pos, des, up, res_x, res_y, std::tanf(foc_y / foc_x * 0.5f) * 2, near_plane, far_plane)
{
	foc_x = focx;
	foc_y = focy;
}

Camera::Camera(glm::vec3 pos, glm::vec3 des, glm::vec3 up, unsigned int res_x, unsigned int res_y, float fov, float near_plane, float far_plane)
	: position{ pos },
	  res_x { res_x },
	  res_y { res_y },
	  fov { fov },
	  z_near { near_plane },
	  z_far { far_plane }
{
	set_dir(des - pos);
}
