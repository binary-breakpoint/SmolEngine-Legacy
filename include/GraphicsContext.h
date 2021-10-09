#pragma once

#ifdef  OPENGL_IMPL
#include "Backends/OpenGL/OpenglContext.h"
#include "Backends/OpenGL/OpenglRendererAPI.h"
#else
#include "Backends/Vulkan/VulkanContext.h"
#endif

#include "Common/Memory.h"
#include "Common/Flags.h"
#include "Window/Window.h"
#include "Window/Events.h"
#include "Camera/EditorCamera.h"
#include "Camera/Camera.h"
#include "Primitives/Framebuffer.h"

#include "Pools/MaterialPool.h"
#include "Pools/MeshPool.h"

#include "GUI/Backends/ImGuiContext.h"
#include "GUI/Backends/NuklearContext.h"

#include <functional>

namespace SmolEngine
{
	struct WindowCreateInfo;
	struct WindowData;
	struct GraphicsContextState;
	struct RendererStorage;
	struct Renderer2DStorage;
	struct RendererStorageBase;

	class CubeMap;
	class Texture;
	class MaterialPool;
	class JobsSystemInstance;

	struct GraphicsContextInitInfo
	{
		bool              bTargetsSwapchain = true;
		bool              bAutoResize = true;
		bool              bVsync = true;
		FeaturesFlags     eFeaturesFlags = FeaturesFlags::Imgui | FeaturesFlags::RendererDebug;
		MSAASamples       eMSAASamples = MSAASamples::SAMPLE_COUNT_1;
		WindowCreateInfo* pWindowCI = nullptr;
		std::string       ResourcesFolderPath = "../resources/";
	};

	struct SceneViewProjection
	{
		SceneViewProjection() = default;
		SceneViewProjection(Camera* cam)
		{
			Update(cam);
		}

		void Update(Camera* cam)
		{
			View = cam->GetViewMatrix();
			Projection = cam->GetProjection();
			CamPos = glm::vec4(cam->GetPosition(), 1.0f);
			NearClip = cam->GetNearClip();
			FarClip = cam->GetFarClip();
			SkyBoxMatrix = glm::mat4(glm::mat3(View));
		}

		glm::mat4 Projection = glm::mat4(1.0f);
		glm::mat4 View = glm::mat4(1.0f);
		glm::vec4 CamPos = glm::vec4(1.0f);
		float     NearClip = 0.0f;
		float     FarClip = 0.0f;
		glm::vec2 Pad1;
		glm::mat4 SkyBoxMatrix = glm::mat4(1.0f);

		friend struct RendererStorage;
		friend struct RendererDrawList;
	};

	struct ClearInfo
	{
		bool      bClear = true;
		glm::vec4 color = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	};

	class GraphicsContext
	{
	public:
		GraphicsContext() = default;
		GraphicsContext(GraphicsContextInitInfo* info);
		~GraphicsContext();
								      
		void                              ProcessEvents();
		void                              BeginFrame(float time);
		void                              SwapBuffers();
		void                              ShutDown();      		          
		static GraphicsContext*           GetSingleton();
		Ref<Framebuffer>                  GetMainFramebuffer() const;
		Ref<Texture>                      GetWhiteTexture() const;
		Window* GetWindow() const;
		float                             GetGltfTime() const;
		float                             GetDeltaTime() const;
		float                             GetLastFrameTime() const;
		const std::string&                GetResourcesPath() const;							      	          
		void                              SetEventCallback(std::function<void(Event&)> callback);
		void                              SetFramebufferSize(uint32_t width, uint32_t height);
		float                             CalculateDeltaTime();
		bool                              IsWindowMinimized() const;
		void                              Resize(uint32_t* width, uint32_t* height);
									      
	private:					          
		void                              OnEvent(Event& event);

	private:	
		static GraphicsContext*           s_Instance;
		Window*                           m_Window = nullptr;
		Ref<Texture>                      m_DummyTexure = nullptr;
		Ref<Texture>                      m_StorageTexure = nullptr;
		Ref<Texture>                      m_DummyCubeMap = nullptr;
		Ref<Framebuffer>                  m_Framebuffer = nullptr;
		Ref<MaterialPool>                 m_MaterialPool = nullptr;
		Ref<MeshPool>                     m_MeshPool = nullptr;
		Ref<JobsSystemInstance>           m_JobsSystem = nullptr;
		ContextBaseGUI*                   m_ImGuiContext = nullptr;
		ContextBaseGUI*                   m_NuklearContext = nullptr;
		bool                              m_bWindowMinimized = false;
		bool                              m_bIs2DStoragePreAlloc = false;
		bool                              m_bIsStoragePreAlloc = false;
		float                             m_LastFrameTime = 1.0f;
		float                             m_DeltaTime = 0.0f;
#ifdef  OPENGL_IMPL		      			  
		OpenglContext                     m_OpenglContext = {};
		OpenglRendererAPI*                m_RendererAPI = nullptr;
#else								      
		VulkanContext                     m_VulkanContext = {};
#endif								      
		GraphicsContextInitInfo           m_CreateInfo = {};
		EventSender                       m_EventHandler = {};
		std::string                       m_ResourcesFolderPath = "";
		std::function<void(Event&)>       m_EventCallback;
		std::vector<RendererStorageBase*> m_StorageList;

		friend struct RendererStorage;
		friend struct Renderer2DStorage;
		friend struct RendererStorageBase;
		friend class GraphicsPipeline;
		friend class DebugRenderer;
		friend class ImGuiContext;
		friend class VulkanPBR;
		friend class VulkanContext;
		friend class VulkanDescriptor;
		friend class Window;
		friend class EnvironmentMap;
	};
}
