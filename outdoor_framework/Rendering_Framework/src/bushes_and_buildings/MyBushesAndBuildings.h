#pragma once

#include <glm/mat4x4.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include "../DynamicSceneObject.h"
#include "../MyPoissonSample.h"

class MyBushesAndBuildings {
public:
    MyBushesAndBuildings();            // Constructor to initialize and load all assets
    virtual ~MyBushesAndBuildings();   // Destructor to clean up resources

    void render(const glm::mat4& viewMat, const glm::mat4& projMat); // Renders all objects

private:
    struct InstanceData {
        glm::mat4 modelMatrix;          // Model matrix for instance placement
        glm::vec3 boundingSphereCenter; // Center of the bounding sphere (object-space)
        float boundingSphereRadius;     // Radius of the bounding sphere
    };

    std::vector<DynamicSceneObject*> m_sceneObjects; // Dynamic scene objects for models
    std::vector<GLuint> m_textures;                 // Texture IDs for each model
    std::vector<InstanceData> m_instanceData;       // Per-instance data (model matrices, bounding spheres)
    GLuint m_instanceSSBO;                          // SSBO for instance data
    GLuint m_indirectBuffer;                        // Indirect draw command buffer
    MyPoissonSample* m_sampleData;                  // Poisson sample data

    void createSSBO();                              // Creates the SSBO for GPU-driven rendering
    void performFrustumCulling(const glm::mat4& viewMat, const glm::mat4& projMat); // GPU-driven frustum culling
};
