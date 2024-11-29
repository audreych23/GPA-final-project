#include "GUI.h"
#include <iostream>
namespace INANOA {
	GUI::GUI() {}

	GUI::~GUI() {}

	void GUI::render() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	    ImGui::SetNextWindowPos(ImVec2(10, 10));
		ImGui::SetNextWindowSize(ImVec2(220, 220));


        ImGui::Begin("Settings");

		ImGui::Checkbox("normal", _options);
		ImGui::Checkbox("bloom", _options + 1);
		ImGui::InputFloat("lookat x", _look_at);
		ImGui::InputFloat("lookat y", _look_at + 1);
		ImGui::InputFloat("lookat z", _look_at + 2);
		
        //ImGui::SliderInt("Split", &slider, 0, 800);

        ImGui::End();

        /* End */
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}


}