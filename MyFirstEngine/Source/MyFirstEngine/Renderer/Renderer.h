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
			glm::vec4 color{};
		};

		struct SceneUBO
		{
			glm::mat4 projection{ 1.0f };
			glm::mat4 view{ 1.0f };
			glm::mat4 inverseView{ 1.0f };
			glm::vec4 ambientColor{ 1.0f, 1.0f, 1.0f, 0.1f };
			PointLight pointLights[MAX_LIGHTS];
			int numLights;
		};

	private:
		static SceneUBO* sceneUBO;
	};
}