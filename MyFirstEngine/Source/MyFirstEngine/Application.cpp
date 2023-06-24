#include "Mfepch.h"
#include "Application.h"

#include "Log.h"
#include "Input.h"

// TODELETE
#include "MyFirstEngine/Renderer/GameObject.h"
#include "MyFirstEngine/Renderer/VulkanContext.h"
#include "MyFirstEngine/Renderer/Model.h"
#include "MyFirstEngine/Renderer/Material.h"
#include "MyFirstEngine/Renderer/Camera.h"

namespace MyFirstEngine
{
	Application* Application::instance = nullptr;

	Application::Application()
		: window((VulkanGlfwWindow*)VulkanGlfwWindow::Create()),
		  imguiLayer(new ImGuiLayer())
	{
		MFE_CORE_ASSERT(!instance, "Application already exists!");
		instance = this;
		window->SetEventCallback(MFE_BIND_EVENT_FN(Application::OnEvent));

		PushOverlay(imguiLayer);
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		// TODELETE

		struct UniformBufferObject
		{
			glm::mat4 projection{ 1.0f };
			glm::mat4 view{ 1.0f };
			glm::mat4 inverseView{ 1.0f };
			glm::vec4 ambientColor{ 1.0f, 1.0f, 1.0f, 0.02f };
			PointLight pointLights[MAX_LIGHTS];
			int numLights;
		};

		VulkanContext* graphicsContext = static_cast<VulkanContext*>(GetWindow().GetGraphicsContext());

		GameObject gameObjTest = GameObject::CreateGameObject();
		gameObjTest.model = Model::CreateModelFromFile(graphicsContext->GetDevice(), "../Resources/Models/smooth_vase.obj");
		gameObjTest.material = Material::CreateMatFromFile(graphicsContext->GetDevice(), "../Resources/Shaders/diffuse.vert.spv", "../Resources/Shaders/diffuse.frag.spv");
		glm::mat4 modelMatrix{ 1.0f };
		gameObjTest.material->AddPushConstant(sizeof(glm::mat4), static_cast<void*>(&modelMatrix));
		glm::mat4 normalMatrix{ 1.0f };
		gameObjTest.material->AddPushConstant(sizeof(glm::mat4), static_cast<void*>(&normalMatrix));
		gameObjTest.material->CreatePipeline();
		
		gameObjTest.transform.translation = { 0.0f, 0.5f, 1.0f };
		gameObjTest.transform.scale = glm::vec3(3.0f);

		Camera camera{};
		camera.SetViewTarget(glm::vec3(-1.0f, -2.0f, -0.5f), glm::vec3(0.0f, 0.0f, 0.0f));
		// !TODELETE

		while (isRunning)
		{
			window->BeginUpdate();

			// TODELETE
			float aspectRatio = graphicsContext->GetRenderer().GetAspectRatio();
			camera.SetPerspectiveProjection(glm::radians(50.0f), aspectRatio, 0.1f, 1000.0f);

			UniformBufferObject ubo{};
			ubo.projection = camera.GetProjectionMatrix();
			ubo.view = camera.GetViewMatrix();
			ubo.inverseView = camera.GetInverseViewMatrix();

			graphicsContext->GetUniformBuffer(graphicsContext->GetRenderer().GetFrameIndex()).writeToBuffer(&ubo);
			graphicsContext->GetUniformBuffer(graphicsContext->GetRenderer().GetFrameIndex()).flush();
			// !TODELETE

			// TODELETE
			gameObjTest.material->Bind();
			gameObjTest.model->Bind();
			gameObjTest.model->Draw();
			// !TODELETE

			for (Layer* layer : layerStack)
				layer->OnUpdate();

			imguiLayer->Begin();
			for (Layer* layer : layerStack)
				layer->OnImGuiRender();
			imguiLayer->End();

			window->EndUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(MFE_BIND_EVENT_FN(Application::OnWindowClose));
		
		for (auto it = layerStack.end(); it != layerStack.begin();)
		{
			(*--it)->OnEvent(e);
			if (e.isHandeld)
				break;
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		layerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		layerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		isRunning = false;
		return true;
	}
}