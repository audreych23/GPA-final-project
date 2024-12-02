#include "RegularEffect.h"

namespace INANOA {
	namespace POST_PROCESSING {
		RegularEffect::RegularEffect() {}

		RegularEffect::~RegularEffect() {}

		void RegularEffect::init(ScreenQuad* screen_quad) {
			// setup screen quad
			_screen_quad = screen_quad;
			setupFBO();
		}

		void RegularEffect::render() {
			//if (!_use_postprocessing) return;
			//regular_fbo_shader->useProgram();

			glDisable(GL_DEPTH_TEST);
			// unbind fbo
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

			glClear(GL_DEPTH_BUFFER_BIT);

			// bind texture
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, _fbo_texture);

			_screen_quad->render();
			glEnable(GL_DEPTH_TEST);
		}

		void RegularEffect::resize(int width, int height) {
			// this is called when resize is called (hopefully)
			glDeleteRenderbuffers(1, &_rbo);
			glDeleteTextures(1, &_fbo_texture);

			// Generate RBO 
			glGenRenderbuffers(1, &_rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);

			// Generate Color texture for FBO
			glGenTextures(1, &_fbo_texture);
			glBindTexture(GL_TEXTURE_2D, _fbo_texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fbo_texture, 0);
			glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rbo);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
				printf("Error: Framebuffer is not complete!\n");
			}

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}

		void RegularEffect::bindFBO() {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
		}

		/* private method */
		void RegularEffect::setupFBO() {
			glGenFramebuffers(1, &_fbo);

			// set texture 0 to screenTexture
			// Actually this differs from which 'shader Im planning to use'
			// for first function this is just the screen texture, second function is the original screen texture
			/*glUniform1i(glGetUniformLocation(regular_fbo_shader->programId(), "screenTexture"), 0);*/
		}
	}
}