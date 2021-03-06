#include "stdafx.h"
#ifdef OPENGL_IMPL
#include "Backends/OpenGL/OpenglFramebuffer.h"

#include <glad/glad.h>

namespace SmolEngine
{
	OpenglFramebuffer::OpenglFramebuffer()
	{

	}

	OpenglFramebuffer::~OpenglFramebuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(1, &m_ColorAttachment);
		glDeleteTextures(1, &m_DepthAttachment);
	}

	void OpenglFramebuffer::Init(const FramebufferSpecification& data)
	{
		m_Data = data;
		Recreate();
	}

	void OpenglFramebuffer::Recreate()
	{
		if (m_RendererID != 0)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(1, &m_ColorAttachment);
			glDeleteTextures(1, &m_DepthAttachment);
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Data.Width, m_Data.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Data.Width, m_Data.Height);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

#if 0
		if (!glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		{
			DebugLog::LogError("Creating framebuffer is failed!");
		}
#endif


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenglFramebuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Data.Width, m_Data.Height);
	}

	void OpenglFramebuffer::BindColorAttachment(uint32_t slot)
	{
		glBindTexture(slot, m_ColorAttachment);
	}

	void OpenglFramebuffer::UnBind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenglFramebuffer::OnResize(const uint32_t width, const uint32_t height)
	{
		if (width == 0 || height == 0 || width > m_MaxSize || height > m_MaxSize) { DebugLog::LogWarn("Framebuffer: invalid value"); return; }

		m_Data.Width = width; m_Data.Height = height;
		Recreate();
	}

	const FramebufferSpecification& OpenglFramebuffer::GetSpecification() const
	{
		return m_Data;
	}

	uint32_t OpenglFramebuffer::GetColorAttachmentID() const
	{
		return m_ColorAttachment;
	}

	uint32_t OpenglFramebuffer::GetRendererID() const
	{
		return m_RendererID;
	}
}
#endif