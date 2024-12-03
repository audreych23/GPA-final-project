#include "GUI.h"
#include <iostream>
namespace INANOA {
	GUI::GUI() { _deferredOption = 0; }

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
		ImGui::Checkbox("Normal", _options);
		ImGui::Checkbox("Bloom", _options + 1);

		/* Input for Deferred Option */
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

		/* Input Float of Look At */
		ImGui::Text("Look At Position");
		ImGui::Separator();
		ImGui::InputFloat("Look X", &_look_at[0]);
		ImGui::InputFloat("Look Y", &_look_at[1]);
		ImGui::InputFloat("Look Z", &_look_at[2]);

		/* Input Float of Light Position */
		ImGui::Text("Light Position");
		ImGui::Separator();
		ImGui::InputFloat("Light X", &_light_pos[0]);
		ImGui::InputFloat("Light Y", &_light_pos[1]);
		ImGui::InputFloat("Light Z", &_light_pos[2]);

        /* End */
		ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}

//ImGui::SliderInt("Split", &slider, 0, 800);