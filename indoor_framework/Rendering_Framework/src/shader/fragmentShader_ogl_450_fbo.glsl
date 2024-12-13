#version 450 core

layout(binding = 0) uniform sampler2D screenTexture; // gFinalImage
layout(binding = 1) uniform sampler2D blurTexture;
layout(binding = 2) uniform sampler2D inColor3;
layout(binding = 3) uniform sampler2D inColor4;
layout(binding = 4) uniform sampler2D inColor5;
layout(binding = 5) uniform sampler2D inColor6;
layout(binding = 6) uniform sampler2D inColor7; // gNormal, w=isMetal
layout(binding = 7) uniform sampler2D inColor8;  // Sampler2D Shadow,

out vec4 fragColor;

in VS_OUT
{
	vec2 texcoord;
} fs_in;


layout (location = 0) uniform int postProcessingEffect;
// step by step for different postProcessing 
layout (location = 1) uniform int postSubProcess;
layout (location = 2) uniform bool horizontal;
/// Indicate where is the light source on the screen (2D position)
layout (location = 3) uniform vec2 lightPositionOnScreen;
layout (location = 25) uniform bool hasDirectionalLight;
layout (location = 26) uniform bool hasToon;
layout (location = 28) uniform bool hasFXAA;

layout (location = 69) uniform mat4 projectionMat;
layout (location = 70) uniform vec3 samples[64];

layout (location = 50) uniform mat4 invView;
layout (location = 51) uniform mat4 projection;
layout (location = 52) uniform mat4 invProjection;
layout (location = 53) uniform mat4 view;	

vec4 SSR();

const float exposure = 1.2;

uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

const vec2 noiseScale = vec2(1344.0 / 4.0, 756.0 / 4.0);
float CalculateSSAO(){
	const float radius = 0.5;
	const float bias = 0.025;

	vec3 fragPos = texture(screenTexture, fs_in.texcoord).rgb;
    vec3 normal = normalize(texture(blurTexture, fs_in.texcoord).rgb);
    vec3 randomVec = normalize(texture(inColor6, fs_in.texcoord * noiseScale).xyz);
	// vec3 randomVec = vec3(-0.1, -0.1, 0.0);

    // Create TBN matrix for hemisphere orientation
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // Accumulate SSAO
    float occlusion = 0.0;
    for (int i = 0; i < 64; ++i) {
        // Sample position in tangent space
        vec3 msample = TBN * samples[i];
        msample = fragPos + msample * radius;

        // Project sample position into screen space
        vec4 offset = projectionMat * vec4(msample, 1.0);
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;
		
        float sampleDepth = texture(screenTexture, offset.xy).z;

        // Occlusion check
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= msample.z + bias ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / 64.0);
   	return occlusion;
	// return 1.0;
}

void RegularEffect() {
	fragColor = vec4(texture(screenTexture, fs_in.texcoord).xyz, 1.0);
}

vec4 VolumetricLight()
{
	if(!hasDirectionalLight) return vec4(0.0, 0.0, 0.0, 0.0);
	// fragColor = texture2D(blurTexture, fs_in.texcoord);
	// return;
	const float decay=0.96815;
	const float exposure=0.2;
	const float density=0.926;
	const float weight=0.58767;
	const float sample_weight=0.4;
	/// NUM_SAMPLES will describe the rays quality, you can play with
	const int NUM_SAMPLES = 100;
	vec2 tc = fs_in.texcoord;
	vec2 deltaTexCoord = (tc - lightPositionOnScreen);
	deltaTexCoord = deltaTexCoord * 1.0 / float(NUM_SAMPLES) * density;
	float illuminationDecay = 1.0;
	vec4 color = texture2D(inColor3, tc) * sample_weight;
	for(int i=0; i < NUM_SAMPLES; i++)
	{
		tc = tc - deltaTexCoord;
		vec4 sampled = texture2D(inColor3, tc) * sample_weight;
		sampled = sampled * illuminationDecay * weight;
		color = color + sampled;
		illuminationDecay = illuminationDecay * decay;
	}
	//return vec4(1.0, 0.0, 0.0, 1.0);
	return color * exposure;
	// fragColor = realColor;
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

#define FXAA_REDUCE_MIN (1.0/128.0)
#define FXAA_REDUCE_MUL (1.0/8.0)
#define FXAA_SPAN_MAX 8.0

vec3 calculateFXAA() {	
    vec3 luma = vec3(0.299, 0.587, 0.114);
	vec2 screenResolution = textureSize(screenTexture, 0);
    float lumaNW = dot(texture(screenTexture, fs_in.texcoord + vec2(-1.0, -1.0) / screenResolution).rgb, luma);
    float lumaNE = dot(texture(screenTexture, fs_in.texcoord + vec2(1.0, -1.0) / screenResolution).rgb, luma);
    float lumaSW = dot(texture(screenTexture, fs_in.texcoord + vec2(-1.0, 1.0) / screenResolution).rgb, luma);
    float lumaSE = dot(texture(screenTexture, fs_in.texcoord + vec2(1.0, 1.0) / screenResolution).rgb, luma);
    float lumaM = dot(texture(screenTexture, fs_in.texcoord).rgb, luma);
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y = ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);

    dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),
              max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
                  dir * rcpDirMin)) / screenResolution;

    vec3 rgbA = 0.5 * (
        texture(screenTexture, fs_in.texcoord + dir * (1.0 / 3.0 - 0.5)).rgb +
        texture(screenTexture, fs_in.texcoord + dir * (2.0 / 3.0 - 0.5)).rgb);
    vec3 rgbB = rgbA * 0.5 + 0.25 * (
        texture(screenTexture, fs_in.texcoord + dir * 0.5).rgb +
        texture(screenTexture, fs_in.texcoord - dir * 0.5).rgb);

    return rgbB;
}

void BloomFinalEffect() {
	/* BLOOM EFFECT */
	const float gamma = 2.2;
	vec3 hdrColor = texture(screenTexture, fs_in.texcoord).rgb;      
	if(hasFXAA) hdrColor = calculateFXAA();
    vec3 bloomColor = texture(blurTexture, fs_in.texcoord).rgb;
	hdrColor += bloomColor; // additive blending
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure); // tone mapping
    result = pow(result, vec3(1.0 / gamma)); // also gamma correct while we're at it      
	result = hdrColor;
    vec4 outColor = vec4(result, 1.0) + VolumetricLight();

    float Metallic;
    Metallic = texture2D(inColor7, fs_in.texcoord).a;

    if(Metallic < 0.5){
        fragColor = outColor;
    }
    else{
        
        fragColor = outColor + SSR();
    }
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
    fragColor = vec4(vec3(texture(screenTexture, fs_in.texcoord).rgb - final), 1.0);
}


void DefferedShading() {
    if (postSubProcess == 0) {
        fragColor = vec4(texture(screenTexture, fs_in.texcoord).xyz, 1.0);
    } else if (postSubProcess == 1) {
        fragColor = vec4(texture(blurTexture, fs_in.texcoord).xyz, 1.0);
    } else if (postSubProcess == 2) {
        fragColor = vec4(texture(inColor3, fs_in.texcoord).xyz, 1.0);
    } else if (postSubProcess == 3) {
        fragColor = vec4(texture(inColor4, fs_in.texcoord).xyz, 1.0);
    } else if (postSubProcess == 4){ 
        fragColor = vec4(texture(inColor5, fs_in.texcoord).xyz, 1.0);
    }
	else{
		float ssao = CalculateSSAO();
		fragColor = vec4(ssao, ssao, ssao, 1.0);
		// fragColor = vec4(1.0);
	}
}

float LinearizeDepth(float depth)
{
	const float near_plane = 0.1f;
	const float far_plane = 10.0f;
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));	
}

/* ===================================== SSR ========================================== */
bool rayIsOutofScreen(vec2 ray){
	return (ray.x > 1 || ray.y > 1 || ray.x < 0 || ray.y < 0) ? true : false;
}

vec3 TraceRay(vec3 rayPos, vec3 dir, int iterationCount){
	float sampleDepth;
	vec3 hitColor = vec3(0, 0, 0);
	bool hit = false;
    float bias = 0.0005;

	for(int i = 0; i <= iterationCount * 2; i++){
        if(i == iterationCount){
            return vec3(0, 0, 0);
        }
		rayPos += dir / 2;
		if(rayIsOutofScreen(rayPos.xy)){
			return vec3(0, 0, 0);
		}

		sampleDepth = texture(inColor8, rayPos.xy).r;
        hitColor = texture(screenTexture, rayPos.xy).rgb;
		float depthDif = rayPos.z - sampleDepth - bias;
		if(depthDif >= 0 && depthDif < 0.0005){
			hit = true;
			hitColor = texture(screenTexture, rayPos.xy).rgb;
			break;
		}
        
	}
	return hitColor;
}

vec4 SSR()
{
    float SCR_WIDTH = textureSize(screenTexture, 0).x;
    float SCR_HEIGHT = textureSize(screenTexture, 0).y;

    float maxRayDistance = 100.0f;

	//View Space ray calculation
	vec3 pixelPositionTexture;
	pixelPositionTexture.xy = vec2(gl_FragCoord.x / SCR_WIDTH,  gl_FragCoord.y / SCR_HEIGHT);
	vec3 normalView = (texture(inColor7, pixelPositionTexture.xy) - 0.5).rgb * 2;	
	float pixelDepth = texture(inColor8, pixelPositionTexture.xy).r;
	pixelPositionTexture.z = pixelDepth;		
	vec4 positionView = invProjection * vec4(pixelPositionTexture * 2 - vec3(1), 1);
	positionView /= positionView.w;
	vec3 reflectionView = normalize(reflect(positionView.xyz, normalView));
	if(reflectionView.z > 0){
		return vec4(0, 0, 0, 1);
	}
	vec3 rayEndPositionView = positionView.xyz + reflectionView * maxRayDistance;


	//Texture Space ray calculation
	vec4 rayEndPositionTexture = projection * vec4(rayEndPositionView,1);
	rayEndPositionTexture /= rayEndPositionTexture.w;
	rayEndPositionTexture.xyz = (rayEndPositionTexture.xyz + vec3(1)) / 2.0f;
	vec3 rayDirectionTexture = rayEndPositionTexture.xyz - pixelPositionTexture;

	ivec2 screenSpaceStartPosition = ivec2(pixelPositionTexture.x * SCR_WIDTH, pixelPositionTexture.y * SCR_HEIGHT); 
	ivec2 screenSpaceEndPosition = ivec2(rayEndPositionTexture.x * SCR_WIDTH, rayEndPositionTexture.y * SCR_HEIGHT); 
	ivec2 screenSpaceDistance = screenSpaceEndPosition - screenSpaceStartPosition;
	int screenSpaceMaxDistance = max(abs(screenSpaceDistance.x), abs(screenSpaceDistance.y)) / 2;
	rayDirectionTexture /= max(screenSpaceMaxDistance, 0.001f);


	//trace the ray
	vec3 outColor = TraceRay(pixelPositionTexture, rayDirectionTexture, screenSpaceMaxDistance);
	return vec4(outColor, 1);
}

/* ======================= END OF SSR ========================================== */



void main()
{	
	if(postProcessingEffect == 2){
		DefferedShading();
		return;
	}
	if (postProcessingEffect == 3) {
		float depthValue = texture(screenTexture, fs_in.texcoord).r;
		float tt = LinearizeDepth(depthValue);
		fragColor = vec4(vec3(tt), 1.0);
		return;
	}

	if (postSubProcess == 0) {
		// blur
		BloomBlurEffect();
	} 
	else if(postSubProcess == 1){

        BloomFinalEffect();
	}
	else{
		ToonFinalEffect();
	}
}