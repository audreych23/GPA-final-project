#pragma once

#include <vector>
#include <map>

#include <Rendering_Framework/src/Rendering/RendererBase.h>
#include <Rendering_Framework/src/Scene/Model.h>
#include <Rendering_Framework/src/Rendering/Camera/Camera.h>
#include <Rendering_Framework/src/Scene/Indoor.h>
#include <Rendering_Framework/src/Scene/Trice.h>
#include <Rendering_Framework/src/Scene/LightSphere.h>
#include <Rendering_Framework/src/Rendering/PostProcessingBase.h>
#include <Rendering_Framework/src/RenderWidgets/GUI.h>

#include <Rendering_Framework/src/PostProcessing/RegularEffect.h>
#include <Rendering_Framework/src/PostProcessing/BloomEffect.h>
#include <Rendering_Framework/src/PostProcessing/DeferredShading.h>
#include <Rendering_Framework/src/PostProcessing/DirectionalShadowMapping.h>
#include <Rendering_Framework/src/PostProcessing/VolumetricLights.h>
//#include "Shader.h"

namespace INANOA {
	class RenderingOrderExp
	{
	public:
		RenderingOrderExp();
		virtual ~RenderingOrderExp();

	public:
		bool init(const int w, const int h);
		void resize(const int w, const int h);
		void update();
		void render();

		void setKeyMap(std::map<int, int>& key_map) { _key_map = key_map; }

		void setMouseOffset(float x, float y) { _x_offset = x; _y_offset = y; }
	private:
		// camera
		Camera* m_playerCamera = nullptr;
		Camera* m_godCamera = nullptr;

		glm::vec3 m_cameraForwardMagnitude;
		float m_cameraForwardSpeed;
		float m_cameraRotationSpeed;

		// renderer
		OPENGL::RendererBase* m_renderer = nullptr;

		// model
		MODEL::Indoor* indoor = nullptr;
		MODEL::Trice* trice = nullptr;
		MODEL::LightSphere* light_sphere = nullptr;
		MODEL::LightSphere* sun_sphere = nullptr;
		// screen quad for post processing
		ScreenQuad* _screen_quad;

		// post processing
		OPENGL::PostProcessingBase* _post_processing = nullptr;

		POST_PROCESSING::RegularEffect* _regular_effect = nullptr;
		POST_PROCESSING::BloomEffect* _bloom_effect = nullptr;
		POST_PROCESSING::DeferredShading* _deferred_shading = nullptr;
		POST_PROCESSING::DirectionalShadowMapping* _dir_shadow_mapping = nullptr;
		POST_PROCESSING::VolumetricLights* _volumetric_light = nullptr;

		// for screen size viewport
		int m_frameWidth;
		int m_frameHeight;

		// keyboard and mouse support
		std::map<int, int> _key_map;
		float _x_offset;
		float _y_offset;

		// gui 
		GUI _gui;
	};

}