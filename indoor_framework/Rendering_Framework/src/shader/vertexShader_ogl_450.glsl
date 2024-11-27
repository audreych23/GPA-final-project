#version 450 core

layout (location = 0) in vec3 v_vertex;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec3 v_texCoord;
layout (location = 3) in vec4 v_instancePos;

out vec3 f_worldVertex;
out vec3 f_viewVertex;
out vec3 f_neg_viewVertex;
out vec3 f_normal;
out vec3 f_texCoord;

layout (location = 0) uniform mat4 modelMat;
layout (location = 1) uniform mat4 viewMat;
layout (location = 2) uniform mat4 projMat;


void main(){
	vec4 worldVertex = modelMat * vec4(v_vertex, 1.0);
	vec4 viewVertex = viewMat * worldVertex;
	
	f_worldVertex = worldVertex.xyz;
	f_viewVertex = viewVertex.xyz;
	f_neg_viewVertex = -viewVertex.xyz;
	f_normal = mat3(viewMat * modelMat) * v_normal.xyz;
	f_texCoord = v_texCoord;

	gl_Position = projMat * viewVertex;
}