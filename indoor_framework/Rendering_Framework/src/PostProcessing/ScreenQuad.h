#pragma once

#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <iostream>

#include <Rendering_Framework/src/Rendering/ShaderParameterBindingPoint.h>
#include <Rendering_Framework/src/Rendering/Shader.h>

namespace INANOA {
	class ScreenQuad
	{
	public:
		ScreenQuad();
		~ScreenQuad();

		void init();
		void setupQuad();

		void render();
	private:
		GLuint _rect_vao, _rect_vbo, _rect_ebo;
	};
}

