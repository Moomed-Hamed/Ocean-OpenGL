#ifndef SHADER_HPP
#define SHADER_HPP 1

#include "intermediary.h"

class Shader
{
  unsigned shaderId;
  GLenum type;

  std::string getSource(const std::string & fileName) const;
  std::string getCompileMessageErrorAndClear() const;

  Shader(const std::string & fileName, GLenum t);
public:
  static Shader createVertexShader(const std::string & fileName)
  {
    return Shader(fileName, GL_VERTEX_SHADER);
  }

  static Shader createFragmentShader(const std::string & fileName)
  {
    return Shader(fileName, GL_FRAGMENT_SHADER);
  }

  static Shader createGeometryShader(const std::string & fileName)
  {
    return Shader(fileName, GL_GEOMETRY_SHADER);
  }

  static Shader createTessalationControlShader(const std::string & fileName)
  {
    return Shader(fileName, GL_TESS_CONTROL_SHADER);
  }

  static Shader createTessalationEvaluationShader(const std::string & fileName)
  {
    return Shader(fileName, GL_TESS_EVALUATION_SHADER);
  }

  static Shader createComputeShader(const std::string & fileName)
  {
    return Shader(fileName, GL_COMPUTE_SHADER);
  }

  void clear() const{glDeleteShader(shaderId);}

  unsigned getId() const {return shaderId;}
  GLenum getType() const {return type;}
};

Shader::Shader(const std::string & fileName, GLenum t) : type(t)
{
	std::string source = getSource(fileName);
	const char* data = source.c_str();
	shaderId = glCreateShader(type);
	glShaderSource(shaderId, 1, &data, nullptr);
	glCompileShader(shaderId);

	int success;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
	if (!success)
		throw std::runtime_error(getCompileMessageErrorAndClear());
}

std::string Shader::getSource(const std::string & fileName) const
{
	std::ifstream file(fileName, std::ios::binary);
	if (!file.is_open())
		throw std::runtime_error("Can\'t open shader file: " + fileName + ".");

	std::stringstream stream;
	stream << file.rdbuf();

	file.clear();
	file.close();
	return stream.str();
}

std::string Shader::getCompileMessageErrorAndClear() const
{
	int length;
	glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
	char* message = new char[length];

	glGetShaderInfoLog(shaderId, length, &length, message);
	glDeleteShader(shaderId);

	std::string finalMess = message;
	delete[] message;
	return finalMess;
}
#endif
