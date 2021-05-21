#include <sstream>
#include <fstream>

#include "window_and_input.h"

void set_mat4(GLuint shader_id, const char* name, mat4 value)
{
	glUniformMatrix4fv(glGetUniformLocation(shader_id, name), 1, GL_FALSE, (float*)&value);
}

void set_int(GLuint shader_id, const char * name, int value)
{
	glUniform1i(glGetUniformLocation(shader_id, name), value);
}

void set_float(GLuint shader_id, const char* name, float value)
{
	glUniform1f(glGetUniformLocation(shader_id, name), value);
}

void set_vec3(GLuint shader_id, const char* name, vec3 value)
{
	glUniform3f(glGetUniformLocation(shader_id, name), value.x, value.y, value.z);
}

void loadTexture(unsigned & textureId, const std::string & fileName)
{
	int width, height, nrChannels;
	unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nrChannels, 0);
	if (!data)
	{
		stbi_image_free(data);
		out("Failed to load texture from file: " << fileName << ".");
	}

	// get texture format
	GLenum format = 0;
	switch (nrChannels)
	{
	case 1: format = GL_RED;  break;
	case 3: format = GL_RGB;  break;
	case 4: format = GL_RGBA; break;
	default: out("TEXTURE FORMAT ERROR : Can't find image format from number of channels");
	}

	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	stbi_image_free(data);

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8);
}

// returns id of created shader
GLuint load_shader(const char* file_path, GLuint shader_type)
{
	using std::string;

	std::ifstream file(file_path, std::ios::binary);
	if (!file.is_open()) out("ERROR : Cant open shader file: " << file_path);

	std::stringstream stream;
	stream << file.rdbuf();

	file.clear();
	file.close();

	string source = stream.str();

	const char* data = source.c_str();
	GLuint ret = glCreateShader(shader_type);
	glShaderSource(ret, 1, &data, nullptr);
	glCompileShader(ret);

	int success = 0;
	glGetShaderiv(ret, GL_COMPILE_STATUS, &success);
	if (success == NULL)
	{
		int length;
		glGetShaderiv(ret, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];

		glGetShaderInfoLog(ret, length, &length, message);
		glDeleteShader(ret);

		out("SHADER ERROR : " << message);
		delete[] message;
	}

	return ret;
}

// -- 3D Camera -- //

#define DIR_FORWARD	0
#define DIR_BACKWARD	1
#define DIR_LEFT		2
#define DIR_RIGHT	   3

struct Camera
{
	vec3 position;
	vec3 front, right, up;
	float yaw, pitch;
};

void camera_update_dir(Camera* camera, float dx, float dy, float sensitivity = 0.003)
{
	camera->yaw   += (dx * sensitivity) / TWOPI;
	camera->pitch += (dy * sensitivity) / TWOPI;

	if (camera->pitch >  PI / 2.01) camera->pitch =  PI / 2.01;
	if (camera->pitch < -PI / 2.01) camera->pitch = -PI / 2.01;

	camera->front.y = sin(camera->pitch);
	camera->front.x = cos(camera->pitch) * cos(camera->yaw);
	camera->front.z = cos(camera->pitch) * sin(camera->yaw);

	camera->front = normalize(camera->front);
	camera->right = normalize(cross(camera->front, vec3(0, 1, 0)));
	camera->up    = normalize(cross(camera->right, camera->front));
}
void camera_update_pos(Camera* camera, int direction, float distance)
{
	if (direction == DIR_FORWARD ) camera->position += camera->front * distance;
	if (direction == DIR_LEFT    ) camera->position -= camera->right * distance;
	if (direction == DIR_RIGHT   ) camera->position += camera->right * distance;
	if (direction == DIR_BACKWARD) camera->position -= camera->front * distance;
}