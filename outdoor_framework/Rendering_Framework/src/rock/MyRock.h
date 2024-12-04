#pragma once

#include <glm\gtx\quaternion.hpp>
#include <glm\mat4x4.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <glm\gtx\rotate_vector.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <vector>
#include <iostream>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\cimport.h>
#include <assimp\postprocess.h>
#include "../MyCameraManager.h"
#include "../DynamicSceneObject.h"
#include "stb_image.h"

class MyRock {
  public:
    MyRock();
    virtual ~MyRock();
    void render();
    void update(const glm::mat4& viewMat, const glm::vec3& viewPos);

    DynamicSceneObject* dso;
    GLuint gTex;
    GLuint gNorm;
};