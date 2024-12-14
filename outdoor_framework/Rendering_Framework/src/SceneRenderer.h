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
	ShaderProgram *m_shaderProgram = nullptr;
	ShaderProgram* m_resetCSProgram = nullptr;
	ShaderProgram* m_cullingCSProgram = nullptr;

	glm::mat4 m_projMat;
	glm::mat4 m_viewMat;
	int m_frameWidth;
	int m_frameHeight;	

	std::vector<DynamicSceneObject*> m_dynamicSOs;
	TerrainSceneObject* m_terrainSO = nullptr;
	DynamicSceneObject* m_dynamicBushesBuildings;


public:
	void resize(const int w, const int h);
	bool initialize(const int w, const int h, ShaderProgram* shaderProgram);

	void setProjection(const glm::mat4 &proj);
	void setView(const glm::mat4 &view);
	void setViewport(const int x, const int y, const int w, const int h);
	void appendDynamicSceneObject(DynamicSceneObject *obj);
	void appendTerrainSceneObject(TerrainSceneObject* tSO);
	void appendDynamicBushesBuildings(DynamicSceneObject* obj);

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

