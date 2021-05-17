#ifndef PROGRAM_HPP
#define PROGRAM_HPP 1

#include "intermediary.h"
#include "Shader.hpp"
#define _CRT_SECURE_NO_WARNINGS // because printf is "too dangerous"
class Program
{
	unsigned programId = 0;
	unsigned * amount; // number of shader programs

	std::string getLinkMessageErrorAndClear() const;
	unsigned getUniformId(const char * name) const;

	void swap(const Program & program);
	void clear();

public:
	Program();
	Program(const Program & program);
	Program & operator=(const Program & program);
	Program(const Shader2 & vertex, const Shader2 & fragment);
	Program(const std::string & vertFileName, const std::string & fragFileName);
	~Program();

	void create();
	void link() const;
	void attachShader(const Shader2 & shader) const;

	void use() const { glUseProgram(programId); }

	void setInt(const char * name, int i) const;
	void setFloat(const char * name, float f) const;
	void setVec2(const char * name, const glm::vec2 & vec) const;
	void setVec3(const char * name, const glm::vec3 & vec) const;
	void setVec4(const char * name, const glm::vec4 & vec) const;
	void setMat2(const char * name, const glm::mat2 & mat) const;
	void setMat3(const char * name, const glm::mat3 & mat) const;
	void setMat4(const char * name, const glm::mat4 & mat) const;

	unsigned getId() const { return programId; }
};

Program::Program()
{
	amount = new unsigned;
	*amount = 1;
}

Program::Program(const Shader2 & vertex, const Shader2 & fragment)
{
	programId = glCreateProgram();
	glAttachShader(programId, vertex.getId());
	glAttachShader(programId, fragment.getId());
	link();
	vertex.clear();
	fragment.clear();
	amount = new unsigned;
	*amount = 1;
}

Program::Program(const std::string & vertFileName, const std::string & fragFileName)
{
	programId = glCreateProgram();
	Shader2 vertex = Shader2::createVertexShader(vertFileName);
	Shader2 fragment = Shader2::createFragmentShader(fragFileName);
	glAttachShader(programId, vertex.getId());
	glAttachShader(programId, fragment.getId());
	link();
	vertex.clear();
	fragment.clear();
	amount = new unsigned;
	*amount = 1;
}

Program::Program(const Program & program)
{
	swap(program);
}

Program & Program::operator=(const Program & program)
{
	clear();
	swap(program);
	return *this;
}

void Program::swap(const Program & program)
{
	programId = program.programId;
	amount = program.amount;
	*amount = *amount + 1;
}

void Program::clear()
{
	*amount = *amount - 1;
	if (*amount == 0)
	{
		delete amount;
		if (programId != 0)
			glDeleteProgram(programId);
	}
}

Program::~Program()
{
	clear();
}

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

void Program::create()
{
	if (programId != 0)
		throw std::runtime_error("Can't create exists program");
	programId = glCreateProgram();
}

void Program::attachShader(const Shader2 & shader) const
{
	if (programId == 0)
		throw std::runtime_error("Can't attach shader to empty program");
	glAttachShader(programId, shader.getId());
	shader.clear();
}

void Program::link() const
{
	if (programId == 0)
		throw std::runtime_error("Can't link empty program");
	glLinkProgram(programId);
	int success;
	glGetProgramiv(programId, GL_LINK_STATUS, &success);
	if (!success)
		throw std::runtime_error(getLinkMessageErrorAndClear());
}

unsigned Program::getUniformId(const char * name) const
{
	return glGetUniformLocation(programId, name);
}

void Program::setInt(const char * name, int i) const
{
	glUniform1i(getUniformId(name), i);
}

void Program::setFloat(const char * name, float f) const
{
	glUniform1f(getUniformId(name), f);
}

void Program::setVec2(const char * name, const glm::vec2 & vec) const
{
	glUniform2fv(getUniformId(name), 1, (float*)&vec);
}

void Program::setVec3(const char * name, const glm::vec3 & vec) const
{
	glUniform3fv(getUniformId(name), 1, (float*)&vec);
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

void Program::setMat4(const char * name, const glm::mat4 & mat) const
{
	glUniformMatrix4fv(getUniformId(name), 1, GL_FALSE, (float*)&mat);
}

#endif
