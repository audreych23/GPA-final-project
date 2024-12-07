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
    GLuint count;           // Number of indices to draw
    GLuint instanceCount;   // Number of instances to draw
    GLuint firstIndex;      // Starting index in the index buffer
    GLint baseVertex;       // Base vertex in the vertex buffer
    GLuint baseInstance;    // Base instance for instanced draws
};

MyBushesAndBuildings::MyBushesAndBuildings() 
    : m_instanceSSBO(0), m_indirectBuffer(0), m_visibleInstanceBuffer(0) {
    
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
    std::vector<InstanceData> allInstanceData;

    GLuint baseInstance = 0;
    for (size_t i = 0; i < modelFiles.size(); ++i) {
        // Load model
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(modelFiles[i], 
            aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            continue;
        }

        aiMesh* mesh = scene->mMeshes[0];
        
        // Create and setup DynamicSceneObject
        DynamicSceneObject* dso = new DynamicSceneObject(
            mesh->mNumVertices, mesh->mNumFaces * 3, false, true);
        
        float* databuffer = dso->dataBuffer();
        unsigned int* indexbuffer = dso->indexBuffer();

        // Fill vertex data
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

        // Fill index data
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
        unsigned char* image = stbi_load(textureFiles[i].c_str(), 
            &width, &height, &channels, STBI_rgb_alpha);
        
        if (!image) {
            std::cerr << "Failed to load texture: " << textureFiles[i] << std::endl;
            continue;
        }

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, 
            GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_image_free(image);
        m_textures.push_back(textureID);

        // Load and process instance data
        MyPoissonSample* sampleData = MyPoissonSample::fromFile(sampleFiles[i].c_str());
        if (!sampleData) {
            std::cerr << "Failed to load sample file: " << sampleFiles[i] << std::endl;
            continue;
        }

        // Create draw command
        DrawElementsIndirectCommand cmd{
            static_cast<GLuint>(mesh->mNumFaces * 3),  // count
            static_cast<GLuint>(sampleData->m_numSample), // instanceCount
            0,                    // firstIndex
            0,                    // baseVertex
            baseInstance          // baseInstance
        };
        drawCommands.push_back(cmd);

        // Process instance data
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

            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), position) *
                glm::toMat4(glm::quat(rotation));

            InstanceData instance{
                modelMatrix,
                boundingSphereCenters[i],
                boundingSphereRadii[i]
            };
            allInstanceData.push_back(instance);
        }

        baseInstance += sampleData->m_numSample;
        delete sampleData;
    }

    // Create and initialize buffers
    glGenBuffers(1, &m_instanceSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_instanceSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 
        sizeof(InstanceData) * allInstanceData.size(),
        allInstanceData.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_instanceSSBO);

    glGenBuffers(1, &m_indirectBuffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_indirectBuffer);
    glBufferData(GL_DRAW_INDIRECT_BUFFER, 
        sizeof(DrawElementsIndirectCommand) * drawCommands.size(),
        drawCommands.data(), GL_STATIC_DRAW);

    // Create buffer for visible instances
    glGenBuffers(1, &m_visibleInstanceBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_visibleInstanceBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 
        sizeof(InstanceData) * allInstanceData.size(),
        nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_visibleInstanceBuffer);

    m_instanceData = allInstanceData;
    std::cout << "Initialized with " << allInstanceData.size() << " total instances" << std::endl;
}

void MyBushesAndBuildings::performFrustumCulling(
    const glm::mat4& viewMat, const glm::mat4& projMat) {
    
    glm::mat4 viewProj = projMat * viewMat;
    std::vector<InstanceData> visibleInstances;

    // Extract frustum planes
    struct FrustumPlane {
        glm::vec3 normal;
        float distance;
    };
    std::vector<FrustumPlane> planes(6);

    // Extract planes from view-projection matrix
    // Left plane
    planes[0].normal.x = viewProj[0][3] + viewProj[0][0];
    planes[0].normal.y = viewProj[1][3] + viewProj[1][0];
    planes[0].normal.z = viewProj[2][3] + viewProj[2][0];
    planes[0].distance = viewProj[3][3] + viewProj[3][0];

    // Right plane
    planes[1].normal.x = viewProj[0][3] - viewProj[0][0];
    planes[1].normal.y = viewProj[1][3] - viewProj[1][0];
    planes[1].normal.z = viewProj[2][3] - viewProj[2][0];
    planes[1].distance = viewProj[3][3] - viewProj[3][0];

    // Bottom plane
    planes[2].normal.x = viewProj[0][3] + viewProj[0][1];
    planes[2].normal.y = viewProj[1][3] + viewProj[1][1];
    planes[2].normal.z = viewProj[2][3] + viewProj[2][1];
    planes[2].distance = viewProj[3][3] + viewProj[3][1];

    // Top plane
    planes[3].normal.x = viewProj[0][3] - viewProj[0][1];
    planes[3].normal.y = viewProj[1][3] - viewProj[1][1];
    planes[3].normal.z = viewProj[2][3] - viewProj[2][1];
    planes[3].distance = viewProj[3][3] - viewProj[3][1];

    // Near plane
    planes[4].normal.x = viewProj[0][3] + viewProj[0][2];
    planes[4].normal.y = viewProj[1][3] + viewProj[1][2];
    planes[4].normal.z = viewProj[2][3] + viewProj[2][2];
    planes[4].distance = viewProj[3][3] + viewProj[3][2];

    // Far plane
    planes[5].normal.x = viewProj[0][3] - viewProj[0][2];
    planes[5].normal.y = viewProj[1][3] - viewProj[1][2];
    planes[5].normal.z = viewProj[2][3] - viewProj[2][2];
    planes[5].distance = viewProj[3][3] - viewProj[3][2];

    // Normalize all planes
    for (auto& plane : planes) {
        float length = glm::length(plane.normal);
        plane.normal /= length;
        plane.distance /= length;
    }

    // Test each instance against the frustum
    for (const auto& instance : m_instanceData) {
        glm::vec3 worldCenter = glm::vec3(
            instance.modelMatrix * glm::vec4(instance.boundingSphereCenter, 1.0f));
        
        bool isVisible = true;
        for (const auto& plane : planes) {
            float distance = glm::dot(plane.normal, worldCenter) + plane.distance;
            if (distance < -instance.boundingSphereRadius) {
                isVisible = false;
                break;
            }
        }

        if (isVisible) {
            visibleInstances.push_back(instance);
        }
    }

    // Update visible instance buffer
    if (!visibleInstances.empty()) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_visibleInstanceBuffer);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
            sizeof(InstanceData) * visibleInstances.size(),
            visibleInstances.data());
    }

    std::cout << "Visible instances: " << visibleInstances.size() << std::endl;
}

void MyBushesAndBuildings::render(const glm::mat4& viewMat, const glm::mat4& projMat) {
    // Set isInstanced uniform to true
    //performFrustumCulling(viewMat, projMat);
    glUniform1i(10, true);  // location 10 is isInstanced

    // Bind the first VAO (assumes all models share the same structure)
    glBindVertexArray(m_sceneObjects[0]->vao());
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, m_indirectBuffer);

    // Loop through textures and bind them
    for (size_t i = 0; i < m_textures.size(); ++i) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_textures[i]);

        // Perform indirect draw
        glMultiDrawElementsIndirect(
            GL_TRIANGLES,
            GL_UNSIGNED_INT,
            (void*)(i * sizeof(DrawElementsIndirectCommand)),
            1,
            0
        );
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

    if (m_visibleInstanceBuffer) {
        glDeleteBuffers(1, &m_visibleInstanceBuffer);
    }
}
