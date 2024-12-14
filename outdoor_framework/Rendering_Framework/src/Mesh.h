#pragma once

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>


#include "Shader.h"

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tex_coords;
};
struct Texture {
	GLuint id;
	std::string type;
	std::string path;
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	//void render();

	//void createModelMat(); // for creating model matrix per instance 
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
private:
	//GLuint _vao, _vbo, _ebo, cmdBufferHandle, _instance_buffer;

	//glm::mat4 m_modelMat;

	////INANOA::SCENE::EXPERIMENTAL::SpatialSample* sample_0;

	//const float* instance_pos = nullptr;
	//int num_sample;

	//void setupMesh();
};
