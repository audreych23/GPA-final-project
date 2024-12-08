#pragma once

#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <iostream>

#include <Rendering_Framework/src/Rendering/Shader.h>
#include <Rendering_Framework/src/Rendering/ShaderParameterBindingPoint.h>
#include <Rendering_Framework/src/PostProcessing/ScreenQuad.h>

namespace INANOA {
	namespace OPENGL {
		enum class PostProcessingType : int {
			REGULAR_EFFECT = 0,
			BLOOM_EFFECT = 1,
			DEFERRED_SHADING = 2,
			DIRECTIONAL_SHADOW_MAPPING = 3,
			CARTOON_EFFECT = 4,
			VOLUMETRIC_LIGHT = 5,
			POINT_SHADOW = 6
		};		
		
		class PostProcessingBase
		{
		public:
			explicit PostProcessingBase();
			virtual ~PostProcessingBase();

			PostProcessingBase(const PostProcessingBase&) = delete;
			PostProcessingBase(const PostProcessingBase&&) = delete;
			PostProcessingBase& operator=(const PostProcessingBase&) = delete;

		public:
			bool init(const std::string& vsResource, const std::string& fsResource, const std::string& gsResource, const int width, const int height);
			//void resize(const int w, const int h);

			void usePostProcessingShaderProgram();
		public:
			inline void setPostProcessingType(const PostProcessingType type) {
				glUniform1i(SHADER_POST_PARAMETER_BINDING::POST_PROCESSING_ID_LOCATION, static_cast<int>(type));
			}

		private:
			ShaderProgram* m_postProcessShaderProgram = nullptr;
		};
	}
}

