#pragma once
#include "Core/Core.h"
#include "TexturesLoader.h"

#include <string>
#include <vector>
#include <future>
#include <functional>
#include <unordered_map>
#include <glm/glm/glm.hpp>
#include <imgui/imgui.h>

namespace SmolEngine
{
	class EditorLayer;
	class Texture;
	class Framebuffer;

	class FileExplorer
	{
	public:
		FileExplorer();

		void Create(const std::string& current_path);
		void ClearSelection();
		void Update();
		void Import();

		void SetOnFileSelectedCallback(const std::function<void(const std::string&, const std::string&, int)>& callback);
		void SetOnFileDeletedCallaback(const std::function<void(const std::string&, const std::string&)>& callback);

	private:
		enum class PendeingActionFlags
		{
			None    = 0,
			Rename  = 1,
			NewFile = 2,
		};

		enum class PopUpFlags
		{
			None    = 0,
			Node    = 1,
			Folder  = 2,
		};

		struct PendeingAction
		{
			PendeingActionFlags Type;
			std::string         Path;
			std::string         NewName;
		};

		void    DrawPopUp();
		void    ClosePopUp();
		void    Reset();
		void    DrawHierarchy();
		void    DrawSelectable(const std::string& name, const std::string& path, const std::string& ext, bool is_folder);
		bool    DrawDirectory(const std::filesystem::path& path);
		void    DrawNode(const std::filesystem::path& path);
		void    DrawIcon(const std::string& path, const std::string& ext = "");
		void    DrawToolBar();
		void    AddPendingAction(const std::string& path, PendeingActionFlags action);
		bool    IsAnyActionPending(const std::filesystem::path& node_path);
		size_t  GetNodeSize(const std::string& path);
		void    GetIcon(const std::string& path, const std::string& ext, void*& descriptorPtr);
			    
	private:

		std::function<void(const std::string&,
			const std::string&, int)>          m_pOnFileSelected = nullptr;

		std::function<void(const std::string&, 
			const std::string&)>               m_pOnFileDeleted = nullptr;

		std::unordered_map<std::string,
			Ref<Texture>>                      m_IconsMap = {};

		TexturesLoader*                        m_pTextureLoader = nullptr;
		PendeingAction*                        m_pPendeingAction = nullptr;
		PopUpFlags                             m_ePopUpFlags = PopUpFlags::None;
		int                                    m_SelectionIndex = 0;
		glm::vec2                              m_ButtonSize;
		ImVec4                                 m_SelectColor;
		std::string                            m_PopUpBuffer;
		std::string                            m_CurrentDir;
		std::string                            m_HomeDir;
		std::string                            m_SelectedNode;
		std::string                            m_SearchBuffer;
		std::string                            m_DragAndDropBuffer; 
		std::vector<const char*>               m_FileExtensions = { ".s_image", ".s_scene", ".s_material", ".s_prefab", ".s_audio", ".s_animation", ".gltf" };
	};													                  
}