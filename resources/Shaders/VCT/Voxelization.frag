#version 450
#extension GL_ARB_shader_image_load_store : require

struct Material
{
	vec4  Albedo;

	float Metalness;
	float Roughness;
	float EmissionStrength;
	uint  UseAlbedroTex;

	uint  UseNormalTex;
	uint  UseMetallicTex;
	uint  UseRoughnessTex;
    uint  UseAOTex;

	uint UseEmissiveTex;
	uint AlbedroTexIndex;
	uint NormalTexIndex;
	uint MetallicTexIndex;

	uint RoughnessTexIndex;
	uint AOTexIndex;
	uint EmissiveTexIndex;
	uint ID;
};

layout (location = 0) in Input
{
	vec3 wsPosition;
    vec3 position;
    vec3 texCoord;
	mat3 TBN;
	flat vec4 triangleAABB;
	flat Material Material;
} In;

layout (std140, binding = 202) uniform Data
{
    mat4 viewProjections[3];
	mat4 viewProjectionsI[3];

    vec3 worldMinPoint;
	float voxelScale;

	uint volumeDimension;
	uint flagStaticVoxels;
	vec2 pad;
};

layout (location = 0) out vec4 fragColor;

layout(binding = 0, r32ui) uniform uimage3D AlbedoBuffer;
layout(binding = 1, r32ui) uniform uimage3D NormalBuffer;
layout(binding = 2, r32ui) uniform uimage3D EmissionBuffer;

layout(binding = 3, rgba8) uniform writeonly image3D voxelAlbedo;
layout(binding = 4, rgba8) uniform writeonly image3D voxelNormal;
layout(binding = 5, rgba8) uniform writeonly image3D voxelEmission;

layout(binding = 6, r8) uniform image3D staticVoxelFlag;

layout (binding = 24) uniform sampler2D texturesMap[4096];

vec4 convRGBA8ToVec4(uint val)
{
    return vec4(float((val & 0x000000FF)), 
    float((val & 0x0000FF00) >> 8U), 
    float((val & 0x00FF0000) >> 16U), 
    float((val & 0xFF000000) >> 24U));
}

uint convVec4ToRGBA8(vec4 val)
{
    return (uint(val.w) & 0x000000FF) << 24U | 
    (uint(val.z) & 0x000000FF) << 16U | 
    (uint(val.y) & 0x000000FF) << 8U | 
    (uint(val.x) & 0x000000FF);
}

vec3 EncodeNormal(vec3 normal)
{
    return normal * 0.5f + vec3(0.5f);
}

vec3 DecodeNormal(vec3 normal)
{
    return normal * 2.0f - vec3(1.0f);
}

vec4 fetchAlbedoMap() 
{
	if( In.Material.UseAlbedroTex == 1)
	{
		 return vec4(texture(texturesMap[In.Material.AlbedroTexIndex], In.texCoord.xy).rgb, 1);
	}

	return In.Material.Albedo;
}

vec3 fetchNormalMap() 
{
	if(In.Material.UseNormalTex == 1)
	{  
        vec3 normal = texture(texturesMap[In.Material.NormalTexIndex], In.texCoord.xy).rgb;
        return normalize(In.TBN * (normal * 2.0 - 1.0));
	}
	else
	{
		return normalize(In.TBN[2]);
	}
}

float fetchMetallicMap() 
{
	if(In.Material.UseMetallicTex == 1)
	{
		return texture(texturesMap[In.Material.MetallicTexIndex], In.texCoord.xy).r;
	}

	return In.Material.Metalness;
}

float fetchRoughnessMap() 
{
	if(In.Material.UseRoughnessTex == 1)
	{
       return texture(texturesMap[In.Material.RoughnessTexIndex], In.texCoord.xy).r;
	}

	return In.Material.Roughness;
}

vec4 fetchEmissiveMap() 
{
	if(In.Material.UseEmissiveTex == 1)
	{
		return texture(texturesMap[In.Material.EmissiveTexIndex], In.texCoord.xy);
	}

	return vec4(In.Material.EmissionStrength);
}

float fetchAOMap() 
{
	if(In.Material.UseAOTex == 1)
	{
		return texture(texturesMap[In.Material.AOTexIndex], In.texCoord.xy).r;
	}

	return 1.0;
}

void StoreAvgAlbedo(vec4 value, ivec3 position)
{
		//imageStore(voxelAlbedo, position, value);
		//return;

	    value.rgb *= 255.0;                 // optimize following calculations
        uint newVal = convVec4ToRGBA8(value);
        uint prevStoredVal = 0;
        uint curStoredVal;
        uint numIterations = 0;
    
        while((curStoredVal = imageAtomicCompSwap(AlbedoBuffer, position, prevStoredVal, newVal)) 
                != prevStoredVal
                && numIterations < 255)
        {
            prevStoredVal = curStoredVal;
            vec4 rval = convRGBA8ToVec4(curStoredVal);
            rval.rgb = (rval.rgb * rval.a); // Denormalize
            vec4 curValF = rval + value;    // Add
            curValF.rgb /= curValF.a;       // Renormalize
            newVal = convVec4ToRGBA8(curValF);
    
            ++numIterations;
        }

		imageStore(voxelAlbedo, position, convRGBA8ToVec4(newVal));
}

void StoreAvgNormal(vec4 value, ivec3 position)
{
		//imageStore(voxelNormal, position, value);
		//return;

	    value.rgb *= 255.0;                 // optimize following calculations
        uint newVal = convVec4ToRGBA8(value);
        uint prevStoredVal = 0;
        uint curStoredVal;
        uint numIterations = 0;
    
        while((curStoredVal = imageAtomicCompSwap(NormalBuffer, position, prevStoredVal, newVal)) 
                != prevStoredVal
                && numIterations < 255)
        {
            prevStoredVal = curStoredVal;
            vec4 rval = convRGBA8ToVec4(curStoredVal);
            rval.rgb = (rval.rgb * rval.a); // Denormalize
            vec4 curValF = rval + value;    // Add
            curValF.rgb /= curValF.a;       // Renormalize
            newVal = convVec4ToRGBA8(curValF);
    
            ++numIterations;
        }

		imageStore(voxelNormal, position, convRGBA8ToVec4(newVal));
}

void StoreAvgEmission(vec4 value, ivec3 position)
{
		//imageStore(voxelEmission, position, value);
		//return;

	    value.rgb *= 255.0;                 // optimize following calculations
        uint newVal = convVec4ToRGBA8(value);
        uint prevStoredVal = 0;
        uint curStoredVal;
        uint numIterations = 0;
    
        while((curStoredVal = imageAtomicCompSwap(EmissionBuffer, position, prevStoredVal, newVal)) 
                != prevStoredVal
                && numIterations < 255)
        {
            prevStoredVal = curStoredVal;
            vec4 rval = convRGBA8ToVec4(curStoredVal);
            rval.rgb = (rval.rgb * rval.a); // Denormalize
            vec4 curValF = rval + value;    // Add
            curValF.rgb /= curValF.a;       // Renormalize
            newVal = convVec4ToRGBA8(curValF);
    
            ++numIterations;
        }
}

void main()
{
	 if(In.position.x < In.triangleAABB.x || In.position.y < In.triangleAABB.y || 
		In.position.x > In.triangleAABB.z || In.position.y > In.triangleAABB.w )
	{
		discard;
	}

	vec3 N = fetchNormalMap(); 		
	vec4 albedo = fetchAlbedoMap();
	vec4 emissive = fetchEmissiveMap();
	float metallic = fetchMetallicMap();
	float roughness = fetchRoughnessMap();
	roughness = max(roughness, 0.04);
    float ao = fetchAOMap();	
	
	// writing coords position
    ivec3 position = ivec3(In.wsPosition);

    if(flagStaticVoxels == 0)
    {
        bool isStatic = imageLoad(staticVoxelFlag, position).r > 0.0;
    }

	StoreAvgAlbedo(albedo, position);
	// average normal per fragments sorrounding the voxel volume
    vec4 normal = vec4(EncodeNormal(N), 1.0f);
	StoreAvgNormal(normal, position);
	// average emission per fragments sorrounding the voxel volume
	StoreAvgEmission(emissive, position);

    // doing a static flagging pass for static geometry voxelization
    if(flagStaticVoxels == 1)
    {
        imageStore(staticVoxelFlag, position, vec4(1.0));
    }
}