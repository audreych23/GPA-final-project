#pragma once

#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>

#include <Rendering_Framework/src/Rendering/Shader.h>
#include <Rendering_Framework/src/Rendering/ShaderParameterBindingPoint.h>
#include <Rendering_Framework/src/PostProcessing/ScreenQuad.h>
#include <vector>

namespace INANOA {
	namespace POST_PROCESSING {
		class VolumetricLights
		{
		public:
			VolumetricLights();

			~VolumetricLights();

			void init(ScreenQuad* screen_quad);

			void render();

			void resize(int width, int height);

			void bindFBO();

			const glm::vec3 getLightPosition() const { return _light_position; };

			void calculateInNDC(glm::mat4 view_mat, glm::mat4 proj_mat, std::vector<float> viewport, glm::vec3 mLightPos);
		private:
			void setupFBO();

			//inline void setBloomSubProcess(const BloomSubProcess type) {
				//glUniform1i(SHADER_POST_PARAMETER_BINDING::POST_PROCESSING_SUB_ID_LOCATION, static_cast<int>(type));
			//}

			ScreenQuad* _screen_quad = nullptr;

			// for color
			GLuint _fbo, _rbo, _fbo_texture[2];

			glm::vec3 _light_position;
		};
	}
}
