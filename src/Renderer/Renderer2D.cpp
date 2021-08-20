#include "stdafx.h"
#include "Renderer/Renderer2D.h"

#include "Primitives/GraphicsPipeline.h"
#include "Primitives/Mesh.h"
#include "Primitives/Text.h"

#include "Tools/Utils.h"

#include <imgui/imgui.h>

#ifdef FROSTIUM_SMOLENGINE_IMPL
namespace SmolEngine
#else
namespace Frostium
#endif
{
	void Renderer2DStorage::Initilize()
	{
		CreateFramebuffers();
		CreatePipelines();
	}

	void Renderer2DStorage::SetRenderTarget(Framebuffer* target)
	{
		MainPipeline.SetFramebuffers({ target });
		MainFB = target;
	}

	void Renderer2DStorage::CreatePipelines()
	{
		const std::string& path = GraphicsContext::GetSingleton()->GetResourcesPath();
		GraphicsPipelineCreateInfo pipelineCI = {};

		// Main pipeline
		{
			BufferLayout PBRlayout =
			{
				{ DataTypes::Float3, "aPos" },
				{ DataTypes::Float3, "aNormal" },
				{ DataTypes::Float3, "aTangent" },
				{ DataTypes::Float2, "aUV" },
				{ DataTypes::Int4,   "aBoneIDs"},
				{ DataTypes::Float4, "aWeight"}
			};

			GraphicsPipelineShaderCreateInfo shaderCI = {};
			{
				shaderCI.FilePaths[ShaderType::Vertex] = path + "Shaders/2D.vert";
				shaderCI.FilePaths[ShaderType::Fragment] = path + "Shaders/2D.frag";

				ShaderBufferInfo bufferInfo = {};
				bufferInfo.bGlobal = false;
				bufferInfo.Size = sizeof(ShaderInstanceSize) * RendererDrawList2D::MaxQuads;

				shaderCI.BufferInfos[1] = bufferInfo;
			};

			pipelineCI.bDepthWriteEnabled = false;
			pipelineCI.bDepthTestEnabled = false;
			pipelineCI.PipelineName = "Deferred_2D";
			pipelineCI.VertexInputInfos = { VertexInputInfo(sizeof(PBRVertex), PBRlayout) };
			pipelineCI.eCullMode = CullMode::None;
			pipelineCI.eSrcColorBlendFactor = BlendFactor::SRC_ALPHA;
			pipelineCI.eDstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;
			pipelineCI.eSrcAlphaBlendFactor = BlendFactor::SRC_ALPHA;
			pipelineCI.eDstAlphaBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;
			pipelineCI.eColorBlendOp = BlendOp::ADD;
			pipelineCI.eAlphaBlendOp = BlendOp::SUBTRACT;

			pipelineCI.TargetFramebuffers = { MainFB };
			pipelineCI.ShaderCreateInfo = shaderCI;

			assert(MainPipeline.Create(&pipelineCI) == PipelineCreateResult::SUCCESS);
		}

		// Text Pipeline
		{
			pipelineCI = {};
			BufferLayout layout =
			{
				{ DataTypes::Float3, "aPos" },
				{ DataTypes::Float2, "aUV" },
			};

			GraphicsPipelineShaderCreateInfo shaderCI = {};
			{
				shaderCI.FilePaths[ShaderType::Vertex] = path + "Shaders/Text.vert";
				shaderCI.FilePaths[ShaderType::Fragment] = path + "Shaders/Text.frag";
			};

			pipelineCI.PipelineName = "Text";
			pipelineCI.eCullMode = CullMode::None;
			pipelineCI.bDepthTestEnabled = false;
			pipelineCI.eSrcColorBlendFactor = BlendFactor::ONE;
			pipelineCI.eDstColorBlendFactor = BlendFactor::ONE_MINUS_SRC_ALPHA;
			pipelineCI.eSrcAlphaBlendFactor = BlendFactor::ONE;
			pipelineCI.eDstAlphaBlendFactor = BlendFactor::ZERO;
			pipelineCI.VertexInputInfos = { VertexInputInfo(sizeof(TextVertex), layout) };
			pipelineCI.TargetFramebuffers = { MainFB };
			pipelineCI.ShaderCreateInfo = shaderCI;

			assert(TextPipeline.Create(&pipelineCI) == PipelineCreateResult::SUCCESS);
		}

		Mesh::Create(path + "Models/plane.gltf", &PlaneMesh);
	}

	void Renderer2DStorage::CreateFramebuffers()
	{
		MainFB = GraphicsContext::GetSingleton()->GetFramebuffer();
	}

	void Renderer2DStorage::UpdateUniforms(RendererDrawList2D* drawList)
	{
		MainPipeline.SubmitBuffer(SceneDataBP, sizeof(SceneViewProjection), drawList->SceneInfo);
		MainPipeline.SubmitBuffer(InstancesBP, ShaderInstanceSize * drawList->InstIndex, drawList->ShaderInstances);
		MainPipeline.UpdateSamplers(drawList->Textures, 0);
		TextPipeline.UpdateSamplers(drawList->FontTextures, 1);
	}

	void Renderer2D::DrawFrame(ClearInfo* clearInfo, Renderer2DStorage* storage, RendererDrawList2D* drawList, bool batch_cmd)
	{
		CommandBufferStorage cmdBuffer;
		PrepareCmdBuffer(&cmdBuffer, storage, batch_cmd);
		ClearAtachments(clearInfo, storage);
		UpdateUniforms(storage, drawList);
		UpdateCmdBuffer(storage, drawList);

		if (!batch_cmd)
			VulkanCommandBuffer::ExecuteCommandBuffer(&cmdBuffer);
	}

	void Renderer2D::UpdateCmdBuffer(Renderer2DStorage* storage, RendererDrawList2D* drawList)
	{
		if (drawList->InstIndex > 0 || drawList->TextIndex > 0)
		{
			storage->MainPipeline.BeginRenderPass();
			{
				// Sprites
				for (uint32_t i = 0; i < RendererDrawList2D::MaxLayers; ++i)
				{
					auto& cmd = drawList->CommandBuffer[i];
					if (cmd.Instances > 0)
					{
						storage->MainPipeline.SubmitPushConstant(ShaderType::Vertex, sizeof(uint32_t), &cmd.DataOffset);
						storage->MainPipeline.DrawMeshIndexed(&storage->PlaneMesh, cmd.Instances);

						cmd.Reset();
					}
				}

				// Text
				for (uint32_t i = 0; i < drawList->TextIndex; ++i)
				{
					auto& msg = drawList->TextMessages[i];

					storage->TextPushConstant.Model = msg.Model;
					storage->TextPushConstant.Color = msg.Obj->m_Color;
					storage->TextPushConstant.TexIndex = msg.TextureIndex;

					storage->TextPipeline.SubmitPushConstant(ShaderType::Vertex, storage->TextPCSize, &storage->TextPushConstant);
					storage->TextPipeline.DrawIndexed(&msg.Obj->m_VertexBuffer, &msg.Obj->m_IndexBuffer);
				}
			}
			storage->MainPipeline.EndRenderPass();
		}
	}

	void Renderer2D::ClearAtachments(const ClearInfo* clearInfo, Renderer2DStorage* storage)
	{
		if (clearInfo->bClear)
		{
			storage->MainPipeline.BeginRenderPass();
			storage->MainPipeline.ClearColors(clearInfo->color);
			storage->MainPipeline.EndRenderPass();
		}
	}

	void Renderer2D::UpdateUniforms(Renderer2DStorage* storage, RendererDrawList2D* drawList)
	{
		storage->UpdateUniforms(drawList);
	}

	void Renderer2D::PrepareCmdBuffer(CommandBufferStorage* cmd, Renderer2DStorage* storage, bool batch_cmd)
	{
		if (batch_cmd)
		{
			storage->MainPipeline.BeginCommandBuffer(batch_cmd);
			storage->TextPipeline.BeginCommandBuffer(batch_cmd);
			return;
		}

		VulkanCommandBuffer::CreateCommandBuffer(cmd);
		storage->MainPipeline.SetCommandBuffer(cmd->Buffer);
		storage->TextPipeline.SetCommandBuffer(cmd->Buffer);
	}

	RendererDrawList2D::RendererDrawList2D()
	{
		Textures.resize(MaxTextureSlot);
		FontTextures.resize(MaxTextMessages);

		Textures[0] = GraphicsContext::GetSingleton()->GetWhiteTexture();
	}

	void RendererDrawList2D::BeginSubmit(SceneViewProjection* sceneViewProj)
	{
		ClearDrawList();

		SceneInfo = sceneViewProj;
		Frustum.Update(sceneViewProj->Projection * sceneViewProj->View);
	}

	void RendererDrawList2D::EndSubmit()
	{
		BuildDrawList();
	}

	void RendererDrawList2D::SubmitSprite(const glm::vec3& worldPos, const glm::vec3& scale, const glm::vec3& rotation, uint32_t layerIndex, Texture* texture, bool frustumCulling, const glm::vec4& color)
	{
		if (!IsDrawable(worldPos, frustumCulling))
			return;

		uint32_t index = InstIndex;
		Instances[index].Layer = layerIndex > RendererDrawList2D::MaxLayers ? const_cast<uint32_t*>(&RendererDrawList2D::MaxLayers) : &layerIndex;
		Instances[index].Color = const_cast<glm::vec4*>(&color);
		Instances[index].Position = const_cast<glm::vec3*>(&worldPos);
		Instances[index].Rotation = const_cast<glm::vec3*>(&rotation);
		Instances[index].Scale = const_cast<glm::vec3*>(&scale);
		Instances[index].TextureIndex = AddTexture(texture);

		InstIndex++;
	}

	void RendererDrawList2D::SubmitQuad(const glm::vec3& worldPos, const glm::vec3& scale, const glm::vec3& rotation, uint32_t layerIndex, bool frustumCulling, const glm::vec4& color)
	{
		if (!IsDrawable(worldPos, frustumCulling))
			return;

		uint32_t index = InstIndex;
		Instances[index].Layer = layerIndex > RendererDrawList2D::MaxLayers ? const_cast<uint32_t*>(&RendererDrawList2D::MaxLayers) : &layerIndex;
		Instances[index].Color = const_cast<glm::vec4*>(&color);
		Instances[index].Position = const_cast<glm::vec3*>(&worldPos);
		Instances[index].Rotation = const_cast<glm::vec3*>(&rotation);
		Instances[index].Scale = const_cast<glm::vec3*>(&scale);
		Instances[index].TextureIndex = 0;

		InstIndex++;
	}

	void RendererDrawList2D::SubmitLight2D(const glm::vec3& worldPos, const glm::vec4& color, float radius, float lightIntensity, bool frustumCulling)
	{

	}

	void RendererDrawList2D::SubmitText(Text* text)
	{
		auto& msg = TextMessages[TextIndex];
		msg.Obj = text;
		msg.TextureIndex = TextIndex;

		FontTextures[TextIndex] = &text->m_SDFTexture;
		Utils::ComposeTransform(text->m_Pos, text->m_Rotation, text->m_Scale, msg.Model);
		TextIndex++;
	}

	uint32_t RendererDrawList2D::AddTexture(Texture* in_texture)
	{
		// temp
		for (uint32_t i = 0; i < SampleIndex; ++i)
		{
			Texture* tex = Textures[i];
			if (tex == in_texture)
				return i;
		}

		uint32_t index = SampleIndex;
		Textures[index] = in_texture;
		SampleIndex++;
		return index;
	}

	void RendererDrawList2D::SetViewProjection(SceneViewProjection* sceneViewProj)
	{
		SceneInfo = sceneViewProj;
		Frustum.Update(sceneViewProj->Projection * sceneViewProj->View);
	}

	Frustum& RendererDrawList2D::GetFrustum()
	{
		return Frustum;
	}

	void RendererDrawList2D::ClearDrawList()
	{
		InstIndex = 0;
		SampleIndex = 1;
		TextIndex = 0;
		FontSampleIndex = 0;
	}

	void RendererDrawList2D::BuildDrawList()
	{
		for (uint32_t i = 0; i < InstIndex; ++i)
		{
			for (uint32_t l = 0; l < RendererDrawList2D::MaxLayers; ++l)
			{
				Instance& inst = Instances[i];
				if (l == *inst.Layer)
				{
					auto& cmd = CommandBuffer[l];
					if (cmd.OffsetApplied == false)
					{
						cmd.DataOffset = i;
						cmd.OffsetApplied = true;
					}

					cmd.Instances++;
					ShaderInstances[i].Color = *inst.Color;
					ShaderInstances[i].Params.w = *inst.Layer;
					ShaderInstances[i].Params.x = inst.TextureIndex;
					Utils::ComposeTransform2D(*inst.Position, *inst.Rotation, *inst.Scale, ShaderInstances[i].Model);
				}
			}
		}
	}

	bool RendererDrawList2D::IsDrawable(const glm::vec3& worldPos, bool frustumCulling)
	{
		if (frustumCulling)
		{
			if (!Frustum.CheckSphere(worldPos)) { return false; }
		}

		if (InstIndex >= RendererDrawList2D::MaxQuads)
			return false;

		return true;
	}
}