#include "Mfepch.h"
#include "Renderer.h"
#include "Utils.h"

namespace MyFirstEngine
{
	Renderer::SceneUBO* Renderer::m_SceneUBO = new Renderer::SceneUBO;

	void Renderer::BeginScene(Camera& camera, const std::unordered_map<unsigned int, MyFirstEngine::GameObject>& gameObjects)
	{
		Application::GetInstance().GetWindow().BeginUpdate();

		m_SceneUBO->m_Projection = camera.GetProjectionMatrix();
		m_SceneUBO->m_View = camera.GetViewMatrix();
		m_SceneUBO->m_InverseView = camera.GetInverseViewMatrix();

		int numLights = 0;
		for (auto& kv : gameObjects)
		{
			auto& obj = kv.second;
			if (obj.m_PointLight == nullptr){ continue; }	

			m_SceneUBO->m_PointLights[numLights].position = obj.m_Transform.translation;
			m_SceneUBO->m_PointLights[numLights].color = glm::vec4(obj.m_Color, obj.m_PointLight->lightIntensity);
			numLights++;
		}
		m_SceneUBO->m_NumLights = numLights;

		VulkanContext* graphicsContext = static_cast<VulkanContext*>(Application::GetInstance().GetWindow().GetGraphicsContext());

		graphicsContext->GetUniformBuffer(graphicsContext->GetRenderer().GetFrameIndex()).WriteToBuffer(m_SceneUBO);
		graphicsContext->GetUniformBuffer(graphicsContext->GetRenderer().GetFrameIndex()).Flush();
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const GameObject& object)
	{
		if(object.m_PointLight == nullptr)
		{
			object.m_Material->UpdateUniform(0, ConvertToBytes(object.m_Transform.transform()), true);
			object.m_Material->UpdateUniform(1, ConvertToBytes(glm::mat4(object.m_Transform.normalMatrix())), true);
		}
		else
		{
			object.m_Material->UpdateUniform(0, ConvertToBytes(glm::vec4(object.m_Transform.translation, 1.0)), true);
			object.m_Material->UpdateUniform(1, ConvertToBytes(glm::vec4(object.m_Color, object.m_PointLight->lightIntensity)), true);
		}

		RenderCommand::DrawObject(object);
	}
}