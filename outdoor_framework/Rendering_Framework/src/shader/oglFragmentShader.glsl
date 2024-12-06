#version 430 core

in vec3 f_viewVertex ;
in vec3 f_uv ;
in vec3 f_viewNormal;
in vec3 f_worldPos;
in vec3 f_worldVertex;
in vec3 f_worldNormal;

layout (location = 0) out vec4 fragColor ;

layout(location = 2) uniform int pixelProcessId;
layout(location = 4) uniform sampler2D albedoTexture ;
layout(location = 6) uniform sampler2D normalMap ;
layout(location = 7) uniform mat4 viewMat;
layout(location = 20) uniform bool isNormalMap;
layout(location = 21) uniform int renderMode;

const vec3 lightDirWorld = normalize(vec3(0.4, 0.5, 0.8));
const vec3 Ia = vec3(0.2, 0.2, 0.2);
const vec3 Id = vec3(0.64, 0.64, 0.64);
const vec3 Is = vec3(0.16, 0.16, 0.16);

vec4 calculateBlinnPhong(vec4 albedo, vec3 normal, vec3 specular, float shininess, bool invert) {
    // Transform light direction to view space
		vec3 lightPos = lightDirWorld;
		if (invert) {
			lightPos.x = -lightPos.x;
			lightPos.z = -lightPos.z;
		}
    vec3 L = normalize(mat3(viewMat) * lightPos);
    vec3 N = normalize(normal);
    vec3 V = normalize(-f_viewVertex);
    vec3 H = normalize(L + V);

    // Ambient
    vec3 ambient = Ia * albedo.rgb;

    // Diffuse
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = Id * diff * albedo.rgb;

		// Shininess
		float spec = pow(max(dot(N, H), 0.0), shininess);
    vec3 specularColor = Is * spec * specular;

    // Combine components
    vec3 finalColor = ambient + diffuse + specularColor;
    return vec4(finalColor, albedo.a);
}

vec4 normalBlinnPhong(vec4 albedo, vec3 normal, vec4 normalMap, vec3 specular, float shininess) {
    // Transform light direction to view space
    vec3 L = normalize(mat3(viewMat) * lightDirWorld);
    vec3 V = normalize(-f_viewVertex);
    vec3 H = normalize(L + V);

    // Start with the mesh normal
    vec3 N = normalize(normal);
    
    // Unpack and blend in the normal map details
    vec3 normalDetail = normalize(normalMap.rgb * 2.0 - 1.0);
    // Blend the normal map with the base mesh normal
    N = normalize(N * 0.75 + normalDetail * 1.25);

    // Ambient
    vec3 ambient = Ia * albedo.rgb;

    // Diffuse
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = Id * diff * albedo.rgb;

    // Specular
    float spec = pow(max(dot(N, H), 0.0), shininess);
    vec3 specularColor = Is * spec * specular;

    vec3 combined = ambient + diffuse + specularColor;
    return vec4(combined, 1.0);
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
	vec3 normal = f_viewNormal.xyz;

	vec4 litColor = calculateBlinnPhong(texel, normal, vec3(0.0f), 1.0f, true);
	vec4 fogColor = withFog(litColor);
	vec4 gammaCorrectedColor = pow(fogColor, vec4(0.5));

	if (renderMode == 1) {
		vec3 worldVertexColor = normalize(f_worldVertex) * 0.5 + 0.5;
		fragColor = vec4(worldVertexColor, 1.0);
	}
	else if (renderMode == 2) {
		vec3 worldNormalColor = normalize(f_worldNormal) * 0.5 + 0.5;
		fragColor = vec4(worldNormalColor, 1.0);
	}
	else if (renderMode == 3 || renderMode == 4) {
		fragColor = vec4(texel.rgb, 1.0);
	}
	else if (renderMode == 5) {
		fragColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
	else {
		fragColor = vec4(gammaCorrectedColor.rgb, 1.0);
	}
}

void pureColor(){
	vec4 baseColor = vec4(1.0, 0.0, 0.0, 1.0) ;
	fragColor = withFog(baseColor) ;
}

void airplanePass() {
	vec4 texel = texture(albedoTexture, f_uv.xy) ;
	vec4 litColor = calculateBlinnPhong(texel, f_viewNormal, vec3(1.0f), 32.0f, false);
	fragColor = withFog(litColor); 
	fragColor.rgb = pow(fragColor.rgb, vec3(0.5));
	fragColor.a = 1.0;

	if (renderMode == 1) {
		vec3 worldVertexColor = normalize(f_worldVertex) * 0.5 + 0.5;
		fragColor = vec4(worldVertexColor, 1.0);
	}
	else if (renderMode == 2) {
		vec3 worldNormalColor = normalize(f_worldNormal) * 0.5 + 0.5;
		fragColor = vec4(worldNormalColor, 1.0);
	}
	else if (renderMode == 3 || renderMode == 4) {
		fragColor = vec4(texel.rgb, 1.0);
	}
	else if (renderMode == 5) {
		fragColor = vec4(1.0, 1.0, 1.0, 1.0);
	}
	else {
		fragColor = vec4(fragColor.rgb, 1.0);
	}
}

void rockPass() {
	vec4 texel = texture(albedoTexture, f_uv.xy) ;
	vec4 normal = texture(normalMap, f_uv.xy) ;
	vec4 litColor = calculateBlinnPhong(texel, f_viewNormal, vec3(1.0f), 32.0f, false);

	if (isNormalMap) {
		litColor = normalBlinnPhong(texel, f_viewNormal, normal, vec3(1.0f), 32.0f);
	}

	fragColor = withFog(litColor); 
	fragColor.rgb = pow(fragColor.rgb, vec3(0.5));
	fragColor.a = 1.0;

	if (renderMode == 1) {
		vec3 worldVertexColor = normalize(f_worldVertex) * 0.5 + 0.5;
		fragColor = vec4(worldVertexColor, 1.0);
	}
	else if (renderMode == 2) {
		vec3 worldNormalColor = normalize(f_worldNormal) * 0.5 + 0.5;
		fragColor = vec4(worldNormalColor, 1.0);
	}
	else if (renderMode == 3 || renderMode == 4) {
		fragColor = vec4(texel.rgb, 1.0);
	}
	else if (renderMode == 5) {
		fragColor = vec4(1.0, 1.0, 1.0, 1.0);
	}
	else {
		fragColor = vec4(fragColor.rgb, 1.0);
	}
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