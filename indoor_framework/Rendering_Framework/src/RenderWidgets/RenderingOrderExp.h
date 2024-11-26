#pragma once

#include <vector>
#include <map>

#include <Rendering_Framework/src/Rendering/RendererBase.h>
#include <Rendering_Framework/src/Scene/Model.h>
#include <Rendering_Framework/src/Scene/MergedModel.h>
#include <Rendering_Framework/src/Rendering/Camera/Camera.h>
#include <Rendering_Framework/src/Scene/Indoor.h>
#include <Rendering_Framework/src/Scene/Trice.h>
//#include "Shader.h"

namespace INANOA {
	class RenderingOrderExp
	{
	public:
		RenderingOrderExp();
		virtual ~RenderingOrderExp();

	public:
		bool init(const int w, const int h) ;
		void resize(const int w, const int h) ;
		void update() ;
		void render();

		void setKeyMap(std::map<int, int>& key_map) { _key_map = key_map;  }

		void setMouseOffset(float x, float y, bool is_dragged) { _x_offset = x; _y_offset = y; _is_dragged = is_dragged;  }
	private:
		Camera* m_playerCamera = nullptr;
		Camera* m_godCamera = nullptr;

		//MODEL::MergedModel* merged_model = nullptr;
		/*MODEL::Model* indoor = nullptr;
		MODEL::Model* trice = nullptr;*/
		MODEL::Indoor* indoor = nullptr;
		MODEL::Trice* trice = nullptr;

		glm::vec3 m_cameraForwardMagnitude;
		float m_cameraForwardSpeed;
		float m_cameraRotationSpeed;

		int m_frameWidth;
		int m_frameHeight;

		OPENGL::RendererBase* m_renderer = nullptr;

		std::map<int, int> _key_map;
		float _x_offset;
		float _y_offset;
		bool _is_dragged;
	};

}


