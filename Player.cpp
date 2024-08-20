#include "Player.h"

Player::Player(Shape2D& arg_shape) : shape(arg_shape) {
	shape = arg_shape;
}

void Player::move(glm::vec3 direction) {
	position += direction;

	// update shape
	shape =  Shape2D(
		{
			{ position.x, position.y, position.z },
			{ position.x, position.y + 50, position.z },
			{ position.x + 50, position.y + 50, position.z},
			{ position.x + 50, position.y, position.z }
		},
		{ 0, 1, 2, 0, 2, 3 }
	);
}