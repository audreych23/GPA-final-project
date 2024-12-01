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
		const std::string vsFile = "src\\shader\\vertexShader_ogl_450.glsl";
		const std::string fsFile = "src\\shader\\fragmentShader_ogl_450.glsl";
		if (renderer->init(vsFile, fsFile, w, h) == false) {
			return false;
		}

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

			this->_post_processing = new PostProcessing();
			this->_post_processing->init();
		}


		this->resize(w, h);		
		return true;
	}
	void RenderingOrderExp::resize(const int w, const int h) {
		this->m_godCamera->resize(w, h);
		m_renderer->resize(w, h);
		this->m_frameWidth = w;
		this->m_frameHeight = h;

		this->_post_processing->resize(w, h);
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
		// for post processing effect
		this->_post_processing->bindFBO();

		this->m_renderer->useRenderBaseProgram();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		this->m_renderer->clearRenderTarget();
		const int HW = this->m_frameWidth * 0.5;


		// =====================================================
		// god view
		this->m_renderer->setCamera(
			m_godCamera->projMatrix(),
			m_godCamera->viewMatrix(),
			m_godCamera->viewOrig()
		);

		this->m_renderer->setViewport(0, 0, this->m_frameWidth, this->m_frameHeight);
		/*this->m_renderer->setShadingModel(OPENGL::ShadingModelType::UNLIT);
		this->m_viewFrustum->render();
		this->m_renderer->setShadingModel(OPENGL::ShadingModelType::PROCEDURAL_GRID);
		this->m_horizontalGround->render();*/
		//// this is what supposed to happen in a higher level
		

		this->m_renderer->setShadingModel(OPENGL::ShadingModelType::INDOOR_MODEL);
		this->indoor->render();

		this->m_renderer->setShadingModel(OPENGL::ShadingModelType::TRICE_MODEL);
		this->trice->render();

		this->m_renderer->setShadingModel(OPENGL::ShadingModelType::LIGHT_SPHERE);
		this->light_sphere->render();

		// post_processing
		this->_post_processing->setPostProcessingType(PostProcessingType::REGULAR_EFFECT);
		this->_post_processing->render();

		// set camera gui 
		this->_gui.setLookAt(m_godCamera->lookCenter());
		this->_gui.render();
		glm::vec3 new_look_at = _gui.getLookAt();
		this->m_godCamera->setLookCenter(new_look_at);
	}
}