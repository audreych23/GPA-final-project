#include "VolumetricLights.h"

namespace INANOA {
	namespace POST_PROCESSING {
		VolumetricLights::VolumetricLights() {}

		VolumetricLights::~VolumetricLights() {}


		void VolumetricLights::init(ScreenQuad* screen_quad) {
			// hardcoding everything rightnow
			_light_position = glm::vec3(-2.845 * 5, 2.028 * 2.5, -1.293 * 5);

			_screen_quad = screen_quad;
			setupFBO();
		}


		void VolumetricLights::resize(int width, int height) {
			glDeleteRenderbuffers(1, &_rbo);
			// not sure if im deleting it correcrly
			glDeleteTextures(2, _fbo_texture);
			//glDeleteTextures(1, &_fbo_texture_bloom[1]);

			// bind framebuffer
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
			// Generate RBO
			glGenRenderbuffers(1, &_rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rbo);

			glGenTextures(2, _fbo_texture);
			// Generate Color Texture For fbo
			for (unsigned int i = 0; i < 2; i++)
			{
				glBindTexture(GL_TEXTURE_2D, _fbo_texture[i]);
				glTexImage2D(
					GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL
				);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				// attach texture to framebuffer
				glFramebufferTexture2D(
					GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, _fbo_texture[i], 0
				);
			}

			// explicitly tell opengl we're rendering to multiple color buffers
			unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
			glDrawBuffers(2, attachments);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				printf("Error: Framebuffer is not complete!\n");
			}

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}

		void VolumetricLights::render() {
			glDisable(GL_DEPTH_TEST);
			// Unbind FBO
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			// Clear Depth
			glClear(GL_DEPTH_BUFFER_BIT);
			for (int i = 0; i < 2; i++) {
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, _fbo_texture[i]);
			}

			// render Quad 
			_screen_quad->render();

			glEnable(GL_DEPTH_TEST);
		}

		void VolumetricLights::bindFBO() {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
		}

		// x, y, width, heihtt
		void VolumetricLights::calculateInNDC(glm::mat4 view_mat, glm::mat4 proj_mat, std::vector<float> viewport) {
			auto clip_space = proj_mat * view_mat * glm::vec4(_light_position, 1.0);
			
			glm::vec3 ndc = glm::vec3(clip_space) / clip_space.w;

			// Map NDC to window coordinates
			glm::vec2 window_coordinates;
			window_coordinates.x = viewport[0] + (viewport[2] * (ndc.x + 1.0f) / 2.0f);
			window_coordinates.y = viewport[1] + (viewport[3] * (ndc.y + 1.0f) / 2.0f);
			//windowCoordinates.z = (ndc.z + 1.0f) / 2.0f;  // Depth value in [0, 1] range
			window_coordinates.x /= viewport[2];
			window_coordinates.y /= viewport[3];
			//std::cout << "x" << window_coordinates.x << '\n';
			//std::cout << window_coordinates.y << '\n';
			glUniform2fv(SHADER_POST_PARAMETER_BINDING::LIGHT_POS_SCREEN_LOCATION, 1, glm::value_ptr(window_coordinates));
		}

		/* private methods */
		void VolumetricLights::setupFBO() {
			glGenFramebuffers(1, &_fbo);
		}

		


	}

}