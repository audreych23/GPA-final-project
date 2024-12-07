#pragma once

#include <iostream>
#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>

#include <Rendering_Framework/src/Rendering/Shader.h>
#include <Rendering_Framework/src/Rendering/ShaderParameterBindingPoint.h>
#include <Rendering_Framework/src/PostProcessing/ScreenQuad.h>

namespace INANOA {
	namespace POST_PROCESSING {
		class DeferredShading
		{
		public:
			enum class DeferredShadingOption : int {
				WORLD_SPACE_VERTEX = 0,
				WORLD_SPACE_NORMAL = 1,
				AMBIENT_COLOR_MAP = 2,
				DIFFUSE_COLOR_MAP = 3,
				SPECULAR_COLOR_MAP = 4,
			};

			DeferredShading();
			~DeferredShading();

			void init(ScreenQuad* screen_quad);

			void setupSSAO();

			void render(DeferredShadingOption option);

			void resize(int width, int height);

			void bindFBO();
		private:
			void setupFBO();

			inline void setDeferredShadingOption(const DeferredShadingOption type) {
				glUniform1i(SHADER_POST_PARAMETER_BINDING::POST_PROCESSING_SUB_ID_LOCATION, static_cast<int>(type));
			}

			ScreenQuad* _screen_quad = nullptr;

			// for color
			GLuint _fbo, _rbo, _fbo_texture[5];
			/* SSAO */
			GLuint noiseTexture;
			std::vector<glm::vec3> ssaoKernel;
		};
	}
}