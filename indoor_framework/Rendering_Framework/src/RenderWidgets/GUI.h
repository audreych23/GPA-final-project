#pragma once

#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw.h>
#include <imgui\imgui_impl_opengl3.h>

#include <vector>

namespace INANOA {
	class GUI
	{
	public:
		GUI();
		~GUI();

		void render();

	private:
		bool _options[10] = {};
		float _look_at[3] = {};
	};
}
