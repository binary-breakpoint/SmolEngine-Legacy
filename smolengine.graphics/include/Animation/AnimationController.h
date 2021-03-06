#pragma once

#include "Animation/AnimationClip.h"

#include <unordered_map>

namespace SmolEngine
{
	class AnimationController
	{
	public:

		bool                                                          AddClip(const AnimationClipCreateInfo& info, const std::string& name, bool set_as_active = false);
		bool                                                          RemoveClip(const std::string& name);
		bool                                                          SetActiveClip(const std::string& name);
		Ref<AnimationClip>                                            GetClip(const std::string& name);
		Ref<AnimationClip>                                            GetActiveClip();

		const std::unordered_map<std::string, Ref<AnimationClip>>&    GetClips() const;
						    
	protected:
		void                                                          Update();
		void                                                          CopyJoints(std::vector<glm::mat4>& dist, uint32_t& out_index);
							
	protected:				
		Ref<AnimationClip>                                            m_ActiveClip = nullptr;
		std::unordered_map<std::string, Ref<AnimationClip>>           m_Clips;

		friend struct RendererDrawList;
	};
}