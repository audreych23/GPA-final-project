#pragma once

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>


#include "../Rendering/Shader.h"

namespace INANOA {
	namespace MODEL {
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

		//struct DrawElementsIndirectCommand {
		//	// number of index need to draw
		//	GLuint count;
		//	// number of instance
		//	GLuint instanceCount;
		//	// location of the first index
		//	GLuint firstIndex;
		//	// a constant that is added to each index
		//	GLuint baseVertex;
		//	// a base instance for fetching instanced vertex attributes
		//	GLuint baseInstance;
		//};

		class Mesh
		{
		public:
			Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, 
				glm::vec3 ka, glm::vec3 kd, glm::vec3 ks, glm::vec3 ke, float shininess);
			void render();

			//void createModelMat(); // for creating model matrix per instance 
			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;
			std::vector<Texture> textures;
			glm::vec3 ka;
			glm::vec3 kd;
			glm::vec3 ks;
			glm::vec3 ke;
			float ns;

		private:
			GLuint _vao, _vbo, _ebo, cmdBufferHandle, _instance_buffer;

			int _has_texture;

			glm::mat4 m_modelMat;

			void setupMesh();
		};
	}
}
