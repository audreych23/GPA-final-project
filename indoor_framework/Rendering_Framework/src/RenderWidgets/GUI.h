#pragma once

#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw.h>
#include <imgui\imgui_impl_opengl3.h>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

namespace INANOA {
	class GUI
	{
	public:
		GUI();
		~GUI();

		void render();

		const glm::vec3 getLookAt() const { return glm::vec3(_look_at[0], _look_at[1], _look_at[2]); }
		const glm::vec3 getLightPos() const { return glm::vec3(_light_pos[0], _light_pos[1], _light_pos[2]); }
		const glm::vec3 getViewOrigin() const { return glm::vec3(_view_org[0], _view_org[1], _view_org[2]); }


		const int getDeferredOption() const { return _deferredOption; }
		const int getOptions() const { return _options; }

		const bool getDirectional() const { return directionalEnable; }
		const bool getToon() const { return toonEnable; }
		const bool getDeferred() const { return deferredEnable; }
		const bool getNormal() const { return normalEnable; }
		const bool getFXAA() const { return FXAAEnable; }

		void setLookAt(glm::vec3& look_at) {
			_look_at[0] = look_at.x; 
			_look_at[1] = look_at.y; 
			_look_at[2] = look_at.z;
		}

		void setViewOrg(glm::vec3& view_org) {
			_view_org[0] = view_org.x;
			_view_org[1] = view_org.y;
			_view_org[2] = view_org.z;
		}

	private:
		int _options;
		float _look_at[3] = {};
		float _view_org[3] = {};
		float _light_pos[3] = {};

		int _deferredOption;

		bool directionalEnable;
		bool toonEnable;
		bool deferredEnable;
		bool normalEnable;	
		bool FXAAEnable;
	};
}
