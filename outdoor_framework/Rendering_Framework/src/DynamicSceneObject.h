#pragma once

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "SceneManager.h"

class DynamicSceneObject 
{
public: 
	struct InstanceProperties {
		glm::vec4 position;
		glm::mat4 rotMatrix;
	};

	struct InstancePropertiesOut {
		int index;
	};
private:
	GLuint m_indexBufferHandle;
	float *m_dataBuffer = nullptr;
	unsigned int *m_indexBuffer = nullptr;

	GLuint m_vao;
	GLuint m_dataBufferHandle;
	GLenum m_primitive;
	int m_pixelFunctionId;
	int m_indexCount;

	glm::mat4 m_modelMat;

	GLuint m_elevationHandle;
	GLuint m_normalHandle;
	GLuint m_albedoHandle;
	GLuint m_arrayTexHandle;


	// for array instanced draw call
	GLuint m_cmdBufferHandle;

	// for building and bushes
	GLuint m_rawInstanceDataBufferHandle;
	GLuint m_validInstanceDataBufferHandle;


	bool m_isNormalMap;
	InstanceProperties* m_rawInstBuffer = nullptr;

public:
	GLuint vao() const { return m_vao; } 
	DynamicSceneObject(const int maxNumVertex, const int maxNumIndex, 
		const bool normalFlag, const bool uvFlag, const bool instPosFlag = false, 
		const int maxNumInstance = 0, InstanceProperties* rawInstData = nullptr);	virtual ~DynamicSceneObject();
	void update(bool isMergedModel = false);
	float* dataBuffer();
	unsigned int *indexBuffer();

	void updateDataBuffer(const int byteOffset, const int dataByte);
	void updateIndexBuffer(const int byteOffset, const int dataByte);

	void setPixelFunctionId(const int functionId);
	void setPrimitive(const GLenum primitive);
	void setModelMat(const glm::mat4& modelMat);
	void setNormalMap(const bool isNormalMap);

		// added for texture
	void setElevationTextureHandle(const GLuint texHandle);
	void setNormalTextureHandle(const GLuint texHandle);
	void setAlbedoTextureHandle(const GLuint texHandle);
	void setArrayTextureHandle(const GLuint texHandle);
	void setCmdBufferHandle(const GLuint cmdBufferHandle);

};

