#include "RenderingOrderExp.h"
#include <GLFW\glfw3.h>

namespace INANOA {
	

	// ===========================================================
	RenderingOrderExp::RenderingOrderExp(){
		this->m_cameraForwardSpeed = 0.01f;
		this->m_cameraForwardMagnitude = glm::vec3(0.0f, 0.0f, 0.0f);
		this->m_cameraRotationSpeed = glm::radians(0.1f);
		this->m_frameWidth = 64;
		this->m_frameHeight = 64;

		_x_offset = 0.0f;
		_y_offset = 0.0f;
	}
	RenderingOrderExp::~RenderingOrderExp(){}

	bool RenderingOrderExp::init(const int w, const int h) {
		INANOA::OPENGL::RendererBase* renderer = new INANOA::OPENGL::RendererBase();
		INANOA::OPENGL::RendererBase* renderer2 = new INANOA::OPENGL::RendererBase();

		const std::string vsFile = "src\\shader\\vertexShader_ogl_450.glsl";
		const std::string fsFile = "src\\shader\\fragmentShader_ogl_450.glsl";
		if (renderer->init(vsFile, fsFile, w, h) == false) {
			return false;
		}

		const std::string vsFileG = "src\\shader\\vertexDepth.glsl";
		const std::string fsFileG = "src\\shader\\fragmentDepth.glsl";
		const std::string gsFileG = "src\\shader\\geometryShader.glsl";
		if (renderer2->init(vsFileG, fsFileG, gsFileG, w, h) == false) {
			return false;
		}

		this->m_geometryRenderer = renderer2;
		this->m_renderer = renderer;

		// camera constructor for the god camera 
		this->m_godCamera = new Camera(glm::vec3(4.0f, 1.0f, -1.5f), glm::vec3(3.0f, 3.0f, -1.5f), 
			glm::vec3(0.0f, 1.0f, 0.0f), 5.0f, 45.0f, 0.1f, 100.0f);
		// update camera projection matrix (call glm::perspecctive)
		this->m_godCamera->resize(w, h);

		// set view matrix 
		this->m_godCamera->setViewOrg(glm::vec3(4.0f, 1.0f, -1.5f));
		// set target
		this->m_godCamera->setLookCenter(glm::vec3(3.0f, 1.0f, -1.5f));

		this->m_godCamera->setDistance(1.0f);
		this->m_godCamera->update();

		m_renderer->setCamera(
			this->m_godCamera->projMatrix(),
			this->m_godCamera->viewMatrix(),
			this->m_godCamera->viewOrig()
		);

		// initialize model
		{		
			this->indoor = new MODEL::Indoor();
			this->indoor->init(glm::mat4(1.0));
			// get model matrix of indoor 
			glm::mat4 base_model_mat = this->indoor->getModelMat();

			this->trice = new MODEL::Trice();
			this->trice->init(base_model_mat);

			this->light_sphere = new MODEL::LightSphere();
			this->light_sphere->init(base_model_mat);

			this->sun_sphere = new MODEL::LightSphere();
			this->sun_sphere->init(base_model_mat);

			this->area_light = new MODEL::AreaLightModel();
			this->area_light->init(glm::vec3(0.8f, 0.6f, 0.0f), base_model_mat);

			/*this->light_sphere_sun = new MODEL::LightSphere();
			this->light_sphere_sun->init(base_model_mat);*/
		}

		// initialize screen quad
		{
			this->_screen_quad = new ScreenQuad();
			_screen_quad->init();
		}


		OPENGL::PostProcessingBase* post_processing = new OPENGL::PostProcessingBase();
		const std::string vsPostFile = "src\\shader\\vertexShader_ogl_450_fbo.glsl";
		const std::string fsPostFile = "src\\shader\\fragmentShader_ogl_450_fbo.glsl";

		if (post_processing->init(vsPostFile, fsPostFile, w, h) == false) {
			return false;
		}

		this->_post_processing = post_processing;
		
		// initialize post processing effect
		{
			this->_regular_effect = new POST_PROCESSING::RegularEffect();
			this->_regular_effect->init(_screen_quad);

			this->_bloom_effect = new POST_PROCESSING::BloomEffect();
			this->_bloom_effect->init(_screen_quad);

			this->_deferred_shading = new POST_PROCESSING::DeferredShading();
			this->_deferred_shading->init(_screen_quad);

			this->_dir_shadow_mapping = new POST_PROCESSING::DirectionalShadowMapping();
			this->_dir_shadow_mapping->init(_screen_quad);

			this->_volumetric_light = new POST_PROCESSING::VolumetricLights();
			this->_volumetric_light->init(_screen_quad);

			this->pointLightShadow = new POST_PROCESSING::PointLightShadow();
			this->pointLightShadow->init();

			glm::mat4 base_model_mat = this->indoor->getModelMat();
		}


		this->resize(w, h);		

		int maxUniformLocations;
		glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &maxUniformLocations);
		printf("MAX UNIFORM = %d\n", maxUniformLocations);
		return true;
	}
	void RenderingOrderExp::resize(const int w, const int h) {
		this->m_godCamera->resize(w, h);
		m_renderer->resize(w, h);
		this->m_frameWidth = w;
		this->m_frameHeight = h;

		// Postprocessing
		this->_regular_effect->resize(w, h);
		this->_bloom_effect->resize(w, h);
		this->_deferred_shading->resize(w, h);
		this->_dir_shadow_mapping->resize(w, h);
		this->_volumetric_light->resize(w, h);
	}


	void RenderingOrderExp::update() {		
		// camera update (god)
		if (_key_map[GLFW_KEY_W]) {
			m_godCamera->forward(glm::vec3(0.0f, 0.0f, -m_cameraForwardSpeed), false);
		}
		else if (_key_map[GLFW_KEY_D]) {
			m_godCamera->forward(glm::vec3(m_cameraForwardSpeed, 0.0f, 0.0f), false);
		}
		else if (_key_map[GLFW_KEY_S]) {
			m_godCamera->forward(glm::vec3(0.0f, 0.0f, m_cameraForwardSpeed), false);
		}
		else if (_key_map[GLFW_KEY_A]) {
			m_godCamera->forward(glm::vec3(-m_cameraForwardSpeed, 0.0f, 0.0f), false);
		}
		else if (_key_map[GLFW_KEY_Z]) {
			m_godCamera->forward(glm::vec3(0.0f, m_cameraForwardSpeed, 0.0f), false);
		}
		else if (_key_map[GLFW_KEY_X]) {
			m_godCamera->forward(glm::vec3(0.0f, -m_cameraForwardSpeed, 0.0f), false);
		}
		else if (_x_offset != 0.0f || _y_offset != 0.0f) {
			this->m_godCamera->rotateLookCenterAccordingToViewOrg(glm::radians(_x_offset) * 0.1);
			this->m_godCamera->rotateLookCenterYaw(glm::radians(_y_offset) * 0.1);

			// set back mouse offset to 0 after one camera update
			_x_offset = 0.0;
			_y_offset = 0.0;
		}

		m_godCamera->update();
	}

	void RenderingOrderExp::render() {
		int curOptions = this->_gui.getOptions();
		glm::vec3 blinpengPos = this->_gui.getLightPos();
		glm::vec3 sunPos = this->_gui.getLightDirPos();
		glm::vec3 areaTranslate = this->_gui.getTranslate();
		glm::mat4 areaRotation = this->_gui.getRotation();
		
		glm::vec3 camTranslate = this->_gui.getCamT();
		m_godCamera->translateLookCenterAndViewOrg(camTranslate);

		// =====================================================
		// Point Light Mapping

		this->m_geometryRenderer->useRenderBaseProgram();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		this->m_renderer->clearRenderTarget();

		{
			this->pointLightShadow->bind(blinpengPos);

			this->trice->render();
			this->indoor->render();

			this->pointLightShadow->unbind();
		}


		// =====================================================
		// Basic Pipeline

		glClearColor(0.19f, 0.19f, 0.19f, 1.0f);
		this->m_renderer->useRenderBaseProgram();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		this->m_renderer->clearRenderTarget();

		// =====================================================
		// Lazy Uniform for Basic Pipeline

		glUniform1i(SHADER_PARAMETER_BINDING::POST_PROCESSING, POST_PROCESSING_TYPE::SHADOW_EFFECT);
		glUniform3fv(SHADER_PARAMETER_BINDING::LIGHT_BLOOM_POS, 1, glm::value_ptr(blinpengPos));
		glUniform1i(SHADER_PARAMETER_BINDING::HAS_DIRECTIONAL_LIGHT, _gui.getDirectional());
		glUniform1i(SHADER_PARAMETER_BINDING::HAS_TOON, _gui.getToon());
		glUniform1i(SHADER_PARAMETER_BINDING::HAS_NORMAL, _gui.getNormal());
		glUniform1i(SHADER_PARAMETER_BINDING::HAS_AREA_LIGHT, _gui.getAreaLight());
		glUniform3fv(SHADER_PARAMETER_BINDING::DIRECTIONAL_LIGHT_POS, 1, glm::value_ptr(sunPos));
		glUniform3fv(SHADER_PARAMETER_BINDING::AREA_LIGHT_POS, 1, glm::value_ptr(areaTranslate));
		glUniformMatrix4fv(SHADER_PARAMETER_BINDING::AREA_LIGHT_ROT, 1, GL_FALSE, glm::value_ptr(areaRotation));


		// =====================================================
		// Directional Shadow Mapping

		{
			this->_dir_shadow_mapping->bindFBO();
			this->_dir_shadow_mapping->renderLightSpace(8.0f, sunPos);
			this->pointLightShadow->attachTexture();

			/* Render Object */
			this->m_renderer->setShadingModel(OPENGL::ShadingModelType::AREA_LIGHT);
			this->area_light->render(areaTranslate, areaRotation);

			this->m_renderer->setShadingModel(OPENGL::ShadingModelType::TRICE_MODEL);
			this->trice->render();

			this->m_renderer->setShadingModel(OPENGL::ShadingModelType::INDOOR_MODEL);
			this->indoor->render();

			this->m_renderer->setShadingModel(OPENGL::ShadingModelType::LIGHT_SPHERE);
			this->light_sphere->render(blinpengPos);

			this->m_renderer->setShadingModel(OPENGL::ShadingModelType::SUN_SPHERE);
			this->sun_sphere->render(sunPos * glm::vec3(5.0, 2.5, 5.0));

			this->_dir_shadow_mapping->unbindFBO();

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		// =====================================================
		// View
		this->m_renderer->setCamera(
			m_godCamera->projMatrix(),
			m_godCamera->viewMatrix(),
			m_godCamera->viewOrig()
		);

		this->m_renderer->setViewport(0, 0, this->m_frameWidth, this->m_frameHeight);

		// =====================================================
		// Render Scene
		{
			// Directional Shadow Mapping
			{
				this->_bloom_effect->bindFBO();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				this->_dir_shadow_mapping->renderShadow();
			}

			if(this->_gui.getDeferred()) glUniform1i(SHADER_PARAMETER_BINDING::POST_PROCESSING, POST_PROCESSING_TYPE::DEFERRED_EFFECT);
			else glUniform1i(SHADER_PARAMETER_BINDING::POST_PROCESSING, POST_PROCESSING_TYPE::BLOOM_EFFECT);


			this->m_renderer->setShadingModel(OPENGL::ShadingModelType::AREA_LIGHT);
			this->area_light->render(areaTranslate, areaRotation);

			this->m_renderer->setShadingModel(OPENGL::ShadingModelType::TRICE_MODEL);
			this->trice->render();

			this->m_renderer->setShadingModel(OPENGL::ShadingModelType::INDOOR_MODEL);
			this->indoor->render();

			this->m_renderer->setShadingModel(OPENGL::ShadingModelType::LIGHT_SPHERE);
			this->light_sphere->render(blinpengPos);

			this->m_renderer->setShadingModel(OPENGL::ShadingModelType::SUN_SPHERE);
			this->sun_sphere->render(sunPos * glm::vec3(5.0, 2.5, 5.0));

			// Directional Shadow Mapping
			{
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
				this->_dir_shadow_mapping->finishRender();
			}
		}

		// =====================================================
		// Render the Post Processing
		this->_post_processing->usePostProcessingShaderProgram();
		std::vector<float> viewport({ 0, 0, (float)m_frameWidth, (float)m_frameHeight });
		this->_volumetric_light->calculateInNDC(m_godCamera->viewMatrix(), m_godCamera->projMatrix(), viewport, sunPos * glm::vec3(5.0, 2.5, 5.0));
		this->_post_processing->setPostProcessingType(OPENGL::PostProcessingType::BLOOM_EFFECT);

		// =====================================================
		// Uniform of the Post Processing
		glUniform1i(SHADER_PARAMETER_BINDING::HAS_DIRECTIONAL_LIGHT, _gui.getDirectional());
		glUniform1i(SHADER_PARAMETER_BINDING::HAS_TOON, _gui.getToon());
		glUniform1i(SHADER_PARAMETER_BINDING::HAS_FXAA, _gui.getFXAA());

		if (this->_gui.getDeferred()) {
			glUniformMatrix4fv(69, 1, GL_FALSE, glm::value_ptr(m_godCamera->projMatrix()));
			this->_post_processing->setPostProcessingType(OPENGL::PostProcessingType::DEFERRED_SHADING);
			this->_bloom_effect->renderDeferred(_gui.getDeferredOption());
		}
		else if (this->_gui.getToon()) {
			this->_bloom_effect->renderToon();
		}
		else {
			this->_bloom_effect->render();
		}

		// =====================================================
		// GUI
		this->_gui.setLookAt(m_godCamera->lookCenter());
		this->_gui.setViewOrg(m_godCamera->viewOrig());
		this->_gui.render();
		glm::vec3 new_look_at = _gui.getLookAt();
		//glm::vec3 new_view_orig = _gui.getLookAt();
		this->m_godCamera->setLookCenter(new_look_at);
		//this->m_godCamera->setViewOrg(new_view_orig);
	}
}