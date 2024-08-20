#include "Shape2D.h"
#include <glad/glad.h>

Shape2D::Shape2D(std::vector<Vertex> vertices, std::vector<int> indices) {
	mesh = {
		vertices,
		indices
	};
	// VBO setup
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), mesh.vertices.data(), GL_STATIC_DRAW);
	// VAO setup
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);	// vertex position
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);	// vertex texture coordinates
	// EBO setup
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(int), mesh.indices.data(), GL_STATIC_DRAW);
}

Shape2D::~Shape2D() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Shape2D::draw() {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, mesh.indices.size() * sizeof(int), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);	// Unbind VAO
}