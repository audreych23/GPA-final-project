#include "MyImGuiPanel.h"



MyImGuiPanel::MyImGuiPanel()
{
	this->m_avgFPS = 0.0;
	this->m_avgFrameTime = 0.0;
}


MyImGuiPanel::~MyImGuiPanel()
{
}

void MyImGuiPanel::update() {
	// performance information
	const std::string FPS_STR = "FPS: " + std::to_string(this->m_avgFPS);
	ImGui::TextColored(ImVec4(0, 220, 0, 255), FPS_STR.c_str());
	const std::string FT_STR = "Frame: " + std::to_string(this->m_avgFrameTime);
	ImGui::TextColored(ImVec4(0, 220, 0, 255), FT_STR.c_str());
	
	ImGui::Checkbox("Normal Map", &isNormalMap);

	this->isTeleport0 = ImGui::Button("Teleport 0");
	this->isTeleport1 = ImGui::Button("Teleport 1");
	this->isTeleport2 = ImGui::Button("Teleport 2");

	ImGui::Combo("Render Mode", &this->renderMode, 
		"Default\0World Space Vertex\0World Space Normal\0Ambient\0Diffuse\0Specular\0"
	);



}

void MyImGuiPanel::setAvgFPS(const double avgFPS){
	this->m_avgFPS = avgFPS;
}
void MyImGuiPanel::setAvgFrameTime(const double avgFrameTime){
	this->m_avgFrameTime = avgFrameTime;
}