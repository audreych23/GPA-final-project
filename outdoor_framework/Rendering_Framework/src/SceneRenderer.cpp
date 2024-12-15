#include "SceneRenderer.h"


SceneRenderer::SceneRenderer()
{
}


SceneRenderer::~SceneRenderer()
{
}
void SceneRenderer::startNewFrame() {
	this->m_shaderProgram->useProgram();
	this->clear();
}

void SceneRenderer::useCullingCSProgram() {
	// set camera before this, so can bind correctly
	this->m_cullingCsProgram->useProgram();
	glUniform1i(SceneManager::Instance()->m_numTotalInstanceHandle, m_numTotalInstance);
	//glUniform4fv(SHADER_PARAMETER_BINDING::SLIME_POS, 1, &slime_pos[0]);
	// hard coded value 
	for (int i = 0; i < 6; ++i) {
		glm::vec4 dummyVec4;
		dummyVec4.x = m_planes[i].normal.x;
		dummyVec4.y = m_planes[i].normal.y;
		dummyVec4.z = m_planes[i].normal.z;
		dummyVec4.w = m_planes[i].distance;
		if (i == 0) glUniform4fv(SceneManager::Instance()->m_planeEquationHandle1, 1, glm::value_ptr(dummyVec4));
		if (i == 1) glUniform4fv(SceneManager::Instance()->m_planeEquationHandle2, 1, glm::value_ptr(dummyVec4));
		if (i == 2) glUniform4fv(SceneManager::Instance()->m_planeEquationHandle3, 1, glm::value_ptr(dummyVec4));
		if (i == 3) glUniform4fv(SceneManager::Instance()->m_planeEquationHandle4, 1, glm::value_ptr(dummyVec4));
		if (i == 4) glUniform4fv(SceneManager::Instance()->m_planeEquationHandle5, 1, glm::value_ptr(dummyVec4));
		if (i == 5) glUniform4fv(SceneManager::Instance()->m_planeEquationHandle6, 1, glm::value_ptr(dummyVec4));
	}
	
	glDispatchCompute((m_numTotalInstance / 1024) + 1, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void SceneRenderer::useResetCSProgram() {
	this->m_resetCsProgram->useProgram();
	glDispatchCompute(3, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void SceneRenderer::renderPass(){
	SceneManager *manager = SceneManager::Instance();	

	glUniformMatrix4fv(manager->m_projMatHandle, 1, false, glm::value_ptr(this->m_projMat));
	glUniformMatrix4fv(manager->m_viewMatHandle, 1, false, glm::value_ptr(this->m_viewMat));

	if (this->m_terrainSO != nullptr) {
		glUniform1i(SceneManager::Instance()->m_vs_vertexProcessIdHandle, SceneManager::Instance()->m_vs_terrainProcess);
		this->m_terrainSO->update();
	}

	if (this->m_dynamicSOs.size() > 0) {
		glUniform1i(SceneManager::Instance()->m_vs_vertexProcessIdHandle, SceneManager::Instance()->m_vs_commonProcess);
		for (DynamicSceneObject *obj : this->m_dynamicSOs) {
			obj->update();
		}
	}

	if (this->m_dynamicBushesBuildingsSO != nullptr) {
		glUniform1i(SceneManager::Instance()->m_vs_vertexProcessIdHandle, SceneManager::Instance()->m_vs_multiInstanceProcess);
		this->m_dynamicBushesBuildingsSO->update(true);
	}
	
}

// =======================================
void SceneRenderer::resize(const int w, const int h){
	this->m_frameWidth = w;
	this->m_frameHeight = h;
}
bool SceneRenderer::initialize(const int w, const int h, ShaderProgram* shaderProgram){
	this->m_shaderProgram = shaderProgram;

	this->resize(w, h);
	const bool flag = this->setUpShader();
	
	if (!flag) {
		return false;
	}

	// initialize planes frustum definition
	m_planes = std::vector<FrustumPlane>(6);
	
	glEnable(GL_DEPTH_TEST);

	return true;
}
void SceneRenderer::setProjection(const glm::mat4 &proj){
	this->m_projMat = proj;
}
void SceneRenderer::setView(const glm::mat4 &view){
	this->m_viewMat = view;
}
void SceneRenderer::setViewport(const int x, const int y, const int w, const int h) {
	glViewport(x, y, w, h);
}
void SceneRenderer::appendDynamicSceneObject(DynamicSceneObject *obj) {
	this->m_dynamicSOs.push_back(obj);
}
void SceneRenderer::appendTerrainSceneObject(TerrainSceneObject* tSO) {
	this->m_terrainSO = tSO;
}

void SceneRenderer::appendDynamicBushesBuildings(DynamicSceneObject* obj) {
	this->m_dynamicBushesBuildingsSO = obj;
}

void SceneRenderer::setResetComputeShader(ShaderProgram* resetCsProgram) {
	this->m_resetCsProgram = resetCsProgram;
}

void SceneRenderer::setCullingComputeShader(ShaderProgram* cullingCsProgram) {
	this->m_cullingCsProgram = cullingCsProgram;
}

void SceneRenderer::setNumInstance(int numInstance) {
	m_numTotalInstance = numInstance;
}

void SceneRenderer::setFrustumPlaneEquation(glm::mat4& playerView, glm::mat4& playerProj) {
	glm::mat4 viewProj = playerProj * playerView;

	// Extract planes from view-projection matrix
	// Left plane
	m_planes[0].normal.x = viewProj[0][3] + viewProj[0][0];
	m_planes[0].normal.y = viewProj[1][3] + viewProj[1][0];
	m_planes[0].normal.z = viewProj[2][3] + viewProj[2][0];
	m_planes[0].distance = viewProj[3][3] + viewProj[3][0];

	// Right plane
	m_planes[1].normal.x = viewProj[0][3] - viewProj[0][0];
	m_planes[1].normal.y = viewProj[1][3] - viewProj[1][0];
	m_planes[1].normal.z = viewProj[2][3] - viewProj[2][0];
	m_planes[1].distance = viewProj[3][3] - viewProj[3][0];

	// Bottom plane
	m_planes[2].normal.x = viewProj[0][3] + viewProj[0][1];
	m_planes[2].normal.y = viewProj[1][3] + viewProj[1][1];
	m_planes[2].normal.z = viewProj[2][3] + viewProj[2][1];
	m_planes[2].distance = viewProj[3][3] + viewProj[3][1];

	// Top plane
	m_planes[3].normal.x = viewProj[0][3] - viewProj[0][1];
	m_planes[3].normal.y = viewProj[1][3] - viewProj[1][1];
	m_planes[3].normal.z = viewProj[2][3] - viewProj[2][1];
	m_planes[3].distance = viewProj[3][3] - viewProj[3][1];

	// Near plane
	m_planes[4].normal.x = viewProj[0][3] + viewProj[0][2];
	m_planes[4].normal.y = viewProj[1][3] + viewProj[1][2];
	m_planes[4].normal.z = viewProj[2][3] + viewProj[2][2];
	m_planes[4].distance = viewProj[3][3] + viewProj[3][2];

	// Far plane
	m_planes[5].normal.x = viewProj[0][3] - viewProj[0][2];
	m_planes[5].normal.y = viewProj[1][3] - viewProj[1][2];
	m_planes[5].normal.z = viewProj[2][3] - viewProj[2][2];
	m_planes[5].distance = viewProj[3][3] - viewProj[3][2];

	// Normalize all planes
	/*for (auto& plane : planes) {
		float length = glm::length(plane.normal);
		plane.normal /= length;
		plane.distance /= length;
	}*/
}

void SceneRenderer::clear(const glm::vec4 &clearColor, const float depth){
	static const float COLOR[] = { 0.0, 0.0, 0.0, 1.0 };
	static const float DEPTH[] = { 1.0 };

	glClearBufferfv(GL_COLOR, 0, COLOR);
	glClearBufferfv(GL_DEPTH, 0, DEPTH);
}
bool SceneRenderer::setUpShader(){
	if (this->m_shaderProgram == nullptr) {
		return false;
	}

	this->m_shaderProgram->useProgram();

	// shader attributes binding
	const GLuint programId = this->m_shaderProgram->programId();

	SceneManager *manager = SceneManager::Instance();
	manager->m_vertexHandle = 0;
	manager->m_normalHandle = 1;
	manager->m_uvHandle = 2;
	manager->m_instPosHandle = 3;

	// =================================
	manager->m_modelMatHandle = 0;
	manager->m_viewMatHandle = 7;
	manager->m_projMatHandle = 8;
	manager->m_terrainVToUVMatHandle = 9;
	manager->m_numTotalInstanceHandle = 10;

	manager->m_planeEquationHandle1 = 15;
	manager->m_planeEquationHandle2 = 16;
	manager->m_planeEquationHandle3 = 17;
	manager->m_planeEquationHandle4 = 18;
	manager->m_planeEquationHandle5 = 19;
	manager->m_planeEquationHandle6 = 20;


	manager->m_albedoMapHandle = 4;
	manager->m_albedoMapTexIdx = 0;
	glUniform1i(manager->m_albedoMapHandle, manager->m_albedoMapTexIdx);

	manager->m_elevationMapHandle = 5;
	manager->m_elevationMapTexIdx = 3;
	glUniform1i(manager->m_elevationMapHandle, manager->m_elevationMapTexIdx);
	
	manager->m_normalMapHandle = 6;
	manager->m_normalMapTexIdx = 2;
	glUniform1i(manager->m_normalMapHandle, manager->m_normalMapTexIdx);

	manager->m_arrayTexHandle = 3;
	manager->m_arrayTexIdx = 1;
	glUniform1i(manager->m_arrayTexHandle, manager->m_arrayTexIdx);
	
	manager->m_albedoTexUnit = GL_TEXTURE0;
	manager->m_arrayTexUnit = GL_TEXTURE1;
	manager->m_elevationTexUnit = GL_TEXTURE3;
	manager->m_normalTexUnit = GL_TEXTURE2;

	manager->m_vs_vertexProcessIdHandle = 1;
	manager->m_vs_commonProcess = 0;
	manager->m_vs_terrainProcess = 3;
	manager->m_vs_multiInstanceProcess = 2;

	manager->m_fs_pixelProcessIdHandle = 2;
	manager->m_fs_pureColor = 5;
	manager->m_fs_terrainPass = 7;
	manager->m_fs_bushesBuildingsPass = 8;
	
	// ------------------------------ ssbo binding location
	manager->m_rawInstanceDataBufferId = 1;
	manager->m_validInstanceDataBufferId = 2;
	manager->m_cmdBufferId = 3;
	
	return true;
}
