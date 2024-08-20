#pragma once
#include "Shape2D.h"
#include <glm/glm.hpp>

class ShapeGenerator
{
	public:
		static Shape2D getRectangle(glm::vec3 position, glm::vec2 size);
		static Shape2D getOutline(glm::vec3 position, glm::vec2 size, int border);
};
