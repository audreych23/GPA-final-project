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
		class RegularEffect
		{
		public:
			RegularEffect();
			~RegularEffect();


			void init(ScreenQuad* screen_quad);

			void render();

			void resize(int width, int height);

			void bindFBO();

			void unbindFBO();

		private:
			void setupFBO();

			ScreenQuad* _screen_quad = nullptr;

			GLuint _fbo, _rbo, _fbo_texture;
		};
	}
}

