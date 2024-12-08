#version 450 core

layout (location = 0) out vec4 outColor1;
layout (location = 1) out vec4 outColor2;
layout (location = 2) out vec4 outColor3;
layout (location = 3) out vec4 outColor4;
layout (location = 4) out vec4 outColor5;

layout (location = 3) uniform vec3 cameraPosition;
layout (location = 4) uniform int postProcessId;
layout (location = 5) uniform int shadingModelId;

layout (location = 6) uniform vec3 ka;
layout (location = 7) uniform vec3 kd;
layout (location = 8) uniform vec3 ks;
layout (location = 9) uniform vec3 ke;
layout (location = 10) uniform float ns;
layout (location = 11) uniform int hasTexture;

layout (location = 20) uniform int PostProcessMain;

layout (location = 25) uniform bool hasDirectionalLight;
layout (location = 26) uniform bool hasToon;
layout (location = 27) uniform bool hasNormal;
layout (location = 28) uniform bool isMetalic;

layout (location = 31) uniform vec3 areaLightVertices0;
layout (location = 32) uniform vec3 areaLightVertices1;
layout (location = 33) uniform vec3 areaLightVertices2;
layout (location = 34) uniform vec3 areaLightVertices3;
layout (location = 35) uniform bool hasAreaLight;

uniform sampler2D modelTexture;
uniform sampler2D modelTextureNormal;
uniform sampler2DShadow modelTextureShadow;
uniform sampler2D LTC1;
uniform sampler2D LTC2;

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

// vec3 lightBloomPos = vec3(1.87659, 0.4625 , 0.103928);

const float quantized = 4.0;
float constant = 1.0f;
float linear = 0.7f;
float quadratic = 0.14f;
const vec3 areaLightColor = vec3(0.8, 0.6, 0.0);

layout (location = 21) uniform vec3 lightBloomPos;
vec3 lightPosition = vec3(1.87659, 0.4625 , 0.103928);
const float lightThreshold = 0.9;


/*
	##################################
	#   AREA LIGHT FUNCTION HELPER   #
	##################################
*/

const float LUT_SIZE  = 64.0; // ltc_texture size
const float LUT_SCALE = (LUT_SIZE - 1.0)/LUT_SIZE;
const float LUT_BIAS  = 0.5/LUT_SIZE;


// Vector form without project to the plane (dot with the normal)
// Use for proxy sphere clipping
vec3 IntegrateEdgeVec(vec3 v1, vec3 v2)
{
    // Using built-in acos() function will result flaws
    // Using fitting result for calculating acos()
    float x = dot(v1, v2);
    float y = abs(x);

    float a = 0.8543985 + (0.4965155 + 0.0145206*y)*y;
    float b = 3.4175940 + (4.1616724 + y)*y;
    float v = a / b;

    float theta_sintheta = (x > 0.0) ? v : 0.5*inversesqrt(max(1.0 - x*x, 1e-7)) - v;

    return cross(v1, v2)*theta_sintheta;
}

float IntegrateEdge(vec3 v1, vec3 v2)
{
    return IntegrateEdgeVec(v1, v2).z;
}

// P is fragPos in world space (LTC distribution)
vec3 LTC_Evaluate(vec3 N, vec3 V, vec3 P, mat3 Minv, vec3 points[4], bool twoSided)
{
    // construct orthonormal basis around N
    vec3 T1, T2;
    T1 = normalize(V - N * dot(V, N));
    T2 = cross(N, T1);

    // rotate area light in (T1, T2, N) basis
    Minv = Minv * transpose(mat3(T1, T2, N));

    // polygon (allocate 4 vertices for clipping)
    vec3 L[4];
    // transform polygon from LTC back to origin Do (cosine weighted)
    L[0] = Minv * (points[0] - P);
    L[1] = Minv * (points[1] - P);
    L[2] = Minv * (points[2] - P);
    L[3] = Minv * (points[3] - P);

    // use tabulated horizon-clipped sphere
    // check if the shading point is behind the light
    vec3 dir = points[0] - P; // LTC space
    vec3 lightNormal = cross(points[1] - points[0], points[3] - points[0]);
    bool behind = (dot(dir, lightNormal) < 0.0);

    // cos weighted space
    L[0] = normalize(L[0]);
    L[1] = normalize(L[1]);
    L[2] = normalize(L[2]);
    L[3] = normalize(L[3]);

    // integrate
    vec3 vsum = vec3(0.0);
    vsum += IntegrateEdgeVec(L[0], L[1]);
    vsum += IntegrateEdgeVec(L[1], L[2]);
    vsum += IntegrateEdgeVec(L[2], L[3]);
    vsum += IntegrateEdgeVec(L[3], L[0]);

    // form factor of the polygon in direction vsum
    float len = length(vsum);

    float z = vsum.z/len;
    if (behind)
        z = -z;

    vec2 uv = vec2(z*0.5f + 0.5f, len); // range [0, 1]
    uv = uv*LUT_SCALE + LUT_BIAS;

    // Fetch the form factor for horizon clipping
    float scale = texture(LTC2, uv).w;

    float sum = len*scale;
    if (!behind && !twoSided)
        sum = 0.0;

    // Outgoing radiance (solid angle) for the entire polygon
    vec3 Lo_i = vec3(sum, sum, sum);
    return Lo_i;
}

// PBR-maps for roughness (and metallic) are usually stored in non-linear
// color space (sRGB), so we use these functions to convert into linear RGB.
vec3 PowVec3(vec3 v, float p)
{
    return vec3(pow(v.x, p), pow(v.y, p), pow(v.z, p));
}

const float gamma = 2.2;
vec3 ToLinear(vec3 v) { return PowVec3(v, gamma); }
vec3 ToSRGB(vec3 v)   { return PowVec3(v, 1.0/gamma); }

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

	// outColor1 = vec4(color, originalColor.a);
	float lightproj = textureProj(modelTextureShadow, vertexData.shadowCoord);
	outColor1 = lightproj * vec4(color, originalColor.a);
	outColor2 = 0.0001 * vec4(1.0) * textureProj(modelTextureShadow, vertexData.shadowCoord);
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

	// outColor1 = vec4(color, 1.0);
	float lightproj = textureProj(modelTextureShadow, vertexData.shadowCoord);
	outColor1 = lightproj * vec4(color, 1.0);
	outColor2 = 0.0001 * vec4(1.0) * textureProj(modelTextureShadow, vertexData.shadowCoord);
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
	/* Output */
	outColor1 = vec4(normalize(f_worldPosition) * 0.5 + 0.5, 1.0);
	outColor2 = vec4(normalize(vertexData.N) * 0.5 + 0.5, 1.0);
	outColor3 = vec4(ka, 1.0);
	outColor4 = vec4(originalColor.rgb, 1.0);
	outColor5 = vec4(ks, 1.0);
}

void RenderTriceDeferred(){
	/* Init */
	vec4 originalColor = vec4(kd, 1.0);
	vec3 N = normalize(vertexData.N);
	/* Output */
	outColor1 = vec4(normalize(f_worldPosition) * 0.5 + 0.5, 1.0);
	outColor2 = vec4(normalize(vertexData.N) * 0.5 + 0.5, 1.0);
	outColor3 = vec4(ka, 1.0);
	outColor4 = vec4(originalColor.rgb, 1.0);
	outColor5 = vec4(ks, 1.0);
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
	float spec = pow(max(dot(N, H), 0.0), ns);

	/* TOON SHADING MOTHER FATHER */
	if(hasToon){
		diff = floor(diff * quantized) / quantized;
		spec = floor(spec * quantized) / quantized;
	}

	float pointLightDistance = length(lightBloomPos - f_worldPosition);
	float attenuation = 1.0 / (constant + linear * pointLightDistance + quadratic * (pointLightDistance * pointLightDistance));

	vec3 diffuse = Id * diff * originalColor.rgb;
	vec3 specular = Is * spec * ks;

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 color = ambient + diffuse + specular;

	vec3 bloomOut = color;
	vec3 dirOut = vec3(0.0);
	vec3 areaOut = vec3(0.0);

	// outColor1 = vec4(color, originalColor.a);

	float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > lightThreshold)
		outColor2 = vec4(color, originalColor.a);
	else
		outColor2 = vec4(0.0, 0.0, 0.0, originalColor.a);
	
	if(hasDirectionalLight){
		float lightproj = textureProj(modelTextureShadow, vertexData.shadowCoord);
		outColor1 = lightproj * vec4(color, originalColor.a);
		dirOut = lightproj * vec4(color, originalColor.a).rgb;
		outColor3 = 0.0001 * vec4(1.0) * lightproj;
	}
	else{
		outColor1 = vec4(color, originalColor.a);
	}

	

	if(hasAreaLight){
	/* ===================== Area Light ======================== */
		vec3 mSpecular = ToLinear(vec3(0.23f, 0.23f, 0.23f)); // mDiffuse
		vec3 ambient = Ia * kd;

		vec3 result = vec3(0.0f);

		vec3 V = normalize(cameraPosition - f_worldPosition);
		vec3 P = f_worldPosition;
		float dotNV = clamp(dot(N, V), 0.0f, 1.0f);

		// use roughness and sqrt(1-cos_theta) to sample M_texture
		// gpt : Roughness = sqrt(1.0 - Ns / maxNs)
		float roughness = sqrt(1.0 - ns / 1000);
		vec2 uv = vec2(roughness, sqrt(1.0f - dotNV));
		uv = uv * LUT_SCALE + LUT_BIAS;

		// get 4 parameters for inverse_M
		vec4 t1 = texture(LTC1, uv);

		// Get 2 parameters for Fresnel calculation
		vec4 t2 = texture(LTC2, uv);

		mat3 Minv = mat3(
			vec3(t1.x, 0, t1.y),
			vec3(  0,  1,    0),
			vec3(t1.z, 0, t1.w)
		);

		// translate light source for testing
		vec3 area_light_translate = vec3(1.0f, 0.5f, -0.5f);
		vec3 translatedPoints[4];
		translatedPoints[0] = areaLightVertices0 + area_light_translate;
		translatedPoints[1] = areaLightVertices1 + area_light_translate;
		translatedPoints[2] = areaLightVertices2 + area_light_translate;
		translatedPoints[3] = areaLightVertices3 + area_light_translate;

		// Evaluate LTC shading
		vec3 diffuse = LTC_Evaluate(N, V, P, mat3(1.0f), translatedPoints, false);
		vec3 specular = LTC_Evaluate(N, V, P, Minv, translatedPoints, false);

		// GGX BRDF shadowing and Fresnel
		// t2.x: shadowedF90 (F90 normally it should be 1.0)
		// t2.y: Smith function for Geometric Attenuation Term, it is dot(V or L, H).
		specular = specular * (mSpecular * t2.x + (1.0f - mSpecular) * t2.y);

		result = areaLightColor /* * intensity*/  * (/*ambient +*/ Is * specular + originalColor.rgb * diffuse * Id);

		areaOut = ToSRGB(result);//vec4(ToSRGB(result), 1.0f).rgb;
	/* ==================== End of Area Light ================== */
	}

	// 	vec3 bloomOut = color;
	// vec3 dirOut = vec3(0.0);
	// vec3 areaOut = vec3(0.0);

	outColor1 = vec4((bloomOut + dirOut + areaOut) / 3.0, 1.0);
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


	float pointLightDistance = length(lightBloomPos - f_worldPosition);
	float attenuation = 1.0 / (constant + linear * pointLightDistance + quadratic * (pointLightDistance * pointLightDistance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 color = ambient + diffuse + specular;
	outColor1 = vec4(color, 1.0);

			float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
		if(brightness > 1.0)
			outColor2 = vec4(color, 1.0);
		else
			outColor2 = vec4(0.0, 0.0, 0.0, 1.0);

	*/
	vec3 N = normalize(vertexData.N);
	vec3 L = normalize(vertexData.L);
	vec3 H = normalize(vertexData.H);
	if(hasNormal){
		 N = normalize(texture(modelTextureNormal, vertexData.texCoord.xy).rgb * 2.0 - vec3(1.0));
		//vec3 V = normalize(vertexData.eyeDirNormalMapping);
		 L = normalize(vertexData.lightDirNormalMapping);
		 H = normalize(vertexData.halfwayDirNormalMapping);
	}


	// vec3 R = reflect(-L, N);
	// actually trice has no texture
	vec3 ambient = Ia * kd;

	float diff = max(dot(N, L), 0.0);
	float spec = pow(max(dot(N, H), 0.0), ns);
	
	/* TOON SHADING MOTHER FATHER */
	if(hasToon){
		diff = floor(diff * quantized) / quantized;
		spec = floor(spec * quantized) / quantized;
	}

	vec3 diffuse = Id * diff * kd;
	vec3 specular = Is * spec * ks;

	float pointLightDistance = length(lightBloomPos - f_worldPosition);
	float attenuation = 1.0 / (constant + linear * pointLightDistance + quadratic * (pointLightDistance * pointLightDistance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 color = ambient + diffuse + specular;

	float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > lightThreshold)
		outColor2 = vec4(color, 1.0);
	else
		outColor2 = vec4(0.0, 0.0, 0.0, 1.0); 
	// outColor1 = outColor2;
	if(hasDirectionalLight){
		float lightproj = textureProj(modelTextureShadow, vertexData.shadowCoord);
		outColor1 = lightproj * vec4(color, 1.0);
		outColor3 = vec4(0.0); //0.0001 * vec4(1.0) * textureProj(modelTextureShadow, vertexData.shadowCoord);
	}
	else{
		outColor1 = vec4(color, 1.0);
	}
}



void RenderLightSphere() {
	outColor1 = vec4(1.0); // set all 4 vector values to 1.0

	float brightness = dot(outColor1.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > lightThreshold)
		outColor2 = vec4(1.0);
	else
		outColor2 = vec4(0.0, 0.0, 0.0, 1.0);
	outColor3 = vec4(0.0, 0.0, 0.0, 0.0);
	// outColor1 = outColor2;
}

void RenderSunSphere(){
	outColor1 = vec4(1.0); // set all 4 vector values to 1.0

	float brightness = dot(outColor1.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > lightThreshold)
		outColor3 = vec4(1.0);
	else
		outColor3 = vec4(0.0, 0.0, 0.0, 1.0);
	outColor2 = vec4(1.0);
	// outColor1 = outColor2;
	
}

void RenderAreaLight() {
	outColor1 = vec4(areaLightColor, 1.0);
	outColor2 = vec4(0.0, 0.0, 0.0, 0.0);
	outColor3 = vec4(0.0, 0.0, 0.0, 0.0);
}


void main() {
	// shadow map does not need any color in fragment
	if(PostProcessMain == 2) {
		if (shadingModelId == 0) {
			RenderIndoorDeferred();
		} else if (shadingModelId == 1) {
			RenderTriceDeferred();
		} else{
			discard;
		} 
		return;
	}
	
	if(PostProcessMain == 3 && postProcessId == 0) {
		outColor2 = vec4(0.0, 0.0, 0.0, 0.0);
		outColor3 = vec4(0.0, 0.0, 0.0, 0.0);
		return;
	}

	if (shadingModelId == 0) {
		RenderIndoor();
	} else if (shadingModelId == 1) {
		RenderTrice();
	} else if (shadingModelId == 2) {
		RenderLightSphere();
	} else if (shadingModelId == 3) {
		RenderSunSphere();
	} else if( shadingModelId == 4){
		if(hasAreaLight){
			RenderAreaLight();
		}
		else{
			discard;
		}
			// outColor1 = vec4(0.0, 0.0, 0.0, 0.0);
			// outColor2 = vec4(0.0, 0.0, 0.0, 0.0);
			// outColor3 = vec4(0.0, 0.0, 0.0, 0.0);
	}

	if(!hasDirectionalLight){
		outColor3 = vec4(0.0, 0.0, 0.0, 0.0);
	}

}