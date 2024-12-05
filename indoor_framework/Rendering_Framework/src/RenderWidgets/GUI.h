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

		const int getDeferredOption() const { return _deferredOption; }
		const int getOptions() const { return _options; }

		void setLookAt(glm::vec3& look_at) {
			_look_at[0] = look_at.x; 
			_look_at[1] = look_at.y; 
			_look_at[2] = look_at.z;
		}

	private:
		int _options;
		float _look_at[3] = {};
		float _light_pos[3] = {};

		int _deferredOption;
	};
}
