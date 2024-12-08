#pragma once

#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include <Rendering_Framework/src/Rendering/ShaderParameterBindingPoint.h>
#include <Rendering_Framework/src/Rendering/Shader.h>

#include "LTCMatrix.h"

namespace INANOA {
	namespace MODEL {
		class AreaLightModel
		{
		public:
			AreaLightModel();

			~AreaLightModel();

			void init(glm::vec3 light_color, glm::mat4 base);

			void render(glm::vec3 translateVec, glm::mat4 rotate);

		private:
			struct LTCMatrices {
				GLuint mat1;
				GLuint mat2;
			};

			void setupVAO();

			void loadMTexture();

			void loadLUTTexture();

			// make a getter from a header file 
			static const float* getLTC1();
			static const float* getLTC2();

			GLuint _vao, _vbo, _ebo;

			GLuint _m_texture;
			GLuint _lut_texture;

			glm::mat4 m_model_mat;
			glm::mat4 m_base;

			glm::vec3 _light_color;

			static float _area_light_vertices[48];
		};
	}
}