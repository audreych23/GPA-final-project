#include "PostProcessing.h"

#include <iostream>
namespace INANOA {
	PostProcessing::PostProcessing() {}

	PostProcessing::~PostProcessing() {}

	bool PostProcessing::init() {
		regular_fbo_shader = OPENGL::ShaderProgram::createShaderProgram(
			"src\\shader\\vertexShader_ogl_450_fbo.glsl", 
			"src\\shader\\fragmentShader_ogl_450_fbo.glsl");

		if (this->regular_fbo_shader == nullptr) {
			return false;
		}

		_use_postprocessing = true;
		
		// make sure this method is called before resize
		setupQuadWindow();
		setupFBO();
	}

	void PostProcessing::setupQuadWindow() {
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

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), 0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (const GLvoid*)(sizeof(GL_FLOAT) * 2));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
	}

	void PostProcessing::setupFBO() {
		glGenFramebuffers(1, &_fbo_regular);

		// set texture 0 to screenTexture
		glUniform1i(glGetUniformLocation(regular_fbo_shader->programId(), "screenTexture"), 0);
	}

	void PostProcessing::resize(int width, int height) {
		// this is called when resize is called (hopefully)
		glDeleteRenderbuffers(1, &_rbo_regular);
		glDeleteTextures(1, &_fbo_texture);

		// Generate RBO 
		glGenRenderbuffers(1, &_rbo_regular);
		glBindRenderbuffer(GL_RENDERBUFFER, _rbo_regular);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);

		// Generate Color texture for FBO
		glGenTextures(1, &_fbo_texture);
		glBindTexture(GL_TEXTURE_2D, _fbo_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo_regular);
		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fbo_texture, 0);
		glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rbo_regular);
		
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			printf("Error: Framebuffer is not complete!\n");
		}

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	void PostProcessing::render() {
		if (!_use_postprocessing) return;
		glDisable(GL_DEPTH_TEST);
		// unbind fbo
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClear(GL_DEPTH_BUFFER_BIT);
		regular_fbo_shader->useProgram();
		glBindVertexArray(_rect_vao);

		// bind texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _fbo_texture);

		// END ANSWER
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glEnable(GL_DEPTH_TEST);
	}

	void PostProcessing::bindFBO() {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo_regular);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
	}

}