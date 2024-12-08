#include "PointShadow.h"

namespace INANOA {
	namespace POST_PROCESSING {
		PointShadow::PointShadow() {}
		PointShadow::~PointShadow() {}

		void PointShadow::init(ScreenQuad* screen_quad) {
			_light_pos = glm::vec3(1.87659, 0.4625, 0.103928);
			_light_near = 0.22f;
			_light_far = 10.0f;

			_light_proj_mat = glm::perspective(glm::radians(90.0f), 
				(float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, _light_near, _light_far);
			
			_light_view_mat.push_back(glm::lookAt(_light_pos, _light_pos + 
				glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
			_light_view_mat.push_back(glm::lookAt(_light_pos, _light_pos + 
				glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
			_light_view_mat.push_back(glm::lookAt(_light_pos, _light_pos + 
				glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
			_light_view_mat.push_back(glm::lookAt(_light_pos, _light_pos + 
				glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
			_light_view_mat.push_back(glm::lookAt(_light_pos, _light_pos + 
				glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
			_light_view_mat.push_back(glm::lookAt(_light_pos, _light_pos + 
				glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

			_screen_quad = screen_quad;
			setupFBO();
		}

		void PointShadow::renderLightSpace(float offset) {
			glEnable(GL_DEPTH_TEST);

			setPointShadowSubProcess(PointShadowSubProcess::LIGHT_SPACE_RENDER);

			// brute force cuz why not ;)

			glUniformMatrix4fv(SHADER_PARAMETER_BINDING::LIGHT_VIEW_MAT_LOCATION_0, 1, false, glm::value_ptr(_light_view_mat[0]));
			glUniformMatrix4fv(SHADER_PARAMETER_BINDING::LIGHT_VIEW_MAT_LOCATION_1, 1, false, glm::value_ptr(_light_view_mat[1]));
			glUniformMatrix4fv(SHADER_PARAMETER_BINDING::LIGHT_VIEW_MAT_LOCATION_2, 1, false, glm::value_ptr(_light_view_mat[2]));
			glUniformMatrix4fv(SHADER_PARAMETER_BINDING::LIGHT_VIEW_MAT_LOCATION_3, 1, false, glm::value_ptr(_light_view_mat[3]));
			glUniformMatrix4fv(SHADER_PARAMETER_BINDING::LIGHT_VIEW_MAT_LOCATION_4, 1, false, glm::value_ptr(_light_view_mat[4]));
			glUniformMatrix4fv(SHADER_PARAMETER_BINDING::LIGHT_VIEW_MAT_LOCATION_5, 1, false, glm::value_ptr(_light_view_mat[5]));

			glUniformMatrix4fv(SHADER_PARAMETER_BINDING::LIGHT_PROJ_MAT_LOCATION, 1, false, glm::value_ptr(_light_proj_mat));
			glUniform1f(SHADER_PARAMETER_BINDING::POINT_SHADOW_FAR_PLANE, _light_far);
			glUniform3fv(SHADER_PARAMETER_BINDING::LIGHT_BLOOM_POS, 1, glm::value_ptr(_light_pos));

			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glClear(GL_DEPTH_BUFFER_BIT);
		}

		void PointShadow::renderShadow() {
			setPointShadowSubProcess(PointShadowSubProcess::SHADOW_RENDER);

			glm::mat4 scale_bias_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
			scale_bias_matrix = glm::scale(scale_bias_matrix, glm::vec3(0.5f, 0.5f, 0.5f));
			glUniformMatrix4fv(SHADER_PARAMETER_BINDING::LIGHT_BIAS_MAT_LOCATION, 1, false, glm::value_ptr(scale_bias_matrix));
			glUniform1f(SHADER_PARAMETER_BINDING::POINT_SHADOW_FAR_PLANE, _light_far);
			glUniform3fv(SHADER_PARAMETER_BINDING::LIGHT_BLOOM_POS, 1, glm::value_ptr(_light_pos));

			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_CUBE_MAP, _fbo_texture);
		}

		void PointShadow::renderDebug() {
			
		}

		void PointShadow::openGLPolygonHelper(float offset) {
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(offset, offset);
		}

		void PointShadow::resize(int width, int height) {
			glDeleteTextures(1, &_fbo_texture);
			
			glGenTextures(1, &_fbo_texture);
			glBindTexture(GL_TEXTURE_CUBE_MAP, _fbo_texture);

			for (unsigned int i = 0; i < 6; ++i)
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, 
					SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			// attach depth texture as FBO's depth buffer
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
			glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _fbo_texture, 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				std::cout << "Error: Framebuffer is not complete!" << std::endl;
			}

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}

		void PointShadow::bindFBO() {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
		}

		void PointShadow::unbindFBO() {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}

		void PointShadow::finishRender() {
			setPointShadowSubProcess(PointShadowSubProcess::FINISH_RENDER);
		}

		/* private */
		void PointShadow::setupFBO() {
			glGenFramebuffers(1, &_fbo);
		}

	}
}