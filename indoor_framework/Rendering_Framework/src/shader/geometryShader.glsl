#version 450 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

layout (location = 100) uniform mat4 shadowMatrices0;
layout (location = 101) uniform mat4 shadowMatrices1;
layout (location = 102) uniform mat4 shadowMatrices2;
layout (location = 103) uniform mat4 shadowMatrices3;
layout (location = 104) uniform mat4 shadowMatrices4;
layout (location = 105) uniform mat4 shadowMatrices5;

out vec4 FragPos;

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face;
        for(int i = 0; i < 3; ++i)
        {
            FragPos = gl_in[i].gl_Position;
            if(face == 0) gl_Position = shadowMatrices0 * FragPos;
            if(face == 1) gl_Position = shadowMatrices1 * FragPos;
            if(face == 2) gl_Position = shadowMatrices2 * FragPos;
            if(face == 3) gl_Position = shadowMatrices3 * FragPos;
            if(face == 5) gl_Position = shadowMatrices4 * FragPos;
            if(face == 6) gl_Position = shadowMatrices5 * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}  