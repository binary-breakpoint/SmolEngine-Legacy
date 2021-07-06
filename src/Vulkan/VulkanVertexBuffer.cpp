#include "stdafx.h"
#ifndef FROSTIUM_OPENGL_IMPL
#include "Vulkan/VulkanVertexBuffer.h"

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	void VulkanVertexBuffer::Init(const void* data, uint64_t size, bool is_static)
	{
		if (is_static)
		{
			CreateStaticBuffer(data, size, 
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

			return;
		}

		CreateBuffer(data, size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	}

	void VulkanVertexBuffer::Init(uint64_t size)
	{
		CreateBuffer(size, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	}
}
#endif