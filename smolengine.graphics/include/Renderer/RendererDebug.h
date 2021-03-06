#pragma once
#include "Memory.h"
#include "Common/BoundingBox.h"
#include "Tools/GLM.h"

namespace SmolEngine
{
	class Mesh;

	class RendererDebug
	{
	public:
		static void BeginDebug();
		static void EndDebug();

		static void DrawSphere(float radius, const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale);
		static void DrawCapsule(float radius, float halfHeight, uint32_t upAxis, const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale);
		static void DrawBox(const glm::vec3& min, const glm::vec3& max, const glm::vec3& pos, const glm::vec3& rot, const glm::vec3& scale);
		static void DrawAABB(const BoundingBox& aabb, const glm::vec3& pos, const glm::vec3& scale);
		static void DrawLine(const glm::vec3& pos1, const glm::vec3& pos2, float width = 1.0f);
		static void DrawQuad(const glm::vec3& pos, const glm::vec3& rotation, const glm::vec3& scale);
		static void DrawWireframes(const glm::vec3& pos, const glm::vec3& rotation, const glm::vec3& scale, Ref<Mesh>& mesh);
		static void DrawCirlce(const glm::vec3& pos, const glm::vec3& scale);
		static void SetColor(const glm::vec4& color);

	private:
		static void Init();
		static void DrawSphereLines(const glm::vec3& center, const glm::vec3& up, const glm::vec3& axis, float radius, float minTh,float maxTh, float minPs, float maxPs, float stepDegrees = 10.f, bool drawCenter = true);

		friend class GraphicsContext;
	};
}