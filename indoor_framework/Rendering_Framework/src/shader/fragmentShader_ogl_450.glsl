#version 450 core

layout (location = 0) out vec4 fragColor;
layout (location = 5) uniform int shadingModelId;

layout (location = 6) uniform vec3 ka;
layout (location = 7) uniform vec3 kd;
layout (location = 8) uniform vec3 ks;
layout (location = 9) uniform vec3 ke;
layout (location = 10) uniform float ns;
layout (location = 11) uniform int hasTexture;

uniform sampler2D modelTexture;
uniform sampler2D modelTextureNormal;

in VertexData
{
    vec3 N; // eye space normal
    vec3 L; // eye space light vector
    vec3 H; // eye space halfway vector
    vec3 texCoord;

	// for normal mapping
	vec3 lightDirNormalMapping;
	vec3 eyeDirNormalMapping;
	vec3 halfwayDirNormalMapping;
} vertexData;

vec3 Ia = vec3(0.1, 0.1, 0.1);
vec3 Id = vec3(0.7, 0.7, 0.7);
vec3 Is = vec3(0.2, 0.2, 0.2);

void RenderIndoor() {
	vec3 N = normalize(vertexData.N);
	vec3 L = normalize(vertexData.L);
	vec3 H = normalize(vertexData.H);

	if (hasTexture == 0) {

		vec3 ambient = Ia * kd;

        float diff = max(dot(N, L), 0.0);
        vec3 diffuse = Id * diff * kd;

        float spec = pow(max(dot(N, H), 0.0), ns);
        vec3 specular = Is * spec * ks;

        vec3 color = ambient + diffuse + specular;

        fragColor = vec4(color, 1.0);
	} else {
		vec4 texel = texture(modelTexture, vertexData.texCoord.xy);

		if (texel.a < 0.5) {
			discard;
		}

		vec3 ambient = Ia * texel.rgb;

        float diff = max(dot(N, L), 0.0);
        vec3 diffuse = Id * diff * texel.rgb;

        float spec = pow(max(dot(N, H), 0.0), ns);
        vec3 specular = Is * spec * ks;

        vec3 color = ambient + diffuse + specular;

        fragColor = vec4(color, texel.a);
	}
}

void RenderTrice() {
	// vec3 N = normalize(vertexData.N);
	// vec3 L = normalize(vertexData.L);
	// vec3 H = normalize(vertexData.H);
	// vec3 ambient = Ia * kd;

	// float diff = max(dot(N, L), 0.0);
	// vec3 diffuse = Id * diff * kd;

	// float spec = pow(max(dot(N, H), 0.0), ns);
	// vec3 specular = Is * spec * ks;

	// vec3 color = ambient + diffuse + specular;
	// fragColor = vec4(color, 1.0);

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

	fragColor = vec4(color, 1.0);
}


void main(){
 	if (shadingModelId == 0) {
		RenderIndoor();
	} else if (shadingModelId == 1) {
		RenderTrice();
	} 
}