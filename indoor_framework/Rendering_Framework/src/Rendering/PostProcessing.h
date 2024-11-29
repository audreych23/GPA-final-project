#pragma once

#include "Shader.h"
#include <Rendering_Framework/src/Rendering/ShaderParameterBindingPoint.h>

namespace INANOA {

	class PostProcessing
	{
	public:
		PostProcessing();

		~PostProcessing();

		bool init();

		void render();

		void setupQuadWindow();

		void setupFBO();

		void resize(int width, int height);

		void usePostProcessing(bool use) { _use_postprocessing = use; }

		void bindFBO();
		void unbindFBO();
	private:
		OPENGL::ShaderProgram* regular_fbo_shader = nullptr;

		bool _use_postprocessing;
		GLuint _rect_vao, _rect_vbo, _rect_ebo;

		GLuint _fbo_regular, _rbo_regular, _fbo_texture;
	};

}