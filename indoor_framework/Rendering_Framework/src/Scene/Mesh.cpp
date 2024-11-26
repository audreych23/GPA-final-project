#include "Mesh.h"

#include <glm/gtc/type_ptr.hpp>
#include <Rendering_Framework/src/Rendering/ShaderParameterBindingPoint.h>

namespace INANOA {
	namespace MODEL {
		Mesh::Mesh(std::vector<Vertex> vertices,
			std::vector<unsigned int> indices,
			std::vector<Texture> textures,
			glm::vec3 ka, glm::vec3 kd, glm::vec3 ks, glm::vec3 ke, float ns) {
			this->vertices = vertices;
			this->indices = indices;
			this->textures = textures;
			this->ka = ka;
			this->kd = kd;
			this->ks = ks;
			this->ke = ke;
			this->ns = ns;
			//std::cout << "======mesh======" << '\n';
			//std::cout << vertices.size() << '\n';
			//std::cout << indices.size() << '\n';
			//std::cout << textures.size() << '\n';
			//if (textures.size() > 0) {
			//	std::cout << "texture path: " << textures[0].path << '\n';
			//}
			//std::cout << "====================" << '\n';

			setupMesh();
			_has_texture = 1;
			//this->m_modelMat = glm::mat4(1.0f);
			if (this->textures.empty()) {
				_has_texture = 0;
			}
			//std::cout << _has_texture;

			//for (auto v : vertices) {
			//	std::cout << v.position[0] << " " << v.position[1] << " " << v.position[2] << '\n';
			//}
		}

		void Mesh::render() {
			// WARNING: lazy way of coding (not really but 
			// normally should start from zero, but zero is sampler2datrray
			glActiveTexture(GL_TEXTURE1);
			//shader.setInt(name + number, i); done in rendererbhase
			if (!textures.empty()) glBindTexture(GL_TEXTURE_2D, textures[0].id);
			// bind model matrix
			//glUniformMatrix4fv(SHADER_PARAMETER_BINDING::MODEL_MAT_LOCATION, 1, false, glm::value_ptr(this->m_modelMat));
			// glUniform3fv(SHADER_PARAMETER_BINDING::KA_LOCATION, 1, &ka[0]);
			glUniform3fv(SHADER_PARAMETER_BINDING::KD_LOCATION, 1, &kd[0]);
			//glUniform3fv(SHADER_PARAMETER_BINDING::KS_LOCATION, 1, &ks[0]);
			//glUniform3fv(SHADER_PARAMETER_BINDING::KE_LOCATION, 1, &ke[0]);
			//glUniform1f(SHADER_PARAMETER_BINDING::NS_LOCATION, ns);
			glUniform1i(SHADER_PARAMETER_BINDING::HAS_TEXTURE_LOCATION, _has_texture);


			glBindVertexArray(_vao);
			glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			glActiveTexture(GL_TEXTURE0);
		}

		void Mesh::setupMesh() {
			glGenVertexArrays(1, &_vao);
			glGenBuffers(1, &_vbo);
			glGenBuffers(1, &_ebo);

			glBindVertexArray(_vao);
			glBindBuffer(GL_ARRAY_BUFFER, _vbo);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

			// vertex positions for vao
			glEnableVertexAttribArray(SHADER_PARAMETER_BINDING::VERTEX_LOCATION);
			glVertexAttribPointer(SHADER_PARAMETER_BINDING::VERTEX_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

			glEnableVertexAttribArray(SHADER_PARAMETER_BINDING::NORMAL_LOCATION);
			glVertexAttribPointer(SHADER_PARAMETER_BINDING::NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

			glEnableVertexAttribArray(SHADER_PARAMETER_BINDING::TEX_COORDS_LOCATION);
			glVertexAttribPointer(SHADER_PARAMETER_BINDING::TEX_COORDS_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coords));

			glBindVertexArray(0);
		}
	}
}

