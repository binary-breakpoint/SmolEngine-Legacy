#version 450 core

layout(location = 0) in vec3 v_WorldPos;
layout(location = 1) in float v_Exposure;

layout (location = 0) out vec4 out_color;

layout (binding = 1) uniform samplerCube samplerCubeMap;

const float PI = 3.141592653589793238462643383279502884197169;
const vec3 UP = vec3(0.0, 1.0, 0.0);

layout (std140, binding = 36) uniform DynamicSky
{
	vec4 primaries;
	vec4 sunPosition;
	vec4 mieKCoefficient;

	float depolarizationFactor;
	float luminance;
	float mieCoefficient;
	float mieDirectionalG;

	float mieV;
	float mieZenithLength;
	float numMolecules;
	float rayleigh;

	float rayleighZenithLength;
	float refractiveIndex;
	float sunAngularDiameterDegrees;
	float sunIntensityFactor;

	float sunIntensityFalloffSteepness;
	float tonemapWeighting;
	float turbidity;
};

layout (std140, binding = 27) uniform SceneBuffer
{
	float nearClip;
    float farClip;
    float exoposure;
    float pad;

	mat4 projection;
	mat4 view;
	mat4 skyBoxMatrix;
	vec4 cameraPos;
	vec4 ambientColor;

} sceneData;

vec3 totalRayleigh(vec3 lambda)
{
	return (8.0 * pow(PI, 3.0) * pow(pow(refractiveIndex, 2.0) - 1.0, 2.0) * (6.0 + 3.0 * depolarizationFactor)) / (3.0 * numMolecules * pow(lambda, vec3(4.0)) * (6.0 - 7.0 * depolarizationFactor));
}

vec3 totalMie(vec3 lambda, vec3 K, float T)
{
	float c = 0.2 * T * 10e-18;
	return 0.434 * c * PI * pow((2.0 * PI) / lambda, vec3(mieV - 2.0)) * K;
}

float rayleighPhase(float cosTheta)
{
	return (3.0 / (16.0 * PI)) * (1.0 + pow(cosTheta, 2.0));
}

float henyeyGreensteinPhase(float cosTheta, float g)
{
	return (1.0 / (4.0 * PI)) * ((1.0 - pow(g, 2.0)) / pow(1.0 - 2.0 * g * cosTheta + pow(g, 2.0), 1.5));
}

float sunIntensity(float zenithAngleCos)
{
	float cutoffAngle = PI / 1.95; // Earth shadow hack
	return sunIntensityFactor * max(0.0, 1.0 - exp(-((cutoffAngle - acos(zenithAngleCos)) / sunIntensityFalloffSteepness)));
}

// Whitescale tonemapping calculation, see http://filmicgames.com/archives/75
// Also see http://blenderartists.org/forum/showthread.php?321110-Shaders-and-Skybox-madness
const float A = 0.15; // Shoulder strength
const float B = 0.50; // Linear strength
const float C = 0.10; // Linear angle
const float D = 0.20; // Toe strength
const float E = 0.02; // Toe numerator
const float F = 0.30; // Toe denominator
vec3 Uncharted2Tonemap(vec3 W)
{
	return ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
}

layout(push_constant) uniform ConstantData
{
	uint state;
};

void main()
{
	vec4 skyColor = vec4(0);
	switch(state)
	{
		case 0:
		{
			vec3 texColor = texture(samplerCubeMap, v_WorldPos).rgb;
	        skyColor = vec4(texColor, 0.0);
			break;
		}

		case 1:
		{
			// Rayleigh coefficient
	        float sunfade = 1.0 - clamp(1.0 - exp((sunPosition.y / 450000.0)), 0.0, 1.0);
	        float rayleighCoefficient = rayleigh - (1.0 * (1.0 - sunfade));
	        vec3 betaR = totalRayleigh(primaries.xyz) * rayleighCoefficient;
	
	        // Mie coefficient
	        vec3 betaM = totalMie(primaries.xyz, mieKCoefficient.xyz, turbidity) * mieCoefficient;
	        
	        // Optical length, cutoff angle at 90 to avoid singularity
	        float zenithAngle = acos(max(0.0, dot(UP, normalize(v_WorldPos - sceneData.cameraPos.xyz))));
	        float denom = cos(zenithAngle) + 0.15 * pow(93.885 - ((zenithAngle * 180.0) / PI), -1.253);
	        float sR = rayleighZenithLength / denom;
	        float sM = mieZenithLength / denom;
	        
	        // Combined extinction factor
	        vec3 Fex = exp(-(betaR * sR + betaM * sM));
	        
	        // In-scattering
	        vec3 sunDirection = normalize(sunPosition.xyz);
	        float cosTheta = dot(normalize(v_WorldPos - sceneData.cameraPos.xyz), sunDirection);
	        vec3 betaRTheta = betaR * rayleighPhase(cosTheta * 0.5 + 0.5);
	        vec3 betaMTheta = betaM * henyeyGreensteinPhase(cosTheta, mieDirectionalG);
	        float sunE = sunIntensity(dot(sunDirection, UP));
	        vec3 Lin = pow(sunE * ((betaRTheta + betaMTheta) / (betaR + betaM)) * (1.0 - Fex), vec3(1.5));
	        Lin *= mix(vec3(1.0), pow(sunE * ((betaRTheta + betaMTheta) / (betaR + betaM)) * Fex, vec3(0.5)), clamp(pow(1.0 - dot(UP, sunDirection), 5.0), 0.0, 1.0));
	        
	        // Composition + solar disc
	        float sunAngularDiameterCos = cos(sunAngularDiameterDegrees);
	        float sundisk = smoothstep(sunAngularDiameterCos, sunAngularDiameterCos + 0.00002, cosTheta);
	        vec3 L0 = vec3(0.1) * Fex;
	        L0 += sunE * 19000.0 * Fex * sundisk;
	        vec3 texColor = Lin + L0;
	        texColor *= 0.04;
	        texColor += vec3(0.0, 0.001, 0.0025) * 0.3;
	        
	        // Tonemapping
	        vec3 whiteScale = 1.0 / Uncharted2Tonemap(vec3(tonemapWeighting));
	        vec3 curr = Uncharted2Tonemap((log2(2.0 / pow(luminance, 4.0))) * texColor);
	        vec3 color = curr * whiteScale;
	        vec3 retColor = pow(color, vec3(1.0 / (1.2 + (1.2 * sunfade))));
        
	        skyColor = vec4(retColor, 0.0);
			break;
		}
	}

	out_color = skyColor;
}