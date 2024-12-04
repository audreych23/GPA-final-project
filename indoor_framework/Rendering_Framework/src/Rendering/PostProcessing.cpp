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

		this->regular_fbo_shader->useProgram();

		_use_postprocessing = true;
		
		// make sure this method is called before resize
		setupQuadWindow();
		setupFBO();

		return true;
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

		glVertexAttribPointer(SHADER_POST_PARAMETER_BINDING::VERTEX_LOCATION, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), 0);
		glVertexAttribPointer(SHADER_POST_PARAMETER_BINDING::TEX_COORDS_LOCATION, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (const GLvoid*)(sizeof(GL_FLOAT) * 2));

		glEnableVertexAttribArray(SHADER_POST_PARAMETER_BINDING::VERTEX_LOCATION);
		glEnableVertexAttribArray(SHADER_POST_PARAMETER_BINDING::TEX_COORDS_LOCATION);
	}

	void PostProcessing::setupFBO() {
		glGenFramebuffers(1, &_fbo_regular);

		// set texture 0 to screenTexture
		// Actually this differs from which 'shader Im planning to use'
		// for first function this is just the screen texture, second function is the original screen texture
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
		regular_fbo_shader->useProgram();

		glDisable(GL_DEPTH_TEST);
		// unbind fbo
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glClear(GL_DEPTH_BUFFER_BIT);
		//regular_fbo_shader->useProgram();
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

	bool PostProcessing::initBloom() {
		regular_fbo_shader = OPENGL::ShaderProgram::createShaderProgram(
			"src\\shader\\vertexShader_ogl_450_fbo.glsl",
			"src\\shader\\fragmentShader_ogl_450_fbo.glsl");

		if (this->regular_fbo_shader == nullptr) {
			return false;
		}
		this->regular_fbo_shader->useProgram();
		_use_postprocessing = true;

		// make sure this method is called before resize
		setupQuadWindow();
		setupBloomFBO();
	}

	// every post process effect has to use the same program, which should be enabled evryime
	void PostProcessing::useBloom() {
		if (!_use_postprocessing) return;
		glDisable(GL_DEPTH_TEST);
		// unbind fbo
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		// should it??
		glClear(GL_DEPTH_BUFFER_BIT);
		// use shader for blur
		// set blur function here
		regular_fbo_shader->useProgram();
		setBloomSubProcess(BloomSubProcess::BLUR_EFFECT);

		bool horizontal = true;
		bool first_iteration = true;
		int blur_amount = 10;
		for (unsigned int i = 0; i < blur_amount; ++i) {
			glBindFramebuffer(GL_FRAMEBUFFER, _fbo_ping_pong[horizontal]);
			glUniform1i(SHADER_POST_PARAMETER_BINDING::HORIZONTAL_FLAG_LOCATION, (int)horizontal);
			glBindTexture(GL_TEXTURE_2D, first_iteration ? _fbo_texture_bloom[1] : _fbo_texture_ping_pong[!horizontal]);

			// render Quad 
			glBindVertexArray(_rect_vao);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			horizontal = !horizontal;
			if (first_iteration) first_iteration = false;
		}

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		// not sure if can use api
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// set differetn shader
		regular_fbo_shader->useProgram();
		setBloomSubProcess(BloomSubProcess::FINAL_EFFECT);
		//setPostProcessingType();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _fbo_texture_bloom[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, _fbo_texture_ping_pong [!horizontal]);
		//glUniform1i(SHADER_PARAMETER_BINDING::BLOOM_FLAG_LOCATION, (int)bloom);
		//glUniform1f(SHADER_PAREMTER_BINDING::EXPOSURE_LOCATION, exposure);
		
		glBindVertexArray(_rect_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glEnable(GL_DEPTH_TEST);
	}
	
	void PostProcessing::resizeBloomColor(int width, int height) {
		// Need To Write 2 Textures to FBO
		glDeleteRenderbuffers(1, &_rbo_bloom);
		// not sure if im deleting it correcrly
		glDeleteTextures(2, _fbo_texture_bloom);
		//glDeleteTextures(1, &_fbo_texture_bloom[1]);

		// bind framebuffer
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo_bloom);
		// Generate RBO
		glGenRenderbuffers(1, &_rbo_bloom);
		glBindRenderbuffer(GL_RENDERBUFFER, _rbo_bloom);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height);

		glGenTextures(2, _fbo_texture_bloom);
		// Generate Color Texture For fbo
		for (unsigned int i = 0; i < 2; i++)
		{
			glBindTexture(GL_TEXTURE_2D, _fbo_texture_bloom[i]);
			glTexImage2D(
				GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL
			);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			// attach texture to framebuffer
			glFramebufferTexture2D(
				GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, _fbo_texture_bloom[i], 0
			);
		}

		// explicitly tell opengl we're rendering to multiple color buffers
		unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			printf("Error: Framebuffer is not complete!\n");
		}

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);		
	}

	void PostProcessing::resizeBloomBlur(int width, int height) {
		glDeleteTextures(2, _fbo_texture_ping_pong);

		glGenTextures(2, _fbo_texture_ping_pong);

		for (unsigned int i = 0; i < 2; ++i) {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo_ping_pong[i]);
			glBindTexture(GL_TEXTURE_2D, _fbo_texture_ping_pong[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _fbo_texture_ping_pong[i], 0);
			// also check if framebuffers are complete (no need for depth buffer) idk why
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << "Framebuffer not complete!" << std::endl;
		}
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	}

	void PostProcessing::setupBloomFBO() {
		glGenFramebuffers(1, &_fbo_bloom);
		glGenFramebuffers(2, _fbo_ping_pong);

		glUniform1i(glGetUniformLocation(regular_fbo_shader->programId(), "screenTexture"), 0);
		glUniform1i(glGetUniformLocation(regular_fbo_shader->programId(), "blurTexture"), 1);
	}

	void PostProcessing::bindBloomFBO() {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _fbo_bloom);
	}
}