#pragma once
#ifndef OPENGL_IMPL
#include "Backends/Vulkan/Vulkan.h"

namespace SmolEngine
{
	class VulkanInstance
	{
	public:

		VulkanInstance();
		~VulkanInstance();

		void Init();

		// Getters
		const VkInstance GetInstance() const;

	private:

		bool CreateAppInfo();
		bool CreateInstance(const VkApplicationInfo& info);

	private:

		VkInstance m_VKInstance = VK_NULL_HANDLE;

		friend class VulkanRendererAPI;
	};
}

#endif