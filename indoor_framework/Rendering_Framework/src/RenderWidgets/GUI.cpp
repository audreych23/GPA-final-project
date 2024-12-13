#include "GUI.h"
#include <iostream>
namespace INANOA {
	GUI::GUI() {
		_deferredOption = 3;
		_options = 2;

		_light_pos[0] = 1.87659;
		_light_pos[1] = 0.4625;
		_light_pos[2] = 0.103928;

		_light_dir[0] = -2.845;
		_light_dir[1] = 2.028;
		_light_dir[2] = -1.293;

		translatePos[0] = 1.0f;
		translatePos[1] = 0.5f;
		translatePos[2] = -0.5f;

		_view_org[0] = 4.0f;
		_view_org[1] = 1.0f;
		_view_org[2] = -1.5f;

		_look_at[0] = 4.0f;
		_look_at[1] = 1.0f;
		_look_at[2] = -1.5f;

		prev = glm::vec3(_view_org[0], _view_org[1], _view_org[2]);
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
		ImGui::Checkbox("Area Light Enable", &areaLightEnable);
		ImGui::Checkbox("Point Light Shadow", &hasPointShadow);
		ImGui::Checkbox("SSR", &screenSpace);

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
			ImGui::PopStyleVar(2); // Pop
		}
	
		/* Input Float of Look At */

		if (ImGui::CollapsingHeader("Camera Position", ImGuiTreeNodeFlags_None)) {
			ImGui::InputFloat("LX", &_look_at[0], 0.1f, 1.0f, "%.3f");
			ImGui::InputFloat("LY", &_look_at[1], 0.1f, 1.0f, "%.3f");
			ImGui::InputFloat("LZ", &_look_at[2], 0.1f, 1.0f, "%.3f");
			ImGui::Separator();
			ImGui::InputFloat("VX", &_view_org[0], 0.1f, 1.0f, "%.3f");
			ImGui::InputFloat("VY", &_view_org[1], 0.1f, 1.0f, "%.3f");
			ImGui::InputFloat("VZ", &_view_org[2], 0.1f, 1.0f, "%.3f");
		}

		if (ImGui::CollapsingHeader("Area Position", ImGuiTreeNodeFlags_None)) {
			ImGui::InputFloat("AX", &translatePos[0], 0.1f, 1.0f, "%.3f");
			ImGui::InputFloat("AY", &translatePos[1], 0.1f, 1.0f, "%.3f");
			ImGui::InputFloat("AZ", &translatePos[2], 0.1f, 1.0f, "%.3f");
			ImGui::InputFloat("Rotate", &areaRotation, 1.0f, 10.0f, "%.3f");
		}

		if (ImGui::CollapsingHeader("Directional Light", ImGuiTreeNodeFlags_None)) {
			ImGui::InputFloat("DX", &_light_dir[0], 0.1f, 1.0f, "%.3f");
			ImGui::InputFloat("DY", &_light_dir[1], 0.1f, 1.0f, "%.3f");
			ImGui::InputFloat("DZ", &_light_dir[2], 0.1f, 1.0f, "%.3f");
		}

		/* Input Float of Light Position */
		if (ImGui::CollapsingHeader("Light Bloom Position", ImGuiTreeNodeFlags_None)) {
			ImGui::InputFloat("X", &_light_pos[0], 0.1f, 1.0f, "%.3f");
			ImGui::InputFloat("Y", &_light_pos[1], 0.1f, 1.0f, "%.3f");
			ImGui::InputFloat("Z", &_light_pos[2], 0.1f, 1.0f, "%.3f");
		}

        /* End */
		ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

//ImGui::SliderInt("Split", &slider, 0, 800);