#include "MyBushesAndBuildings.h"


MyBushesAndBuildings::MyBushesAndBuildings(
	const GLuint fsFunctionHandle, const int pixelProcessFuncBinding) 
{
	this->m_modelBush01 = new Model("assets\\bush01_lod2.obj", "assets\\poissonPoints_1010.ppd2", "assets\\bush01.png", 0.0f);
	this->m_modelBush05 = new Model("assets\\bush05_lod2.obj", "assets\\poissonPoints_2797.ppd2", "assets\\bush05.png", 1.0f);
	this->m_modelGrass = new Model("assets\\grassB.obj", "assets\\poissonPoints_621043_after.ppd2", "assets\\grassB_albedo.png", 2.0f);
	//this->m_house = new Model("assets\\Medieval_Building_LowPoly\\medieval_building_lowpoly_2.obj", "assets\\cityLots_sub_0.ppd2", 3.0f);
	//this->m_house2 = new Model("assets\\Medieval_Building_LowPoly\\medieval_building_lowpoly_1.obj", "assets\\cityLots_sub_1.ppd2", 4.0f);

	m_totalInstance = m_modelBush01->getNumSamples() +
		m_modelBush05->getNumSamples() + m_modelGrass->getNumSamples();
	// pass in 3 model max num vertex, pass in 3 model maxnum index
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
	auto modelMesh1 = m_modelBush01->getMeshes();
	auto modelMesh2 = m_modelBush05->getMeshes();
	auto modelMesh3 = m_modelGrass->getMeshes();
	//auto modelMesh4 = m_house->getMeshes();
	//auto modelMesh5 = m_house->getMeshes();

	int maxNumVertex = modelMesh1[0].vertices.size() + 
		modelMesh2[0].vertices.size() + modelMesh3[0].vertices.size();
	int maxNumIndices = modelMesh1[0].indices.size() + 
		modelMesh2[0].indices.size() + modelMesh3[0].indices.size();

	int maxNumInstance = m_modelBush01->getNumSamples() +
		m_modelBush05->getNumSamples() + m_modelGrass->getNumSamples();

	//InstanceProperties* rawInsData = new InstanceProperties[NUM_TOTAL_INSTANCE];

	DynamicSceneObject::InstanceProperties* rawInstData = new DynamicSceneObject::InstanceProperties[maxNumInstance];

	int offset = 0;
	for (int i = 0; i < m_modelBush01->getNumSamples(); ++i) {
		const float* instPos = m_modelBush01->getInstancePositions(i);
		rawInstData[offset + i].position = glm::vec4(instPos[0], instPos[1], instPos[2], 0.0f);
		//const float* instRad = m_modelBush01->getInstanceRadians(i);
		//glm::quat q = glm::quat(glm::vec3(instRad[0], instRad[1], instRad[2]));
		//rawInstData[offset + i].rotMatrix = glm::toMat4(q);
	}
	
	offset += m_modelBush01->getNumSamples();
	for (int i = 0; i < m_modelBush05->getNumSamples(); ++i) {
		const float* instPos = m_modelBush05->getInstancePositions(i);
		rawInstData[offset + i].position = glm::vec4(instPos[0], instPos[1], instPos[2], 1.0f);
		//const float* instRad = m_modelBush05->getInstanceRadians(i);
		//glm::quat q = glm::quat(glm::vec3(instRad[0], instRad[1], instRad[2]));
		//rawInstData[offset + i].rotMatrix = glm::toMat4(q);
	}

	offset += m_modelBush05->getNumSamples();
	for (int i = 0; i < m_modelGrass->getNumSamples(); ++i) {
		const float* instPos = m_modelGrass->getInstancePositions(i);
		rawInstData[offset + i].position = glm::vec4(instPos[0], instPos[1], instPos[2], 2.0f);
		//const float* instRad = m_modelGrass->getInstanceRadians(i);
		//glm::quat q = glm::quat(glm::vec3(instRad[0], instRad[1], instRad[2]));
		//rawInstData[offset + i].rotMatrix = glm::toMat4(q);
	}
	
	// no normal but has texture coord and has instance instance
	// pass in the raw Instance Data since it is quite difficult to rebind it (small hacks) 
	this->m_dynamicSO = new DynamicSceneObject(maxNumVertex, maxNumIndices, false, true, true, maxNumInstance, rawInstData);


	auto data = this->m_dynamicSO->dataBuffer();
	// modify data
	offset = 0;
	for (unsigned int i = 0; i < modelMesh1[0].vertices.size(); ++i) {
		data[offset + i * 9 + 0] = modelMesh1[0].vertices[i].position.x;
		data[offset + i * 9 + 1] = modelMesh1[0].vertices[i].position.y;
		data[offset + i * 9 + 2] = modelMesh1[0].vertices[i].position.z;
		data[offset + i * 9 + 3] = modelMesh1[0].vertices[i].normal.x;
		data[offset + i * 9 + 4] = modelMesh1[0].vertices[i].normal.y;
		data[offset + i * 9 + 5] = modelMesh1[0].vertices[i].normal.z;
		data[offset + i * 9 + 6] = modelMesh1[0].vertices[i].tex_coords.x;
		data[offset + i * 9 + 7] = modelMesh1[0].vertices[i].tex_coords.y;
		data[offset + i * 9 + 8] = modelMesh1[0].vertices[i].tex_coords.z;
	}

	offset += modelMesh1[0].vertices.size();
	for (unsigned int i = 0; i < modelMesh2[0].vertices.size(); ++i) {
		data[offset + i * 9 + 0] = modelMesh2[0].vertices[i].position.x;
		data[offset + i * 9 + 1] = modelMesh2[0].vertices[i].position.y;
		data[offset + i * 9 + 2] = modelMesh2[0].vertices[i].position.z;
		data[offset + i * 9 + 3] = modelMesh2[0].vertices[i].normal.x;
		data[offset + i * 9 + 4] = modelMesh2[0].vertices[i].normal.y;
		data[offset + i * 9 + 5] = modelMesh2[0].vertices[i].normal.z;
		data[offset + i * 9 + 6] = modelMesh2[0].vertices[i].tex_coords.x;
		data[offset + i * 9 + 7] = modelMesh2[0].vertices[i].tex_coords.y;
		data[offset + i * 9 + 8] = modelMesh2[0].vertices[i].tex_coords.z;
	}

	offset += modelMesh2[0].vertices.size();
	for (unsigned int i = 0; i < modelMesh3[0].vertices.size(); ++i) {
		data[offset + i * 9 + 0] = modelMesh3[0].vertices[i].position.x;
		data[offset + i * 9 + 1] = modelMesh3[0].vertices[i].position.y;
		data[offset + i * 9 + 2] = modelMesh3[0].vertices[i].position.z;
		data[offset + i * 9 + 3] = modelMesh3[0].vertices[i].normal.x;
		data[offset + i * 9 + 4] = modelMesh3[0].vertices[i].normal.y;
		data[offset + i * 9 + 5] = modelMesh3[0].vertices[i].normal.z;
		data[offset + i * 9 + 6] = modelMesh3[0].vertices[i].tex_coords.x;
		data[offset + i * 9 + 7] = modelMesh3[0].vertices[i].tex_coords.y;
		data[offset + i * 9 + 8] = modelMesh3[0].vertices[i].tex_coords.z;
	}

	// fill in updateDataBuffer
	this->m_dynamicSO->updateDataBuffer(0, maxNumVertex * 9 * sizeof(float));


	auto indexData = this->m_dynamicSO->indexBuffer();
	// modify index data
	offset = 0;
	for (unsigned int i = 0; i < modelMesh1[0].indices.size(); ++i) {
		indexData[offset + i] = modelMesh1[0].indices[i];
	}

	offset += modelMesh1[0].indices.size();
	for (unsigned int i = 0; i < modelMesh2[0].indices.size(); ++i) {
		indexData[offset + i] = modelMesh2[0].indices[i];
	}

	offset += modelMesh2[0].indices.size();
	for (unsigned int i = 0; i < modelMesh3[0].indices.size(); ++i) {
		indexData[offset + i] = modelMesh3[0].indices[i];
	}
	// fill in updateIndex Buffer
	this->m_dynamicSO->updateIndexBuffer(0, maxNumIndices * sizeof(unsigned int));

	//GLuint rawInstanceDataBufferHandle;
	//glGenBuffers(1, &rawInstanceDataBufferHandle);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, rawInstanceDataBufferHandle);
	//glBufferStorage(GL_SHADER_STORAGE_BUFFER, NUM_TOTAL_INSTANCE * sizeof(InstanceProperties), rawInsData, GL_MAP_READ_BIT);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, rawInstanceDataBufferHandle);

	//// prepare a SSBO for storing VALID instance data
	//GLuint validInstanceDataBufferHandle;
	//glGenBuffers(1, &validInstanceDataBufferHandle);
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, validInstanceDataBufferHandle);
	//glBufferStorage(GL_SHADER_STORAGE_BUFFER, NUM_TOTAL_INSTANCE * sizeof(InstancePropertiesOut), nullptr, GL_MAP_READ_BIT);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, validInstanceDataBufferHandle);

	DrawElementsIndirectCommand drawCommands[3];
	// first bush model
	drawCommands[0].count = modelMesh1[0].indices.size();
	drawCommands[0].instanceCount = m_modelBush01->getNumSamples();
	drawCommands[0].firstIndex = 0;
	drawCommands[0].baseVertex = 0;
	drawCommands[0].baseInstance = 0 * sizeof(float);

	// second bush model
	drawCommands[1].count = modelMesh2[0].indices.size();
	drawCommands[1].instanceCount = m_modelBush05->getNumSamples();
	drawCommands[1].firstIndex = modelMesh1[0].indices.size();
	drawCommands[1].baseVertex = modelMesh1[0].vertices.size();
	drawCommands[1].baseInstance = m_modelBush01->getNumSamples();

	// third bush model
	drawCommands[2].count = modelMesh3[0].indices.size();
	drawCommands[2].instanceCount = m_modelGrass->getNumSamples();
	drawCommands[2].firstIndex = modelMesh1[0].indices.size() + modelMesh2[0].indices.size();
	drawCommands[2].baseVertex = modelMesh1[0].vertices.size() + modelMesh2[0].vertices.size();
	drawCommands[2].baseInstance = m_modelBush01->getNumSamples() + m_modelBush05->getNumSamples();

	//// first house model
	//drawCommands[3].count = model_mesh_3[0].indices.size();
	//drawCommands[3].instanceCount = m_model_grass->getNumSamples();
	//drawCommands[3].firstIndex = model_mesh_1[0].indices.size() + model_mesh_2[0].indices.size();
	//drawCommands[3].baseVertex = model_mesh_1[0].vertices.size() + model_mesh_2[0].vertices.size();
	//drawCommands[3].baseInstance = m_model_bush_01->getNumSamples() + m_model_bush_05->getNumSamples();

	//// second house model
	//drawCommands[4].count = model_mesh_3[0].indices.size();
	//drawCommands[4].instanceCount = m_model_grass->getNumSamples();
	//drawCommands[4].firstIndex = model_mesh_1[0].indices.size() + model_mesh_2[0].indices.size();
	//drawCommands[4].baseVertex = model_mesh_1[0].vertices.size() + model_mesh_2[0].vertices.size();
	//drawCommands[4].baseInstance = m_model_bush_01->getNumSamples() + m_model_bush_05->getNumSamples();

	//GLuint cmd_buffer_handle;
	GLuint cmdBufferHandle;

	glGenBuffers(1, &cmdBufferHandle);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, cmdBufferHandle);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(DrawElementsIndirectCommand) * 3,
		drawCommands, GL_MAP_READ_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SceneManager::Instance()->m_cmdBufferId, cmdBufferHandle);

	this->m_dynamicSO->setCmdBufferHandle(cmdBufferHandle);

	//// Instance buffer ( think about this) 
	//glBindBuffer(GL_ARRAY_BUFFER, validInstanceDataBufferHandle);
	//glEnableVertexAttribArray(SHADER_PARAMETER_BINDING::INSTANCE_POS_LOCATION);
	//glVertexAttribPointer(SHADER_PARAMETER_BINDING::INSTANCE_POS_LOCATION, 4, GL_FLOAT, false, 0, (void*)0);
	//glVertexAttribDivisor(SHADER_PARAMETER_BINDING::INSTANCE_POS_LOCATION, 1);
}


void MyBushesAndBuildings::loadMergedTextureFromFile() {
	auto modelMesh1 = m_modelBush01->getMeshes();
	auto modelMesh2 = m_modelBush05->getMeshes();
	auto modelMesh3 = m_modelGrass->getMeshes();
	//auto modelMesh4 = m_house->getMeshes();
	//auto modelMesh5 = m_house2->getMeshes();

	// luckily there's only 1 texture and 1 mesh
	std::string path0 = modelMesh1[0].textures[0].path;
	std::string path1 = modelMesh2[0].textures[0].path;
	std::string path2 = modelMesh3[0].textures[0].path;
	//std::string path3 = modelMesh4[0].textures[0].path;
	//std::string path4 = modelMesh5[0].textures[0].path;

	// 3 different models filename
	std::string filename0 = std::string(path0);
	std::string filename1 = std::string(path1);
	std::string filename2 = std::string(path2);
	std::cout << filename0 << " " << filename1 << " " << filename2 << '\n';
	//std::string filename3 = std::string(path3);
	//std::string filename4 = std::string(path4);
	//filename = "textures/" + filename;
	//std::cout << "file_name:\n " << filename_0 << '\n';

	int width, height, nrComponents;
	// get different texture 
	unsigned char* data0 = stbi_load(filename0.c_str(), &width, &height, &nrComponents, 0);
	unsigned char* data1 = stbi_load(filename1.c_str(), &width, &height, &nrComponents, 0);
	unsigned char* data2 = stbi_load(filename2.c_str(), &width, &height, &nrComponents, 0);
	//unsigned char* data3 = stbi_load(filename3.c_str(), &width, &height, &nrComponents, 0);
	//unsigned char* data4 = stbi_load(filename4.c_str(), &width, &height, &nrComponents, 0);

	const int NUM_TEXTURE = 5;
	const int IMG_WIDTH = 1024;
	const int IMG_HEIGHT = 1024;
	const int IMG_CHANNEL = 4;

	unsigned char* textureArrayData = new unsigned char[IMG_WIDTH * IMG_HEIGHT * IMG_CHANNEL * NUM_TEXTURE];

	// memcpy(void* dest, const void* source, size_t n);
	memcpy(textureArrayData, data0, sizeof(unsigned char) * IMG_WIDTH * IMG_HEIGHT * IMG_CHANNEL);
	memcpy(textureArrayData + IMG_WIDTH * IMG_HEIGHT * IMG_CHANNEL, data1, sizeof(unsigned char) * IMG_WIDTH * IMG_HEIGHT * IMG_CHANNEL);
	memcpy(textureArrayData + IMG_WIDTH * IMG_HEIGHT * IMG_CHANNEL * 2, data2, sizeof(unsigned char) * IMG_WIDTH * IMG_HEIGHT * IMG_CHANNEL);
	//memcpy(textureArrayData + IMG_WIDTH * IMG_HEIGHT * IMG_CHANNEL * 3, data3, sizeof(unsigned char) * IMG_WIDTH * IMG_HEIGHT * IMG_CHANNEL);
	//memcpy(textureArrayData + IMG_WIDTH * IMG_HEIGHT * IMG_CHANNEL * 4, data4, sizeof(unsigned char) * IMG_WIDTH * IMG_HEIGHT * IMG_CHANNEL);

	GLuint textureArrayHandler;
	if (data0 && data1 && data2 /* && data3 && data4 */)
	{
		glGenTextures(1, &textureArrayHandler);
		glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayHandler);

		// albedo texture handler (special since it is GL_TEXTURE_ARRAY_2D)
		// for rendering later
		this->m_dynamicSO->setArrayTextureHandle(textureArrayHandler);
		// the internal format for glTexStorageXD must be "Sized Internal Formats“
		// max mipmap level = log2(1024) + 1 = 11
		glTexStorage3D(GL_TEXTURE_2D_ARRAY, 11, GL_RGBA8, IMG_WIDTH, IMG_HEIGHT, NUM_TEXTURE);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0,
			IMG_WIDTH, IMG_HEIGHT, NUM_TEXTURE, GL_RGBA, GL_UNSIGNED_BYTE, textureArrayData);

		glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

		//IMPORTANT !! Use mipmap for the foliage rendering
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);

		stbi_image_free(data0);
		stbi_image_free(data1);
		stbi_image_free(data2);
		//stbi_image_free(data3);
		//stbi_image_free(data4);
	}
	else
	{
		if (!data0) {
			std::cout << "Texture failed to load at path: " << filename0 << std::endl;
			stbi_image_free(data0);
		}

		if (!data1) {
			std::cout << "Texture failed to load at path: " << filename1 << std::endl;
			stbi_image_free(data1);
		}

		if (!data2) {
			std::cout << "Texture failed to load at path: " << filename2 << std::endl;
			stbi_image_free(data2);
		}

		//if (!data3) {
		//	std::cout << "Texture failed to load at path: " << filename3 << std::endl;
		//	stbi_image_free(data3);
		//}

		//if (!data4) {
		//	std::cout << "Texture failed to load at path: " << filename4 << std::endl;
		//	stbi_image_free(data4);
		//}
	}
}