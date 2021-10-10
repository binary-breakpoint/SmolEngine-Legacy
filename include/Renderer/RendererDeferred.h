#pragma once
#include "Renderer/RendererShared.h"

#include "Primitives/GraphicsPipeline.h"
#include "Primitives/ComputePipeline.h"
#include "Primitives/Framebuffer.h"
#include "Primitives/Texture.h"
#include "Primitives/Mesh.h"
#include "Primitives/EnvironmentMap.h"
#include "Animation/AnimationController.h"
#include "Camera/Frustum.h"
#include "Tools/GLM.h"

namespace SmolEngine
{
	struct SceneData;
	struct SceneViewProjection;
	struct CommandBufferStorage;
	struct ClearInfo;

	static const uint32_t max_animations = 100;
	static const uint32_t max_anim_joints = 1000;
	static const uint32_t max_materials = 1000;
	static const uint32_t max_lights = 1000;
	static const uint32_t max_objects = 15000;

#pragma region Shader-Side Structures

	struct DirectionalLight
	{
		glm::vec4      Direction = glm::vec4(67, 56, 0, 0);
		glm::vec4      Color = glm::vec4(1.0);
		float          Intensity = 1.0f;
		float          Bias = 1.0f;
		float          zNear = 1.0f;
		float          zFar = 350.0f;
				        
		float          lightFOV = 45.0f;
		bool           IsActive = false;
		GLSL_BOOLPAD   Pad1;
		bool           IsCastShadows = false;
		GLSL_BOOLPAD   Pad2;
		bool           IsUseSoftShadows = true;
		GLSL_BOOLPAD   Pad3;
	};

	struct PointLight
	{
		glm::vec4      Position = glm::vec4(0);
		glm::vec4      Color = glm::vec4(1.0);
		float          Intensity = 1.0f;
		float          Raduis = 10.0f;
		float          Bias = 1.0f;
		bool           IsActive = true;
	private:
		GLSL_BOOLPAD   Pad1;
	};

	struct SpotLight
	{
		glm::vec4      Position = glm::vec4(0, 0, 0, 0);
		glm::vec4      Direction = glm::vec4(0, 0, 40, 0);
		glm::vec4      Color = glm::vec4(1.0);
		float          Intensity = 10.0f;
		float          CutOff = 40.0f;
		float          OuterCutOff = 5.0f;
		float          Raduis = 10.0f;

		float          Bias = 0.005f;
		bool           IsActive = true;
	private:		   
		GLSL_BOOLPAD   Pad1;
		uint32_t       Pad2[2];
	};

	struct InstanceData
	{
		uint32_t       MaterialID = 0;
		uint32_t       IsAnimated = false;
		uint32_t       AnimOffset = 0;
		uint32_t       EntityID = 0;
		glm::mat4      ModelView = glm::mat4(1.0f);
	};

	struct IBLProperties
	{
		glm::vec4      AmbientColor = glm::vec4(1.0f);
		float          IBLStrength = 1.0f;
		bool           Enabled = true;
	private:		   
		GLSL_BOOLPAD   Pad1;
	};

	struct BloomProperties
	{
		float          Threshold = 0.7f;
		float          Knee = 0.1f;
		float          UpsampleScale = 1.0f;
		float          Intensity = 1.0f;
		float          DirtIntensity = 1.0f;
		float          Exposure = 1.0f;
		float          SkyboxMod = 1.0f;
		bool           Enabled = false;
	private:
		GLSL_BOOLPAD   Pad1;
	};

	struct FXAAProperties
	{
		float          EdgeThresholdMin = 0.0312f;
		float          EdgeThresholdMax = 0.125f;
		float          Iterations = 29.0f;
		float          SubPixelQuality = 0.75f;
		// don't use! for internal needs
		glm::vec2      InverseScreenSize = glm::vec2(0.0f);
		bool           Enabled = true;
	private:	       
		GLSL_BOOLPAD   Pad1;
		float          Pad2;
	};			       

#pragma endregion

	struct DirtMask
	{
		Texture* Mask = nullptr;
		float    Intensity = 1.0f;
		float    BaseIntensity = 0.1f;
	};

	enum class ShadowMapSize : uint16_t
	{
		SIZE_2,
		SIZE_4,
		SIZE_8,
		SIZE_16
	};

	enum class DebugViewFlags : uint32_t
	{
		None = 0,
		Albedro,
		Position,
		Normals,
		Materials,
		Emission,
		ShadowMap,
		ShadowMapCood,
		AO
	};

	struct RendererStateEX
	{
		bool                   bDrawSkyBox = true;
		bool                   bDrawGrid = true;
		DebugViewFlags         eDebugView = DebugViewFlags::None;
		IBLProperties          IBL = {};
		BloomProperties        Bloom = {};
		FXAAProperties         FXAA = {};
	};

	struct CommandBuffer
	{
		uint32_t               InstancesCount = 0;
		uint32_t               Offset = 0;
		Ref<Mesh>              Mesh = nullptr;
	};

	struct InstancePackage
	{
		struct Package
		{
			uint32_t              MaterialID = 0;
			glm::vec3*            WorldPos = nullptr;
			glm::vec3*            Rotation = nullptr;
			glm::vec3*            Scale = nullptr;
			AnimationController*  AnimController = nullptr;
		};

		uint32_t                  CurrentIndex = 0;
		std::vector<Package>      Packages;
	};

	struct RendererDrawList
	{
		RendererDrawList();

		void                                            BeginSubmit(SceneViewProjection* sceneViewProj);
		void                                            EndSubmit();				    
		void                                            SubmitMesh(const glm::vec3& pos, const glm::vec3& rotation, const glm::vec3& scale, Ref<Mesh>& mesh, const uint32_t& material_id = 0, bool submit_childs = true, AnimationController* anim_controller = nullptr);
		void                                            SubmitDirLight(DirectionalLight* light);
		void                                            SubmitPointLight(PointLight* light);
		void                                            SubmitSpotLight(SpotLight* light);    
		void                                            CalculateFrustum(SceneViewProjection* viewProj);
		void                                            SetDefaultState();
		Frustum&                                        GetFrustum();
													    
	private:										    
		void                                            CalculateDepthMVP();
		void                                            BuildDrawList();
		void                                            ResetDrawList();

	private:
		SceneViewProjection*                            m_SceneInfo = nullptr;
													    
		uint32_t                                        m_Objects = 0;
		uint32_t                                        m_InstanceDataIndex = 0;
		uint32_t                                        m_PointLightIndex = 0;
		uint32_t                                        m_SpotLightIndex = 0;
		uint32_t                                        m_LastAnimationOffset = 0;

		Frustum                                         m_Frustum{};
		DirectionalLight                                m_DirLight{};
		glm::mat4                                       m_DepthMVP{};
		std::vector<Ref<Mesh>>                          m_UsedMeshes;
		std::vector<CommandBuffer>                      m_DrawList;
		std::array<InstanceData, max_objects>           m_InstancesData;
		std::array<PointLight, max_lights>              m_PointLights;
		std::array<SpotLight, max_lights>               m_SpotLights;
		std::vector<glm::mat4>                          m_AnimationJoints;
		std::unordered_map<Ref<Mesh>, InstancePackage>  m_Packages;
		std::unordered_map<Ref<Mesh>, uint32_t>         m_RootOffsets;

		friend struct RendererStorage;
		friend class RendererDeferred;
	};

	struct RendererStorage: RendererStorageBase
	{
		void                          SetDynamicSkybox(DynamicSkyProperties& properties, const glm::mat4& proj, bool regeneratePBRmaps);
		void                          SetStaticSkybox(Ref<Texture>& skybox);
		void                          SetRenderTarget(Ref<Framebuffer>& target);
		void                          SetDefaultState();
		RendererStateEX&              GetState();
		void                          OnResize(uint32_t width, uint32_t height) override;
		void                          OnUpdateMaterials();

	private:
		void                          Initilize() override;
		void                          CreatePipelines();
		void                          CreateFramebuffers();
		void                          CreatePBRMaps();
		void                          UpdateUniforms(RendererDrawList* drawList, Ref<Framebuffer>& target);

	private:
		// Bindings					
		const uint32_t                m_TexturesBinding = 24;
		const uint32_t                m_ShaderDataBinding = 25;
		const uint32_t                m_MaterialsBinding = 26;
		const uint32_t                m_SceneDataBinding = 27;
		const uint32_t                m_AnimBinding = 28;
		const uint32_t                m_PointLightBinding = 30;
		const uint32_t                m_SpotLightBinding = 31;
		const uint32_t                m_DirLightBinding = 32;
		const uint32_t                m_LightingStateBinding = 33;
		const uint32_t                m_BloomStateBinding = 34;
		const uint32_t                m_FXAAStateBinding = 35;
		const uint32_t                m_DynamicSkyBinding = 36;
		const uint32_t                m_BloomComputeWorkgroupSize = 4;
		// Pipelines				
		Ref<GraphicsPipeline>         p_Gbuffer = nullptr;
		Ref<GraphicsPipeline>         p_Lighting = nullptr;
		Ref<GraphicsPipeline>         p_Combination = nullptr;
		Ref<GraphicsPipeline>         p_Skybox = nullptr;
		Ref<GraphicsPipeline>         p_DepthPass = nullptr;
		Ref<GraphicsPipeline>         p_Grid = nullptr;
		Ref<GraphicsPipeline>         p_Debug = nullptr;
		Ref<GraphicsPipeline>         p_Mask = nullptr;
		Ref<GraphicsPipeline>         p_DOF = nullptr;
		Ref<ComputePipeline>          p_Bloom = nullptr;
		// Framebuffers				
		Ref<Framebuffer>              f_Main = nullptr;
		Ref<Framebuffer>              f_GBuffer = nullptr;
		Ref<Framebuffer>              f_Lighting = nullptr;
		Ref<Framebuffer>              f_Depth = nullptr;
		Ref<Framebuffer>              f_DOF = nullptr;
				            
		Ref<Mesh>                     m_GridMesh = {};
		RendererStateEX               m_State{};	
		std::vector<Ref<Texture>>     m_BloomTex{};
				
		VulkanPBR*                    m_VulkanPBR = nullptr;
		Ref<EnvironmentMap>           m_EnvironmentMap = nullptr;
		ShadowMapSize                 m_MapSize = ShadowMapSize::SIZE_8;
		glm::mat4                     m_GridModel{};
												    
		friend class RendererDeferred;
	};

	class RendererDeferred
	{
		struct SubmitInfo
		{
			ClearInfo*            pClearInfo = nullptr;
			RendererStorage*      pStorage = nullptr;
			RendererDrawList*     pDrawList = nullptr;
			CommandBufferStorage* pCmdStorage = nullptr;
		};
	public:
		static void DrawFrame(ClearInfo* clearInfo, RendererStorage* storage, RendererDrawList* drawList, bool batch_cmd = true);

	private:
		static void GBufferPass(SubmitInfo* info);
		static void LightingPass(SubmitInfo* info);
		static void DepthPass(SubmitInfo* info);
		static void BloomPass(SubmitInfo* info);
		static bool DebugViewPass(SubmitInfo* info);
		static void CompositionPass(SubmitInfo* info);
		static void UpdateUniforms(SubmitInfo* info);
	};
}