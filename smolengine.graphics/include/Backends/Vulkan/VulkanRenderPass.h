#pragma once
#ifndef OPENGL_IMPL
#include "Backends/Vulkan/Vulkan.h"

namespace SmolEngine
{
	struct RenderPassGenInfo
	{
		VkFormat                     ColorFormat;
		VkFormat                     DepthFormat;
		VkSampleCountFlagBits        MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		uint32_t                     NumDepthAttachments = 0;
		uint32_t                     NumColorAttachments = 0;
		uint32_t                     NumResolveAttachments = 0;
	};

	class VulkanRenderPass
	{
	public:
		static void Create(FramebufferSpecification* framebufferSpec, RenderPassGenInfo* renderPassInfo, VkRenderPass& outPass);
	};
}
#endif