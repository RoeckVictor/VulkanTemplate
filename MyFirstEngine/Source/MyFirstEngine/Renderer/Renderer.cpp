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
		object.material->UpdatePushConstant("ModelMatrix", static_cast<void*>(&object.transform.transform()));
		object.material->UpdatePushConstant("NormalMatrix", static_cast<void*>(&object.transform.normalMatrix()));

		object.material->Bind();
		object.model->Bind();
		RenderCommand::DrawObject(object);
	}
}