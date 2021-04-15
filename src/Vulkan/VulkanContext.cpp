#include "stdafx.h"
#ifndef FROSTIUM_OPENGL_IMPL
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanRenderPass.h"

#include <GLFW/glfw3.h>
#include <imgui/examples/imgui_impl_vulkan.h>

namespace Frostium
{
	VulkanContext::~VulkanContext()
	{
		m_Swapchain.~VulkanSwapchain();
		m_CommandBuffer.~VulkanCommandBuffer();
		m_CommandPool.~VulkanCommandPool();
		m_Semaphore.~VulkanSemaphore();
		m_Instance.~VulkanInstance();
		m_Device.~VulkanDevice();
	}

	void VulkanContext::OnResize(uint32_t* width, uint32_t* height)
	{
		if (m_IsInitialized == false)
		{
			return;
		}

		m_Swapchain.OnResize(width, height, &m_CommandBuffer);
	}

	void VulkanContext::Setup(GLFWwindow* window, uint32_t* width, uint32_t* height)
	{
		assert(glfwVulkanSupported() == GLFW_TRUE);

		// Vulkan Initialization

		bool swapchain_initialized = false;
		{
			m_Instance.Init();
			m_Device.Init(&m_Instance);
			m_CommandPool.Init(&m_Device);

			swapchain_initialized = m_Swapchain.Init(&m_Instance, &m_Device, window);
			if (swapchain_initialized)
			{
				m_Swapchain.Create(width, height);

				m_CommandBuffer.Init(&m_Device, &m_CommandPool, &m_Swapchain);
				m_Semaphore.Init(&m_Device, &m_CommandBuffer);

				m_Swapchain.Prepare(*width, *height);
			}
		}

		if (swapchain_initialized)
		{
			m_IsInitialized = true;
			s_ContextInstance = this;
			m_Window = window;

			return;
		}

		NATIVE_ERROR("Couldn't create Vulkan Context!");
		abort();
	}

	void VulkanContext::BeginFrame()
	{
		// Get next image in the swap chain (back/front buffer)
		VK_CHECK_RESULT(m_Swapchain.AcquireNextImage(m_Semaphore.GetPresentCompleteSemaphore()));

		m_CurrentVkCmdBuffer = VulkanContext::GetCommandBuffer().GetVkCommandBuffer();
		VkCommandBufferBeginInfo cmdBufInfo = {};
		{
			cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			cmdBufInfo.pNext = nullptr;
		}

		VK_CHECK_RESULT(vkBeginCommandBuffer(m_CurrentVkCmdBuffer, &cmdBufInfo));
	}

	void VulkanContext::SwapBuffers(bool skip)
	{
		// ImGUI pass
		{
			if (m_UseImGUI)
			{
				if (ImGui::GetDrawData()->CmdListsCount > 0)
				{
					auto framebuffer = m_Swapchain.GetCurrentFramebuffer();
					uint32_t width = m_Swapchain.GetWidth();
					uint32_t height = m_Swapchain.GetHeight();

					// Set clear values for all framebuffer attachments with loadOp set to clear
					// We use two attachments (color and depth) that are cleared at the start of the subpass and as such we need to set clear values for both
					VkClearValue clearValues[2];
					clearValues[1].depthStencil = { 1.0f, 0 };

					VkRenderPassBeginInfo renderPassBeginInfo = {};
					renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
					renderPassBeginInfo.pNext = nullptr;
					renderPassBeginInfo.renderPass = m_Swapchain.GetVkRenderPass();
					renderPassBeginInfo.renderArea.offset.x = 0;
					renderPassBeginInfo.renderArea.offset.y = 0;
					renderPassBeginInfo.renderArea.extent.width = width;
					renderPassBeginInfo.renderArea.extent.height = height;
					renderPassBeginInfo.clearValueCount = 2;
					renderPassBeginInfo.pClearValues = clearValues;

					// Set target frame buffer
					renderPassBeginInfo.framebuffer = framebuffer;

					// Start the first sub pass specified in our default render pass setup by the base class
					// This will clear the color and depth attachment
					vkCmdBeginRenderPass(m_CurrentVkCmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

					// Update dynamic viewport state
					VkViewport viewport = {};
					viewport.x = 0;
					viewport.y = (float)height;
					viewport.height = -(float)height;
					viewport.width = (float)width;
					viewport.minDepth = (float)0.0f;
					viewport.maxDepth = (float)1.0f;
					vkCmdSetViewport(m_CurrentVkCmdBuffer, 0, 1, &viewport);

					// Update dynamic scissor state
					VkRect2D scissor = {};
					scissor.extent.width = width;
					scissor.extent.height = height;
					scissor.offset.x = 0;
					scissor.offset.y = 0;
					vkCmdSetScissor(m_CurrentVkCmdBuffer, 0, 1, &scissor);

					// Draw Imgui
					ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_CurrentVkCmdBuffer);

					vkCmdEndRenderPass(m_CurrentVkCmdBuffer);
				}
			}
		}

		const auto& present_ref = m_Semaphore.GetPresentCompleteSemaphore();
		const auto& render_ref = m_Semaphore.GetRenderCompleteSemaphore();
		constexpr uint64_t DEFAULT_FENCE_TIME_OUT = 100000000000;
		// Use a fence to wait until the command buffer has finished execution before using it again

		VK_CHECK_RESULT(vkWaitForFences(m_Device.GetLogicalDevice(), 1, &m_Semaphore.GetVkFences()[m_Swapchain.GetCurrentBufferIndex()], VK_TRUE, UINT64_MAX));
		VK_CHECK_RESULT(vkResetFences(m_Device.GetLogicalDevice(), 1, &m_Semaphore.GetVkFences()[m_Swapchain.GetCurrentBufferIndex()]));

		// Pipeline stage at which the queue submission will wait (via pWaitSemaphores)
		VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		// The submit info structure specifies a command buffer queue submission batch
		VkSubmitInfo submitInfo = {};
		{
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.pWaitDstStageMask = &waitStageMask;               // Pointer to the list of pipeline stages that the semaphore waits will occur at
			submitInfo.pWaitSemaphores = &present_ref;      // Semaphore(s) to wait upon before the submitted command buffer starts executing
			submitInfo.waitSemaphoreCount = 1;                           // One wait semaphore
			submitInfo.pSignalSemaphores = &render_ref;     // Semaphore(s) to be signaled when command buffers have completed
			submitInfo.signalSemaphoreCount = 1;                         // One signal semaphore
			submitInfo.pCommandBuffers = &m_CurrentVkCmdBuffer; // Command buffers(s) to execute in this batch (submission)
			submitInfo.commandBufferCount = 1;                           // One command buffer
		}

		// Ending the render pass will add an implicit barrier transitioning the frame buffer color attachment to
		// VK_IMAGE_LAYOUT_PRESENT_SRC_KHR for presenting it to the windowing system
		VK_CHECK_RESULT(vkEndCommandBuffer(m_CurrentVkCmdBuffer));

		// Submit to the graphics queue passing a wait fence
		VK_CHECK_RESULT(vkQueueSubmit(m_Device.GetQueue(), 1, &submitInfo, m_Semaphore.GetVkFences()[m_Swapchain.GetCurrentBufferIndex()]));

		// Present the current buffer to the swap chain
		// Pass the semaphore signaled by the command buffer submission from the submit info as the wait semaphore for swap chain presentation
		// This ensures that the image is not presented to the windowing system until all commands have been submitted
		VkResult present = m_Swapchain.QueuePresent(m_Device.GetQueue(), render_ref);
		if (!((present == VK_SUCCESS) || (present == VK_SUBOPTIMAL_KHR))) {

			if (present == VK_ERROR_OUT_OF_DATE_KHR)
			{
				uint32_t w = m_Swapchain.GetWidth();
				uint32_t h = m_Swapchain.GetHeight();

				m_Swapchain.OnResize(&w, &h, &m_CommandBuffer);
				return;
			}
			else
			{
				VK_CHECK_RESULT(present);
			}
		}

		VK_CHECK_RESULT(vkWaitForFences(m_Device.GetLogicalDevice(), 1, &m_Semaphore.GetVkFences()[m_Swapchain.GetCurrentBufferIndex()], VK_TRUE, DEFAULT_FENCE_TIME_OUT));
	}

	inline uint64_t VulkanContext::GetBufferDeviceAddress(VkBuffer buffer)
	{
		VkBufferDeviceAddressInfoKHR bufferDeviceAI{};
		bufferDeviceAI.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
		bufferDeviceAI.buffer = buffer;
		return vkGetBufferDeviceAddressKHR(m_Device.GetLogicalDevice(), &bufferDeviceAI);
	}
}
#endif