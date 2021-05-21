// CONSIDER ADDING IMGUI WIDGETS
#define _CRT_SECURE_NO_WARNINGS // because printf is "too dangerous"

#include "Shader.hpp"
#include "Program.hpp"
#include "TextureLoader.hpp"

#include "renderer.h"

const float    CAM_FOV      = 45.0f;
const unsigned NUM_TEXTURES = 13;
const unsigned TESS_LEVEL   = 1;
const float    DEPTH        = 0.11f;

float interpolateFactor = 0.0f;
double deltaTime = 0.0;

bool rotate = false;

int main()
{
	Window window = {};
	Keyboard keys = {};

	init_window(&window, 1280, 720, "GL Waves");
	init_keyboard(&keys);

	mat4 projection = glm::perspective(glm::radians(CAM_FOV), (float)1280.f / (float)720.f, 0.1f, 500.f);

	Shader_Program program = {};
	program.id = glCreateProgram();

	// --- Create Shader Program
	Shader2 temp_shader = Shader2::createVertexShader("assets/shaders/water.vert");
	glAttachShader(program.id, temp_shader.getId());
	temp_shader.clear();

	temp_shader = Shader2::createTessalationControlShader("assets/shaders/water_tess_control.glsl");
	glAttachShader(program.id, temp_shader.getId());
	temp_shader.clear();

	temp_shader = Shader2::createTessalationEvaluationShader("assets/shaders/water_tess_eval.glsl");
	glAttachShader(program.id, temp_shader.getId());
	temp_shader.clear();

	temp_shader = Shader2::createFragmentShader("assets/shaders/water.frag");
	glAttachShader(program.id, temp_shader.getId());
	temp_shader.clear();

	// Link Shader Program
	int success = 0;
	glLinkProgram(program.id);
	glGetProgramiv(program.id, GL_LINK_STATUS, &success);
	if (success == NULL) out("ERROR : COULD NOT LINK SHADER PROGRAM!");

	// other stuff
	GLuint VAO = 0;
	GLuint heightMap[NUM_TEXTURES];
	GLuint normalMap[NUM_TEXTURES];

	glGenVertexArrays(1, &VAO);
	glGenTextures(NUM_TEXTURES, heightMap);
	glGenTextures(NUM_TEXTURES, normalMap);
	

	for (int i = 0; i < NUM_TEXTURES; ++i)
	{
		char heightmap_filename[256] = {};
		sprintf(heightmap_filename, "assets/textures/heights/%d.png", i+1);
		TextureLoader::loadTexture(heightMap[i], std::string(heightmap_filename));

		char normalmap_filename[256] = {};
		sprintf(normalmap_filename, "assets/textures/normals/%d.png", i+1);
		TextureLoader::loadTexture(normalMap[i], std::string(normalmap_filename));
	}

	GLuint waterTex;
	GLuint wavesNormalMap;
	GLuint wavesHeightMap;

	glGenTextures(1, &waterTex);
	TextureLoader::loadTexture(waterTex, "assets/textures/water.jpg");
	TextureLoader::loadTexture(wavesNormalMap, "assets/textures/wavesNormal.jpg");
	TextureLoader::loadTexture(wavesHeightMap, "assets/textures/wavesHeight.jpg");

	glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(100.0f));
	glm::vec3 viewPos = glm::vec3(1.0f);
	glm::mat4 view = glm::lookAt(viewPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// set shader uniforms
	glUseProgram(program.id);
	setVec3 (program.id, "light.direction"   , glm::vec3(0.0 , -1.0, 0.0 ));
	setVec3 (program.id, "light.ambient"     , glm::vec3(0.15, 0.15, 0.15));
	setVec3 (program.id, "light.diffuse"     , glm::vec3(0.75, 0.75, 0.75));
	setVec3 (program.id, "light.specular"    , glm::vec3(1.0 , 1.0 , 1.0 ));
	setFloat(program.id, "interpolateFactor" , interpolateFactor);
	setFloat(program.id, "depth"             , DEPTH);
	setInt  (program.id, "tessLevel"         , TESS_LEVEL);

	double lastFrame = 0;
	double actualFrame = 0;
	float radius = 75.0f;

	float camX = sin(glfwGetTime() * 0.5) * radius;
	float camZ = cos(glfwGetTime() * 0.5) * radius;
	viewPos = glm::vec3(camX, 50.0f, camZ);

	while (!glfwWindowShouldClose(window.instance))
	{
		update_window(window);
		update_keyboard(&keys, window);

		static int fix = 0;
		if (++fix % 2) continue;

		// controls
		if (keys.ESC.is_pressed) glfwSetWindowShouldClose(window.instance, true);
		if (keys.P.is_pressed) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if (keys.O.is_pressed) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (keys.R.is_pressed) rotate = true;
		if (keys.T.is_pressed) rotate = false;

		actualFrame = glfwGetTime();
		deltaTime = actualFrame - lastFrame;
		lastFrame = actualFrame;

		if (rotate)
		{
			radius = 60.0f;
			float camX = sin(glfwGetTime() * 0.5) * radius;
			float camZ = cos(glfwGetTime() * 0.5) * radius;
			viewPos = glm::vec3(camX, 30.0f, camZ);
		}

		view = glm::lookAt(viewPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(program.id);
		setMat4(program.id, "model"  , model);
		setMat4(program.id, "mvp"    , projection * view * model);
		setVec3(program.id, "viewPos", viewPos);

		// This section used to be renderWater();
		unsigned firstIndex = 0;
		unsigned lastIndex = 1;
		{
			glUseProgram(program.id);
			setInt(program.id, "heightMap1"     , 0);
			setInt(program.id, "heightMap2"     , 1);
			setInt(program.id, "normalMap1"     , 2);
			setInt(program.id, "normalMap2"     , 3);
			setInt(program.id, "water"          , 4);
			setInt(program.id, "wavesHeightMap" , 5);
			setInt(program.id, "wavesNormalMap" , 6);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, heightMap[firstIndex]);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, heightMap[lastIndex]);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, normalMap[firstIndex]);

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, normalMap[lastIndex]);
			
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, waterTex);
			
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, wavesHeightMap);
			
			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, wavesNormalMap);

			if (interpolateFactor >= 1)
			{
				interpolateFactor = 0.0f;
				if (lastIndex == NUM_TEXTURES - 1)
				{
					firstIndex = 0;
					lastIndex = 1;
				}
				else
				{
					++firstIndex;
					++lastIndex;
				}
			}
			else
			{
				interpolateFactor += 0.4 * deltaTime;
				setFloat(program.id, "interpolateFactor", interpolateFactor);
			}

			static float offset = 0.0f;
			if (offset >= INT_MAX - 2)
			{
				offset = 0;
			}
			offset += 0.2 * deltaTime;
			setFloat(program.id, "wavesOffset", offset);

			glBindVertexArray(VAO);
			glPatchParameteri(GL_PATCH_VERTICES, 4);
			glDrawArraysInstanced(GL_PATCHES, 0, 4, 64 * 64);
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		glfwPollEvents();
		glfwSwapBuffers(window.instance);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteTextures(NUM_TEXTURES, heightMap);
	glDeleteTextures(NUM_TEXTURES, normalMap);
	glDeleteTextures(1, &waterTex);
	glDeleteTextures(1, &wavesHeightMap);
	glDeleteTextures(1, &wavesNormalMap);
	glfwTerminate();
	return 0;
}