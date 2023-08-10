#include "Mfepch.h"
#include "Renderer.h"

namespace MyFirstEngine
{
	Renderer::SceneUBO* Renderer::sceneUBO = new Renderer::SceneUBO;

	void Renderer::BeginScene(Camera& camera, const std::unordered_map<unsigned int, MyFirstEngine::GameObject>& gameObjects)
	{
		Application::GetInstance().GetWindow().BeginUpdate();

		sceneUBO->projection = camera.GetProjectionMatrix();
		sceneUBO->view = camera.GetViewMatrix();
		sceneUBO->inverseView = camera.GetInverseViewMatrix();

		int numLights = 0;
		for (auto& kv : gameObjects)
		{
			auto& obj = kv.second;
			if (obj.pointLight == nullptr)
				continue;

			sceneUBO->pointLights[numLights].color = glm::vec4(obj.color, obj.pointLight->lightIntensity);
			sceneUBO->pointLights[numLights].position = obj.transform.translation;
			numLights++;
		}
		sceneUBO->numLights = numLights;

		VulkanContext* graphicsContext = static_cast<VulkanContext*>(Application::GetInstance().GetWindow().GetGraphicsContext());

		graphicsContext->GetUniformBuffer(graphicsContext->GetRenderer().GetFrameIndex()).writeToBuffer(sceneUBO);
		graphicsContext->GetUniformBuffer(graphicsContext->GetRenderer().GetFrameIndex()).flush();
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const GameObject& object)
	{
		object.material->UpdateUniform(0, static_cast<void*>(&object.transform.transform()), true);
		object.material->UpdateUniform(1, static_cast<void*>(&object.transform.normalMatrix()), true);

		RenderCommand::DrawObject(object);
	}
}