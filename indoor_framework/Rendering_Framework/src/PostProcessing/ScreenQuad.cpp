#include "ScreenQuad.h"

namespace INANOA {
	ScreenQuad::ScreenQuad() {}
	ScreenQuad::~ScreenQuad() {}

	void ScreenQuad::init() {
		setupQuad();
	}

	void ScreenQuad::setupQuad() {
		const float rect_vertices[] =
		{
			1.0f , -1.0f, 1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f, 0.0f,
			-1.0f, 1.0f, 0.0f, 1.0f,
			1.0f , 1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f, 0.0f,
			-1.0f , 1.0f, 0.0f, 1.0f
		};

		glGenVertexArrays(1, &_rect_vao);
		glBindVertexArray(_rect_vao);

		glGenBuffers(1, &_rect_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _rect_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, rect_vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(SHADER_POST_PARAMETER_BINDING::VERTEX_LOCATION, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), 0);
		glVertexAttribPointer(SHADER_POST_PARAMETER_BINDING::TEX_COORDS_LOCATION, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (const GLvoid*)(sizeof(GL_FLOAT) * 2));

		glEnableVertexAttribArray(SHADER_POST_PARAMETER_BINDING::VERTEX_LOCATION);
		glEnableVertexAttribArray(SHADER_POST_PARAMETER_BINDING::TEX_COORDS_LOCATION);
	}

	void ScreenQuad::render() {
		glBindVertexArray(_rect_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

}