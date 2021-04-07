#pragma once
#include "Common/Core.h"
#include "GraphicsContext.h"
#ifndef FROSTIUM_OPENGL_IMPL
#include "Vulkan/VulkanPipeline.h"
#endif

#include "Common/GraphicsPipelineShaderCreateInfo.h"
#include "Common/VertexArray.h"
#include "Common/VertexBuffer.h"
#include "Common/IndexBuffer.h"
#include "Common/Texture.h"
#include "Common/Shared.h"
#include "Common/Shader.h"

namespace Frostium
{
	class Framebuffer;
	class CubeTexture;
	class Mesh;

	enum class DrawMode : uint16_t
	{
		Triangle,
		Line,
		Fan
	};

	enum class CullMode : uint16_t
	{
		None,
		Back,
		Front
	};

	enum class PipelineCreateResult : uint16_t
	{
		SUCCESS,
		ERROR_INVALID_CREATE_INFO,
		ERROR_PIPELINE_NOT_INVALIDATED,
		ERROR_PIPELINE_NOT_CREATED,
		ERROR_SHADER_NOT_RELOADED
	};

	struct GraphicsPipelineCreateInfo
	{
		//TODO: Add flags

		CullMode                             PipelineCullMode = CullMode::Back;
		bool                                 bDepthTestEnabled = true;
		bool                                 bDepthBiasEnabled = false;

		float                                MinDepth = 0.0f;
		float                                MaxDepth = 1.0f;

		int32_t                              DescriptorSets = 1;
		int32_t                              StageCount = -1;

		Framebuffer*                         TargetFramebuffer = nullptr;
		GraphicsPipelineShaderCreateInfo*    ShaderCreateInfo = nullptr;
		std::string                          PipelineName = "";
		std::vector<DrawMode>                PipelineDrawModes = { DrawMode::Triangle };
		std::vector<VertexInputInfo>         VertexInputInfos;
	};

	class GraphicsPipeline
	{
	public:

		~GraphicsPipeline();

		PipelineCreateResult Create(GraphicsPipelineCreateInfo* pipelineInfo);
		PipelineCreateResult Reload();
		void Destroy();

		// Render Pass
		void ClearColors(const glm::vec4& clearColors = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
		void BeginRenderPass(uint32_t framebufferIndex = 0, bool flip = false);
		void EndRenderPass();

		// Cmd Buffer
		void BeginCommandBuffer(bool isMainCmdBufferInUse = false);
		void EndCommandBuffer();
		void FlushCommandBuffer();

		// Draw
		void DrawIndexed(DrawMode mode = DrawMode::Triangle, uint32_t vertexBufferIndex = 0, 
			uint32_t indexBufferIndex = 0, uint32_t descriptorSetIndex = 0);
		void Draw(uint32_t vertextCount, DrawMode mode = DrawMode::Triangle,
			uint32_t vertexBufferIndex = 0, uint32_t descriptorSetIndex = 0);
		void DrawMesh(Mesh* mesh, DrawMode mode = DrawMode::Triangle,
			uint32_t instances = 1, uint32_t descriptorSetIndex = 0);

		// Submit
		void SubmitBuffer(uint32_t bindingPoint, size_t size, const void* data, uint32_t offset = 0);
		template<typename T>
		void SubmitUniform(const std::string& name, const void* data, uint32_t arrayElements = 0, size_t size = 0)
		{
#ifdef FROSTIUM_OPENGL_IMPL
			m_Shader->Bind();
			m_Shader->SumbitUniform<T>(name, data, arrayElements, size);
#endif
		}
		void SubmitPushConstant(ShaderType shaderStage, size_t size, const void* data);

		// Update Resources
		void SetVertexBuffers(std::vector<Ref<VertexBuffer>> buffer);
		void SetIndexBuffers(std::vector<Ref<IndexBuffer>> buffer);
		void UpdateVertextBuffer(void* vertices, size_t size, uint32_t bufferIndex = 0, uint32_t offset = 0);
		void UpdateIndexBuffer(uint32_t* indices, size_t count, uint32_t bufferIndex = 0, uint32_t offset = 0);
#ifndef FROSTIUM_OPENGL_IMPL
		void CmdUpdateVertextBuffer(const void* data, size_t size, uint32_t bufferIndex = 0, uint32_t offset = 0)
		{
			m_VertexBuffers[bufferIndex]->CmdUpdateData(m_CommandBuffer, data, size, offset);
		}

		void CmdUpdateIndexBuffer(uint32_t* indices, size_t count, uint32_t bufferIndex = 0, uint32_t offset = 0)
		{
			m_IndexBuffers[bufferIndex]->CmdUpdateData(m_CommandBuffer, indices, sizeof(uint32_t) * count, offset);
		}

		bool UpdateVulkanImageDescriptor(uint32_t bindingPoint, const VkDescriptorImageInfo& imageInfo, uint32_t descriptorSetIndex = 0)
		{
			return m_VulkanPipeline.m_Descriptors[descriptorSetIndex].UpdateImageResource(bindingPoint, imageInfo);
		}

#endif
		bool UpdateSamplers(const std::vector<Texture*>& textures, uint32_t bindingPoint, uint32_t descriptorSetIndex = 0);
		bool UpdateSampler(Ref<Texture>& tetxure, uint32_t bindingPoint, uint32_t descriptorSetIndex = 0);
		bool UpdateCubeMap(Texture* cubeMap, uint32_t bindingPoint, uint32_t descriptorSetIndex = 0);

#ifndef FROSTIUM_OPENGL_IMPL
		const VkPipeline& GetVkPipeline(DrawMode mode)
		{
			return m_VulkanPipeline.GetVkPipeline(mode);
		}

		const VulkanShader* GetVulkanShader() const
		{
			return m_Shader->GetVulkanShader();
		}

		VkCommandBuffer GetVkCommandBuffer() const
		{
			return m_CommandBuffer;
		}

		void SetCommandBuffer(VkCommandBuffer cmdBuffer)
		{
			m_CommandBuffer = cmdBuffer;
		}
#endif
#ifdef FROSTIUM_OPENGL_IMPL

		void BindOpenGLShader()
		{
			m_Shader->Bind();
		}
#endif
	private:

		// Helpres
		bool IsPipelineCreateInfoValid(const GraphicsPipelineCreateInfo* pipelineInfo);

	private:

#ifndef FROSTIUM_OPENGL_IMPL
		VulkanPipeline                    m_VulkanPipeline = {};
		VkCommandBuffer                   m_CommandBuffer = nullptr;
		bool                              m_IsMainCmdBufferInUse = false;
#endif
		Ref<VertexArray>                  m_VextexArray = nullptr;
		Ref<Shader>                       m_Shader = nullptr;
		GraphicsContext*                  m_GraphicsContext = nullptr;
		GraphicsPipelineCreateInfo        m_PiplineCreateInfo;

		std::vector<Ref<VertexBuffer>>    m_VertexBuffers;
		std::vector<Ref<IndexBuffer>>     m_IndexBuffers;
	};
}