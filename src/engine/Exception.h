#pragma once

#include <string>
#include <format>
#include <exception>

class CameraPlacementException : public std::exception {
public:
	CameraPlacementException(const std::string& msg, const std::string& file, int line);
	const char* what() const throw () override;
protected:
	const std::string file;
	int line;

	const std::string msg;
	const std::string format_msg;
};

inline CameraPlacementException::CameraPlacementException(const std::string& msg, const std::string& file, int line)
	: msg {msg}, file {file}, line {line}, format_msg { std::format("[{}] at line {} {}", file, line, msg) }
{
}


class SetupException : public CameraPlacementException {
public:
	SetupException(const std::string& msg, const std::string& file, int line) : CameraPlacementException{msg, file, line} {};
};

class ShaderException : public CameraPlacementException {
public:
	ShaderException(const std::string& msg, const std::string& file, int line) : CameraPlacementException{ msg, file, line } {};
};

class IOException : public CameraPlacementException {
public:
	IOException(const std::string& msg, const std::string& file, int line) : CameraPlacementException{ msg, file, line } {};
};

class RuntimeException : public CameraPlacementException {
public:
	RuntimeException(const std::string& msg, const std::string& file, int line) : CameraPlacementException{ msg, file, line } {};
};

class NotImplementedException : public CameraPlacementException {
public:
	NotImplementedException(const std::string& msg, const std::string& file, int line) : CameraPlacementException{ msg, file, line } {};
};