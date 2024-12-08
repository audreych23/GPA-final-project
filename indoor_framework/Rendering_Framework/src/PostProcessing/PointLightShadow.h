#pragma once

#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>


#include <Rendering_Framework/src/Rendering/Shader.h>
#include <Rendering_Framework/src/Rendering/ShaderParameterBindingPoint.h>
#include <Rendering_Framework/src/PostProcessing/ScreenQuad.h>

namespace INANOA {
	namespace POST_PROCESSING {
		class PointLightShadow
		{
		public:
			void init();
			void bind(glm::vec3 light_pos);
			void unbind();
			void attachTexture();
			void setPos(glm::vec3 lightPos);
			
		private:
			std::vector<glm::mat4> shadowTransforms;

			unsigned int depthCubemap;
			GLuint depthMapFBO;
		};
	}
}

