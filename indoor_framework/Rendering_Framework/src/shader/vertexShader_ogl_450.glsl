#version 450 core

layout (location = 0) in vec3 v_vertex;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec3 v_texCoord;
layout (location = 3) in vec4 v_instancePos;

out VertexData {
	vec3 N; // view space normal
	vec3 L; // view space light vector
	vec3 H; // view space halfway vector
	vec3 texCoord;
} vertexData;

layout (location = 0) uniform mat4 modelMat;
layout (location = 1) uniform mat4 viewMat;
layout (location = 2) uniform mat4 projMat;

vec3 lightPosition = vec3(-2.845, 2.028, -1.293);

void main(){
	vec4 worldVertex = modelMat * vec4(v_vertex, 1.0);
	vec4 viewVertex = viewMat * worldVertex;
	vec3 viewLightPosition = mat3(viewMat) * lightPosition;
	
	vertexData.texCoord = v_texCoord;
	vertexData.N = mat3(viewMat * modelMat) * v_normal;
	vertexData.L = normalize(viewLightPosition - vec3(viewVertex));
	vec3 V = normalize(-viewVertex).xyz;
	vertexData.H = normalize(vertexData.L + V);

	gl_Position = projMat * viewVertex;
}