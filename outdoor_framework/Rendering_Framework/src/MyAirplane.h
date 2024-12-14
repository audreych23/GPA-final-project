#pragma once

#include "DynamicSceneObject.h"

#include <glm\mat4x4.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\cimport.h>
#include <assimp\postprocess.h>
#include "stb_image.h"

#include <iostream>

class MyAirplane
{
public:
	MyAirplane(const GLuint fsFunctionHandle, const int pixelProcessFuncBinding);
	virtual ~MyAirplane();

	DynamicSceneObject* sceneObject() const;

public:
	void updateDataBuffer();
	void updateState(const glm::mat4& viewMat, const glm::vec3& viewPos);

private:
	DynamicSceneObject* m_dynamicSO;
	int m_totalDataByte;

	GLuint _tex;
};

