#include "Mfepch.h"
#include "MyFirstEngine/Application.h"
#include "VulkanRendererAPI.h"

#include "VulkanContext.h"
#include "VulkanRenderer.h"

namespace MyFirstEngine
{
	void VulkanRendererAPI::SetClearColor(const glm::vec4& color)
	{
		VulkanContext* graphicsContext = static_cast<VulkanContext*>(Application::GetInstance().GetWindow().GetGraphicsContext());
		graphicsContext->GetRenderer().SetClearColor(color.r, color.g, color.b, color.a);
	}

	void VulkanRendererAPI::DrawObject(const GameObject& object)
	{
		object.material->Bind();
		object.model->Bind();
		object.model->Draw();
	}
}