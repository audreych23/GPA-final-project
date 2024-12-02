#version 450 core

uniform sampler2D screenTexture;
uniform sampler2D blurTexture;
// uniform sampler2D

out vec4 fragColor;

in VS_OUT
{
	vec2 texcoord;
} fs_in;


layout (location = 0) uniform int postProcessingEffect;
// step by step for different postProcessing 
layout (location = 1) uniform int postSubProcess;

layout (location = 2) uniform bool horizontal;

const float exposure = 1.2;

uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void RegularEffect() {
	fragColor = vec4(texture(screenTexture, fs_in.texcoord).xyz, 1.0);
}

void BloomBlurEffect() {
	vec2 tex_offset = 1.0 / textureSize(screenTexture, 0); // gets size of single texel
	vec3 result = texture(screenTexture, fs_in.texcoord).rgb * weight[0];
	if(horizontal)
	{
		for(int i = 1; i < 5; ++i)
		{
		result += texture(screenTexture, fs_in.texcoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
		result += texture(screenTexture, fs_in.texcoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
		}
	}
	else
	{
		for(int i = 1; i < 5; ++i)
		{
			result += texture(screenTexture, fs_in.texcoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
			result += texture(screenTexture, fs_in.texcoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
		}
	}
	fragColor = vec4(result, 1.0);
}

void BloomFinalEffect() {
	const float gamma = 2.2;
    vec3 hdrColor = texture(screenTexture, fs_in.texcoord).rgb;      
    vec3 bloomColor = texture(blurTexture, fs_in.texcoord).rgb;
    // if(bloom)
	hdrColor += bloomColor; // additive blending
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    fragColor = vec4(hdrColor, 1.0);
}

void main()
{
	if (postProcessingEffect == 0) {
		RegularEffect();
	} else if (postProcessingEffect == 1) {
		if (postSubProcess == 0) {
			// blur
			BloomBlurEffect();
		} else {
			// final bloom
			BloomFinalEffect();
		}
	}
}