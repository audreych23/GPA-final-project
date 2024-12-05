#include "MyBushesAndBuildings.h"
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <stb_image.h>
#include <vector>

// Struct for indirect draw commands
struct DrawElementsIndirectCommand {
    GLuint vertexCount;
    GLuint instanceCount;
    GLuint firstIndex;
    GLuint baseVertex;
    GLuint baseInstance;
};

MyBushesAndBuildings::MyBushesAndBuildings()
    : m_instanceSSBO(0), m_indirectBuffer(0) {
    const std::vector<std::string> modelFiles = {
        "assets/grassB.obj",
        "assets/bush01_lod2.obj",
        "assets/bush05_lod2.obj",
        "assets/Medieval_Building_LowPoly/medieval_building_lowpoly_2.obj",
        "assets/Medieval_Building_LowPoly/medieval_building_lowpoly_1.obj"
    };

    const std::vector<std::string> textureFiles = {
        "assets/grassB_albedo.png",
        "assets/bush01.png",
        "assets/bush05.png",
        "assets/Medieval_Building_LowPoly/Medieval_Building_LowPoly_V2_Albedo_small.png",
        "assets/Medieval_Building_LowPoly/Medieval_Building_LowPoly_V1_Albedo_small.png"
    };

    const std::vector<std::string> sampleFiles = {
        "assets/poissonPoints_621043_after.ppd2",
        "assets/poissonPoints_1010.ppd2",
        "assets/poissonPoints_2797.ppd2",
        "assets/cityLots_sub_0.ppd2",
        "assets/cityLots_sub_1.ppd2"
    };

    const std::vector<glm::vec3> boundingSphereCenters = {
        {0.0f, 0.66f, 0.0f}, {0.0f, 2.55f, 0.0f}, {0.0f, 1.76f, 0.0f},
        {0.0f, 4.57f, 0.0f}, {0.0f, 4.57f, 0.0f}
    };
    const std::vector<float> boundingSphereRadii = { 1.4f, 3.4f, 2.6f, 8.5f, 10.2f };

    std::vector<DrawElementsIndirectCommand> drawCommands;

    GLuint baseInstance = 0;
    for (size_t i = 0; i < modelFiles.size(); ++i) {
        // Load the model
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(modelFiles[i], aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            continue;
        }

        aiMesh* mesh = scene->mMeshes[0];
        DynamicSceneObject* dso = new DynamicSceneObject(mesh->mNumVertices, mesh->mNumFaces * 3, false, true);
        float* databuffer = dso->dataBuffer();
        unsigned int* indexbuffer = dso->indexBuffer();

        for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
            aiVector3D pos = mesh->mVertices[j];
            aiVector3D texCoord = mesh->mTextureCoords[0][j];
            databuffer[j * 6 + 0] = pos.x;
            databuffer[j * 6 + 1] = pos.y;
            databuffer[j * 6 + 2] = pos.z;
            databuffer[j * 6 + 3] = texCoord.x;
            databuffer[j * 6 + 4] = texCoord.y;
            databuffer[j * 6 + 5] = 0.0f;
        }

        for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
            for (unsigned int k = 0; k < 3; k++) {
                indexbuffer[j * 3 + k] = mesh->mFaces[j].mIndices[k];
            }
        }

        dso->updateDataBuffer(0, mesh->mNumVertices * 6 * sizeof(float));
        dso->updateIndexBuffer(0, mesh->mNumFaces * 3 * sizeof(unsigned int));
        m_sceneObjects.push_back(dso);

        // Load texture
        GLuint textureID;
        int width, height, channels;
        unsigned char* image = stbi_load(textureFiles[i].c_str(), &width, &height, &channels, STBI_rgb_alpha);
        if (!image) {
            std::cerr << "ERROR::TEXTURE::Failed to load texture: " << textureFiles[i] << std::endl;
            continue;
        }

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(image);

        m_textures.push_back(textureID);

        // Load sample data
        MyPoissonSample* sampleData = MyPoissonSample::fromFile(sampleFiles[i]);
        if (!sampleData) {
            std::cerr << "ERROR::SAMPLE::Failed to load sample file: " << sampleFiles[i] << std::endl;
            continue;
        }

        for (int j = 0; j < sampleData->m_numSample; ++j) {
            glm::vec3 position(
                sampleData->m_positions[j * 3 + 0],
                sampleData->m_positions[j * 3 + 1],
                sampleData->m_positions[j * 3 + 2]
            );
            glm::vec3 rotation(
                sampleData->m_radians[j * 3 + 0],
                sampleData->m_radians[j * 3 + 1],
                sampleData->m_radians[j * 3 + 2]
            );
            glm::mat4 modelMatrix = glm::translate(position) * glm::toMat4(glm::quat(rotation));

            InstanceData instance = { modelMatrix, boundingSphereCenters[i], boundingSphereRadii[i] };
            m_instanceData.push_back(instance);
        }

        delete sampleData; // Free sample data after processing

        // Prepare draw command
        DrawElementsIndirectCommand cmd = {
            mesh->mNumFaces * 3,  // vertexCount
            static_cast<GLuint>(sampleData->m_numSample), // instanceCount
            0,                    // firstIndex
            0,                    // baseVertex
            baseInstance           // baseInstance
        };
        drawCommands.push_back(cmd);
        baseInstance += sampleData->m_numSample;
    }

    // Load indirect draw buffer
    glGenBuffers(1, &m_indirectBuffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_indirectBuffer);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, drawCommands.size() * sizeof(DrawElementsIndirectCommand), drawCommands.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

    createSSBO();
}

void MyBushesAndBuildings::createSSBO() {
    glGenBuffers(1, &m_instanceSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_instanceSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(InstanceData) * m_instanceData.size(), m_instanceData.data(), GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_instanceSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    std::cout << "SSBO Data Size: " << m_instanceData.size() * sizeof(InstanceData) << " bytes" << std::endl;
}

void MyBushesAndBuildings::performFrustumCulling(const glm::mat4& viewMat, const glm::mat4& projMat) {
    glm::mat4 viewProj = projMat * viewMat;

    // Define frustum planes
    struct Plane {
        glm::vec3 normal;
        float distance;
    };
    Plane frustumPlanes[6];

    auto extractPlane = [&](int index, const glm::vec4& row1, const glm::vec4& row2) {
        glm::vec4 plane = row1 + (index % 2 == 0 ? row2 : -row2);
        float length = glm::length(glm::vec3(plane));
        return Plane{ glm::vec3(plane) / length, plane.w / length };
    };

    // Extract frustum planes from the view-projection matrix
    const glm::vec4& row0 = viewProj[0];
    const glm::vec4& row1 = viewProj[1];
    const glm::vec4& row2 = viewProj[2];
    const glm::vec4& row3 = viewProj[3];
    frustumPlanes[0] = extractPlane(0, row3, row0); // Left
    frustumPlanes[1] = extractPlane(1, row3, row0); // Right
    frustumPlanes[2] = extractPlane(2, row3, row1); // Bottom
    frustumPlanes[3] = extractPlane(3, row3, row1); // Top
    frustumPlanes[4] = extractPlane(4, row3, row2); // Near
    frustumPlanes[5] = extractPlane(5, row3, row2); // Far

    // Collect visible instances
    std::vector<InstanceData> visibleInstances;
    for (const auto& instance : m_instanceData) {
        glm::vec3 worldCenter = glm::vec3(instance.modelMatrix * glm::vec4(instance.boundingSphereCenter, 1.0f));
        bool isInside = true;

        for (const auto& plane : frustumPlanes) {
            float distance = glm::dot(plane.normal, worldCenter) + plane.distance;
            if (distance < -instance.boundingSphereRadius) {
                isInside = false;
                break;
            }
        }

        if (isInside) {
            visibleInstances.push_back(instance);
        }
    }

    // Update the SSBO with only visible instances
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_instanceSSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(InstanceData) * visibleInstances.size(), visibleInstances.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    std::cout << "Frustum Culling: Visible Instances = " << visibleInstances.size() << std::endl;
}


void MyBushesAndBuildings::render(const glm::mat4& viewMat, const glm::mat4& projMat) {

    // Perform frustum culling and update SSBO with culled instances
    performFrustumCulling(viewMat, projMat);

    // Bind the first VAO (assumes all models share the same structure)
    glBindVertexArray(m_sceneObjects[0]->vao());
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_indirectBuffer);

    // Loop through textures and bind them
    for (size_t i = 0; i < m_textures.size(); ++i) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_textures[i]);

        // Perform an indirect draw call, limited to the visible instances
        std::cout << "Indirect Draw Call: Scene Objects = " << m_sceneObjects.size() << std::endl;
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, m_sceneObjects.size(), 0);
        std::cout << "Indirect Draw Completed for Texture: " << i << std::endl;
    }

    // Clean up
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
    glBindVertexArray(0);
}


MyBushesAndBuildings::~MyBushesAndBuildings() {
    for (auto* dso : m_sceneObjects) {
        delete dso;
    }
    m_sceneObjects.clear();

    for (auto texture : m_textures) {
        glDeleteTextures(1, &texture);
    }

    if (m_instanceSSBO) {
        glDeleteBuffers(1, &m_instanceSSBO);
    }

    if (m_indirectBuffer) {
        glDeleteBuffers(1, &m_indirectBuffer);
    }
}
