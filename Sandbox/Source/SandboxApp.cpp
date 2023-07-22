#include <MyFirstEngine.h>

class ExampleLayer : public MyFirstEngine::Layer
{
public:
	ExampleLayer()
	: Layer("Example"),
		graphicsContext(static_cast<MyFirstEngine::VulkanContext*>(MyFirstEngine::Application::GetInstance().GetWindow().GetGraphicsContext())),
		camera(),
		viewerObject(MyFirstEngine::GameObject::CreateGameObject()),
		currentTime(std::chrono::high_resolution_clock::now()),
		gameObjects()
	{
		MyFirstEngine::GameObject gameObjTest = MyFirstEngine::GameObject::CreateGameObject();

		MyFirstEngine::VertexLayout vertexLayout = {
			{"Position", MyFirstEngine::VertexDataType::Float3, sizeof(glm::vec3)},
			{"Color", MyFirstEngine::VertexDataType::Float3, sizeof(glm::vec3)},
			{"Normal", MyFirstEngine::VertexDataType::Float3, sizeof(glm::vec3)},
			{"TexCoord", MyFirstEngine::VertexDataType::Float2, sizeof(glm::vec2)}
		};

		std::unique_ptr<MyFirstEngine::Model> model = MyFirstEngine::Model::CreateModelFromFile("../Resources/Models/smooth_vase.obj", vertexLayout);
		gameObjTest.model = std::shared_ptr<MyFirstEngine::VulkanModel>(dynamic_cast<MyFirstEngine::VulkanModel*>(model.release()));

		gameObjTest.material = MyFirstEngine::Material::CreateMatFromFile(graphicsContext->GetDevice(), "../Resources/Shaders/texture_test.vert.spv", "../Resources/Shaders/texture_test.frag.spv");
		glm::mat4 modelMatrix{ 1.0f };
		glm::mat4 normalMatrix{ 1.0f };
		gameObjTest.material->AddPushConstant("ModelMatrix", sizeof(glm::mat4), static_cast<void*>(&modelMatrix));
		gameObjTest.material->AddPushConstant("NormalMatrix", sizeof(glm::mat4), static_cast<void*>(&normalMatrix));
		gameObjTest.material->AddTexture("AlbedoMap", std::make_unique<MyFirstEngine::Texture>("../Resources/Textures/uv_checker.png", graphicsContext->GetDevice()));
		MyFirstEngine::VulkanVertexArray vertexArray = MyFirstEngine::VulkanVertexArray(vertexLayout);
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
			auto pointLight = MyFirstEngine::GameObject::MakePointLight();
			pointLight.color = lightColors[i];
			auto rotateLight = glm::rotate(
				glm::mat4(1.0f),
				(i * glm::two_pi<float>()) / lightColors.size(),
				{ 0.0f, -1.0f, 0.0f }
			);
			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f));
			gameObjects.emplace(pointLight.GetId(), std::move(pointLight));
		}

		camera.SetViewTarget(glm::vec3(-1.0f, -2.0f, -0.5f), glm::vec3(0.0f, 0.0f, 0.0f));

		MyFirstEngine::RenderCommand::SetClearColor({ 0.01f, 0.01f, 0.01f, 1.0f });
	}

	void OnUpdate() override
	{
		auto newTime = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
		currentTime = newTime;

		MoveViewerObject(deltaTime, viewerObject);
		camera.SetViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

		float aspectRatio = graphicsContext->GetRenderer().GetAspectRatio();
		camera.SetPerspectiveProjection(glm::radians(50.0f), aspectRatio, 0.1f, 1000.0f);

		MyFirstEngine::Renderer::BeginScene(camera);

		for (auto& kv : gameObjects)
		{
			auto& obj = kv.second;
			if (obj.model == nullptr)
				continue;

			MyFirstEngine::Renderer::Submit(obj);
		}

		MyFirstEngine::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{

	}

	void OnEvent(MyFirstEngine::Event& event) override
	{

	}

	void MoveViewerObject(float dt, MyFirstEngine::GameObject& gameObject)
	{
		float moveSpeed{ 3.0f };
		float lookSpeed{ 1.5f };

		glm::vec3 rotate{ 0 };

		if (MyFirstEngine::Input::IsKeyPressed(MFE_KEY_LEFT)) rotate.y -= 1.0f;
		if (MyFirstEngine::Input::IsKeyPressed(MFE_KEY_RIGHT)) rotate.y += 1.0f;
		if (MyFirstEngine::Input::IsKeyPressed(MFE_KEY_DOWN)) rotate.x -= 1.0f;
		if (MyFirstEngine::Input::IsKeyPressed(MFE_KEY_UP)) rotate.x += 1.0f;

		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
			gameObject.transform.rotation += lookSpeed * dt * glm::normalize(rotate);

		gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
		gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

		float yaw = gameObject.transform.rotation.y;
		const glm::vec3 forwardDir{ sin(yaw), 0.0f, cos(yaw) };
		const glm::vec3 rightDir{ forwardDir.z, 0.0f, -forwardDir.x };
		const glm::vec3 upDir{ 0.0f, -1.0f, 0.0f };

		glm::vec3 moveDir{ 0.0f };

		if (MyFirstEngine::Input::IsKeyPressed(MFE_KEY_A)) moveDir -= rightDir;
		if (MyFirstEngine::Input::IsKeyPressed(MFE_KEY_D)) moveDir += rightDir;
		if (MyFirstEngine::Input::IsKeyPressed(MFE_KEY_S)) moveDir -= upDir;
		if (MyFirstEngine::Input::IsKeyPressed(MFE_KEY_W)) moveDir += upDir;

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
			gameObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
	}

private:
	MyFirstEngine::VulkanContext* graphicsContext;
	MyFirstEngine::Camera camera;
	MyFirstEngine::GameObject viewerObject;
	std::unordered_map<unsigned int, MyFirstEngine::GameObject> gameObjects;
	std::chrono::time_point<std::chrono::high_resolution_clock> currentTime;
};

class Sandbox : public MyFirstEngine::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{
	}
};

MyFirstEngine::Application* MyFirstEngine::CreateApplication()
{
	return new Sandbox();
}