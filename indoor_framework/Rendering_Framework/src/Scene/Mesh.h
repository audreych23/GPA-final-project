#pragma once

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>


#include <Rendering_Framework/src/Rendering/Shader.h>

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

		class Mesh
		{
		public:
			Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, 
				glm::vec3 ka, glm::vec3 kd, glm::vec3 ks, glm::vec3 ke, float shininess);
			void render();
 
			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;
			std::vector<Texture> textures;
			glm::vec3 ka;
			glm::vec3 kd;
			glm::vec3 ks;
			glm::vec3 ke;
			float ns;

		private:
			GLuint _vao, _vbo, _ebo;

			int _has_texture;

			glm::mat4 m_modelMat;

			void setupMesh();
		};
	}
}
