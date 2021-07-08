#include "stdafx.h"
#include "MaterialLibrary.h"
#include "Common/SLog.h"
#include "Extensions/JobsSystemInstance.h"

#include <filesystem>
#include <mutex>
#include <cereal/archives/json.hpp>

#ifdef FROSTIUM_SMOLENGINE_IMPL
std::mutex* s_Mutex = nullptr;
#endif

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	const uint32_t maxTextures = 4096;
	MaterialLibrary* MaterialLibrary::s_Instance = nullptr;

	MaterialLibrary::MaterialLibrary()
	{
		s_Instance = this;
#ifdef FROSTIUM_SMOLENGINE_IMPL
		s_Mutex = new std::mutex();
#endif
		m_Textures.resize(maxTextures);
	}

	MaterialLibrary::~MaterialLibrary()
	{
		s_Instance = nullptr;
#ifdef FROSTIUM_SMOLENGINE_IMPL
		delete s_Mutex;
#endif
	}

	uint32_t MaterialLibrary::Add(MaterialCreateInfo* infoCI, const std::string& name)
	{
		uint32_t materialID = 0;
		size_t hashID = m_Hash(name);
		if (m_MaterialMap.size() > 0)
		{
			auto& it = m_MaterialMap.find(hashID);
			if (it != m_MaterialMap.end())
			{
				return it->second;
			}
		}

		PBRMaterial newMaterial = {};
		{

#ifdef FROSTIUM_SMOLENGINE_IMPL
			JobsSystemInstance::BeginSubmition();
			{
				JobsSystemInstance::Schedule([&]() {newMaterial.AlbedroTexIndex = AddTexture(&infoCI->AlbedroTex, newMaterial.UseAlbedroTex); });
				JobsSystemInstance::Schedule([&]() {newMaterial.NormalTexIndex = AddTexture(&infoCI->NormalTex, newMaterial.UseNormalTex); });
				JobsSystemInstance::Schedule([&]() {newMaterial.MetallicTexIndex = AddTexture(&infoCI->MetallnessTex, newMaterial.UseMetallicTex); });
				JobsSystemInstance::Schedule([&]() {newMaterial.RoughnessTexIndex = AddTexture(&infoCI->RoughnessTex, newMaterial.UseRoughnessTex); });
				JobsSystemInstance::Schedule([&]() {newMaterial.AOTexIndex = AddTexture(&infoCI->AOTex, newMaterial.UseAOTex); });
				JobsSystemInstance::Schedule([&]() {newMaterial.EmissiveTexIndex = AddTexture(&infoCI->EmissiveTex, newMaterial.UseEmissiveTex); });
			}
			JobsSystemInstance::EndSubmition();
#else
			newMaterial.AlbedroTexIndex = AddTexture(&infoCI->AlbedroTex, newMaterial.UseAlbedroTex);
			newMaterial.NormalTexIndex = AddTexture(&infoCI->NormalTex, newMaterial.UseNormalTex);
			newMaterial.MetallicTexIndex = AddTexture(&infoCI->MetallnessTex, newMaterial.UseMetallicTex);
			newMaterial.RoughnessTexIndex = AddTexture(&infoCI->RoughnessTex, newMaterial.UseRoughnessTex);
			newMaterial.AOTexIndex = AddTexture(&infoCI->AOTex, newMaterial.UseAOTex);
			newMaterial.EmissiveTexIndex = AddTexture(&infoCI->EmissiveTex, newMaterial.UseEmissiveTex);
#endif

			newMaterial.Metalness = infoCI->Metallness;
			newMaterial.Roughness = infoCI->Roughness;
			newMaterial.EmissionStrength = infoCI->EmissionStrength;
			newMaterial.Albedro = glm::vec4(infoCI->AlbedroColor, 1.0f);
		}

		materialID = m_MaterialIndex;
		m_Materials.emplace_back(newMaterial);
		m_MaterialMap.insert({ hashID, materialID });
		m_MaterialIndex++;
		return materialID;
	}

	bool MaterialLibrary::Delete(const std::string& name)
	{
		return false; // temp
	}

	void MaterialLibrary::Reset()
	{
		m_MaterialIndex = 0;
		m_TextureIndex = 0;

		for (const auto& tex : m_Textures)
		{
			if(tex)
				delete tex;
		}

		m_Textures.clear();
		m_Textures.resize(maxTextures);

		m_Materials.clear();
		m_MaterialMap.clear();
	}

	MaterialLibrary* MaterialLibrary::GetSinglenton()
	{
		return s_Instance;
	}

	uint32_t MaterialLibrary::AddTexture(const TextureCreateInfo* info, uint32_t& useTetxure)
	{
		uint32_t index = 0;
		if (info->FilePath.empty() == false)
		{
			Texture* tex = new Texture();
			Texture::Create(info, tex);

#ifdef FROSTIUM_SMOLENGINE_IMPL
			{
				const std::lock_guard<std::mutex> lock(*s_Mutex);

				index = m_TextureIndex;
				m_Textures[index] = tex;
				m_TextureIndex++;
			}
#else

			index = m_TextureIndex;
			m_Textures[index] = tex;
			m_TextureIndex++;
#endif

			useTetxure = true;
			return index;
		}

		useTetxure = false;
		return index;
	}

	PBRMaterial* MaterialLibrary::GetMaterial(uint32_t ID)
	{
		if (ID > m_MaterialIndex)
			return nullptr;

		return &m_Materials[ID];
	}

	PBRMaterial* MaterialLibrary::GetMaterial(std::string& path)
	{
		size_t hashID = m_Hash(path);
		const auto& it = m_MaterialMap.find(hashID);
		if (it == m_MaterialMap.end())
			return nullptr;

		return &m_Materials[it->second];
	}

	int32_t MaterialLibrary::GetMaterialID(std::string& path)
	{
		size_t hashID = m_Hash(path);
		const auto& it = m_MaterialMap.find(hashID);
		if (it == m_MaterialMap.end())
			return 0;

		return it->second;
	}

	int32_t MaterialLibrary::GetMaterialID(size_t& hashed_path)
	{
		const auto& it = m_MaterialMap.find(hashed_path);
		if (it == m_MaterialMap.end())
			return 0;

		return it->second;
	}

	std::vector<PBRMaterial>& MaterialLibrary::GetMaterials()
	{
		return m_Materials;
	}
	void MaterialLibrary::GetMaterialsPtr(void*& data, uint32_t& size)
	{
		if (m_Materials.size() > 0)
		{
			data = m_Materials.data();
			size = static_cast<uint32_t>(sizeof(PBRMaterial) * m_Materials.size());
		}
	}

	void MaterialLibrary::GetTextures(std::vector<Texture*>& out_textures) const
	{
		out_textures = m_Textures;
	}

	void MaterialCreateInfo::SetMetalness(float value)
	{
		Metallness = value;
	}

	void MaterialCreateInfo::SetRoughness(float value)
	{
		Roughness = value;
	}

	void MaterialCreateInfo::SetEmissionStrength(float value)
	{
		EmissionStrength = value;
	}

	void MaterialCreateInfo::SetAlbedro(const glm::vec3& color)
	{
		AlbedroColor = color;
	}

	void MaterialCreateInfo::SetTexture(MaterialTexture type, const TextureCreateInfo* info)
	{
		switch (type)
		{
		case MaterialTexture::Albedro:
			AlbedroTex = *info;
			break;
		case MaterialTexture::Normal:
			NormalTex = *info;
			break;
		case MaterialTexture::Metallic:
			MetallnessTex = *info;
			break;
		case MaterialTexture::Roughness:
			RoughnessTex = *info;
			break;
		case MaterialTexture::AO:
			AOTex = *info;
			break;
		case MaterialTexture::Emissive:
			EmissiveTex = *info;
			break;
		default:
			break;
		}
	}

	void MaterialCreateInfo::GetTextures(std::unordered_map<MaterialTexture, TextureCreateInfo*>& out_hashmap)
	{
		if (AlbedroTex.FilePath.empty() == false)
		{
			out_hashmap[MaterialTexture::Albedro] = &AlbedroTex;
		}

		if (NormalTex.FilePath.empty() == false)
		{
			out_hashmap[MaterialTexture::Normal] = &NormalTex;
		}

		if (MetallnessTex.FilePath.empty() == false)
		{
			out_hashmap[MaterialTexture::Metallic] = &MetallnessTex;
		}

		if (RoughnessTex.FilePath.empty() == false)
		{
			out_hashmap[MaterialTexture::Roughness] = &RoughnessTex;
		}

		if (AOTex.FilePath.empty() == false)
		{
			out_hashmap[MaterialTexture::AO] = &AOTex;
		}

		if (EmissiveTex.FilePath.empty() == false)
		{
			out_hashmap[MaterialTexture::Emissive] = &EmissiveTex;
		}
	}

	bool MaterialCreateInfo::Load(const std::string& filePath)
	{
		std::stringstream storage;
		std::ifstream file(filePath);
		if (!file)
		{
			NATIVE_ERROR("Could not open the file: {}", filePath);

			return false;
		}

		storage << file.rdbuf();
		{
			cereal::JSONInputArchive input{ storage };
			input(Metallness, Roughness, EmissionStrength, AlbedroTex, NormalTex, MetallnessTex, RoughnessTex,
				AOTex, EmissiveTex, AlbedroColor.r, AlbedroColor.g, AlbedroColor.b);
		}

		return true;
	}

	bool MaterialCreateInfo::Save(const std::string& filePath)
	{
		std::stringstream storage;
		{
			cereal::JSONOutputArchive output{ storage };
			serialize(output);
		}

		std::ofstream myfile(filePath);
		if (myfile.is_open())
		{
			myfile << storage.str();
			myfile.close();
			return true;
		}

		return false;
	}
}