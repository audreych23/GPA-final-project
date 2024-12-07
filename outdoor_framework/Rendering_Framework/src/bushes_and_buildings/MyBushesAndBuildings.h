#pragma once
#include "../DynamicSceneObject.h"
#include "../MyPoissonSample.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>

class MyBushesAndBuildings {
public:
    MyBushesAndBuildings();
    ~MyBushesAndBuildings();

    void render(const glm::mat4& viewMat, const glm::mat4& projMat);
    void performFrustumCulling(const glm::mat4& viewMat, const glm::mat4& projMat);

private:
    void createSSBO();

    std::vector<DynamicSceneObject*> m_sceneObjects;
    std::vector<unsigned int> m_textures;
    
    // Instance data storage
    struct InstanceData {
        glm::mat4 modelMatrix;
        glm::vec3 boundingSphereCenter;
        float boundingSphereRadius;
    };
    std::vector<InstanceData> m_instanceData;

    // OpenGL buffer objects
    GLuint m_instanceSSBO;         // Main instance data buffer
    GLuint m_indirectBuffer;       // Indirect draw commands buffer
    GLuint m_visibleInstanceBuffer; // Buffer for visible instances after culling
};
