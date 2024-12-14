#version 430 core


layout (local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;

struct DrawCommand {
    uint count;
    uint instanceCount;
    uint firstIndex;
    uint baseVertex;
    uint baseInstance;
};

struct InstanceProperties {
    // modified
    vec4 position;
};

struct InstancePropertiesOut {
    int index;
};

layout (std430, binding=3) buffer DrawCommandsBlock {
    DrawCommand commands[];
};

layout (std430, binding=1) buffer InstanceData {
    InstanceProperties rawInstanceProps[];
};

layout (std430, binding=2) buffer CurrValidInstanceData {
    InstancePropertiesOut currValidInstanceProps[];
};

// layout (location = 6) uniform vec4 slimePos;
layout (location = 11) uniform int numMaxInstance;
// layout (location = 2) uniform mat4 projMat;
// layout (location = 1) uniform mat4 viewMat;

// hardcoded bounding sphere spec
uniform float radius[5] = float[] (3.4, 2.6, 1.4, 8.5, 10.2);
uniform float center[5] = float[] (2.55, 1.76, 0.66, 4.57, 4.57);

// const float radius = 1.5f;
void main() {
    const uint idx = gl_GlobalInvocationID.x;
    
    // // discarding invalid array - access
    if(idx >= numMaxInstance) { return; }

    // // translate the position to clip space
    // vec4 clipSpaceV = projMat * viewMat * vec4(rawInstanceProps[idx].position.xyz, 1.0);

    // clipSpaceV = clipSpaceV / clipSpaceV.w;

    // // determine if it is culled
    // bool frustumCulled = max(max(abs(clipSpaceV.x) - 1.0, abs(clipSpaceV.y) - 1.0), abs(clipSpaceV.z) - 1.0) > 0.0;

    // doing the frustum culling
    bool frustumCulled = false;

    if(!frustumCulled) {
        // get UNIQUE buffer location for assigning the instance data
        // it also updates instanceCount
        const uint UNIQUE_IDX = atomicAdd(commands[uint(rawInstanceProps[idx].position.w)].instanceCount, 1);
        // put data into valid instance buffer
        currValidInstanceProps[UNIQUE_IDX + commands[uint(rawInstanceProps[idx].position.w)].baseInstance].index = int(idx);
    }

}