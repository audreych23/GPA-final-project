#version 450 core

uniform sampler2D screenTexture;
uniform sampler2D blurTexture;
uniform sampler2D ambientTexture;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
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

void ToonFinalEffect() {
	/* Convolution of edge detection */
    const float kernelX[9] = float[9](
        -1,  0,  1,
        -2,  0,  2,
        -1,  0,  1
    );
    const float kernelY[9] = float[9](
        -1, -2, -1,
         0,  0,  0,
         1,  2,  1
    );
	const float offsetX[9] = float[9](
		-1, 0, 1,
		-1, 0, 1,
		-1, 0, 1
	);
    const vec2 offsets[9] = vec2[9](
        vec2(-1, -1), vec2( 0, -1), vec2( 1, -1),
        vec2(-1,  0), vec2( 0,  0), vec2( 1,  0),
        vec2(-1,  1), vec2( 0,  1), vec2( 1,  1)
    );

	vec2 offset = 1.0 / textureSize(screenTexture, 0);

	float sumx = 0.0;
    float sumy = 0.0;
    for (int i = 0; i < 9; i++) {
        vec3 sampleColor = texture(screenTexture, fs_in.texcoord + offsets[i] * offset).rgb;
        float grayScale = dot(sampleColor, vec3(0.333));
        sumx += grayScale * kernelX[i];
        sumy += grayScale * kernelY[i];
    }

	float final = sqrt(sumx * sumx + sumy * sumy);
	fragColor = vec4(vec3(texture(screenTexture, fs_in.texcoord) - final), 1.0);
}


void DefferedShading() {
    if (postSubProcess == 0) {
        fragColor = vec4(texture(screenTexture, fs_in.texcoord).xyz, 1.0);
    } else if (postSubProcess == 1) {
        fragColor = vec4(texture(blurTexture, fs_in.texcoord).xyz, 1.0);
    } else if (postSubProcess == 2) {
        fragColor = vec4(texture(ambientTexture, fs_in.texcoord).xyz, 1.0);
    } else if (postSubProcess == 3) {
        fragColor = vec4(texture(diffuseTexture, fs_in.texcoord).xyz, 1.0);
    } else { 
        fragColor = vec4(texture(specularTexture, fs_in.texcoord).xyz, 1.0);
    }
}

float LinearizeDepth(float depth)
{
	const float near_plane = 0.1f;
	const float far_plane = 10f;
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

void main()
{	
	if (postProcessingEffect == 0) {
		RegularEffect();
	} 
	else if (postProcessingEffect == 1) {
		if (postSubProcess == 0) {
			// blur
			BloomBlurEffect();
		} 
		else {
			// final bloom
			BloomFinalEffect();
		}
	}
	else if (postProcessingEffect == 2){
		DefferedShading();
	} else if (postProcessingEffect == 3) {
		float depthValue = texture(screenTexture, fs_in.texcoord).r;
		float tt = LinearizeDepth(depthValue);
		fragColor = vec4(vec3(tt), 1.0);
	}
	else if(postProcessingEffect == 4){
		if (postSubProcess == 0) {
			// blur
			BloomBlurEffect();
		} 
		else if(postSubProcess == 1){
			BloomFinalEffect();
		}
		else {
			// Toon final bloom
			ToonFinalEffect();
		}
	}
}