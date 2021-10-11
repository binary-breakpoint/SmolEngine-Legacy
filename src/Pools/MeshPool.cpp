#include "stdafx.h"
#include "Pools/MeshPool.h"

namespace SmolEngine
{
	MeshPool::MeshPool(const std::string& root)
	{
		s_Instance = this;

		m_Cube = Mesh::Create();
		m_Cube->LoadFromFile(root + "Models/cube.gltf");

		m_Sphere = Mesh::Create();
		m_Sphere->LoadFromFile(root + "Models/sphere.gltf");

		m_Capsule = Mesh::Create();
		m_Capsule->LoadFromFile(root + "Models/capsule.gltf");

		m_Torus = Mesh::Create();
		m_Torus->LoadFromFile(root + "Models/torus.gltf");
	}

	MeshPool::~MeshPool()
	{
		s_Instance = nullptr;
	}

	Ref<Mesh> MeshPool::GetByPath(const std::string& path)
	{
		std::hash<std::string_view> hasher{};
		size_t id = hasher(path);

		return GetByID(id);
	}

	Ref<Mesh> MeshPool::GetByID(size_t id)
	{
		auto& it = s_Instance->m_IDMap.find(id);
		if (it != s_Instance->m_IDMap.end())
		{
			return it->second;
		}

		return nullptr;
	}

	Ref<Mesh> MeshPool::ConstructFromFile(const std::string& path)
	{
		std::hash<std::string_view> hasher{};
		size_t id = hasher(path);

		auto& it = s_Instance->m_IDMap.find(id);
		if (it != s_Instance->m_IDMap.end())
		{
			return it->second;
		}


		Ref<Mesh> mesh = Mesh::Create();
		bool is_loaded = mesh->LoadFromFile(path);
		if (is_loaded)
		{
			s_Instance->m_Mutex.lock();
			s_Instance->m_IDMap[id] = mesh;
			s_Instance->m_Mutex.unlock();
		}

		return is_loaded ? mesh : nullptr;
	}

	bool MeshPool::Remove(size_t id)
	{
		return s_Instance->m_IDMap.erase(id);
	}

	void MeshPool::Clear()
	{
		s_Instance->m_IDMap.clear();
	}

	Ref<Mesh> MeshPool::GetCube()
	{
		return s_Instance->m_Cube;
	}

	Ref<Mesh> MeshPool::GetSphere()
	{
		return s_Instance->m_Sphere;
	}

	Ref<Mesh> MeshPool::GetCapsule()
	{
		return s_Instance->m_Capsule;
	}

	Ref<Mesh> MeshPool::GetTorus()
	{
		return s_Instance->m_Torus;
	}
}