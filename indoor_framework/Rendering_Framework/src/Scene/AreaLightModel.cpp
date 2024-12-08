#include "AreaLightModel.h"

namespace INANOA {
	namespace MODEL {
		AreaLightModel::AreaLightModel() {}

		AreaLightModel::~AreaLightModel() {}

		// code kinda ugly here but it's ok
		// static array definition 

		float AreaLightModel::_area_light_vertices[48] = {
			// vertices           // normal         // texcoord
			-0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // Top-left
			 0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // Top-right
			 0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // Bottom-right
			-0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // Top-left
			 0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // Bottom-right
			-0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // Bottom-left
		};

		void AreaLightModel::init(glm::vec3 light_color, glm::mat4 base) {
			setupVAO();

			loadMTexture();
			loadLUTTexture();

			this->_light_color = light_color;

			// special uniform only for area light
			glUniform3fv(SHADER_PARAMETER_BINDING::AREA_LIGHT_VERTICES_0, 1,
				glm::value_ptr(glm::vec3(_area_light_vertices[0], _area_light_vertices[1], _area_light_vertices[2])));
			glUniform3fv(SHADER_PARAMETER_BINDING::AREA_LIGHT_VERTICES_1, 1,
				glm::value_ptr(glm::vec3(_area_light_vertices[1 * 8], _area_light_vertices[1 * 8 + 1], _area_light_vertices[1 * 8 + 2])));
			glUniform3fv(SHADER_PARAMETER_BINDING::AREA_LIGHT_VERTICES_2, 1,
				glm::value_ptr(glm::vec3(_area_light_vertices[4 * 8], _area_light_vertices[4 * 8 + 1], _area_light_vertices[4 * 8 + 2])));
			glUniform3fv(SHADER_PARAMETER_BINDING::AREA_LIGHT_VERTICES_3, 1,
				glm::value_ptr(glm::vec3(_area_light_vertices[5 * 8], _area_light_vertices[5 * 8 + 1], _area_light_vertices[5 * 8 + 2])));

			// for debug try printing the array size later
			this->m_base = base;
		}

		void AreaLightModel::render(glm::vec3 translateVec, glm::mat4 rotate) {
			this->m_model_mat =  glm::translate(glm::mat4(1.0), translateVec) * rotate;
			this->m_model_mat = m_model_mat * m_base;

			// bind light colour
			//glUniform3fv(SHADER_PARAMETER_BINDING::LIGHT_COLOR, 1, glm::value_ptr(_light_color));

			// bind model mat
			glUniformMatrix4fv(SHADER_PARAMETER_BINDING::MODEL_MAT_LOCATION,
				1, false, glm::value_ptr(this->m_model_mat));

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, _m_texture);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, _lut_texture);

			glBindVertexArray(_vao);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0); 
		}

		/* private */
		void AreaLightModel::setupVAO() {

			glGenVertexArrays(1, &_vao);

			glBindVertexArray(_vao);

			glGenBuffers(1, &_vbo);
			glBindBuffer(GL_ARRAY_BUFFER, _vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(_area_light_vertices), _area_light_vertices, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);

			glBindVertexArray(0);
		}

		void AreaLightModel::loadMTexture()
		{
			// load the matrix like a texture
			glGenTextures(1, &_m_texture);
			glBindTexture(GL_TEXTURE_2D, _m_texture);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64,
				0, GL_RGBA, GL_FLOAT, getLTC1());

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void AreaLightModel::loadLUTTexture()
		{
			glGenTextures(1, &_lut_texture);
			glBindTexture(GL_TEXTURE_2D, _lut_texture);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64,
				0, GL_RGBA, GL_FLOAT, getLTC2());

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glBindTexture(GL_TEXTURE_2D, 0);
		}

		const float* AreaLightModel::getLTC1() {
			return LTC1;
		}

		const float* AreaLightModel::getLTC2() {
			return LTC2;
		}
	}
}