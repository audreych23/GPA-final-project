#pragma once

#include <vector>
#include "Shader.h"
#include "SceneManager.h"
#include "DynamicSceneObject.h"
#include "terrain\TerrainSceneObject.h"


class SceneRenderer
{
public:
	SceneRenderer();
	virtual ~SceneRenderer();

private:
	struct FrustumPlane {
		glm::vec3 normal;
		float distance;
	};

	ShaderProgram *m_shaderProgram = nullptr;
	ShaderProgram* m_resetCsProgram = nullptr;
	ShaderProgram* m_cullingCsProgram = nullptr;

	glm::mat4 m_projMat;
	glm::mat4 m_viewMat;
	int m_frameWidth;
	int m_frameHeight;	

	std::vector<DynamicSceneObject*> m_dynamicSOs;
	TerrainSceneObject* m_terrainSO = nullptr;
	DynamicSceneObject* m_dynamicBushesBuildingsSO;

	int m_numTotalInstance;

	// useful for frustum culling
	//glm::mat4 playerViewMat;
	//glm::mat4 playerProjMat;

	std::vector<FrustumPlane> m_planes;

public:
	void resize(const int w, const int h);
	bool initialize(const int w, const int h, ShaderProgram* shaderProgram);

	void setProjection(const glm::mat4 &proj);
	void setView(const glm::mat4 &view);
	void setViewport(const int x, const int y, const int w, const int h);
	void appendDynamicSceneObject(DynamicSceneObject *obj);
	void appendTerrainSceneObject(TerrainSceneObject* tSO);
	void appendDynamicBushesBuildings(DynamicSceneObject* obj);

	void setResetComputeShader(ShaderProgram* shaderProgram);
	void setCullingComputeShader(ShaderProgram* shaderProgram);

	void setNumInstance(int numInstance);

	void setFrustumPlaneEquation(glm::mat4& playerView, glm::mat4& playerProj);
// pipeline
public:
	void startNewFrame();
	void renderPass();

// compute shader pipeline
public:
	void useCullingCSProgram();
	void useResetCSProgram();

private:
	void clear(const glm::vec4 &clearColor = glm::vec4(0.0, 0.0, 0.0, 1.0), const float depth = 1.0);
	bool setUpShader();
};

