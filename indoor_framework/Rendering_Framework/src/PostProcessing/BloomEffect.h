#pragma once

#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>

#include <Rendering_Framework/src/Rendering/Shader.h>
#include <Rendering_Framework/src/Rendering/ShaderParameterBindingPoint.h>
#include <Rendering_Framework/src/PostProcessing/ScreenQuad.h>

namespace INANOA {
	namespace POST_PROCESSING {
		class BloomEffect
		{
		public:
			enum class BloomSubProcess : int {
				BLUR_EFFECT = 0,
				FINAL_EFFECT = 1,
			};

			BloomEffect();
			~BloomEffect();

			void init(ScreenQuad* screen_quad);

			void render();

			void resize(int width, int height);

			void bindFBO();
		private:
			void setupFBO();

			void resizeBloomColor(int width, int height);

			void resizeBloomBlur(int width, int height);

			inline void setBloomSubProcess(const BloomSubProcess type) {
				glUniform1i(SHADER_POST_PARAMETER_BINDING::POST_PROCESSING_SUB_ID_LOCATION, static_cast<int>(type));
			}

			ScreenQuad* _screen_quad = nullptr;

			// for color
			GLuint _fbo, _rbo, _fbo_texture[2];

			// for gaussian blurring
			GLuint _fbo_ping_pong[2];
			GLuint _fbo_texture_ping_pong[2];
		};
	}
}

