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
} vertexData;

layout (location = 0) uniform mat4 modelMat;
layout (location = 1) uniform mat4 viewMat;
layout (location = 2) uniform mat4 projMat;

// light pos is in world space
vec3 lightPosition = vec3(-2.845, 2.028, -1.293);

void main() {
    // Transform vertex position to world space
    vec4 worldVertex = modelMat * vec4(v_vertex, 1.0);
    vec3 worldPosition = worldVertex.xyz;

    // Transform normal and tangent to world space
    vec3 worldNormal = normalize(mat3(modelMat) * v_normal);
    vec3 worldTangent = normalize(mat3(modelMat) * v_tangents);
    vec3 worldBitangent = cross(worldNormal, worldTangent);

    // Compute light vector in world space
    vec3 worldLightVector = normalize(lightPosition - worldPosition);

    // Compute view vector in world space
    vec3 worldViewVector = normalize(-worldPosition);

    // Compute halfway vector in world space
    vec3 worldHalfwayVector = normalize(worldLightVector + worldViewVector);

    // Pass world space data to fragment shader
    vertexData.N = worldNormal;
    vertexData.L = worldLightVector;
    vertexData.H = worldHalfwayVector;
    vertexData.texCoord = v_texCoord;

    // For normal mapping, transform light and view directions into tangent space
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

    // Transform vertex position to clip space for rendering
    gl_Position = projMat * viewMat * worldVertex;
}