// CONSIDER ADDING IMGUI WIDGETS
#define _CRT_SECURE_NO_WARNINGS // because printf is "too dangerous"

#include "Shader.hpp"
#include "Program.hpp"
#include "TextureLoader.hpp"

#include "renderer.h"

const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 500.0f;
const float CAM_FOV = 45.0f;
const unsigned NUM_TEXTURES = 13;
const unsigned TESS_LEVEL = 1;
const float DEPTH = 0.11f;

glm::mat4 projection;
unsigned VAO = 0;
float interpolateFactor = 0.0f;
double deltaTime = 0.0;
unsigned heightMap[NUM_TEXTURES];
unsigned normalMap[NUM_TEXTURES];
unsigned waterTex;
unsigned wavesNormalMap;
unsigned wavesHeightMap;
unsigned firstIndex = 0;
unsigned lastIndex = 1;
bool rotate = false;

int main()
{
	Window window = {};
	Keyboard keys = {};

	init_window(&window, 1280, 720, "GL Waves");
	init_keyboard(&keys);

	projection = glm::perspective(glm::radians(CAM_FOV), (float)1280 / (float)720, NEAR_PLANE, FAR_PLANE);

	//glfwSetFramebufferSizeCallback(window, resizeCallback);

	Program program;
	program.create();
	program.attachShader(Shader2::createVertexShader("assets/shaders/water.vert"));
	program.attachShader(Shader2::createTessalationControlShader("assets/shaders/water_tess_control.glsl"));
	program.attachShader(Shader2::createTessalationEvaluationShader("assets/shaders/water_tess_eval.glsl"));
	program.attachShader(Shader2::createFragmentShader("assets/shaders/water.frag"));
	program.link();

	glGenVertexArrays(1, &VAO);
	glGenTextures(NUM_TEXTURES, heightMap);
	glGenTextures(NUM_TEXTURES, normalMap);
	glGenTextures(1, &waterTex);

	for (int i = 0; i < NUM_TEXTURES; ++i)
	{
		char heightmap_filename[256] = {};
		sprintf(heightmap_filename, "assets/textures/heights/%d.png", i+1);
		TextureLoader::loadTexture(heightMap[i], std::string(heightmap_filename));

		char normalmap_filename[256] = {};
		sprintf(normalmap_filename, "assets/textures/normals/%d.png", i+1);
		TextureLoader::loadTexture(normalMap[i], std::string(normalmap_filename));
	}

	TextureLoader::loadTexture(waterTex, "assets/textures/water.jpg");
	TextureLoader::loadTexture(wavesNormalMap, "assets/textures/wavesNormal.jpg");
	TextureLoader::loadTexture(wavesHeightMap, "assets/textures/wavesHeight.jpg");

	glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(100.0f));
	glm::vec3 viewPos = glm::vec3(1.0f);
	glm::mat4 view = glm::lookAt(viewPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	program.use();
	program.setVec3("light.direction"    , glm::vec3(0.0, -1.0, 0.0));
	program.setVec3("light.ambient"      , glm::vec3(0.15, 0.15, 0.15));
	program.setVec3("light.diffuse"      , glm::vec3(0.75, 0.75, 0.75));
	program.setVec3("light.specular"     , glm::vec3(1.0, 1.0, 1.0));
	program.setFloat("interpolateFactor" , interpolateFactor);
	program.setFloat("depth", DEPTH);
	program.setInt("tessLevel", TESS_LEVEL);

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

		// controls
		if (keys.ESC.is_pressed) glfwSetWindowShouldClose(window.instance, true);
		if (keys.P.is_pressed) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if (keys.O.is_pressed) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (keys.R.is_pressed) rotate = true;
		if (keys.T.is_pressed) rotate = false;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0, 0, 0, 1);

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

		view = glm::lookAt( viewPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		program.use();
		program.setMat4("model", model);
		program.setMat4("mvp", projection * view * model);
		program.setVec3("viewPos", viewPos);
		// This section used to be renderWater();
		{
			program.use();
			program.setInt("heightMap1", 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, heightMap[firstIndex]);

			program.setInt("heightMap2", 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, heightMap[lastIndex]);

			program.setInt("normalMap1", 2);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, normalMap[firstIndex]);

			program.setInt("normalMap2", 3);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, normalMap[lastIndex]);

			program.setInt("water", 4);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, waterTex);

			program.setInt("wavesHeightMap", 5);
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, wavesHeightMap);

			program.setInt("wavesNormalMap", 6);
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
				program.setFloat("interpolateFactor", interpolateFactor);
			}

			static float offset = 0.0f;
			if (offset >= INT_MAX - 2)
			{
				offset = 0;
			}
			offset += 0.2 * deltaTime;
			program.setFloat("wavesOffset", offset);

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

void resizeCallback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void setWindowSize(int width, int height)
{
  projection = glm::perspective(glm::radians(CAM_FOV), (float)width/(float)height, NEAR_PLANE, FAR_PLANE);
  glViewport(0, 0, width, height);
}