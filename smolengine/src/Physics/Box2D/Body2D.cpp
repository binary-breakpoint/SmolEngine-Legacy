#include "stdafx.h"
#include "Physics/Box2D/Body2D.h"
#include "ECS/Actor.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

namespace SmolEngine
{
	Body2D::Body2D(Body2DType type)
		:m_Type((int)type)
	{

	}

	Body2D::Body2D(int type)
		:m_Type(type)
	{

	}
}