#pragma once

#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <Rendering_Framework/src/Rendering/Shader.h>
#include <Rendering_Framework/src/Rendering/ShaderParameterBindingPoint.h>
#include <Rendering_Framework/src/PostProcessing/ScreenQuad.h>

namespace INANOA {
	namespace POST_PROCESSING {
		// This class is a bit unique since we need the original shader
		class DepthSSR
		{
		public:
			enum class DirectionalShadowMappingSubProcess : int {
				LIGHT_SPACE_RENDER = 0,
				SHADOW_RENDER = 1,
				FINISH_RENDER = 2
			};

			DepthSSR();
			~DepthSSR();

			void init(ScreenQuad* screen_quad);

			void renderLightSpace(float offset, glm::vec3 pos_orig, glm::mat4 lookAt, glm::mat4 proj);

			void renderShadow();

			void renderDebug();

			void resize(int width, int height);

			void bindFBO();

			void unbindFBO();

			void finishRender();

			void openGLPolygonHelper(float offset);
			 
		private:
			void setupFBO();

			// since this post processing is done in our main shader
			inline void setDirectionalShadowMappingSubProcess(const DirectionalShadowMappingSubProcess type) {
				glUniform1i(SHADER_PARAMETER_BINDING::POST_PROCESSING_SUB_PROCESS_LOCATION, static_cast<int>(type));
			}

			ScreenQuad* _screen_quad = nullptr;

			GLuint _fbo, _rbo, _fbo_texture;

			int SHADOW_WIDTH = 1024;
			int SHADOW_HEIGHT = 1024;
			glm::vec3 _light_look_at;
			glm::vec3 _light_orig;
			float _light_near;
			float _light_far;
			float _light_range;


			glm::mat4 _light_view_mat;
			glm::mat4 _light_proj_mat;
		};
	}

}
