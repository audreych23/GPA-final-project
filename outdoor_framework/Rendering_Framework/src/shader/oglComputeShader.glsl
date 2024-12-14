#version 450 core

// layout (local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;

// struct DrawCommand {
//     uint count;
//     uint instanceCount;
//     uint firstIndex;
//     uint baseVertex;
//     uint baseInstance;
// };

// struct InstanceProperties {
//     // modified
//     vec4 position;
//     vec4 is_eaten;
// };

// struct InstancePropertiesOut {
//     vec4 position;
// };

// layout (std430, binding=3) buffer DrawCommandsBlock {
//     DrawCommand commands[];
// };

// layout (std430, binding=1) buffer InstanceData {
//     InstanceProperties rawInstanceProps[];
// };

// layout (std430, binding=2) buffer CurrValidInstanceData {
//     InstancePropertiesOut currValidInstanceProps[];
// };

// layout (location = 6) uniform vec4 slimePos;
// layout (location = 3) uniform int numMaxInstance;
// layout (location = 2) uniform mat4 projMat;
// layout (location = 1) uniform mat4 viewMat;

// const float radius = 1.5f;
void main() {
    // const uint idx = gl_GlobalInvocationID.x;
    
    // // discarding invalid array - access
    // if(idx >= numMaxInstance) { return; }

    // // translate the position to clip space
    // vec4 clipSpaceV = projMat * viewMat * vec4(rawInstanceProps[idx].position.xyz, 1.0);

    // clipSpaceV = clipSpaceV / clipSpaceV.w;

    // // determine if it is culled
    // bool frustumCulled = max(max(abs(clipSpaceV.x) - 1.0, abs(clipSpaceV.y) - 1.0), abs(clipSpaceV.z) - 1.0) > 0.0;


    // vec3 dist = rawInstanceProps[idx].position.xyz - slimePos.xyz;

    // rawInstanceProps[idx].is_eaten.x = float((rawInstanceProps[idx].is_eaten.x == 1.0f) || (dot(dist, dist) <= radius * radius));

    // if(!frustumCulled && rawInstanceProps[idx].is_eaten.x == 0.0f) {
    //     // get UNIQUE buffer location for assigning the instance data
    //     // it also updates instanceCount
    //     const uint UNIQUE_IDX = atomicAdd(commands[uint(rawInstanceProps[idx].position.w)].instanceCount, 1);
    //     // put data into valid instance buffer
    //     currValidInstanceProps[UNIQUE_IDX + commands[uint(rawInstanceProps[idx].position.w)].baseInstance].position = rawInstanceProps[idx].position;
    // }

}