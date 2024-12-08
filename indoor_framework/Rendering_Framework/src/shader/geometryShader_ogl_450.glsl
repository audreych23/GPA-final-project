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

layout (location = 4) uniform int postProcessId;

layout (location = 13) uniform mat4 lightProjMat;
layout (location = 20) uniform int PostProcessMain;

layout (location = 40) uniform mat4 lightViewMat0;
layout (location = 41) uniform mat4 lightViewMat1;
layout (location = 42) uniform mat4 lightViewMat2;
layout (location = 43) uniform mat4 lightViewMat3;
layout (location = 44) uniform mat4 lightViewMat4;
layout (location = 45) uniform mat4 lightViewMat5;

void main() {
    if (PostProcessMain == 6) {
        if (postProcessId == 0) {
            for(int face = 0; face < 6; ++face)
            {
                gl_Layer = face; // built-in variable that specifies to which face we render.
                for(int i = 0; i < 3; ++i) // for each triangle's vertices
                {
                    f_worldPosition = vec3(gl_in[i].gl_Position);
                    mat4 shadowViewMat;
                    if (face == 0) { shadowViewMat = lightViewMat0; }
                    else if (face == 1) { shadowViewMat = lightViewMat1; }
                    else if (face == 2) { shadowViewMat = lightViewMat2; }
                    else if (face == 3) { shadowViewMat = lightViewMat3; }
                    else if (face == 4) { shadowViewMat = lightViewMat4; }
                    else if (face == 5) { shadowViewMat = lightViewMat5; }
                    gl_Position = lightProjMat * shadowViewMat * vec4(f_worldPosition, 1.0);
                    EmitVertex();
                }    
                EndPrimitive();
            }
        } else {
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
    } else { 
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
}