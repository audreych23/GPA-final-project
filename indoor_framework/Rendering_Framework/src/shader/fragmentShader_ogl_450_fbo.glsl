#version 410 core

uniform sampler2D screenTexture;
// uniform sampler2D

out vec4 color;

in VS_OUT
{
	vec2 texcoord;
} fs_in;

void main()
{
	// #TODO 9: Do Red-Blue Stereo here 
	// BEGIN ANSWER
	color = vec4(texture(screenTexture, fs_in.texcoord).xyz, 1.0);
	// color = vec4(texture(screenTexture, vec2(fs_in.texcoord.x - offset, fs_in.texcoord.y)).xyz, 1.0);
    // color = vec4(1.0, 0.0, 0.0, 1.0);
	// vec4 left_color = vec4(texture(screenTexture, vec2(fs_in.texcoord.x - offset, fs_in.texcoord.y)).xyz, 1.0);
	// vec4 right_color = vec4(texture(screenTexture, vec2(fs_in.texcoord.x + offset, fs_in.texcoord.y)).xyz, 1.0);

	// color.r = left_color.r * 0.299 + left_color.g * 0.587 + left_color.b * 0.114;
	// color.g = right_color.g;
	// color.b = right_color.b;
	// END ANSWER
}