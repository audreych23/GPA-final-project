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

void main(){
	mat4 mvMat = viewMat * modelMat;
	vec4 viewVertex = mvMat * vec4(v_vertex, 1.0);
	vec3 viewLightPosition = mat3(viewMat) * lightPosition;
	
	vec3 T = normalize(mat3(mvMat) * v_tangents);
	vec3 N = normalize(mat3(mvMat) * v_normal);
	vec3 B = cross(N, T);

	vec3 L = normalize(viewLightPosition - vec3(viewVertex));
	vertexData.lightDirNormalMapping = normalize(vec3(dot(L, T), dot(L, B), dot(L, N)));

	vec3 V = normalize((-viewVertex).xyz);
	vertexData.eyeDirNormalMapping = normalize(vec3(dot(V, T), dot(V, B), dot(V, N)));

	vertexData.H = normalize(L + V);
	vertexData.N = N;
	vertexData.L = L;
	vertexData.texCoord = v_texCoord;  

	gl_Position = projMat * viewVertex;
}