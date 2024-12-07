#version 450 core

layout(triangles) in;  // Input primitive type: a triangle
layout(triangle_strip, max_vertices = 18) out;  // Output primitive type: a triangle

in VertexData {
	vec3 N; // view space normal
	vec3 L; // view space light vector
	vec3 H; // view space halfway vector
	vec3 texCoord;
    vec4 shadowCoord;

	// for normal mapping
	vec3 lightDirNormalMapping;
	vec3 eyeDirNormalMapping;
    vec3 halfwayDirNormalMapping;
} g_vertexData[];

in vec3 g_worldPosition[];

out VertexData
{
    vec3 N; // eye space normal
    vec3 L; // eye space light vector
    vec3 H; // eye space halfway vector
    vec3 texCoord;
	vec4 shadowCoord;

	// for normal mapping
	vec3 lightDirNormalMapping;
	vec3 eyeDirNormalMapping;
	vec3 halfwayDirNormalMapping;
} f_vertexData;

out vec3 f_worldPosition;

void main() {

    // pass through geometry shader
    for (int i = 0; i < 3; ++i) {
        f_vertexData.N = g_vertexData[i].N;
        f_vertexData.L = g_vertexData[i].L;
        f_vertexData.H = g_vertexData[i].H;
        f_vertexData.texCoord = g_vertexData[i].texCoord;
        f_vertexData.shadowCoord = g_vertexData[i].shadowCoord;

        f_vertexData.lightDirNormalMapping = g_vertexData[i].lightDirNormalMapping;
        f_vertexData.eyeDirNormalMapping = g_vertexData[i].eyeDirNormalMapping;
        f_vertexData.halfwayDirNormalMapping = g_vertexData[i].halfwayDirNormalMapping;
        f_worldPosition = g_worldPosition[i];
        gl_Position = gl_in[i].gl_Position;  // Output the vertex as is
        EmitVertex();  // Emit vertex to the output stream
    }
    EndPrimitive();  // End the primitive
}