#version 430 core

in vec3 f_viewVertex ;
in vec3 f_uv ;
in vec3 f_viewNormal ;
in vec3 f_worldPos ;

layout (location = 0) out vec4 fragColor ;
 
layout(location = 2) uniform int pixelProcessId;
layout(location = 4) uniform sampler2D albedoTexture ;
layout(location = 3) uniform sampler2DArray bushesBuildingsTexture;
layout(location = 7) uniform mat4 viewMat;

const vec3 lightDirWorld = normalize(vec3(0.4, 0.5, 0.8));
const vec3 Ia = vec3(0.2, 0.2, 0.2);
const vec3 Id = vec3(0.64, 0.64, 0.64);
const vec3 Is = vec3(0.16, 0.16, 0.16);
const float shininess = 1.0;

vec4 calculateBlinnPhong(vec4 albedo) {
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
    // Specular
    vec3 specular = vec3(0.0);
    // Combine components
    vec3 finalColor = ambient + diffuse + specular;
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
	fragColor = withFog(texel); 
	fragColor.a = 1.0;	
}

void pureColor(){
	fragColor = withFog(vec4(1.0, 0.0, 0.0, 1.0)) ;
}

void bushesBuildingsPass() {
	// for debug
	vec4 texel = texture(bushesBuildingsTexture, f_uv);

	if (texel.a < 0.5) {
		discard;
	}
	vec4 color = calculateBlinnPhong(texel);
	vec4 finalColor = withFog(color);
	finalColor.rgb = pow(finalColor.rgb, vec3(0.5));
	fragColor = vec4(finalColor.rgb, finalColor.a);
}

// void RenderBushModel() {
// 	vec4 texel = texture(bushTexture, f_uv);

// 	if (texel.a < 0.5) {
// 		discard;
// 	}

// 	// fragColor = texel;
// 	// fragColor = vec4(1.0f, 0.0f, 1.0f, 1.0f);
// 	// fragColor = vec4(f_texCoord.x, f_texCoord.y, 1.0f, 1.0f);
// 	// Normalize input vectors
//     vec3 N = normalize(f_normal);
// 	vec3 L = lightDir;
//     vec3 V = normalize(f_neg_viewVertex); // View vector
//     vec3 H = normalize(V + L);
	
//     // Ambient component
//     vec3 ambient = Ka * lightIntensity;

// 	// Diffuse component
// 	vec3 diffuse = max(dot(N, L), 0.0) * Kd * lightIntensity;

//     // Specular component
//     vec3 specular = pow(max(dot(N, H), 0.0), 100.0) * Ks * lightIntensity;

//     // Combine Phong shading components
//     vec3 phongLighting = ambient + diffuse + specular;

//     // Combine texture color with Phong lighting
//     vec3 finalColor = texel.rgb * phongLighting;

// 	vec3 mappedColor = vec3(1.0f) - exp(-finalColor * exposure);

//     // Output final color
//     fragColor = vec4(mappedColor, texel.a);
// }
void main(){	
	if(pixelProcessId == 5){
		pureColor() ;
	}
	else if(pixelProcessId == 7){
		terrainPass() ;
	} else if(pixelProcessId == 8) {
		bushesBuildingsPass();
	}
	else{
		pureColor() ;
	}
}