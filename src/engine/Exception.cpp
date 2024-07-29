#include "Exception.h"

const char* CameraPlacementException::what() const throw()
{
	return format_msg.c_str();
}