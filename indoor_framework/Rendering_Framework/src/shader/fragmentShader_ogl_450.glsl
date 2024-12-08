#version 450 core

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;
layout (location = 2) out vec4 ambientColor;
layout (location = 3) out vec4 diffuseColor;
layout (location = 4) out vec4 specularColor;


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
layout (location = 21) uniform vec3 lightBloomPos;

layout (location = 15) uniform vec3 lightColor;

layout (location = 30) uniform vec3 areaLightVertices0;
layout (location = 31) uniform vec3 areaLightVertices1;
layout (location = 32) uniform vec3 areaLightVertices2;
layout (location = 33) uniform vec3 areaLightVertices3;

layout (location = 46) uniform float pointShadowFarPlane;

uniform sampler2D modelTexture;
uniform sampler2D modelTextureNormal;
uniform sampler2DShadow modelTextureShadow;

uniform sampler2D LTC1;
uniform sampler2D LTC2;

uniform samplerCube modelTextureShadowPoint;

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
} f_vertexData;

in vec3 f_worldPosition;

vec3 Ia = vec3(0.1, 0.1, 0.1);
vec3 Id = vec3(0.7, 0.7, 0.7);
vec3 Is = vec3(0.2, 0.2, 0.2);

// vec3 lightBloomPos = vec3(1.87659, 0.4625 , 0.103928);

const float quantized = 4.0;
float constant = 1.0f;
float linear = 0.7f;
float quadratic = 0.14f;

vec3 lightPosition = vec3(1.87659, 0.4625 , 0.103928);
const float lightThreshold = 0.9;

/*
	##################################
	#      Point Shadow Mapping      #
	##################################
*/
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightBloomPos;
    // use the fragment to light vector to sample from the depth map    
    // float closestDepth = texture(depthMap, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    // closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    // float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    // float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
    // PCF
    // float shadow = 0.0;
    // float bias = 0.05; 
    // float samples = 4.0;
    // float offset = 0.1;
    // for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    // {
        // for(float y = -offset; y < offset; y += offset / (samples * 0.5))
        // {
            // for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            // {
                // float closestDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r; // use lightdir to lookup cubemap
                // closestDepth *= far_plane;   // Undo mapping [0;1]
                // if(currentDepth - bias > closestDepth)
                    // shadow += 1.0;
            // }
        // }
    // }
    // shadow /= (samples * samples * samples);
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(cameraPosition - fragPos);
    float diskRadius = (1.0 + (viewDistance / pointShadowFarPlane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(modelTextureShadowPoint, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= pointShadowFarPlane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
        
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return shadow;
}

void RenderIndoorShadowPoint() {
	vec3 N = normalize(f_vertexData.N);
	vec3 L = normalize(f_vertexData.L);
	vec3 H = normalize(f_vertexData.H);

	vec4 originalColor = vec4(0.0);
	if(hasTexture == 0) {
		originalColor = vec4(kd, 1.0);
	}
	else {
		originalColor = texture(modelTexture, f_vertexData.texCoord.xy);
	}

	if (originalColor.a < 0.5) {
		discard;
	}

	vec3 ambient = Ia * originalColor.rgb;

	float diff = max(dot(N, L), 0.0);
	vec3 diffuse = Id * diff * originalColor.rgb;

	float spec = pow(max(dot(N, H), 0.0), ns);
	vec3 specular = Is * spec * ks;

	// vec3 color = ambient + diffuse + specular;

	// fragColor = vec4(color, originalColor.a);
	float shadow = ShadowCalculation(f_worldPosition);
	// float lightproj = textureProj(modelTextureShadow, f_vertexData.shadowCoord);
	vec3 lighting = (ambient + (1.0 - shadow) * diffuse + (1.0 - shadow) * specular);
	fragColor = vec4(lighting, originalColor.a);
	// brightColor = 0.0001 * vec4(1.0) * textureProj(modelTextureShadow, f_vertexData.shadowCoord);
}

void RenderTriceShadowPoint() {
	vec3 N = normalize(texture(modelTextureNormal, f_vertexData.texCoord.xy).rgb * 2.0 - vec3(1.0));
	vec3 V = normalize(f_vertexData.eyeDirNormalMapping);
	vec3 L = normalize(f_vertexData.lightDirNormalMapping);
	vec3 H = normalize(f_vertexData.halfwayDirNormalMapping);

	// vec3 R = reflect(-L, N);
	// actually trice has no texture
	vec3 ambient = Ia * kd;

	float diff = max(dot(N, L), 0.0);
	vec3 diffuse = Id * diff * kd;

	float spec = pow(max(dot(N, H), 0.0), ns);
	vec3 specular = Is * spec * ks;

	// vec3 color = ambient + diffuse + specular;

	// fragColor = vec4(color, 1.0);
	float shadow = ShadowCalculation(f_worldPosition);
	// float lightproj = textureProj(modelTextureShadow, f_vertexData.shadowCoord);
	vec3 lighting = (ambient + (1.0 - shadow) * diffuse + (1.0 - shadow) * specular);
	fragColor = vec4(lighting, 1.0);
}

void getPointShadowDepthIndoor() {
	float lightDistance = length(f_worldPosition.xyz - lightBloomPos);
    
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / pointShadowFarPlane;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;
}

void getPointShadowDepthTrice() {
	float lightDistance = length(f_worldPosition.xyz - lightBloomPos);
    
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / pointShadowFarPlane;
    
    // write this as modified depth
    gl_FragDepth = lightDistance;

}

/*
	##################################
	#   Directional Shadow Mapping   #
	##################################
*/
void RenderIndoorShadow() {
	vec3 N = normalize(f_vertexData.N);
	vec3 L = normalize(f_vertexData.L);
	vec3 H = normalize(f_vertexData.H);

	vec4 originalColor = vec4(0.0);
	if(hasTexture == 0) {
		originalColor = vec4(kd, 1.0);
	}
	else {
		originalColor = texture(modelTexture, f_vertexData.texCoord.xy);
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
	float lightproj = textureProj(modelTextureShadow, f_vertexData.shadowCoord);
	fragColor = lightproj * vec4(color, originalColor.a);
	brightColor = 0.0001 * vec4(1.0) * textureProj(modelTextureShadow, f_vertexData.shadowCoord);
}

void RenderTriceShadow() {
	vec3 N = normalize(texture(modelTextureNormal, f_vertexData.texCoord.xy).rgb * 2.0 - vec3(1.0));
	vec3 V = normalize(f_vertexData.eyeDirNormalMapping);
	vec3 L = normalize(f_vertexData.lightDirNormalMapping);
	vec3 H = normalize(f_vertexData.halfwayDirNormalMapping);

	// vec3 R = reflect(-L, N);
	// actually trice has no texture
	vec3 ambient = Ia * kd;

	float diff = max(dot(N, L), 0.0);
	vec3 diffuse = Id * diff * kd;

	float spec = pow(max(dot(N, H), 0.0), ns);
	vec3 specular = Is * spec * ks;

	vec3 color = ambient + diffuse + specular;

	// fragColor = vec4(color, 1.0);
	float lightproj = textureProj(modelTextureShadow, f_vertexData.shadowCoord);
	fragColor = lightproj * vec4(color, 1.0);
	brightColor = 0.0001 * vec4(1.0) * textureProj(modelTextureShadow, f_vertexData.shadowCoord);
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
		originalColor = texture(modelTexture, f_vertexData.texCoord.xy);
		if(originalColor.a < 0.5) discard;
	}
	vec3 N = normalize(f_vertexData.N);
	/* Output */
	fragColor = vec4(normalize(f_worldPosition) * 0.5 + 0.5, 1.0);
	brightColor = vec4(normalize(f_vertexData.N) * 0.5 + 0.5, 1.0);
	ambientColor = vec4(ka, 1.0);
	diffuseColor = vec4(originalColor.rgb, 1.0);
	specularColor = vec4(ks, 1.0);
}

void RenderTriceDeferred(){
	/* Init */
	vec4 originalColor = vec4(kd, 1.0);
	vec3 N = normalize(f_vertexData.N);
	/* Output */
	fragColor = vec4(normalize(f_worldPosition) * 0.5 + 0.5, 1.0);
	brightColor = vec4(f_vertexData.N, 1.0);
	ambientColor = vec4(ka, 1.0);
	diffuseColor = vec4(originalColor.rgb, 1.0);
	specularColor = vec4(ks, 1.0);
}

/*
	##################################
	#		TOON	  Shading		 #
	##################################
*/

void RenderIndoorToon() {
	vec3 N = normalize(f_vertexData.N);
	vec3 L = normalize(f_vertexData.L);
	vec3 H = normalize(f_vertexData.H);

	vec4 originalColor = vec4(0.0);
	if(hasTexture == 0) {
		originalColor = vec4(kd, 1.0);
	}
	else {
		originalColor = texture(modelTexture, f_vertexData.texCoord.xy);
	}

	if (originalColor.a < 0.5) {
		discard;
	}

	vec3 ambient = Ia * originalColor.rgb;

	float diff = max(dot(N, L), 0.0);
	float spec = pow(max(dot(N, H), 0.0), ns);
	

	/* TOON SHADING MOTHER FATHER */
	diff = floor(diff * quantized) / quantized;
	spec = floor(spec * quantized) / quantized;
	
	vec3 diffuse = Id * diff * originalColor.rgb;
	vec3 specular = Is * spec * ks;

	float pointLightDistance = length(lightBloomPos - f_worldPosition);
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

void RenderTriceToon() {
	vec3 N = normalize(texture(modelTextureNormal, f_vertexData.texCoord.xy).rgb * 2.0 - vec3(1.0));
	vec3 V = normalize(f_vertexData.eyeDirNormalMapping);
	vec3 L = normalize(f_vertexData.lightDirNormalMapping);
	vec3 H = normalize(f_vertexData.halfwayDirNormalMapping);

	// vec3 R = reflect(-L, N);
	// actually trice has no texture
	vec3 ambient = Ia * kd;

	float diff = max(dot(N, L), 0.0);
	float spec = pow(max(dot(N, H), 0.0), ns);

	/* TOON SHADING MOTHER FATHER */
	diff = floor(diff * quantized) / quantized;
	spec = floor(spec * quantized) / quantized;

	vec3 diffuse = Id * diff * kd;
	vec3 specular = Is * spec * ks;

	float pointLightDistance = length(lightBloomPos - f_worldPosition);
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
}

void RenderLightSphereToon() {
	fragColor = vec4(1.0); // set all 4 vector values to 1.0

	float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > lightThreshold)
		brightColor = vec4(fragColor.rgb, 1.0);
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}

/*
	##################################
	#		Regular  Shading		 #
	##################################
*/

void RenderIndoor() {
	vec3 N = normalize(f_vertexData.N);
	vec3 L = normalize(f_vertexData.L);
	vec3 H = normalize(f_vertexData.H);

	vec4 originalColor = vec4(0.0);
	if(hasTexture == 0) {
		originalColor = vec4(kd, 1.0);
	}
	else {
		originalColor = texture(modelTexture, f_vertexData.texCoord.xy);
	}

	if (originalColor.a < 0.5) {
		discard;
	}

	vec3 ambient = Ia * originalColor.rgb;

	float diff = max(dot(N, L), 0.0);
	float spec = pow(max(dot(N, H), 0.0), ns);

	float pointLightDistance = length(lightBloomPos - f_worldPosition);
	float attenuation = 1.0 / (constant + linear * pointLightDistance + quadratic * (pointLightDistance * pointLightDistance));

	vec3 diffuse = Id * diff * originalColor.rgb;
	vec3 specular = Is * spec * ks;

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
	vec3 N = normalize(f_vertexData.N);
	vec3 L = normalize(f_vertexData.L);
	vec3 H = normalize(f_vertexData.H);
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
	fragColor = vec4(color, 1.0);

			float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
		if(brightness > 1.0)
			brightColor = vec4(color, 1.0);
		else
			brightColor = vec4(0.0, 0.0, 0.0, 1.0);

	*/
	vec3 N = normalize(texture(modelTextureNormal, f_vertexData.texCoord.xy).rgb * 2.0 - vec3(1.0));
	vec3 V = normalize(f_vertexData.eyeDirNormalMapping);
	vec3 L = normalize(f_vertexData.lightDirNormalMapping);
	vec3 H = normalize(f_vertexData.halfwayDirNormalMapping);

	// vec3 R = reflect(-L, N);
	// actually trice has no texture
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
		brightColor = vec4(1.0);
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
	// fragColor = brightColor;
}

void RenderAreaLight() {
	fragColor = vec4(lightColor, 1.0f);
}

// Render Everything but area light

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


void RenderIndoorAreaLight()
{

	vec3 N = normalize(f_vertexData.N);

	vec4 originalColor = vec4(0.0);
	if(hasTexture == 0) {
		originalColor = vec4(kd, 1.0);
	}
	else {
		originalColor = texture(modelTexture, f_vertexData.texCoord.xy);
	}

	if (originalColor.a < 0.5) {
		discard;
	}

    // gamma correction
    // vec3 mDiffuse = texture(modelTexture, texcoord).xyz;// * vec3(0.7f, 0.8f, 0.96f);
	// i have no clue what mspecular is
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

    result = lightColor /* * intensity*/  * (/*ambient +*/ Is * specular + originalColor.rgb * diffuse * Id);

    fragColor = vec4(ToSRGB(result), 1.0f);
}


void main() {
	switch(PostProcessMain)
	{
	case 1:
		{
			if (shadingModelId == 0) {
				RenderIndoor();
			} else if (shadingModelId == 1) {
				RenderTrice();
			} else if (shadingModelId == 2) {
				RenderLightSphere();
			}
			break;
		}
	case 2:
		{
			if (shadingModelId == 0) {
				RenderIndoorDeferred();
			} else if (shadingModelId == 1) {
				RenderTriceDeferred();
			} else if (shadingModelId == 2) {
				discard;
			}
			break;
		}
	case 3:
		{
			if (postProcessId == 0) {
				// shadow map does not need any color in fragment
				return;
			} else { // render with shadow 
				if (shadingModelId == 0) {
					RenderIndoorShadow();
				} else if (shadingModelId == 1) {
					RenderTriceShadow();
				} else if (shadingModelId == 2) {
					RenderLightSphere();
				} 
			}
			break;
		}
	case 4:
		{
			if (shadingModelId == 0) {
				RenderIndoorToon();
			} else if (shadingModelId == 1) {
				RenderTriceToon();
			} else if (shadingModelId == 2) {
				RenderLightSphereToon();
			}
			break;
		}
	case 5:
		{
			if (shadingModelId == 0) {
				RenderIndoorAreaLight();
			} else if (shadingModelId == 1) {
				discard;
				// RenderTriceAreaLight();
			} else if (shadingModelId == 2) {
				// RenderLightSphere();
				discard;
			} else if (shadingModelId == 3) {
				RenderAreaLight();
			}
			break;
		}
	case 6:
		{	
			if (postProcessId == 0) {
				if (shadingModelId == 0) {
					getPointShadowDepthIndoor();
				} else if (shadingModelId == 1) {
					getPointShadowDepthTrice();
				} else if (shadingModelId == 2) {
					discard;
				} else {
					discard;
				}
			} else {
				if (shadingModelId == 0) {
					RenderIndoorShadowPoint();
				} else if (shadingModelId == 1) {
					RenderTriceShadowPoint();
				} else if (shadingModelId == 2) {
					RenderLightSphere();
					// discard;
				} else if (shadingModelId == 3) {
					// RenderIndoorShadowPoint();
					discard;
				}
			}
			break;
		}
	// case 5:
	// 	{
	// 		if (shadingModelId == 0) {
	// 			RenderIndoorShadow();
	// 		} else if (shadingModelId == 1) {
	// 			RenderTriceShadow();
	// 		} else if (shadingModelId == 2) {
	// 			RenderLightSphere();
	// 		}
	// 		break;
	// 	}
	default:
		{
			if (shadingModelId == 0) {
				RenderIndoor();
			} else if (shadingModelId == 1) {
				RenderTrice();
			} else if (shadingModelId == 2) {
				RenderLightSphere();
			}
			break;
		}
	}
}