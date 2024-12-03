#include "DeferredShading.h"

namespace INANOA {
	namespace POST_PROCESSING {
		// Constructor & Destructor
		DeferredShading::DeferredShading() {}
		DeferredShading::~DeferredShading() {}

		// Init
		void DeferredShading::init(ScreenQuad* screen_quad) {
			_screen_quad = screen_quad;
			setupFBO();
		}

		void DeferredShading::render() {
			glDisable(GL_DEPTH_TEST);
			// Unbind FBO
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			// Clear Depth
			glClear(GL_DEPTH_BUFFER_BIT);
			for (int i = 0; i < 5; i++) {
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, _fbo_texture[i]);
			}
			

			// render Quad 
			_screen_quad->render();

			glEnable(GL_DEPTH_TEST);
		}

		void DeferredShading::resize(int width, int height) {
			// 1 RBO
			glDeleteRenderbuffers(1, &_rbo);
			// Delete Texture
			glDeleteTextures(5, _fbo_texture);

			// Bind FBO
			bindFBO();
			// Create RBO
			glGenRenderbuffers(1, &_rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rbo);

			// Generate Texture
			glGenTextures(5, _fbo_texture);
			for (unsigned int i = 0; i < 5; i++)
			{
				glBindTexture(GL_TEXTURE_2D, _fbo_texture[i]);
				glTexImage2D(
					GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL
				);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				// Attach Texture to FrameBuffer
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, _fbo_texture[i], 0);
			}

			// Attachment
			unsigned int attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
			glDrawBuffers(5, attachments);

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		}

		void DeferredShading::bindFBO() {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
		}

		void DeferredShading::setupFBO() {
			glGenFramebuffers(1, &_fbo);
		}
	}
}
