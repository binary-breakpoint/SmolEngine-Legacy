#pragma once
#include "Memory.h"
#include "Primitives/PrimitiveBase.h"
#include "Primitives/Texture.h"

#include <glm/glm.hpp>

namespace SmolEngine
{
	enum class AttachmentFormat : uint16_t
	{
		UNORM_8,
		UNORM2_8,
		UNORM3_8,
		UNORM4_8,

		UNORM_16,
		UNORM2_16,
		UNORM3_16,
		UNORM4_16,

		SFloat_16,
		SFloat2_16,
		SFloat3_16,
		SFloat4_16,

		SFloat_32,
		SFloat2_32,
		SFloat3_32,
		SFloat4_32,

		SInt_8,
		SInt2_8,
		SInt3_8,
		SInt4_8,

		SInt_16,
		SInt2_16,
		SInt3_16,
		SInt4_16,

		SInt_32,
		SInt2_32,
		SInt3_32,
		SInt4_32,

		Color,
		Depth
};

	enum class FramebufferSpecialisation : uint16_t
	{
		None,
		Raytracing,
		ShadowMap,
		CopyBuffer
	};

	enum class MSAASamples : uint16_t
	{
		SAMPLE_COUNT_1,
		SAMPLE_COUNT_2,
		SAMPLE_COUNT_4,
		SAMPLE_COUNT_8,
		SAMPLE_COUNT_16,
		SAMPLE_COUNT_MAX_SUPPORTED
	};

	struct FramebufferAttachment
	{
		FramebufferAttachment() = default;
		FramebufferAttachment(AttachmentFormat _format, bool _bClearOp = false,
			const std::string& _name = "")
			:Format(_format), bClearOp(_bClearOp), Name(_name) { }

		bool                                 bClearOp = true;
		std::string                          Name = "";
		AttachmentFormat                     Format = AttachmentFormat::Color;
		glm::vec4                            ClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	};

	struct FramebufferSpecification
	{
		MSAASamples                          eMSAASampels = MSAASamples::SAMPLE_COUNT_1;
		FramebufferSpecialisation            eSpecialisation = FramebufferSpecialisation::None;
		ImageFilter                          eFiltering = ImageFilter::NEAREST;
		bool                                 bTargetsSwapchain = false;
		bool                                 bUsedByImGui = false;
		bool                                 bResizable = true;
		bool                                 bDepthSampler = false;
		bool                                 bAutoSync = true;
		int32_t                              Width = 0;
		int32_t                              Height = 0;
		std::vector<FramebufferAttachment>   Attachments;
	};

	class Framebuffer: public PrimitiveBase
	{
	public:
		virtual ~Framebuffer() = default;

		virtual bool                      Build(FramebufferSpecification* info) = 0;
		virtual void                      OnResize(uint32_t width, uint32_t height) = 0;
		virtual void*                     GetImGuiTextureID(uint32_t index = 0) = 0;
		virtual void                      SetClearColor(const glm::vec4& color) {};
		const FramebufferSpecification&   GetSpecification() const;
		static Ref<Framebuffer>           Create();

	protected:
		bool                              BuildBase(FramebufferSpecification* info);

	protected:
		FramebufferSpecification          m_Info = {};
	};
}