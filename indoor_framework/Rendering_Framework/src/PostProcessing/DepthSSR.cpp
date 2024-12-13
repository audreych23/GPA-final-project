#include "DepthSSR.h"

namespace INANOA {
	namespace POST_PROCESSING {
		DepthSSR::DepthSSR() {}
		DepthSSR::~DepthSSR() {}

		void DepthSSR::init(ScreenQuad* screen_quad) {
			// make it fixed light fixed
			_light_look_at = glm::vec3(0.542, -0.141, -0.422);
			_light_orig = glm::vec3(-2.845, 2.028, -1.293);
			_light_near = 0.1;
			_light_far = 10;
			_light_range = 5; // where to use this?

			// calculate light view matrix and light projection matrix
			_light_view_mat = glm::lookAt(_light_orig, _light_look_at, glm::vec3(0.0f, 1.0f, 0.0f));
			// follow slides
			_light_proj_mat = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, _light_near, _light_far);
			_screen_quad = screen_quad;
			setupFBO();
		}

		// to get the depth of the object
		void DepthSSR::renderLightSpace(float offset, glm::vec3 pos_orig, glm::mat4 lookAt, glm::mat4 proj) {
			glEnable(GL_DEPTH_TEST);

			// for removing shadow acne
			openGLPolygonHelper(4.0f);

			setDirectionalShadowMappingSubProcess(DirectionalShadowMappingSubProcess::LIGHT_SPACE_RENDER);
			glUniformMatrix4fv(SHADER_PARAMETER_BINDING::LIGHT_VIEW_MAT_LOCATION, 1, false, glm::value_ptr(lookAt));
			glUniformMatrix4fv(SHADER_PARAMETER_BINDING::LIGHT_PROJ_MAT_LOCATION, 1, false, glm::value_ptr(proj));

			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glClear(GL_DEPTH_BUFFER_BIT);

		}

		void DepthSSR::renderShadow() {
			/*
			setDirectionalShadowMappingSubProcess(DirectionalShadowMappingSubProcess::SHADOW_RENDER);

			glm::mat4 scale_bias_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
			scale_bias_matrix = glm::scale(scale_bias_matrix, glm::vec3(0.5f, 0.5f, 0.5f));
			glUniformMatrix4fv(SHADER_PARAMETER_BINDING::LIGHT_BIAS_MAT_LOCATION, 1, false, glm::value_ptr(scale_bias_matrix));*/
			glm::mat4 scale_bias_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
			scale_bias_matrix = glm::scale(scale_bias_matrix, glm::vec3(0.5f, 0.5f, 0.5f));
			glUniformMatrix4fv(200, 1, false, glm::value_ptr(scale_bias_matrix));

			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, _fbo_texture);
		}

		void DepthSSR::renderDebug() {
			// activate shader program and set it as Directional Shadow maaping
			// render Depth map to quad for visual debugging
			// ---------------------------------------------
			glDisable(GL_DEPTH_TEST);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glClear(GL_DEPTH_BUFFER_BIT);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, _fbo_texture);
			_screen_quad->render();
			glEnable(GL_DEPTH_TEST);
		}


		void DepthSSR::openGLPolygonHelper(float offset) {
			// this is needed for method first call 
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(offset, offset);
		}

		void DepthSSR::resize(int width, int height) {
			SHADOW_WIDTH = width;
			SHADOW_HEIGHT = height;
			// set up depth fbo
			glDeleteTextures(1, &_fbo_texture);

			glGenTextures(1, &_fbo_texture);
			glBindTexture(GL_TEXTURE_2D, _fbo_texture);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// for depth buffer
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
			// normally it should be color attachment, but we want to attach it to the depth
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _fbo_texture, 0);

			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				std::cout << "Error: Framebuffer is not complete!" << std::endl;
			}

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);


		}

		void DepthSSR::bindFBO() {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
		}

		void DepthSSR::unbindFBO() {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}

		void DepthSSR::finishRender() {
			setDirectionalShadowMappingSubProcess(DirectionalShadowMappingSubProcess::FINISH_RENDER);
		}
		/* private */
		void DepthSSR::setupFBO() {
			glGenFramebuffers(1, &_fbo);
		}


	}
}