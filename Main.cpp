#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <exception>
#include <vector>
#include "Shader.h"
#define STB_IMAGE_IMPLEMENTATION
#include <STB_IMAGE/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shape2D.h"
#include "ShapeGenerator.h"

// window settings
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720
GLFWwindow* window;

// frame timing
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void initialSetup() {
	// Instantiating the GLFW window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Creating a window
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		throw std::exception("Failed to create GLFW window");
	}
	glfwMakeContextCurrent(window);

	// Initializing GLAD before calling any OpenGL funtions
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw std::exception("Failed to initialize GLAD");
	}

	// Telling OpenGL the window size
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	// Handles window resizing
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Hide mouse cursor and lock it to center of the window
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}


int main() {

	try {
		initialSetup();
	}
	catch (std::exception& e) {
		std::cout << "Exception: " << e.what() << std::endl;
		return -1;
	}

	// floor rect
	glm::vec3 floor_position = glm::vec3(0, 0, 0);
	glm::vec2 floor_size = glm::vec2(WINDOW_WIDTH, 100);
	Shape2D floor_rect = ShapeGenerator::getRectangle(floor_position, floor_size);

	// player rect
	glm::vec3 player_position = { WINDOW_WIDTH / 2, 100, 0.1f };
	glm::vec2 player_size = {150, 150};
	Shape2D player_rect = ShapeGenerator::getOutline(player_position, player_size, 5);

	// Using the shaders
	Shader shader_purple("shader.vert", "shader_purple.frag");
	Shader shader_red("shader.vert", "shader_red.frag");

	// RENDER LOOP
	while (!glfwWindowShouldClose(window))
	{

		processInput(window);

		// Delta time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Setting uniform values
		glm::mat4 projection_matrix = glm::mat4();
		projection_matrix = glm::ortho(0.0f, float(WINDOW_WIDTH), 0.0f, float(WINDOW_HEIGHT));

		shader_purple.use();
		shader_purple.setMat4("projection_matrix", projection_matrix);

		shader_red.use();
		shader_red.setMat4("projection_matrix", projection_matrix);


		// Rendering commands here:
		glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		shader_purple.use();
		floor_rect.draw();

		shader_red.use();
		player_rect.draw();

		// Check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clears all GLFW allocated resources
	glfwTerminate();

	return 0;
}
