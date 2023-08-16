#pragma once

#include "RenderCommand.h"

#include "Camera.h"
#include "MyFirstEngine/Application.h"
#include "MyFirstEngine/Renderer/VulkanRenderer/VulkanContext.h"

namespace MyFirstEngine 
{
	class Renderer
	{
	public:
		static void BeginScene(Camera& camera, const std::unordered_map<unsigned int, MyFirstEngine::GameObject>& gameObjects);
		static void EndScene();

		static void Submit(const GameObject& object);

		inline static RendererAPI::SelectedAPI GetSelectedAPI() { return RendererAPI::GetSelectedAPI(); }

		#define MAX_LIGHTS 100

		struct PointLight
		{
			glm::vec3 position{};
			alignas(16) glm::vec4 color{};
		};

		struct SceneUBO
		{
			glm::mat4 m_Projection{ 1.0f };
			glm::mat4 m_View{ 1.0f };
			glm::mat4 m_InverseView{ 1.0f };
			glm::vec4 m_AmbientColor{ 1.0f, 1.0f, 1.0f, 0.1f };
			PointLight m_PointLights[MAX_LIGHTS];
			int m_NumLights;
		};

	private:
		static SceneUBO* m_SceneUBO;
	};
}