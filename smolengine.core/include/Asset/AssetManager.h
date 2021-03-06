#pragma once
#include "Asset/Asset.h"

#include <string>
#include <mutex>
#include <unordered_map>

namespace SmolEngine
{
	class AssetManager
	{
	public:
		AssetManager();

		static void         Add(const std::string& path, const Ref<Asset>& asset, AssetType type);
		static bool         Contains(const std::string& path);
		static bool         Remove(const std::string& path);
		static std::string  GetPathByID(size_t id);
		static uint32_t     GetCount();
		static void         Clear();

		template<typename T>
		static Ref<T> GetAssetByID(size_t id)
		{
			const auto& it = s_Instance->m_Registry.find(id);
			if (it != s_Instance->m_Registry.end())
				return std::static_pointer_cast<T>(it->second);

			return nullptr;
		}

		template<typename T>
		static Ref<T> GetAsset(const std::string& path)
		{
			const auto& it = s_Instance->m_IDHash.find(path);
			if (it != s_Instance->m_IDHash.end())
				return GetAssetByID<T>(it->second);

			return nullptr;
		}

	private:
		static AssetManager*                    s_Instance;
		std::mutex                              m_Mutex{};
		std::unordered_map<size_t, Ref<Asset>>  m_Registry;
		std::unordered_map<size_t, std::string> m_PathHash;
		std::unordered_map<std::string, size_t> m_IDHash;
	};
}