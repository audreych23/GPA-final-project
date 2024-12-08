#include "PointLightShadow.h"

namespace INANOA {
	namespace POST_PROCESSING {
		const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

		void PointLightShadow::init() {
			glGenFramebuffers(1, &depthMapFBO);
			/* Depth Texture */
			glGenTextures(1, &depthCubemap);
			glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
			for (unsigned int i = 0; i < 6; ++i) {
				glTexImage2D(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
					GL_DEPTH_COMPONENT,
					SHADOW_WIDTH, SHADOW_HEIGHT, 0,
					GL_DEPTH_COMPONENT, GL_FLOAT, NULL
				);
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			
			/* GEN FBO */
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void PointLightShadow::bind(glm::vec3 light_pos) {
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);

			setPos(light_pos);

			glUniform3fv(99, 1, glm::value_ptr(light_pos));
			for(int i=0; i<6; i++)
				glUniformMatrix4fv(100 + i, 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));

		}

		void PointLightShadow::unbind() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void PointLightShadow::attachTexture() {
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void PointLightShadow::setPos(glm::vec3 lightPos) {
			glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, 0.22f, 10.0f);
			shadowTransforms.clear();
			shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
			shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
			shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
			shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
			shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
			shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		}
	}
}

