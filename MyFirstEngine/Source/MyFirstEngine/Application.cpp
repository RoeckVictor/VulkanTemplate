#include "Mfepch.h"
#include "Application.h"

#include "Log.h"
#include "Input.h"

// TODELETE
#include "MyFirstEngine/Renderer/Vertex.h"
#include "MyFirstEngine/Renderer/Model.h"
#include "MyFirstEngine/Renderer/VulkanRenderer/GameObject.h"
#include "MyFirstEngine/Renderer/VulkanRenderer/VulkanContext.h"
#include "MyFirstEngine/Renderer/VulkanRenderer/VulkanVertex.h"
#include "MyFirstEngine/Renderer/VulkanRenderer/VulkanModel.h"
#include "MyFirstEngine/Renderer/VulkanRenderer/Material.h"
#include "MyFirstEngine/Renderer/VulkanRenderer/Camera.h"

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

		VulkanContext* graphicsContext = static_cast<VulkanContext*>(GetWindow().GetGraphicsContext());

		GameObject viewerObject = GameObject::CreateGameObject();

		std::unordered_map<unsigned int, GameObject> gameObjects;

		GameObject gameObjTest = GameObject::CreateGameObject();

		VertexLayout vertexLayout = {
			{"Position", VertexDataType::Float3, sizeof(glm::vec3)},
			{"Color", VertexDataType::Float3, sizeof(glm::vec3)},
			{"Normal", VertexDataType::Float3, sizeof(glm::vec3)},
			{"TexCoord", VertexDataType::Float2, sizeof(glm::vec2)}
		};

		// VertexArray vertices = VertexArray(vertexLayout);
		// glm::vec3 pos1 = { -0.5f, -0.5f, 0.0f };
		// glm::vec3 col1 = { 1.0f, 0.0f, 0.0f };
		// glm::vec3 norm1 = { -0.5f, -0.5f, 0.0f };
		// glm::vec2 uv1 = { 0.0f, 0.0f };
		// vertices.data.push_back({ &pos1, &col1, &norm1, &uv1 });
		// glm::vec3 pos2 = { 0.5f, -0.5f, 0.0f };
		// glm::vec3 col2 = { 0.0f, 1.0f, 0.0f };
		// glm::vec3 norm2 = { 0.5f, -0.5f, 0.0 };
		// glm::vec2 uv2 = { 1.0f, 0.0f };
		// vertices.data.push_back({ &pos2, &col2, &norm2, &uv2 });
		// glm::vec3 pos3 = { 0.5f, 0.5f, 0.0f };
		// glm::vec3 col3 = { 0.0f, 0.0f, 1.0f };
		// glm::vec3 norm3 = { 0.5f, 0.5f, 0.0f };
		// glm::vec2 uv3 = { 1.0f, 1.0f };
		// vertices.data.push_back({ &pos3, &col3, &norm3, &uv3 });
		// 
		// std::vector<uint32_t> indices = {0, 1, 2};
		// std::unique_ptr<Model> model = Model::CreateModelFromData(vertices, indices);
		std::unique_ptr<Model> model = Model::CreateModelFromFile("../Resources/Models/smooth_vase.obj", vertexLayout);
		gameObjTest.model = std::shared_ptr<VulkanModel>(dynamic_cast<VulkanModel*>(model.release()));

		gameObjTest.material = Material::CreateMatFromFile(graphicsContext->GetDevice(), "../Resources/Shaders/texture_test.vert.spv", "../Resources/Shaders/texture_test.frag.spv");
		glm::mat4 modelMatrix{ 1.0f };
		glm::mat4 normalMatrix{ 1.0f };
		gameObjTest.material->AddPushConstant("ModelMatrix", sizeof(glm::mat4), static_cast<void*>(&modelMatrix));
		gameObjTest.material->AddPushConstant("NormalMatrix", sizeof(glm::mat4), static_cast<void*>(&normalMatrix));
		gameObjTest.material->AddTexture("AlbedoMap", std::make_unique<Texture>("../Resources/Textures/uv_checker.png", graphicsContext->GetDevice()));
		VulkanVertexArray vertexArray = VulkanVertexArray(vertexLayout);
		gameObjTest.material->CreatePipeline(vertexArray.GetBindingDescriptions(), vertexArray.GetAttributeDescriptions());
		
		gameObjTest.transform.translation = { 0.0f, 0.36f, 1.2f };
		gameObjTest.transform.scale = glm::vec3(2.0f);

		gameObjects.emplace(gameObjTest.GetId(), std::move(gameObjTest));

		std::vector<glm::vec3> lightColors = {
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f)
		};

		for (int i = 0; i < lightColors.size(); i++)
		{
			auto pointLight = GameObject::MakePointLight();
			pointLight.color = lightColors[i];
			auto rotateLight = glm::rotate(
				glm::mat4(1.0f),
				(i * glm::two_pi<float>()) / lightColors.size(),
				{ 0.0f, -1.0f, 0.0f }
			);
			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f));
			gameObjects.emplace(pointLight.GetId(), std::move(pointLight));
		}

		struct UniformBufferObject
		{
			glm::mat4 projection{ 1.0f };
			glm::mat4 view{ 1.0f };
			glm::mat4 inverseView{ 1.0f };
			glm::vec4 ambientColor{ 1.0f, 1.0f, 1.0f, 0.5f };
			PointLight pointLights[MAX_LIGHTS];
			int numLights;
		};

		Camera camera{};
		camera.SetViewTarget(glm::vec3(-1.0f, -2.0f, -0.5f), glm::vec3(0.0f, 0.0f, 0.0f));
		auto currentTime = std::chrono::high_resolution_clock::now();
		// !TODELETE

		while (isRunning)
		{
			window->BeginUpdate();

			// TODELETE
			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			// cameraController.MoveInPlaneXZ(window.GetWindow(), frameTime, viewerObject);
			camera.SetViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspectRatio = graphicsContext->GetRenderer().GetAspectRatio();
			camera.SetPerspectiveProjection(glm::radians(50.0f), aspectRatio, 0.1f, 1000.0f);

			UniformBufferObject ubo{};
			ubo.projection = camera.GetProjectionMatrix();
			ubo.view = camera.GetViewMatrix();
			ubo.inverseView = camera.GetInverseViewMatrix();

			int lightIndex = 0;
			for (auto& kv : gameObjects)
			{
				auto& obj = kv.second;
				if (obj.pointLight == nullptr)
					continue;

				assert(lightIndex < MAX_LIGHTS && "Too many point lights");

				lightIndex++;
			}
			ubo.numLights = lightIndex;

			graphicsContext->GetUniformBuffer(graphicsContext->GetRenderer().GetFrameIndex()).writeToBuffer(&ubo);
			graphicsContext->GetUniformBuffer(graphicsContext->GetRenderer().GetFrameIndex()).flush();
			// !TODELETE

			// TODELETE
			for (auto& kv : gameObjects)
			{
				auto& obj = kv.second;
				if (obj.model == nullptr)
					continue;

				obj.material->UpdatePushConstant("ModelMatrix", static_cast<void*>(&obj.transform.transform()));
				obj.material->UpdatePushConstant("NormalMatrix", static_cast<void*>(&obj.transform.normalMatrix()));

				obj.material->Bind();
				obj.model->Bind();
				obj.model->Draw();
			}
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