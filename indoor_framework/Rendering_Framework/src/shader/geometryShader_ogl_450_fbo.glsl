#version 450 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

layout (location = 20) uniform int PostProcessMain;

out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main()
{

} 