#version 430 core

in vec3 f_viewVertex ;
in vec3 f_uv ;
in vec3 f_viewNormal;
in vec3 f_worldPos;

layout (location = 0) out vec4 fragColor ;

layout(location = 2) uniform int pixelProcessId;
layout(location = 4) uniform sampler2D albedoTexture ;
layout(location = 7) uniform mat4 viewMat;

const vec3 lightDirWorld = normalize(vec3(0.4, 0.5, 0.8));
const vec3 Ia = vec3(0.2, 0.2, 0.2);
const vec3 Id = vec3(0.64, 0.64, 0.64);
const vec3 Is = vec3(0.16, 0.16, 0.16);

vec4 calculateBlinnPhong(vec4 albedo, vec3 specular, float shininess) {
    // Transform light direction to view space
    vec3 L = normalize(mat3(viewMat) * lightDirWorld);
    vec3 N = normalize(f_viewNormal);
    vec3 V = normalize(-f_viewVertex);
    vec3 H = normalize(L + V);

    // Ambient
    vec3 ambient = Ia * albedo.rgb;

    // Diffuse
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = Id * diff * albedo.rgb;

		// Shininess
		float spec = pow(max(dot(N, H), 0.0), shininess);
		vec3 specularuwu = Is * spec * albedo.rgb;

    // Combine components
    vec3 finalColor = ambient + diffuse + specularuwu;
    return vec4(finalColor, albedo.a);
}

vec4 withFog(vec4 color){
	const vec4 FOG_COLOR = vec4(0.0, 0.0, 0.0, 1) ;
	const float MAX_DIST = 400.0 ;
	const float MIN_DIST = 350.0 ;
	
	float dis = length(f_viewVertex) ;
	float fogFactor = (MAX_DIST - dis) / (MAX_DIST - MIN_DIST) ;
	fogFactor = clamp(fogFactor, 0.0, 1.0) ;
	fogFactor = fogFactor * fogFactor ;
	
	vec4 colorWithFog = mix(FOG_COLOR, color, fogFactor) ;
	return colorWithFog ;
}

void terrainPass(){
	vec4 texel = texture(albedoTexture, f_uv.xy) ;
	vec4 litColor = calculateBlinnPhong(texel, vec3(0.0f), 1.0f);
	fragColor = withFog(litColor);
	fragColor.rgb = pow(fragColor.rgb, vec3(0.5));
	fragColor.a = 1.0;	
}

void pureColor(){
	vec4 baseColor = vec4(1.0, 0.0, 0.0, 1.0) ;
	fragColor = withFog(baseColor) ;
}

void airplanePass() {
	vec4 texel = texture(albedoTexture, f_uv.xy) ;
	vec4 litColor = calculateBlinnPhong(texel, vec3(1.0f), 32.0f);
	fragColor = withFog(litColor); 
	fragColor.rgb = pow(fragColor.rgb, vec3(0.5));
	fragColor.a = 1.0;
}

void rockPass() {
	vec4 texel = texture(albedoTexture, f_uv.xy) ;
	vec4 litColor = calculateBlinnPhong(texel, vec3(1.0f), 32.0f);
	fragColor = withFog(litColor); 
	fragColor.rgb = pow(fragColor.rgb, vec3(0.5));
	fragColor.a = 1.0;
}

void main(){	
	if (pixelProcessId == 1) {
		airplanePass();
	}
	else if (pixelProcessId == 2) {
		rockPass();
	}
	else if(pixelProcessId == 5){
		pureColor() ;
	}
	else if(pixelProcessId == 7){
		terrainPass() ;
	}
	else{
		pureColor() ;
	}
}