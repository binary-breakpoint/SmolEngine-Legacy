#include "stdafx.h"
#ifndef OPENGL_IMPL
#include "Backends/Vulkan/VulkanInstance.h"

#include <assert.h>

#define VK_KHR_WIN32_SURFACE_EXTENSION_NAME "VK_KHR_win32_surface"

namespace SmolEngine
{
	VulkanInstance::VulkanInstance()
	{

	}

	VulkanInstance::~VulkanInstance()
	{
		if (m_VKInstance != VK_NULL_HANDLE)
		{
			//vkDestroyInstance(m_VKInstance, nullptr);
		}
	}

	void VulkanInstance::Init()
	{
		CreateAppInfo();
	}

	bool VulkanInstance::CreateAppInfo()
	{
		VkApplicationInfo appInfo = {};
		{
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

#ifdef  Frostium_EDITOR
			appInfo.pApplicationName = "Frostium Editor";
#else
			appInfo.pApplicationName = "Frostium Game";
#endif
			appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.pEngineName = "Frostium";
			appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.apiVersion = VK_API_VERSION_1_2;
		}

		return CreateInstance(appInfo);
	}

	bool VulkanInstance::CreateInstance(const VkApplicationInfo& appInfo)
	{
		std::vector<const char*> instanceLayers = {};
		std::vector<const char*> instanceExt = { VK_KHR_SURFACE_EXTENSION_NAME };
#ifdef SMOLENGINE_DEBUG
		instanceLayers.push_back("VK_LAYER_KHRONOS_validation");
		instanceExt.push_back("VK_EXT_debug_utils");
#endif

#ifdef _WIN32
		instanceExt.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif 

		VkInstanceCreateInfo instanceInfo = {};
		{
			instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instanceInfo.pApplicationInfo= &appInfo;
			instanceInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExt.size());
			instanceInfo.ppEnabledExtensionNames = instanceExt.data();
			instanceInfo.enabledLayerCount = static_cast<uint32_t>(instanceLayers.size());
			instanceInfo.ppEnabledLayerNames = instanceLayers.data();
		}

		VkResult result = vkCreateInstance(&instanceInfo, nullptr, &m_VKInstance);

		assert(result == VK_SUCCESS);

		return result == VK_SUCCESS;
	}

	const VkInstance VulkanInstance::GetInstance() const
	{
		return m_VKInstance;
	}

}
#endif