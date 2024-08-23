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
#include "ComputeShader.h"
#include "CommonStructs.h"


// window settings
#define WINDOW_WIDTH 640 * 2
#define WINDOW_HEIGHT 480 * 2
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
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
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

TextureData loadTextureFromJpg(const char* path, int texture_index_const) {
	unsigned int new_texture;
	glGenTextures(1, &new_texture);
	glActiveTexture(texture_index_const);
	glBindTexture(GL_TEXTURE_2D, new_texture);
	// Configure texture wrapping / filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Load texture image
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);	// 0 is default
	// Connect the texture with the image
	if (data)
	{
		// convert rgb to rgba
		unsigned char* rgba_data = new unsigned char[width * height * 4];
		for (int i = 0; i < width * height; i++) {
			rgba_data[i * 4 + 0] = data[i * 3 + 0];  // R
			rgba_data[i * 4 + 1] = data[i * 3 + 1];  // G
			rgba_data[i * 4 + 2] = data[i * 3 + 2];  // B
			rgba_data[i * 4 + 3] = 255;              // A (fully opaque)
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba_data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture: "<< path << std::endl;
	}
	// Free the image memory after generating the texture
	stbi_image_free(data);

	return { new_texture, width, height, nrChannels };
}


int main() {

	try {
		initialSetup();
	}
	catch (std::exception& e) {
		std::cout << "Exception: " << e.what() << std::endl;
		return -1;
	}

	// load thermal image from desktop
	//TextureData thermal_tex = loadTextureFromJpg("final_thesis_dataset/hot_cups/1_67.jpg", GL_TEXTURE0);
	stbi_set_flip_vertically_on_load(true);
	TextureData thermal_tex = loadTextureFromJpg("final_thesis_dataset/drone_shots/drone_1.jpg", GL_TEXTURE0);

	// Background rectangle
	glm::vec3 bg_position = glm::vec3(0, 0, 0);
	glm::vec2 bg_size = glm::vec2(WINDOW_WIDTH, WINDOW_HEIGHT);
	Shape2D bg_rect = ShapeGenerator::getRectangle(bg_position, bg_size);

	// Detection outline
	glm::vec3 outline_position = { WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 0.1f };
	glm::vec2 outline_size = {200, 200};
	Shape2D test_outline = ShapeGenerator::getOutline(outline_position, outline_size, 5);
	test_outline = ShapeGenerator::getOutline(outline_position, glm::vec2(100, 200), 5);
	Shape2D test_outline2 = ShapeGenerator::getOutline(outline_position, glm::vec2(100, 100), 5);

	// Using the shaders
	Shader shader_purple("shader.vert", "shader_purple.frag");
	Shader shader_red("shader.vert", "shader_red.frag");
	Shader shader_texture("shader_texture.vert", "shader_texture.frag");

	// Compute shader setup
	ComputeShader thresholdComputeShader("thresholding_shader.comp");
	thresholdComputeShader.use();

	GLuint gVBO;
	glGenBuffers(1, &gVBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, gVBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, thermal_tex.width*thermal_tex.height*sizeof(int), NULL, GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, gVBO);

	glBindImageTexture(1, thermal_tex.id, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8);
	glDispatchCompute(thermal_tex.width / 16, thermal_tex.height / 16, 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);

	// fetching output data from compute shader
	glBindBuffer(GL_ARRAY_BUFFER, gVBO);
	int* ptr = (int*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);

	// Retrieve raw detection values from array
	std::vector<Shape2D*> detection_outlines;
	Shape2D new_shape = test_outline;
	if (ptr) {
		// Assuming the buffer was storing an array of integers
		for (int i = 0; i < thermal_tex.width*thermal_tex.height; i++) {
			int value = ptr[i];
			int x_coord = i % thermal_tex.width;
			int y_coord = (i - x_coord) / thermal_tex.width;
			//std::cout << value;
			if (value == 1) {
				new_shape = ShapeGenerator::getOutline(glm::vec3(x_coord * 2, y_coord * 2, 0.1), glm::vec2(20, 20), 2);
				detection_outlines.push_back(&new_shape);
				//test_outline = ShapeGenerator::getOutline(glm::vec3(x_coord * 2, y_coord * 2, 0.1), glm::vec2(20, 20), 2);
			}
		}
		// Unmap the buffer
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}
	else {
		printf("Failed to map buffer.\n");
	}

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

		shader_texture.use();
		shader_texture.setMat4("projection_matrix", projection_matrix);
		shader_texture.setInt("texture1", 0);


		// Rendering commands here:
		glClearColor(0.529f, 0.808f, 0.922f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		shader_texture.use();
		bg_rect.draw();

		// test
		shader_red.use();
		for (Shape2D* var : detection_outlines)
		{
			var->draw();
		}

		//shader_red.use();
		//test_outline.draw();

		//shader_red.use();
		//detection_outline2.draw();

		// Check and call events and swap the buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clears all GLFW allocated resources
	glfwTerminate();

	return 0;
}
