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
		ImGui::Checkbox("Directional Light", &directionalEnable);
		ImGui::Checkbox("Toon Enable", &toonEnable);
		ImGui::Checkbox("Use Deferred", &deferredEnable);
		ImGui::Checkbox("Normal Enable", &normalEnable);
		ImGui::Checkbox("FXAA Enable", &FXAAEnable);
		// Input for Deferred Option 
		if (deferredEnable) {
			ImGui::Text("Deferred Option");
			ImGui::Separator();
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4)); // Push Style
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2)); // Push Style
			if (ImGui::RadioButton("Option 1", _deferredOption == 0)) _deferredOption = 0;
			if (ImGui::RadioButton("Option 2", _deferredOption == 1)) _deferredOption = 1;
			if (ImGui::RadioButton("Option 3", _deferredOption == 2)) _deferredOption = 2;
			if (ImGui::RadioButton("Option 4", _deferredOption == 3)) _deferredOption = 3;
			if (ImGui::RadioButton("Option 5", _deferredOption == 4)) _deferredOption = 4;
			if (ImGui::RadioButton("Option 6", _deferredOption == 5)) _deferredOption = 5;
			ImGui::PopStyleVar(2); // Pop
		}
	
		/* Input Float of Look At */
		ImGui::Text("Look At Position");
		ImGui::Separator();
		ImGui::InputFloat("LX", &_look_at[0], 0.1f, 1.0f, "%.3f");
		ImGui::InputFloat("LY", &_look_at[1], 0.1f, 1.0f, "%.3f");
		ImGui::InputFloat("LZ", &_look_at[2], 0.1f, 1.0f, "%.3f");

		ImGui::Text("Orig Position");
		ImGui::Separator();
		ImGui::InputFloat("OX", &_view_org[0], 0.1f, 1.0f, "%.3f");
		ImGui::InputFloat("OY", &_view_org[1], 0.1f, 1.0f, "%.3f");
		ImGui::InputFloat("OZ", &_view_org[2], 0.1f, 1.0f, "%.3f");

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