#include "Mfepch.h"
#include "Renderer.h"
#include "Utils.h"

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

			sceneUBO->pointLights[numLights].position = obj.transform.translation;
			sceneUBO->pointLights[numLights].color = glm::vec4(obj.color, obj.pointLight->lightIntensity);
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
		if(object.pointLight == nullptr)
		{
			object.material->UpdateUniform(0, ConvertToBytes(object.transform.transform()), true);
			object.material->UpdateUniform(1, ConvertToBytes(glm::mat4(object.transform.normalMatrix())), true);
		}
		else
		{
			object.material->UpdateUniform(0, ConvertToBytes(glm::vec4(object.transform.translation, 1.0)), true);
			object.material->UpdateUniform(1, ConvertToBytes(glm::vec4(object.color, object.pointLight->lightIntensity)), true);
		}

		RenderCommand::DrawObject(object);
	}
}