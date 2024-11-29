#version 450 core

layout (location = 0) in vec3 v_vertex;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec3 v_texCoord;
layout (location = 3) in vec3 v_tangents;

out VertexData {
	vec3 N; // view space normal
	vec3 L; // view space light vector
	vec3 H; // view space halfway vector
	vec3 texCoord;

	// for normal mapping
	vec3 lightDirNormalMapping;
	vec3 eyeDirNormalMapping;
    vec3 halfwayDirNormalMapping;
} vertexData;

layout (location = 0) uniform mat4 modelMat;
layout (location = 1) uniform mat4 viewMat;
layout (location = 2) uniform mat4 projMat;
layout (location = 3) uniform vec3 cameraPosition;

// light pos is in world space
vec3 lightPosition = vec3(-2.845, 2.028, -1.293);

void main() {
    // For some reason we do it in world space and it works
    vec4 worldVertex = modelMat * vec4(v_vertex, 1.0);
    vec3 worldPosition = worldVertex.xyz;

    vec3 worldNormal = normalize(mat3(modelMat) * v_normal);
    vec3 worldTangent = normalize(mat3(modelMat) * v_tangents);
    vec3 worldBitangent = cross(worldNormal, worldTangent);

    vec3 worldLightVector = normalize(lightPosition - worldPosition);

    vec3 worldViewVector = normalize(cameraPosition - worldPosition);

    vec3 worldHalfwayVector = normalize(worldLightVector + worldViewVector);

    vertexData.N = worldNormal;
    vertexData.L = worldLightVector;
    vertexData.H = worldHalfwayVector;
    vertexData.texCoord = v_texCoord;

    vertexData.lightDirNormalMapping = normalize(vec3(
        dot(worldLightVector, worldTangent),
        dot(worldLightVector, worldBitangent),
        dot(worldLightVector, worldNormal)
    ));

    vertexData.eyeDirNormalMapping = normalize(vec3(
        dot(worldViewVector, worldTangent),
        dot(worldViewVector, worldBitangent),
        dot(worldViewVector, worldNormal)
    ));

    vertexData.halfwayDirNormalMapping = normalize(vec3(
        dot(worldHalfwayVector, worldTangent),
        dot(worldHalfwayVector, worldBitangent),
        dot(worldHalfwayVector, worldNormal)
    ));

    gl_Position = projMat * viewMat * worldVertex;
}