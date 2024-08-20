#include "ShapeGenerator.h"

Shape2D ShapeGenerator::getRectangle(glm::vec3 position, glm::vec2 size) {
	return Shape2D(
		{
			{ position.x, position.y, position.z, 0, 0 },
			{ position.x, position.y + size.y, position.z, 0, 1 },
			{ position.x + size.x, position.y + size.y, position.z, 1, 1},
			{ position.x + size.x, position.y, position.z, 1, 0 }
		},
		{ 0, 1, 2, 0, 2, 3 }
	);
}

Shape2D ShapeGenerator::getOutline(glm::vec3 position, glm::vec2 size, int border) {

	float height = size.y;
	float width = size.x;

	std::vector<Vertex> vertices = {
		{position.x - width / 2, position.y - height / 2, position.z, 0.0f, 0.0f},
		{position.x + width / 2, position.y - height / 2, position.z, 0.0f, 0.0f},
		{position.x + width / 2, position.y + height / 2, position.z, 0.0f, 0.0f},
		{position.x - width / 2, position.y + height / 2, position.z, 0.0f, 0.0f},

		{position.x - (width / 2 + border), position.y - (height / 2 + border), position.z, 0.0f, 0.0f},
		{position.x + (width / 2 + border), position.y - (height / 2 + border), position.z, 0.0f, 0.0f},
		{position.x + (width / 2 + border), position.y + (height / 2 + border), position.z, 0.0f, 0.0f},
		{position.x - (width / 2 + border), position.y + (height / 2 + border), position.z, 0.0f, 0.0f}
	};

	std::vector<int> indices = {
		0, 1, 4,
		1, 4, 5,
		1, 2, 5,
		2, 5, 6,
		2, 3, 6,
		3, 6, 7,
		3, 0, 7,
		0, 4, 7
	};

	Mesh mesh = { vertices, indices };

	return Shape2D(vertices, indices);
}