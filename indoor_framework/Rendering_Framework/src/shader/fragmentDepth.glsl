#version 450 core
in vec4 FragPos;

layout (location = 99) uniform vec3 lightPos;
const float far_plane = 10.0;

void main()
{
    float lightDistance = length(FragPos.xyz - lightPos);
    
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / far_plane;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;
}