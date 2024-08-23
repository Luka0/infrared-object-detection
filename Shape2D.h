#pragma once
#include <vector>
#include "CommonStructs.h"

class Shape2D
{
	public:
		Mesh mesh;
		Shape2D(std::vector<Vertex> vertices, std::vector<int> indices);
		~Shape2D();
		void draw();
		Shape2D& operator=(const Shape2D& other);
		
	private:
		unsigned int VBO;
		unsigned int VAO;
		unsigned int EBO;
};
