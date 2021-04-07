#include "stdafx.h"
#include "Common/Mesh.h"
#include "Common/VertexArray.h"
#include "Common/VertexBuffer.h"
#include "Common/IndexBuffer.h"
#include "Common/Material.h"

#include "Utils/Utils.h"
#include "Utils/ModelImporter.h"

#include <glm/glm.hpp>

namespace Frostium
{
    //TODO: add staging buffer
    Ref<Mesh> Mesh::Create(const std::string& filePath)
    {
        Ref<Mesh> mesh = std::make_shared<Mesh>();
        ImportedData* data = new ImportedData();
        if (ModelImporter::Load(filePath, data))
        {
            mesh->Init(data);
            mesh->m_Initialized = true;
        }

        delete data;
        return mesh;
    }

    void Mesh::Create(const std::string& filePath, Mesh* out_mesh)
    {
        if (out_mesh)
        {
            if (out_mesh->m_Initialized)
                return;

            ImportedData* data = new ImportedData();
            if (ModelImporter::Load(filePath, data))
            {
                out_mesh->Init(data);
                out_mesh->m_Initialized = true;
            }
        }
    }

    Ref<Mesh> Mesh::FindSubMeshByIndex(uint32_t index)
    {
        if (index >= m_SubMeshes.size())
            return nullptr;

        return m_SubMeshes[index];
    }

    Ref<Mesh> Mesh::FindSubMeshByName(const std::string& name)
    {
        for (auto& sub : m_SubMeshes)
        {
            if (sub->m_Name == name)
                return sub;
        }

        return nullptr;
    }

    const std::string& Mesh::GetName() const
    {
        return m_Name;
    }

    void Mesh::Free()
    {
        if (m_SubMeshes.size() != 0)
            m_SubMeshes.clear();

        m_Meshes.clear();
    }

    void Mesh::FindAllMeshes()
    {
        m_Meshes.resize(m_SubMeshes.size() + 1);
        uint32_t index = 0;
        m_Meshes[index] = this;
        index++;

        for (auto& sub : m_SubMeshes)
        {
            m_Meshes[index] = sub.get();
            index++;
        }
    }

    bool Mesh::Init(ImportedData* data)
    {
        Free();
        if (data->Components.size() > 1)
            m_SubMeshes.reserve(data->Components.size() - 1);

        for (uint32_t i = 0; i < static_cast<uint32_t>(data->Components.size()); ++i)
        {
            auto& component = data->Components[i];

            // Main
            if (i == 0)
            {
                CreateVertexAndIndexBuffers(component);
                continue;
            }

            // Sub
            Ref<Mesh> mesh = std::make_shared<Mesh>();
            mesh->CreateVertexAndIndexBuffers(component);
            m_SubMeshes.emplace_back(mesh);
        }

        FindAllMeshes();
        return true;
    }

    void Mesh::CreateVertexAndIndexBuffers(ImportedComponent& component)
    {
        m_Name = component.Name;
        m_VertexCount = static_cast<uint32_t>(component.VertexData.size());
        m_VertexBuffer = VertexBuffer::Create(component.VertexData.data(), static_cast<uint32_t>(sizeof(PBRVertex) * component.VertexData.size()), true);
        m_IndexBuffer = IndexBuffer::Create(component.Indices.data(), static_cast<uint32_t>(component.Indices.size()), true);
    }
}