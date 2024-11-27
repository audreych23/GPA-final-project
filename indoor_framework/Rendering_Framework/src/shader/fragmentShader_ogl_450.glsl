#version 450 core

in vec3 f_worldVertex;
in vec3 f_viewVertex;
in vec3 f_neg_viewVertex;
in vec3 f_normal;
in vec3 f_texCoord;

layout (location = 0) out vec4 fragColor;
layout (location = 5) uniform int shadingModelId;

layout (location = 6) uniform vec3 ka;
layout (location = 7) uniform vec3 kd;
layout (location = 8) uniform vec3 ks;
layout (location = 9) uniform vec3 ke;
layout (location = 10) uniform float ns;
layout (location = 11) uniform int hasTexture;

uniform sampler2D modelTexture;

void RenderIndoor() {
	if (hasTexture == 0) {
		fragColor = vec4(kd, 1.0f);
	} else {
		
		vec4 texel = texture(modelTexture, f_texCoord.xy);

		if (texel.a < 0.5) {
			discard;
		}

		fragColor = texel;
		// fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	// fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);

}

void RenderTrice() {
	if (hasTexture == 0) {
		fragColor = vec4(kd, 1.0f);
	} else {
		
		vec4 texel = texture(modelTexture, f_texCoord.xy);

		if (texel.a < 0.5) {
			discard;
		}

		fragColor = texel;
		// fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	
}


void main(){
 	if (shadingModelId == 0) {
		RenderIndoor();
	} else if (shadingModelId == 1) {
		RenderTrice();
	} 
}