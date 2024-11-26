#version 450 core

in vec3 f_worldVertex;
in vec3 f_viewVertex;
in vec3 f_neg_viewVertex;
in vec3 f_normal;
in vec3 f_texCoord;

layout (location = 0) out vec4 fragColor;
layout (location = 5) uniform int shadingModelId;
//layout (location = 6) uniform vec3 ka;
layout (location = 7) uniform vec3 kd;
//layout (location = 8) uniform vec3 ks;
//layout (location = 9) uniform vec3 ke;
//layout (location = 10) uniform float ns;
layout (location = 11) uniform int hasTexture;


uniform sampler2DArray bushTexture;
uniform sampler2D slimeTexture;

vec4 WithFog(vec4 color){
	const vec4 FOG_COLOR = vec4(0.0, 0.0, 0.0, 1) ;
	const float MAX_DIST = 150.0 ;
	const float MIN_DIST = 120.0 ;
	
	float dis = length(f_viewVertex) ;
	float fogFactor = (MAX_DIST - dis) / (MAX_DIST - MIN_DIST) ;
	fogFactor = clamp(fogFactor, 0.0f, 1.0f) ;
	fogFactor = fogFactor * fogFactor ;
	
	return mix(FOG_COLOR, color, fogFactor) ;
}

float GridFactor(vec3 worldPos){
	const vec2 HALF_LINE_WIDTH = vec2(0.05f, 0.05f);
	// make it repeat
	vec2 fr = fract(worldPos.xz);
	// [0.0, 1.0) -> [-1.0, 1.0)
	fr = 2.0f * fr - 1.0f;
	// fwidth = abs(dFdx) + abs(dFdy)
	vec2 uvDeriv = fwidth(worldPos.xz);
	const vec2 lineAA = uvDeriv * 1.5f;
	// (approximate) constant pixel width
	vec2 finalHalfLineWidth = HALF_LINE_WIDTH * lineAA;
	// prevent from too thickness
	finalHalfLineWidth = max(HALF_LINE_WIDTH, finalHalfLineWidth);
	
	const vec2 SX = vec2(0.0f, 0.0f);	
	const vec2 s = smoothstep(SX - finalHalfLineWidth - lineAA, SX - finalHalfLineWidth,          fr);	
	const vec2 m = smoothstep(SX + finalHalfLineWidth,          SX + finalHalfLineWidth + lineAA, fr);	
	const vec2 res = s - m;
	// fade out the line based on the thickness
	const vec2 fadeOutRes = res * clamp(HALF_LINE_WIDTH / finalHalfLineWidth, vec2(0.0f), vec2(1.0f));	
	// for the area that is usually has Moire patterns, fade out it
	const vec2 deriv2 = clamp(uvDeriv, 0.0f, 1.0f);
	const vec2 finalRes = mix(fadeOutRes, HALF_LINE_WIDTH, deriv2);
	
	return mix(finalRes.x, 1.0, finalRes.y);
}
	
void RenderGrid(){
	const vec4 groundColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	const vec4 gridColor = vec4(0.7f, 0.7f, 0.7f, 1.0f);
	
	vec4 color = mix(groundColor, gridColor, GridFactor(f_worldVertex));
	fragColor = WithFog(color);
}

void RenderViewFrustum(){
	fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}

void RenderSlime() {
	if (hasTexture == 0) {
		fragColor = vec4(kd, 1.0f);
	} else {
		
		vec4 texel = texture(slimeTexture, f_texCoord.xy);

		if (texel.a < 0.5) {
			discard;
		}

		fragColor = texel;
		// fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	// fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);

}

void RenderBushModel() {
	if (hasTexture == 0) {
		fragColor = vec4(kd, 1.0f);
	} else {
		
		vec4 texel = texture(slimeTexture, f_texCoord.xy);

		if (texel.a < 0.5) {
			discard;
		}

		fragColor = texel;
		// fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	
}


void main(){
	if (shadingModelId == 5){
		RenderViewFrustum();
	} else if (shadingModelId == 1) {
		RenderBushModel();
	} else if (shadingModelId == 2) {
		RenderSlime();
	} 
	else {
		RenderGrid();
	}
}