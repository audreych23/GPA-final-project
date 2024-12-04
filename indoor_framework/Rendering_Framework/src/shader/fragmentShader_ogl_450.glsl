#version 450 core

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;
layout (location = 2) out vec4 ambientColor;
layout (location = 3) out vec4 diffuseColor;
layout (location = 4) out vec4 specularColor;


layout (location = 4) uniform int postProcessId;
layout (location = 5) uniform int shadingModelId;

layout (location = 6) uniform vec3 ka;
layout (location = 7) uniform vec3 kd;
layout (location = 8) uniform vec3 ks;
layout (location = 9) uniform vec3 ke;
layout (location = 10) uniform float ns;
layout (location = 11) uniform int hasTexture;

uniform sampler2D modelTexture;
uniform sampler2D modelTextureNormal;
uniform sampler2DShadow modelTextureShadow;

in VertexData
{
    vec3 N; // eye space normal
    vec3 L; // eye space light vector
    vec3 H; // eye space halfway vector
    vec3 texCoord;
	vec4 shadowCoord;

	// for normal mapping
	vec3 lightDirNormalMapping;
	vec3 eyeDirNormalMapping;
	vec3 halfwayDirNormalMapping;
} vertexData;

in vec3 f_worldPosition;

vec3 Ia = vec3(0.1, 0.1, 0.1);
vec3 Id = vec3(0.7, 0.7, 0.7);
vec3 Is = vec3(0.2, 0.2, 0.2);

vec3 pointLightPosition = vec3(1.87659, 0.4625 , 0.103928);

float constant = 1.0f;
float linear = 0.7f;
float quadratic = 0.14f;

const float lightThreshold = 0.5;

/*
	##################################
	#   Directional Shadow Mapping   #
	##################################
*/
void RenderIndoorShadow() {
	vec3 N = normalize(vertexData.N);
	vec3 L = normalize(vertexData.L);
	vec3 H = normalize(vertexData.H);

	vec4 originalColor = vec4(0.0);
	if(hasTexture == 0) {
		originalColor = vec4(kd, 1.0);
	}
	else {
		originalColor = texture(modelTexture, vertexData.texCoord.xy);
	}

	if (originalColor.a < 0.5) {
		discard;
	}

	vec3 ambient = Ia * originalColor.rgb;

	float diff = max(dot(N, L), 0.0);
	vec3 diffuse = Id * diff * originalColor.rgb;

	float spec = pow(max(dot(N, H), 0.0), ns);
	vec3 specular = Is * spec * ks;

	vec3 color = ambient + diffuse + specular;

	// fragColor = vec4(color, originalColor.a);
	
	fragColor = textureProj(modelTextureShadow, vertexData.shadowCoord) * vec4(color, originalColor.a);
}

void RenderTriceShadow() {
	vec3 N = normalize(texture(modelTextureNormal, vertexData.texCoord.xy).rgb * 2.0 - vec3(1.0));
	vec3 V = normalize(vertexData.eyeDirNormalMapping);
	vec3 L = normalize(vertexData.lightDirNormalMapping);
	vec3 H = normalize(vertexData.halfwayDirNormalMapping);

	// vec3 R = reflect(-L, N);
	// actually trice has no texture
	vec3 ambient = Ia * kd;

	float diff = max(dot(N, L), 0.0);
	vec3 diffuse = Id * diff * kd;

	float spec = pow(max(dot(N, H), 0.0), ns);
	vec3 specular = Is * spec * ks;

	vec3 color = ambient + diffuse + specular;

	// fragColor = vec4(color, 1.0);
	fragColor = textureProj(modelTextureShadow, vertexData.shadowCoord) * vec4(color, 1.0);
}

/*
	##################################
	#		Deferred Shading		 #
	##################################
*/
void RenderIndoorDeferred(){
	/* Init */
	vec4 originalColor = vec4(0.0);
	if(hasTexture == 0) {
		originalColor = vec4(kd, 1.0);
	}
	else {
		originalColor = texture(modelTexture, vertexData.texCoord.xy);
		if(originalColor.a < 0.5) discard;
	}
	vec3 N = normalize(vertexData.N);
	vec3 H = normalize(vertexData.H);
	float spec = pow(max(dot(N, H), 0.0), ns);
	/* Output */
	fragColor = vec4(normalize(f_worldPosition) * 0.5 + 0.5, 1.0);
	brightColor = vec4(normalize(vertexData.N) * 0.5 + 0.5, 1.0);
	ambientColor = vec4(Ia, 1.0);
	diffuseColor = vec4(originalColor.rgb, 1.0);
	specularColor = vec4(Is, 1.0);
}

void RenderTriceDeferred(){
	/* Init */
	vec4 originalColor = vec4(kd, 1.0);
	vec3 N = normalize(vertexData.N);
	vec3 H = normalize(vertexData.H);
	float spec = pow(max(dot(N, H), 0.0), ns);
	/* Output */
	fragColor = vec4(normalize(f_worldPosition) * 0.5 + 0.5, 1.0);
	brightColor = vec4(vertexData.N, 1.0);
	ambientColor = vec4(Ia, 1.0);
	diffuseColor = vec4(originalColor.rgb, 1.0);
	specularColor = vec4(Is, 1.0);
}

/*
	##################################
	#		Regular  Shading		 #
	##################################
*/

void RenderIndoor() {
	vec3 N = normalize(vertexData.N);
	vec3 L = normalize(vertexData.L);
	vec3 H = normalize(vertexData.H);

	vec4 originalColor = vec4(0.0);
	if(hasTexture == 0) {
		originalColor = vec4(kd, 1.0);
	}
	else {
		originalColor = texture(modelTexture, vertexData.texCoord.xy);
	}

	if (originalColor.a < 0.5) {
		discard;
	}

	vec3 ambient = Ia * originalColor.rgb;

	float diff = max(dot(N, L), 0.0);
	vec3 diffuse = Id * diff * originalColor.rgb;

	float spec = pow(max(dot(N, H), 0.0), ns);
	vec3 specular = Is * spec * ks;

	float pointLightDistance = length(pointLightPosition - f_worldPosition);
	float attenuation = 1.0 / (constant + linear * pointLightDistance + quadratic * (pointLightDistance * pointLightDistance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 color = ambient + diffuse + specular;

	fragColor = vec4(color, originalColor.a);

	float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > lightThreshold)
		brightColor = vec4(color, originalColor.a);
	else
		brightColor = vec4(0.0, 0.0, 0.0, originalColor.a);
	// fragColor = brightColor;
}

void RenderTrice() {
	/*
	vec3 N = normalize(vertexData.N);
	vec3 L = normalize(vertexData.L);
	vec3 H = normalize(vertexData.H);
	vec3 ambient = Ia * kd;

	float diff = max(dot(N, L), 0.0);
	vec3 diffuse = Id * diff * kd;

	float spec = pow(max(dot(N, H), 0.0), ns);
	vec3 specular = Is * spec * ks;


	float pointLightDistance = length(pointLightPosition - f_worldPosition);
	float attenuation = 1.0 / (constant + linear * pointLightDistance + quadratic * (pointLightDistance * pointLightDistance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 color = ambient + diffuse + specular;
	fragColor = vec4(color, 1.0);

			float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
		if(brightness > 1.0)
			brightColor = vec4(color, 1.0);
		else
			brightColor = vec4(0.0, 0.0, 0.0, 1.0);

	*/
	vec3 N = normalize(texture(modelTextureNormal, vertexData.texCoord.xy).rgb * 2.0 - vec3(1.0));
	vec3 V = normalize(vertexData.eyeDirNormalMapping);
	vec3 L = normalize(vertexData.lightDirNormalMapping);
	vec3 H = normalize(vertexData.halfwayDirNormalMapping);

	// vec3 R = reflect(-L, N);
	// actually trice has no texture
	vec3 ambient = Ia * kd;

	float diff = max(dot(N, L), 0.0);
	vec3 diffuse = Id * diff * kd;

	float spec = pow(max(dot(N, H), 0.0), ns);
	vec3 specular = Is * spec * ks;

	float pointLightDistance = length(pointLightPosition - f_worldPosition);
	float attenuation = 1.0 / (constant + linear * pointLightDistance + quadratic * (pointLightDistance * pointLightDistance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 color = ambient + diffuse + specular;

	fragColor = vec4(color, 1.0);

	float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > lightThreshold)
		brightColor = vec4(color, 1.0);
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0); 
	// fragColor = brightColor;
}

void RenderLightSphere() {
	fragColor = vec4(1.0); // set all 4 vector values to 1.0

	float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > lightThreshold)
		brightColor = vec4(fragColor.rgb, 1.0);
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
	// fragColor = brightColor;
}


void main() {
	// shadow map does not need any color in fragment
	if (postProcessId == 0) {
		return;
	} else { 
		if (shadingModelId == 0) {
			// RenderIndoorDeferred();
			RenderIndoorShadow();
		} else if (shadingModelId == 1) {
			// RenderTriceDeferred();
			RenderTriceShadow();
		} else if (shadingModelId == 2) {
			discard;
			//RenderLightSphere();
		}
	}
}