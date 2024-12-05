#pragma once

#include <Rendering_Framework/src/Rendering/Shader.h>
#include <Rendering_Framework/src/Rendering/ShaderParameterBindingPoint.h>

namespace INANOA {
	enum class PostProcessingType : int {
		REGULAR_EFFECT = 0,
		BLOOM_EFFECT = 1,
		DEFERRED_EFFECT = 2,
		SHADOW_MAPPING = 3,
		TOON_EFFECT = 4
	};

	class PostProcessing
	{
	public:
		enum class BloomSubProcess : int {
			BLUR_EFFECT = 0,
			FINAL_EFFECT = 1,
		};
		PostProcessing();

		~PostProcessing();

		bool init();

		void render();

		void resize(int width, int height);

		void usePostProcessing(bool use) { _use_postprocessing = use; }
		void bindFBO();

		// bloom effect : if it looks troublesome, planning to make it into a new class
		bool initBloom();

		void useBloom();

		void resizeBloomColor(int width, int height);
		void resizeBloomBlur(int width, int height);
		
		void bindBloomFBO();

		inline void setPostProcessingType(const PostProcessingType type) {
			glUniform1i(SHADER_POST_PARAMETER_BINDING::POST_PROCESSING_ID_LOCATION, static_cast<int>(type));
		}

		inline void setBloomSubProcess(const BloomSubProcess type) {
			glUniform1i(SHADER_POST_PARAMETER_BINDING::POST_PROCESSING_SUB_ID_LOCATION, static_cast<int>(type));
		}
	private:
		void setupQuadWindow();
		void setupFBO();

		void setupBloomFBO();

		OPENGL::ShaderProgram* regular_fbo_shader = nullptr;

		bool _use_postprocessing;
		GLuint _rect_vao, _rect_vbo, _rect_ebo;

		GLuint _fbo_regular, _rbo_regular, _fbo_texture;

		// bloom color
		GLuint _fbo_bloom;
		GLuint _fbo_texture_bloom[2];
		GLuint _rbo_bloom;

		// bloom gaussian blur
		GLuint _fbo_ping_pong[2];
		GLuint _fbo_texture_ping_pong[2];

	};

}