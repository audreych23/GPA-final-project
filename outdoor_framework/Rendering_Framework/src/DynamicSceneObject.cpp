#include "DynamicSceneObject.h"


DynamicSceneObject::DynamicSceneObject(const int maxNumVertex, const int maxNumIndex,
	const bool normalFlag, const bool uvFlag, const bool instPosFlag,
	const int maxNumInstance, InstanceProperties* rawInstData)
{
	// the data should be INTERLEAF format

	int totalBufferDataByte = maxNumVertex * 12;
	int strideV = 3;
	if (normalFlag == true) {
		totalBufferDataByte += maxNumVertex * 12;
		strideV += 3;
	}
	if (uvFlag == true) {
		totalBufferDataByte += maxNumVertex * 12;
		strideV += 3;
	}

	this->m_indexCount = maxNumIndex;

	this->m_dataBuffer = new float[totalBufferDataByte / 4];
	this->m_indexBuffer = new unsigned int[maxNumIndex];
	// don't forget to initialize instance data buffer
	

	// Create Geometry Data Buffer
	glCreateBuffers(1, &(this->m_dataBufferHandle));
	glNamedBufferData(this->m_dataBufferHandle, totalBufferDataByte, this->m_dataBuffer, GL_DYNAMIC_DRAW);

	// Create Indices Buffer
	glCreateBuffers(1, &m_indexBufferHandle);
	glNamedBufferData(m_indexBufferHandle, maxNumIndex * 4, this->m_indexBuffer, GL_DYNAMIC_DRAW);

	// create vao
	glGenVertexArrays(1, &(this->m_vao));
	glBindVertexArray(this->m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, this->m_dataBufferHandle);
	int byteOffset = 0;
	glVertexAttribPointer(SceneManager::Instance()->m_vertexHandle, 3, GL_FLOAT, false, strideV * 4, (void*)(byteOffset));
	byteOffset = byteOffset + 12;
	glEnableVertexAttribArray(SceneManager::Instance()->m_vertexHandle);
	if (normalFlag) {
		glVertexAttribPointer(SceneManager::Instance()->m_normalHandle, 3, GL_FLOAT, false, strideV * 4, (void*)(byteOffset));
		byteOffset = byteOffset + 12;
		glEnableVertexAttribArray(SceneManager::Instance()->m_normalHandle);
	}
	if (uvFlag) {
		glVertexAttribPointer(SceneManager::Instance()->m_uvHandle, 3, GL_FLOAT, false, strideV * 4, (void*)(byteOffset));
		byteOffset = byteOffset + 12;
		glEnableVertexAttribArray(SceneManager::Instance()->m_uvHandle);
	}
	
	// Lets do some clean way first then jump to the dirty method if it doesn't work :)
	if (instPosFlag) {
		// create buffer for input ssbo
		glCreateBuffers(1, &(this->m_rawInstanceDataBufferHandle));
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_rawInstanceDataBufferHandle);
		// ugly way
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, maxNumInstance * sizeof(InstanceProperties), rawInstData, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT);
		//glBufferStorage(GL_SHADER_STORAGE_BUFFER, maxNumInstance * sizeof(InstanceProperties), nullptr, GL_MAP_READ_BIT);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SceneManager::Instance()->m_rawInstanceDataBufferId, m_rawInstanceDataBufferHandle);

		// create buffer for output ssbo
		glGenBuffers(1, &(this->m_validInstanceDataBufferHandle));
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_validInstanceDataBufferHandle);
		glBufferStorage(GL_SHADER_STORAGE_BUFFER, maxNumInstance * sizeof(InstancePropertiesOut), nullptr, GL_MAP_READ_BIT);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SceneManager::Instance()->m_validInstanceDataBufferId, m_validInstanceDataBufferHandle);

		
		// bind another vbo for instance position
		glBindBuffer(GL_ARRAY_BUFFER, m_validInstanceDataBufferHandle);
		glEnableVertexAttribArray(SceneManager::Instance()->m_instPosHandle);
		glVertexAttribIPointer(SceneManager::Instance()->m_instPosHandle, 1, GL_INT, 0, (void*)0);
		glVertexAttribDivisor(SceneManager::Instance()->m_instPosHandle, 1);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferHandle);
	glBindVertexArray(0);
}


DynamicSceneObject::~DynamicSceneObject()
{
	delete[] this->m_dataBuffer;
	delete[] this->m_indexBuffer;
}

void DynamicSceneObject::update(bool isMergedModel) {
	// bind buffer
	glBindVertexArray(this->m_vao);

	if (!isMergedModel) {
		// Activatea TEXTURE 0/2/3 (something like that)
		glActiveTexture(SceneManager::Instance()->m_elevationTexUnit);
		glBindTexture(GL_TEXTURE_2D, this->m_elevationHandle);

		glActiveTexture(SceneManager::Instance()->m_normalTexUnit);
		glBindTexture(GL_TEXTURE_2D, this->m_normalHandle);

		glActiveTexture(SceneManager::Instance()->m_albedoTexUnit);
		glBindTexture(GL_TEXTURE_2D, this->m_albedoHandle);

		// model matrix
		glUniformMatrix4fv(SceneManager::Instance()->m_modelMatHandle, 1, false, glm::value_ptr(this->m_modelMat));

		glUniform1i(SceneManager::Instance()->m_fs_pixelProcessIdHandle, this->m_pixelFunctionId);
		glDrawElements(this->m_primitive, this->m_indexCount, GL_UNSIGNED_INT, nullptr);
	}
	else {
		// this is the texture of the combined model already
		glActiveTexture(SceneManager::Instance()->m_arrayTexUnit);
		glBindTexture(GL_TEXTURE_2D_ARRAY, this->m_arrayTexHandle);

		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_cmdBufferHandle);
		// model matrix
		glUniformMatrix4fv(SceneManager::Instance()->m_modelMatHandle, 1, false, glm::value_ptr(this->m_modelMat));

		glUniform1i(SceneManager::Instance()->m_fs_pixelProcessIdHandle, this->m_pixelFunctionId);
		// ask kent, no need index count??
		glMultiDrawElementsIndirect(this->m_primitive, GL_UNSIGNED_INT, (GLvoid*)0, 5, 0);
	}
}

float* DynamicSceneObject::dataBuffer() { return this->m_dataBuffer; }
unsigned int *DynamicSceneObject::indexBuffer() { return this->m_indexBuffer; }

void DynamicSceneObject::updateDataBuffer(const int byteOffset, const int dataByte) { 
	float *data = this->m_dataBuffer + byteOffset / 4;
	glNamedBufferSubData(this->m_dataBufferHandle, byteOffset, dataByte, data); 
}
void DynamicSceneObject::updateIndexBuffer(const int byteOffset, const int dataByte) { 
	unsigned int *data = this->m_indexBuffer + byteOffset / 4;
	glNamedBufferSubData(this->m_indexBufferHandle, byteOffset, dataByte, data); 
}

void DynamicSceneObject::setPixelFunctionId(const int functionId){
	this->m_pixelFunctionId = functionId;
}
void DynamicSceneObject::setPrimitive(const GLenum primitive) {
	this->m_primitive = primitive;
}
void DynamicSceneObject::setModelMat(const glm::mat4& modelMat){
	this->m_modelMat = modelMat;
}

// tex handle is the texture handle or the texture that you will bind 
void DynamicSceneObject::setElevationTextureHandle(const GLuint texHandle) {
	this->m_elevationHandle = texHandle;
}

void DynamicSceneObject::setNormalTextureHandle(const GLuint texHandle) {
	this->m_normalHandle = texHandle;
}

void DynamicSceneObject::setAlbedoTextureHandle(const GLuint texHandle) {
	this->m_albedoHandle = texHandle;
}

void DynamicSceneObject::setArrayTextureHandle(const GLuint texHandle) {
	this->m_arrayTexHandle = texHandle;
}

void DynamicSceneObject::setCmdBufferHandle(const GLuint cmdBufferHandle) {
	this->m_cmdBufferHandle = cmdBufferHandle;
}