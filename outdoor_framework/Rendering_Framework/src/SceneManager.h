#pragma once

#include <glad\glad.h>

// Singleton

class SceneManager
{
private:
	SceneManager(){}
	

public:	

	virtual ~SceneManager() {}

	static SceneManager *Instance() {
		static SceneManager *m_instance = nullptr;
		if (m_instance == nullptr) {
			m_instance = new SceneManager();
		}
		return m_instance;
	}
	
	GLuint m_vertexHandle;
	GLuint m_normalHandle;
	GLuint m_uvHandle;
	// for instance position
	GLuint m_instPosHandle;

	GLuint m_modelMatHandle;
	GLuint m_viewMatHandle;
	GLuint m_projMatHandle;
	GLuint m_terrainVToUVMatHandle;

	// uniform location layout
	GLuint m_arrayTexHandle;
	GLuint m_albedoMapHandle;
	GLuint m_normalMapHandle;
	GLuint m_elevationMapHandle;
	
	GLuint m_fs_pixelProcessIdHandle;
	GLuint m_vs_vertexProcessIdHandle;

	// for compute shader
	GLuint m_numTotalInstanceHandle;

	GLuint m_planeEquationHandle1;
	GLuint m_planeEquationHandle2;
	GLuint m_planeEquationHandle3;
	GLuint m_planeEquationHandle4;
	GLuint m_planeEquationHandle5;
	GLuint m_planeEquationHandle6;

	// GL_TEXTURE0, 1, 2 etc
	GLenum m_albedoTexUnit;
	GLenum m_normalTexUnit;
	GLenum m_elevationTexUnit;
	GLenum m_arrayTexUnit;

	// GL_TEXTURE0 + i, this is the i 
	int m_albedoMapTexIdx;
	int m_elevationMapTexIdx;
	int m_normalMapTexIdx;
	int m_arrayTexIdx;

	int m_vs_commonProcess;
	int m_vs_terrainProcess;
	int m_vs_multiInstanceProcess;
	
	int m_fs_pureColor;	
	int m_fs_terrainPass;
	int m_fs_bushesBuildingsPass;

	// binding location for ssbo 
	int m_rawInstanceDataBufferId;
	int m_validInstanceDataBufferId;
	int m_cmdBufferId;
};

