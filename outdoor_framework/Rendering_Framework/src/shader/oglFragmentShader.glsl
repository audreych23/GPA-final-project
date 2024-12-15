#version 430 core

in vec3 f_viewVertex ;
in vec3 f_uv ;

layout (location = 0) out vec4 fragColor ;
 
layout(location = 2) uniform int pixelProcessId;
layout(location = 4) uniform sampler2D albedoTexture ;
layout(location = 3) uniform sampler2DArray bushesBuildingsTexture;


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

	fragColor = vec4(texel.xyz, texel.a);
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