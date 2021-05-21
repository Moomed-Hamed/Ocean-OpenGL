#ifndef PROGRAM_HPP
#define PROGRAM_HPP 1

#include "intermediary.h"
#include "Shader.hpp"
#define _CRT_SECURE_NO_WARNINGS // because printf is "too dangerous"

struct Shader_Program
{
	GLuint id;
};

void setMat4(GLuint shader_id, const char* name, mat4 value)
{
	glUniformMatrix4fv(glGetUniformLocation(shader_id, name), 1, GL_FALSE, (float*)&value);
}

void setInt(GLuint shader_id, const char * name, int value)
{
	glUniform1i(glGetUniformLocation(shader_id, name), value);
}

void setFloat(GLuint shader_id, const char* name, float value)
{
	glUniform1f(glGetUniformLocation(shader_id, name), value);
}

void setVec3(GLuint shader_id, const char* name, vec3 value)
{
	glUniform3f(glGetUniformLocation(shader_id, name), value.x, value.y, value.z);
}

#endif

/*
std::string Program::getLinkMessageErrorAndClear() const
{
	int length;
	glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &length);
	char* message = new char[length];

	glGetProgramInfoLog(programId, length, &length, message);
	glDeleteProgram(programId);

	std::string finalMess = message;
	delete[] message;
	return finalMess;
}

---- why would you write this if you don't use it?????????????

void Program::setVec2(const char * name, const glm::vec2 & vec) const
{
	glUniform2fv(getUniformId(name), 1, (float*)&vec);
}

void Program::setVec4(const char * name, const glm::vec4 & vec) const
{
	glUniform4fv(getUniformId(name), 1, (float*)&vec);
}

void Program::setMat2(const char * name, const glm::mat2 & mat) const
{
	glUniformMatrix2fv(getUniformId(name), 1, GL_FALSE, (float*)&mat);
}

void Program::setMat3(const char * name, const glm::mat3 & mat) const
{
	glUniformMatrix3fv(getUniformId(name), 1, GL_FALSE, (float*)&mat);
}
*/