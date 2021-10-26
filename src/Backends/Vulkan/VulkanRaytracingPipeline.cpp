#include "stdafx.h"
#ifndef OPENGL_IMPL
#include "Backends/Vulkan/VulkanRaytracingPipeline.h"
#include "Backends/Vulkan/VulkanShader.h"

namespace SmolEngine
{
	bool VulkanRaytracingPipeline::Build(RaytracingPipelineCreateInfo* info)
	{
		if(!BuildEX(info))
			return false;


	}

	void VulkanRaytracingPipeline::SubmitPushConstant(ShaderType stage, size_t size, const void* data)
	{
		vkCmdPushConstants(m_CommandBuffer, m_PipelineLayout, VulkanShader::GetVkShaderStage(stage), 0, static_cast<uint32_t>(size), data);
	}

	bool VulkanRaytracingPipeline::SubmitBuffer(uint32_t binding, size_t size, const void* data, uint32_t offset)
	{
		return false;
	}

	bool VulkanRaytracingPipeline::UpdateSamplers(const std::vector<Ref<Texture>>& textures, uint32_t bindingPoint, bool storageImage)
	{
		return false;
	}

	bool VulkanRaytracingPipeline::UpdateSampler(Ref<Texture>& tetxure, uint32_t bindingPoint, bool storageImage)
	{
		return false;
	}

	bool VulkanRaytracingPipeline::UpdateSampler(Ref<Framebuffer>& framebuffer, uint32_t bindingPoint, uint32_t attachmentIndex)
	{
		return false;
	}

	bool VulkanRaytracingPipeline::UpdateSampler(Ref<Framebuffer>& framebuffer, uint32_t bindingPoint, const std::string& attachmentName)
	{
		return false;
	}

	bool VulkanRaytracingPipeline::UpdateImageDescriptor(uint32_t bindingPoint, void* descriptor)
	{
		return false;
	}

	bool VulkanRaytracingPipeline::UpdateCubeMap(Ref<Texture>& cubeMap, uint32_t bindingPoint)
	{
		return false;
	}
}

#endif