#include "BloomEffect.h"

namespace INANOA {
	namespace POST_PROCESSING {
		BloomEffect::BloomEffect() {}
		BloomEffect::~BloomEffect() {}

		void BloomEffect::init(ScreenQuad* screen_quad) {
			_screen_quad = screen_quad;
			setupFBO();
		}

		void BloomEffect::render() {
			//if (!_use_postprocessing) return;
			glDisable(GL_DEPTH_TEST);
			// unbind fbo
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

			// should it??
			glClear(GL_DEPTH_BUFFER_BIT);
			// use shader for blur
			// set blur function here
			//regular_fbo_shader->useProgram();
			setBloomSubProcess(BloomSubProcess::BLUR_EFFECT);

			bool horizontal = true;
			bool first_iteration = true;
			int blur_amount = 10;
			for (unsigned int i = 0; i < blur_amount; ++i) {
				glBindFramebuffer(GL_FRAMEBUFFER, _fbo_ping_pong[horizontal]);
				glUniform1i(SHADER_POST_PARAMETER_BINDING::HORIZONTAL_FLAG_LOCATION, (int)horizontal);
				glBindTexture(GL_TEXTURE_2D, first_iteration ? _fbo_texture[1] : _fbo_texture_ping_pong[!horizontal]);

				// render Quad 
				_screen_quad->render();

				horizontal = !horizontal;
				if (first_iteration) first_iteration = false;
			}

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

			// not sure if can use api
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// set differetn shader
			//regular_fbo_shader->useProgram();
			setBloomSubProcess(BloomSubProcess::FINAL_EFFECT);
			//setPostProcessingType();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, _fbo_texture[0]);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, _fbo_texture_ping_pong[!horizontal]);
			//glUniform1i(SHADER_PARAMETER_BINDING::BLOOM_FLAG_LOCATION, (int)bloom);
			//glUniform1f(SHADER_PAREMTER_BINDING::EXPOSURE_LOCATION, exposure);

			/*glBindVertexArray(_rect_vao);
			glDrawArrays(GL_TRIANGLES, 0, 6);*/
			_screen_quad->render();

			glEnable(GL_DEPTH_TEST);
		}

		void BloomEffect::resize(int width, int height) {
			resizeBloomColor(width, height);
			resizeBloomBlur(width, height);
		}

		void BloomEffect::bindFBO() {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
		}

		/* private */
		void BloomEffect::setupFBO() {
			glGenFramebuffers(1, &_fbo);
			glGenFramebuffers(2, _fbo_ping_pong);

			//glUniform1i(glGetUniformLocation(regular_fbo_shader->programId(), "screenTexture"), 0);
			//glUniform1i(glGetUniformLocation(regular_fbo_shader->programId(), "blurTexture"), 1);
		}

		void BloomEffect::resizeBloomColor(int width, int height) {
			// Need To Write 2 Textures to FBO
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

		void BloomEffect::resizeBloomBlur(int width, int height) {
			glDeleteTextures(2, _fbo_texture_ping_pong);

			glGenTextures(2, _fbo_texture_ping_pong);

			for (unsigned int i = 0; i < 2; ++i) {
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo_ping_pong[i]);
				glBindTexture(GL_TEXTURE_2D, _fbo_texture_ping_pong[i]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fbo_texture_ping_pong[i], 0);
				// also check if framebuffers are complete (no need for depth buffer) idk why
				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
					std::cout << "Framebuffer not complete!" << std::endl;
			}

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}
	}
}