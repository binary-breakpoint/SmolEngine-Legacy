#pragma once
#ifdef OPENGL_IMPL
#include "Primitives/Framebuffer.h"

namespace SmolEngine
{
	struct FramebufferData;

	class OpenglFramebuffer
	{
	public:

		OpenglFramebuffer();
		~OpenglFramebuffer();

		void Init(const FramebufferSpecification& data);
		void Recreate();

		// Binding
		void Bind();
		void UnBind();
		void BindColorAttachment(uint32_t slot = 0);

		//  Events
		void OnResize(const uint32_t width, const uint32_t height);

		// Getters
		const FramebufferSpecification& GetSpecification() const;
		uint32_t GetColorAttachmentID() const;
		uint32_t GetRendererID() const;

	private:

		FramebufferSpecification    m_Data = {};
		uint32_t                    m_RendererID = 0;
		uint32_t                    m_ColorAttachment = 0;
		uint32_t                    m_DepthAttachment = 0;
		const uint32_t              m_MaxSize = 8192;
	};
}
#endif