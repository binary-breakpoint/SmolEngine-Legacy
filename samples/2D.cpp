#include "2D.h"

#include <Common/Mesh.h>
#include <Common/Input.h>
#include <Common/Text.h>
#include <Utils/Utils.h>
#include <GraphicsContext.h>
#include <Renderer2D.h>

#include <imgui/imgui.h>

using namespace Frostium;

int main(int argc, char** argv)
{
	GraphicsContext* context = nullptr;
	{
		WindowCreateInfo windoInfo = {};
		{
			windoInfo.bFullscreen = false;
			windoInfo.bVSync = false;
			windoInfo.Height = 480;
			windoInfo.Width = 720;
			windoInfo.Title = "Frostium Example";
		}

		EditorCameraCreateInfo cameraCI = {};
		cameraCI.Type = CameraType::Ortho;
		cameraCI.Speed = 55.5;

		GraphicsContextInitInfo info = {};
		{
			info.Flags = Features_Renderer_2D_Flags | Features_ImGui_Flags;
			info.bMSAA = true;
			info.bTargetsSwapchain = true;
			info.ResourcesFolderPath = "../resources/";
			info.pWindowCI = &windoInfo;
			info.pEditorCameraCI = &cameraCI;
		}

		context = new GraphicsContext(&info);
	}

	Text text1 = {};
	Text::CreateSDF("Assets/sdf_fonts/font_1.fnt", "Assets/sdf_fonts/font_1.png", &text1);
	text1.SetText("Frostium3D!");

	Texture texture = {};
	Texture texture2 = {};
	Texture::Create("Assets/Background.png", &texture);
	Texture::Create("Assets/Bricks.png", &texture2);

	ClearInfo clearInfo = {};
	bool process = true;

	context->SetEventCallback([&](Event& e) 
	{
		if(e.IsType(EventType::WINDOW_CLOSE))
			process = false;
	});

	while (process)
	{
		context->ProcessEvents();
		DeltaTime deltaTime = context->CalculateDeltaTime();

		if (context->IsWindowMinimized())
			continue;

		/* 
		   @Calculate physics, process script, etc
		*/

		context->BeginFrame(deltaTime);
		{
			ImGui::Begin("Debug Window");
			{
				float lastFrameTime = deltaTime.GetTimeSeconds() * 100.0f;
				ImGui::InputFloat("ms", &lastFrameTime, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
			}
			ImGui::End();

			Renderer2D::BeginScene(&clearInfo);
			Renderer2D::SubmitSprite(glm::vec3(10, 0, 0),  glm::vec2(1, 1),  0.0f, 0, &texture2);
			Renderer2D::SubmitSprite(glm::vec3(0, 0, 0),   glm::vec2(1, 1),  0.0f, 1, &texture);
			Renderer2D::SubmitSprite(glm::vec3(20, 20, 0), glm::vec2(1, 1),  0.0f, 3, &texture2);
			Renderer2D::EndScene();
		}
		context->SwapBuffers();
	}
}