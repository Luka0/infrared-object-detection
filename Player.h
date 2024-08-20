#pragma once
#include "Shape2D.h"
#include "glm/glm.hpp"

class Player
{
public:
	glm::vec3 position;
	Shape2D& shape;

	Player(Shape2D& arg_shape);
	void move(glm::vec3 direction);
};

