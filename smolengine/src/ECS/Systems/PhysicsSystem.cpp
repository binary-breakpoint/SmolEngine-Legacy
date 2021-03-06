#include "stdafx.h"
#include "ECS/Systems/PhysicsSystem.h"
#include "ECS/Components/TransformComponent.h"
#include "ECS/Components/RigidbodyComponent.h"
#include "ECS/Components/Singletons/WorldAdminStateSComponent.h"
#include "ECS/Components/Singletons/Bullet3WorldSComponent.h"

#include <btBulletDynamicsCommon.h>

namespace SmolEngine
{
	void PhysicsSystem::OnBeginWorld()
	{
		entt::registry* reg = m_World->m_CurrentRegistry;

		const auto& dynamic_group = m_World->m_CurrentRegistry->view<TransformComponent, RigidbodyComponent>();
		for (const auto& entity : dynamic_group)
		{
			const auto& [transform, rigidbodyComponent] = dynamic_group.get<TransformComponent, RigidbodyComponent>(entity);

			rigidbodyComponent.Create(&rigidbodyComponent.CreateInfo, transform.WorldPos, transform.Rotation);

			AttachBodyToActiveScene(dynamic_cast<RigidActor*>(&rigidbodyComponent));
		}
	}

	void PhysicsSystem::OnEndWorld()
	{
		entt::registry* reg = m_World->m_CurrentRegistry;

		{
			btDiscreteDynamicsWorld* world = m_State->World;

			// remove the rigidbodies from the dynamics world and delete them
			for (int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
			{
				btCollisionObject* obj = world->getCollisionObjectArray()[i];
				btRigidBody* body = btRigidBody::upcast(obj);
				if (body && body->getMotionState())
				{
					delete body->getMotionState();
				}

				world->removeCollisionObject(obj);
				delete obj;
			}
		}

		// delete collision shapes and rigidbodies
		const auto& dynamic_group = reg->view<RigidbodyComponent>();
		for (const auto& entity : dynamic_group)
		{
			auto& body = dynamic_group.get<RigidbodyComponent>(entity);

			btCollisionShape* shape = body.m_Shape;
			delete shape;
			body.SetActive(false);
		}
	}

	void PhysicsSystem::OnUpdate(float delta)
	{
		m_State->World->stepSimulation(1.f / 60.0f, 2, delta);
	}

	void PhysicsSystem::OnDestroy(RigidbodyComponent* component)
	{
		btDiscreteDynamicsWorld* world = m_State->World;
		RigidActor* rbActor = dynamic_cast<RigidActor*>(component);
		btRigidBody* body = rbActor->m_Body;
		btCollisionObject* obj = dynamic_cast<btCollisionObject*>(body);
		if (body && obj)
		{
			if(body->getMotionState())
				delete body->getMotionState();

			world->removeCollisionObject(obj);
			delete obj;
		}
	}

	void PhysicsSystem::OnDestroy(Ref<Actor>& actor)
	{
		Scene* scene = WorldAdmin::GetSingleton()->GetActiveScene();
		RigidbodyComponent* rb = scene->GetComponent<RigidbodyComponent>(actor);
		if (rb)
		{
			OnDestroy(rb);
		}
	}

	void PhysicsSystem::UpdateTransforms()
	{
		entt::registry* reg = m_World->m_CurrentRegistry;
		const auto& dynamic_group = m_World->m_CurrentRegistry->view<TransformComponent, RigidbodyComponent>();

		JobsSystem::BeginSubmition();
		for (const auto& entity : dynamic_group)
		{
			auto transform = &dynamic_group.get<TransformComponent>(entity);
			const auto rigidbodyComponent = &dynamic_group.get<RigidbodyComponent>(entity);

			if (rigidbodyComponent->m_Body != nullptr)
			{
				JobsSystem::Schedule([transform, rigidbodyComponent]()
				{
					btRigidBody* body = rigidbodyComponent->m_Body;
					btTransform btTransf;
					body->getMotionState()->getWorldTransform(btTransf);

					glm::vec3 pos, rot;
					RigidActor::BulletToGLMTransform(&btTransf, pos, rot);
					rigidbodyComponent->CreateInfo.pActor->SetPosition(pos);
					rigidbodyComponent->CreateInfo.pActor->SetRotation(rot);
				});
			}
		}
		JobsSystem::EndSubmition();
	}

	void PhysicsSystem::AttachBodyToActiveScene(RigidActor* body)
	{
		btRigidBody* bd = body->m_Body;
		m_State->World->addRigidBody(bd);
	}
}
