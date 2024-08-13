#include "CameraPreview.h"

CameraPreview::CameraPreview(unsigned int type, const std::string& name, const Camera& camera, const Frustum& frustum, const Billboard& billboard)
	: camera_type {type}, name {name}, camera{camera}, frustum{frustum}, icon{billboard}
{
}

void CameraPreview::draw_frustum(std::shared_ptr<Program> program) const
{
	frustum.draw(program);

}

void CameraPreview::draw_icon(std::shared_ptr<Program> program) const
{
	icon.draw(program);
}
