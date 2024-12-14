#include "MyAirplane.h"


MyAirplane::MyAirplane(const GLuint fsFunctionHandle, const int pixelProcessFuncBinding) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile("assets/airplane.obj",
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << "\n";
        return;
    }
    aiMesh* mesh = scene->mMeshes[0];
    this->m_dynamicSO = new DynamicSceneObject(mesh->mNumVertices, mesh->mNumFaces * 3, true, true);
    float* databuffer = this->m_dynamicSO->dataBuffer();
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        aiVector3D pos = mesh->mVertices[i];
        aiVector3D normal = mesh->mNormals[i];
        aiVector3D texCoord = mesh->mTextureCoords[0][i];
        databuffer[i * 9 + 0] = pos.x;
        databuffer[i * 9 + 1] = pos.y;
        databuffer[i * 9 + 2] = pos.z;
        databuffer[i * 9 + 3] = normal.x;
        databuffer[i * 9 + 4] = normal.y;
        databuffer[i * 9 + 5] = normal.z;
        databuffer[i * 9 + 6] = texCoord.x;
        databuffer[i * 9 + 7] = texCoord.y;
        databuffer[i * 9 + 8] = texCoord.z;
    }
    unsigned int* indexbuffer = this->m_dynamicSO->indexBuffer();
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        for (unsigned int j = 0; j < 3; j++) {
            indexbuffer[i * 3 + j] = mesh->mFaces[i].mIndices[j];
        }
    }
    this->m_dynamicSO->updateDataBuffer(0, mesh->mNumVertices * 9 * sizeof(float));
    this->m_dynamicSO->updateIndexBuffer(0, mesh->mNumFaces * 3 * sizeof(unsigned int));
    // texture
    int w, h, c;
    unsigned char* image = stbi_load("assets/Airplane_smooth_DefaultMaterial_BaseMap.jpg", &w, &h, &c, 4);
    if (!image) {
        std::cout << "ERROR::TEXTURE::" << "Failed to load texture" << "\n";
        exit(1);
    }
    unsigned char* tex = new unsigned char[w * h * 4];
    memcpy(tex, image, w * h * 4 * sizeof(unsigned char));
    stbi_image_free(image);
    // put _tex on dynamic object 
    // like setAlbedoTextureHandle(this->m_albedomapHandle = texHandle)
    // set elevated
    glGenTextures(1, &this->_tex);
    glBindTexture(GL_TEXTURE_2D, this->_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

	// initial plane position
	this->m_dynamicSO->setModelMat(glm::translate(glm::vec3(40.0f, 40.0f, 40.0f)));
	this->m_dynamicSO->setPrimitive(GL_TRIANGLES);
	this->m_dynamicSO->setPixelFunctionId(SceneManager::Instance()->m_fs_pureColor);

}


void MyAirplane::updateState(const glm::mat4& viewMat, const glm::vec3& viewPos) {
	glm::mat4 tpos = glm::translate(viewPos);
	glm::mat4 matt = glm::transpose(viewMat);
	glm::vec4 forw = -1.0f * glm::vec4(matt[2].x, matt[2].y, matt[2].z, 0.0f);
	glm::vec4 xaxis = -1.0f * glm::vec4(matt[0].x, matt[0].y, matt[0].z, 0.0f);
	glm::vec4 yaxis = glm::vec4(matt[1].x, matt[1].y, matt[1].z, 0.0f);
	glm::mat4 mat = glm::mat4(xaxis, yaxis, forw, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	this->m_dynamicSO->setModelMat(tpos * mat);
}
