#include "DeferredShading.h"
#include <random>

namespace INANOA {
	namespace POST_PROCESSING {
		// Constructor & Destructor
		DeferredShading::DeferredShading() {}
		DeferredShading::~DeferredShading() {}

		// Init
		void DeferredShading::init(ScreenQuad* screen_quad) {
			_screen_quad = screen_quad;
			setupFBO();
			setupSSAO();
		}

		void DeferredShading::render(DeferredShadingOption option) {
			setDeferredShadingOption(option);
			glDisable(GL_DEPTH_TEST);
			// Unbind FBO
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			// Clear Depth
			glClear(GL_DEPTH_BUFFER_BIT);
			for (int i = 0; i < 5; i++) {
				glActiveTexture(GL_TEXTURE0 + i);
				glBindTexture(GL_TEXTURE_2D, _fbo_texture[i]);
			}
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, noiseTexture);
			glUniform3fv(70, ssaoKernel.size(), glm::value_ptr(ssaoKernel[0]));

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
			unsigned int attachments[6] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };
			glDrawBuffers(6, attachments);

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		}

		void DeferredShading::bindFBO() {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo);
		}

		void DeferredShading::setupFBO() {
			glGenFramebuffers(1, &_fbo);
		}

		static float lerp(float a, float b, float f)
		{
			return a + f * (b - a);
		}

		void DeferredShading::setupSSAO() {
			/* KERNELLL */
			std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
			std::default_random_engine generator;
			std::vector<glm::vec3> ssaoKernel;
			for (unsigned int i = 0; i < 64; ++i)
			{
				glm::vec3 sample(
					randomFloats(generator) * 2.0 - 1.0,
					randomFloats(generator) * 2.0 - 1.0,
					randomFloats(generator)
				);
				sample = glm::normalize(sample);
				sample *= randomFloats(generator);

				//sample = glm::vec3(1.0, 1.0, 1.0);
				ssaoKernel.push_back(sample);

				std::cout << sample[0] << " " << sample[1] << " " << sample[2] << '\n';
			}
			/* NOISE SSAO */
			std::vector<glm::vec3> ssaoNoise;
			for (unsigned int i = 0; i < 16; i++)
			{
				glm::vec3 noise(
					randomFloats(generator),// * 2.0 - 1.0,
					randomFloats(generator),// * 2.0 - 1.0,
					0.0f);
				ssaoNoise.push_back(noise);
			}
			glGenTextures(1, &noiseTexture);
			glBindTexture(GL_TEXTURE_2D, noiseTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			this->ssaoKernel = ssaoKernel;
			this->noiseTexture = noiseTexture;
		}
	}
}
