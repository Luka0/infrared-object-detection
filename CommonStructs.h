#pragma once
#include <vector>

struct Vertex {
	float x;
	float y;
	float z;
	float u;
	float v;
};

struct Mesh {
	std::vector<Vertex> vertices;
	std::vector<int> indices;
};

struct TextureData {
	unsigned int id;
	int width;
	int height;
	int channels_count;
};