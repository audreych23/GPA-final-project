#pragma once

#include "DynamicSceneObject.h"

#include <glm\mat4x4.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtx\quaternion.hpp>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\cimport.h>
#include <assimp\postprocess.h>
#include "stb_image.h"
#include "Model.h"


#include <iostream>

struct DrawElementsIndirectCommand {
	// number of index need to draw
	GLuint count;
	// number of instance
	GLuint instanceCount;
	// location of the first index
	GLuint firstIndex;
	// a constant that is added to each index
	GLuint baseVertex;
	// a base instance for fetching instanced vertex attributes
	GLuint baseInstance;
};



class MyBushesAndBuildings
{
public:
	MyBushesAndBuildings(const GLuint fsFunctionHandle, const int pixelProcessFuncBinding);
	virtual ~MyBushesAndBuildings();

	DynamicSceneObject* sceneObject() const;

public:
	void updateDataBuffer();
	void updateState(const glm::mat4& viewMat, const glm::vec3& viewPos);

public:
	int getNumInstance() { return m_totalInstance; }
private:
	void setupDrawCommand();
	void loadMergedTextureFromFile();
	 

	DynamicSceneObject* m_dynamicSO;
	int m_totalDataByte;
	int m_totalInstance;

	Model* m_modelBush01;
	Model* m_modelBush05;
	Model* m_modelGrass;
	Model* m_house;
	Model* m_house2;

	std::vector<Model*> m_mergedModel;

	int m_numModel;
};

