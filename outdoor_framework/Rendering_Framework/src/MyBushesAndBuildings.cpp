#include "MyBushesAndBuildings.h"


MyBushesAndBuildings::MyBushesAndBuildings(
	const GLuint fsFunctionHandle, const int pixelProcessFuncBinding) 
{
	this->m_modelBush01 = new Model("assets\\bush01_lod2.obj", "assets\\poissonPoints_1010.ppd2", "assets\\bush01.png", 2.0f);
	this->m_modelBush05 = new Model("assets\\bush05_lod2.obj", "assets\\poissonPoints_2797.ppd2", "assets\\bush05.png", 3.0f);
	this->m_modelGrass = new Model("assets\\grassB.obj", "assets\\poissonPoints_621043_after.ppd2", "assets\\grassB_albedo.png", 4.0f);
	this->m_house = new Model("assets\\Medieval_Building_LowPoly\\medieval_building_lowpoly_2.obj", 
		"assets\\cityLots_sub_0.ppd2", "assets\\Medieval_Building_LowPoly\\Medieval_Building_LowPoly_V2_Albedo_small.png", 0.0f);
	this->m_house2 = new Model("assets\\Medieval_Building_LowPoly\\medieval_building_lowpoly_1.obj", 
		"assets\\cityLots_sub_1.ppd2", "assets\\Medieval_Building_LowPoly\\Medieval_Building_LowPoly_V1_Albedo_small.png", 1.0f);
	// pass in 3 model max num vertex, pass in 3 model maxnum index

	this->m_numModel = 5;

	//this->m_mergedModel.push_back(m_modelGrass);
	this->m_mergedModel.push_back(m_house);
	this->m_mergedModel.push_back(m_house2);
	this->m_mergedModel.push_back(m_modelBush01);
	this->m_mergedModel.push_back(m_modelBush05);
	this->m_mergedModel.push_back(m_modelGrass);

	for (auto model : m_mergedModel) {
		m_totalInstance += model->getNumSamples();
	}

	std::cout << "total" << m_totalInstance << " ";
	setupDrawCommand();
	loadMergedTextureFromFile();

	// set modelMat
	this->m_dynamicSO->setModelMat(glm::mat4(1.0f));
	this->m_dynamicSO->setPrimitive(GL_TRIANGLES);
	this->m_dynamicSO->setPixelFunctionId(pixelProcessFuncBinding);
}

MyBushesAndBuildings::~MyBushesAndBuildings() {

}

DynamicSceneObject* MyBushesAndBuildings::sceneObject() const {
	return this->m_dynamicSO;
}

void MyBushesAndBuildings::updateDataBuffer() {

}

void MyBushesAndBuildings::updateState(const glm::mat4& viewMat, const glm::vec3& viewPos) { }

void MyBushesAndBuildings::setupDrawCommand() {
	int maxNumVertex = 0;
	int maxNumIndices = 0;
	for (auto model : m_mergedModel) {
		auto modelMesh = model->getMeshes();
		maxNumVertex += modelMesh[0].vertices.size();
		maxNumIndices += modelMesh[0].indices.size();
	}

	int maxNumInstance = m_totalInstance;

	DynamicSceneObject::InstanceProperties* rawInstData = new DynamicSceneObject::InstanceProperties[maxNumInstance];

	int offset = 0;
	for (int k = 0; k < m_mergedModel.size(); ++k) {
		auto model = m_mergedModel[k];
		for (int i = 0; i < model->getNumSamples(); ++i) {
			const float* instPos = model->getInstancePositions(i);
			rawInstData[offset + i].position = glm::vec4(instPos[0], instPos[1], instPos[2], (float)k);
			const float* instRad = model->getInstanceRadians(i);
			glm::quat q = glm::quat(glm::vec3(instRad[0], instRad[1], instRad[2]));
			rawInstData[offset + i].rotMatrix = glm::toMat4(q);
		}
		offset += model->getNumSamples();
	}
	
	// no normal but has texture coord and has instance instance
	// pass in the raw Instance Data since it is quite difficult to rebind it (small hacks) 
	this->m_dynamicSO = new DynamicSceneObject(maxNumVertex, maxNumIndices, true, true, true, maxNumInstance, rawInstData);


	auto data = this->m_dynamicSO->dataBuffer();
	// modify data
	offset = 0;
	for (int k = 0; k < m_mergedModel.size(); ++k) {
		auto model = m_mergedModel[k];
		auto modelMesh = model->getMeshes();
		for (unsigned int i = 0; i < modelMesh[0].vertices.size(); ++i) {
			data[offset + i * 9 + 0] = modelMesh[0].vertices[i].position.x;
			data[offset + i * 9 + 1] = modelMesh[0].vertices[i].position.y;
			data[offset + i * 9 + 2] = modelMesh[0].vertices[i].position.z;
			data[offset + i * 9 + 3] = modelMesh[0].vertices[i].normal.x;
			data[offset + i * 9 + 4] = modelMesh[0].vertices[i].normal.y;
			data[offset + i * 9 + 5] = modelMesh[0].vertices[i].normal.z;
			data[offset + i * 9 + 6] = modelMesh[0].vertices[i].tex_coords.x;
			data[offset + i * 9 + 7] = modelMesh[0].vertices[i].tex_coords.y;
			data[offset + i * 9 + 8] = (float) k;
		}
		offset += 9 * modelMesh[0].vertices.size();
	}

	// fill in updateDataBuffer
	this->m_dynamicSO->updateDataBuffer(0, maxNumVertex * 9 * sizeof(float));


	auto indexData = this->m_dynamicSO->indexBuffer();
	// modify index data
	offset = 0;
	for (auto model : m_mergedModel) {
		auto modelMesh = model->getMeshes();
		for (unsigned int i = 0; i < modelMesh[0].indices.size(); ++i) {
			indexData[offset + i] = modelMesh[0].indices[i];
		}
		offset += modelMesh[0].indices.size();
	}
	// fill in updateIndex Buffer 
	this->m_dynamicSO->updateIndexBuffer(0, maxNumIndices * sizeof(unsigned int));


	DrawElementsIndirectCommand* drawCommands = 
		new DrawElementsIndirectCommand[m_numModel];

	int prefixFirstIndex = 0;
	int prefixBaseVertex = 0;
	int prefixBaseInstance = 0;
	for (int i = 0; i < m_mergedModel.size(); ++i) {
		auto modelMesh = m_mergedModel[i]->getMeshes();
		// first bush model
		drawCommands[i].count = modelMesh[0].indices.size();
		drawCommands[i].instanceCount = m_mergedModel[i]->getNumSamples();
		drawCommands[i].firstIndex = prefixFirstIndex;
		drawCommands[i].baseVertex = prefixBaseVertex;
		drawCommands[i].baseInstance = prefixBaseInstance;

		prefixFirstIndex += modelMesh[0].indices.size();
		prefixBaseVertex += modelMesh[0].vertices.size();
		prefixBaseInstance += m_mergedModel[i]->getNumSamples();
	}

	//GLuint cmd_buffer_handle;
	GLuint cmdBufferHandle;

	glGenBuffers(1, &cmdBufferHandle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, cmdBufferHandle);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(DrawElementsIndirectCommand) * m_numModel,
		drawCommands, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SceneManager::Instance()->m_cmdBufferId, cmdBufferHandle);

	this->m_dynamicSO->setCmdBufferHandle(cmdBufferHandle);
}


void MyBushesAndBuildings::loadMergedTextureFromFile() {
	int width, height, nrComponents;

	const int IMG_WIDTH = 1024;
	const int IMG_HEIGHT = 1024;
	const int IMG_CHANNEL = 4;

	// get different texture 
	unsigned char* textureArrayData = new unsigned char[IMG_WIDTH * IMG_HEIGHT * IMG_CHANNEL * m_numModel];

	for (int k = 0; k < m_mergedModel.size(); ++k) {
		auto model = m_mergedModel[k];
		auto modelMesh = model->getMeshes();

		std::string path = modelMesh[0].textures[0].path;
		std::string filename = std::string(path);

		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

		memcpy(textureArrayData + IMG_WIDTH * IMG_HEIGHT * IMG_CHANNEL * k, data, sizeof(unsigned char) * IMG_WIDTH * IMG_HEIGHT * IMG_CHANNEL);

		stbi_image_free(data);
	}
	
	//unsigned char* data4 = stbi_load(filename4.c_str(), &width, &height, &nrComponents, 0);

	GLuint textureArrayHandler;
	glGenTextures(1, &textureArrayHandler);
	glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayHandler);

	// albedo texture handler (special since it is GL_TEXTURE_ARRAY_2D)
	// for rendering later
	this->m_dynamicSO->setArrayTextureHandle(textureArrayHandler);
	// the internal format for glTexStorageXD must be "Sized Internal Formats“
	// max mipmap level = log2(1024) + 1 = 11
	glTexStorage3D(GL_TEXTURE_2D_ARRAY, 11, GL_RGBA8, IMG_WIDTH, IMG_HEIGHT, m_numModel);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0,
		IMG_WIDTH, IMG_HEIGHT, m_numModel, GL_RGBA, GL_UNSIGNED_BYTE, textureArrayData);

	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//IMPORTANT !! Use mipmap for the foliage rendering
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

}