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
layout (location = 10) uniform int numMaxInstance;
// layout (location = 2) uniform mat4 projMat;
// layout (location = 1) uniform mat4 viewMat;
layout (location = 15) uniform vec4 planeEquation1;
layout (location = 16) uniform vec4 planeEquation2;
layout (location = 17) uniform vec4 planeEquation3;
layout (location = 18) uniform vec4 planeEquation4;
layout (location = 19) uniform vec4 planeEquation5;
layout (location = 20) uniform vec4 planeEquation6;

// hardcoded bounding sphere spec
uniform float radius[5] = float[] (3.4, 2.6, 1.4, 8.5, 10.2);
uniform float center[5] = float[] (2.55, 1.76, 0.66, 4.57, 4.57);

// const float radius = 1.5f;
void main() {
    const uint idx = gl_GlobalInvocationID.x;
    
    // // discarding invalid array - access
    if(idx >= numMaxInstance) { return; }

    bool frustumCulled = false;

    uint objectId = uint(rawInstanceProps[idx].position.w);
    vec3 worldCenter = vec3(
        rawInstanceProps[idx].position * (center[objectId], 1.0f)
    );

    for (int i = 0; i < 6; ++i) {
        float distance = 0;
        // lack of time no time to think ;)
        if (i == 0) distance = dot(planeEquation1.xyz, worldCenter) + planeEquation1.w;
        else if (i == 1) distance = dot(planeEquation2.xyz, worldCenter) + planeEquation2.w;
        else if (i == 2) distance = dot(planeEquation3.xyz, worldCenter) + planeEquation3.w;
        else if (i == 3) distance = dot(planeEquation4.xyz, worldCenter) + planeEquation4.w;
        else if (i == 4) distance = dot(planeEquation5.xyz, worldCenter) + planeEquation5.w;
        else if (i == 5) distance = dot(planeEquation6.xyz, worldCenter) + planeEquation6.w;

        if (distance < -radius[objectId]) {
            frustumCulled = true;
        }
    }

    if(!frustumCulled) {
        // get UNIQUE buffer location for assigning the instance data
        // it also updates instanceCount
        const uint UNIQUE_IDX = atomicAdd(commands[objectId].instanceCount, 1);
        // put data into valid instance buffer
        currValidInstanceProps[UNIQUE_IDX + commands[objectId].baseInstance].index = int(idx);
    }

}