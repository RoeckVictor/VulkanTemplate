#include "Mfepch.h"
#include "Renderer.h"

namespace MyFirstEngine
{
	Renderer::SceneUBO* Renderer::sceneUBO = new Renderer::SceneUBO;

	void Renderer::BeginScene(Camera& camera)
	{
		Application::GetInstance().GetWindow().BeginUpdate();

		sceneUBO->projection = camera.GetProjectionMatrix();
		sceneUBO->view = camera.GetViewMatrix();
		sceneUBO->inverseView = camera.GetInverseViewMatrix();
		sceneUBO->numLights = 0;

		VulkanContext* graphicsContext = static_cast<VulkanContext*>(Application::GetInstance().GetWindow().GetGraphicsContext());

		graphicsContext->GetUniformBuffer(graphicsContext->GetRenderer().GetFrameIndex()).writeToBuffer(sceneUBO);
		graphicsContext->GetUniformBuffer(graphicsContext->GetRenderer().GetFrameIndex()).flush();
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const GameObject& object)
	{
		object.material->AddUniform("ModelMatrix", sizeof(glm::mat4), static_cast<void*>(&object.transform.transform()), true);
		object.material->AddUniform("NormalMatrix", sizeof(glm::mat4), static_cast<void*>(&object.transform.normalMatrix()), true);

		RenderCommand::DrawObject(object);
	}
}