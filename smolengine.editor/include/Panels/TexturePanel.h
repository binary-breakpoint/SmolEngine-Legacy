#pragma once
#include "Core/Core.h"
#include "Primitives/Texture.h"

namespace SmolEngine
{
	class TexturePanel
	{
	public:
		void               Update();
		void               Reset();
		void               Open(const std::string& filePath);

	private:
		Ref<Texture>       m_Preview = nullptr;
		bool               m_Reload = false;
		int                m_BorderColorFlag = 4;
		int                m_AddressModeFlag = 0;
		int                m_FormatFlag = 1;
		int                m_FilterFlag = 1;
		std::string        m_FilePath;
		std::string        m_Titile;
		TextureCreateInfo  m_Info = {};
	};
}