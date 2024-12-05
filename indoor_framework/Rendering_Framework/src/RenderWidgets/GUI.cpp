#include "GUI.h"
#include <iostream>
namespace INANOA {
	GUI::GUI() { 
		_deferredOption = 3; 
		_options = 2; 
		
		_light_pos[0] = 1.87659;
		_light_pos[1] = 0.4625;
		_light_pos[2] = 0.103928;
	}

	GUI::~GUI() {}

	void GUI::render() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	    ImGui::SetNextWindowPos(ImVec2(10, 10));
		ImGui::SetNextWindowSize(ImVec2(220, 500));
        ImGui::Begin("Settings");

		/* CheckBox */
		ImGui::Text("Options");
		ImGui::Separator();
		if (ImGui::RadioButton("Bloom",		_options == 1)) _options = 1;
		if (ImGui::RadioButton("Deffered",	_options == 2)) _options = 2;
		if (ImGui::RadioButton("Shadow",	_options == 3)) _options = 3;
		if (ImGui::RadioButton("Cartoon",	_options == 4)) _options = 4;

		/* Input for Deferred Option */
		if (_options == 2) {
			ImGui::Text("Deferred Option");
			ImGui::Separator();
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4)); // Push Style
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2)); // Push Style
			if (ImGui::RadioButton("Option 1", _deferredOption == 0)) _deferredOption = 0;
			if (ImGui::RadioButton("Option 2", _deferredOption == 1)) _deferredOption = 1;
			if (ImGui::RadioButton("Option 3", _deferredOption == 2)) _deferredOption = 2;
			if (ImGui::RadioButton("Option 4", _deferredOption == 3)) _deferredOption = 3;
			if (ImGui::RadioButton("Option 5", _deferredOption == 4)) _deferredOption = 4;
			ImGui::PopStyleVar(2); // Pop
		}
	
		/* Input Float of Look At */
		ImGui::Text("Look At Position");
		ImGui::Separator();
		ImGui::InputFloat("LX", &_look_at[0], 0.1f, 1.0f, "%.3f");
		ImGui::InputFloat("LY", &_look_at[1], 0.1f, 1.0f, "%.3f");
		ImGui::InputFloat("LZ", &_look_at[2], 0.1f, 1.0f, "%.3f");

		/* Input Float of Light Position */
		ImGui::Text("Light Position");
		ImGui::Separator();
		ImGui::InputFloat("X", &_light_pos[0], 0.1f, 1.0f, "%.3f");
		ImGui::InputFloat("Y", &_light_pos[1], 0.1f, 1.0f, "%.3f");
		ImGui::InputFloat("Z", &_light_pos[2], 0.1f, 1.0f, "%.3f");

        /* End */
		ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

//ImGui::SliderInt("Split", &slider, 0, 800);