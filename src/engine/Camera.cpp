#include "Camera.h"


Camera::Camera(glm::vec3 pos, glm::vec3 des, glm::vec3 up, unsigned int res_x, unsigned int res_y, float focx, float focy, float near_plane, float far_plane)
	: Camera(pos, des, up, res_x, res_y, std::tanf(foc_y / foc_x), near_plane, far_plane)
{
	foc_x = focx;
	foc_y = focy;
}

Camera::Camera(glm::vec3 pos, glm::vec3 des, glm::vec3 up, unsigned int res_x, unsigned int res_y, float fov, float near_plane, float far_plane)
	: position{ pos },  
	  up{ glm::normalize(up) }, 
	  res_x { res_x },
	  res_y { res_y },
	  fov { fov },
	  z_near { near_plane },
	  z_far { far_plane }
{
	glm::vec3 dir_gt = glm::normalize(des - pos);

	glm::vec3 side = glm::cross(dir_gt, up);
	up = glm::cross(side, dir_gt);

	set_dir(des - pos);
}
