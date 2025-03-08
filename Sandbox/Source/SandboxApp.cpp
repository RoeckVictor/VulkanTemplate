#include <MyFirstEngine.h>
#include "Utils.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public MyFirstEngine::Layer
{
public:
	ExampleLayer()
	: Layer("Example"),
		m_GraphicsContext(static_cast<MyFirstEngine::VulkanContext*>(MyFirstEngine::Application::GetInstance().GetWindow().GetGraphicsContext())),
		m_Camera(),
		m_ViewerObject(MyFirstEngine::GameObject::CreateGameObject()),
		m_CurrentTime(std::chrono::high_resolution_clock::now()),
		m_GameObjects()
	{	
		MFE_PROFILE_FUNCTION();
		MyFirstEngine::GameObject gameObjTest = MyFirstEngine::GameObject::CreateGameObject();

		MyFirstEngine::VertexLayout vertexLayout = {
			{"Position", MyFirstEngine::VertexDataType::Float3, sizeof(glm::vec3)},
			{"Color", MyFirstEngine::VertexDataType::Float3, sizeof(glm::vec3)},
			{"Normal", MyFirstEngine::VertexDataType::Float3, sizeof(glm::vec3)},
			{"TexCoord", MyFirstEngine::VertexDataType::Float2, sizeof(glm::vec2)}
		};

		gameObjTest.m_Model = MyFirstEngine::Model::CreateModelFromFile("../Resources/Models/smooth_vase.obj", vertexLayout);

		const std::vector<std::string> shaderFiles = { "../Resources/Shaders/texture_test.vert.spv", "../Resources/Shaders/texture_test.frag.spv" };
		std::shared_ptr<MyFirstEngine::Shader> shader = MyFirstEngine::Shader::CreateShaderFromCompiledFiles(shaderFiles);
		gameObjTest.m_Material = MyFirstEngine::Material::CreateMatFromShader(shader);
		glm::mat4 modelMatrix{ 1.0f };
		glm::mat4 normalMatrix{ 1.0f };
		gameObjTest.m_Material->AddUniform(0, "ModelMatrix", MyFirstEngine::ConvertToBytes(modelMatrix), true);
		gameObjTest.m_Material->AddUniform(1, "NormalMatrix", MyFirstEngine::ConvertToBytes(normalMatrix), true);
		gameObjTest.m_Material->AddTexture(0, "AlbedoMap", MyFirstEngine::Texture::CreateFromFile("../Resources/Textures/uv_checker.png"));
		MyFirstEngine::VertexArray vertexArray = MyFirstEngine::VertexArray(vertexLayout);
		gameObjTest.m_Material->CreatePipeline(vertexArray);

		gameObjTest.m_Transform.translation = { 0.0f, 0.36f, 0.0f };
		gameObjTest.m_Transform.scale = glm::vec3(2.0f);

		m_GameObjects.emplace(gameObjTest.GetId(), std::move(gameObjTest));

		std::vector<glm::vec3> lightColors = {
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f)
		};

		for (int i = 0; i < lightColors.size(); i++)
		{
			auto pointLight = MyFirstEngine::GameObject::MakePointLight();
			pointLight.m_Color = lightColors[i];
			auto rotateLight = glm::rotate(
				glm::mat4(1.0f),
				(i * glm::two_pi<float>()) / lightColors.size(),
				{ 0.0f, -1.0f, 0.0f }
			);
			pointLight.m_Transform.translation = glm::vec3(rotateLight * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f));
			m_GameObjects.emplace(pointLight.GetId(), std::move(pointLight));
		}

		m_Camera.SetViewTarget(glm::vec3(-1.0f, -2.0f, -0.5f), glm::vec3(0.0f, 0.0f, 0.0f));

		MyFirstEngine::RenderCommand::SetClearColor({ 0.01f, 0.01f, 0.01f, 1.0f });
	}

	void OnUpdate(MyFirstEngine::Timestep timeStep) override
	{
		MFE_PROFILE_FUNCTION();
		MoveViewerObject(timeStep, m_ViewerObject);
		m_Camera.SetViewYXZ(m_ViewerObject.m_Transform.translation, m_ViewerObject.m_Transform.rotation);

		float aspectRatio = m_GraphicsContext->GetRenderer().GetAspectRatio();
		m_Camera.SetPerspectiveProjection(glm::radians(50.0f), aspectRatio, 0.1f, 1000.0f);

		// -- TODELETE --
		for (auto& kv : m_GameObjects)
		{
			MFE_PROFILE_SCOPE("Rotate Light")
			auto& obj = kv.second;
			if (obj.m_PointLight == nullptr) { continue; }

			auto rotateLight = glm::rotate(glm::mat4(1.0f), timeStep.GetSeconds(), { 0.0f, -11.0f, 0.0f });
			obj.m_Transform.translation = glm::vec3(rotateLight * glm::vec4(obj.m_Transform.translation, 1.0f));

			obj.m_Color = m_LightColor;
		}
		// -- !TODELETE --

		MyFirstEngine::Renderer::BeginScene(m_Camera, m_GameObjects);

		for (auto& kv : m_GameObjects)
		{
			MFE_PROFILE_SCOPE("Render Object")
			auto& obj = kv.second;
			if (obj.m_Model == nullptr) { continue; }

			MyFirstEngine::Renderer::Submit(obj);
		}

		MyFirstEngine::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		MFE_PROFILE_FUNCTION();
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Light color", glm::value_ptr(m_LightColor));
		ImGui::End();
	}

	void OnEvent(MyFirstEngine::Event& event) override
	{
		MFE_PROFILE_FUNCTION();
	}

	void MoveViewerObject(float dt, MyFirstEngine::GameObject& gameObject)
	{
		MFE_PROFILE_FUNCTION();
		float moveSpeed{ 3.0f };
		float lookSpeed{ 1.5f };

		glm::vec3 rotate{ 0 };

		if (MyFirstEngine::Input::IsKeyPressed(MFE_KEY_LEFT)) rotate.y -= 1.0f;
		if (MyFirstEngine::Input::IsKeyPressed(MFE_KEY_RIGHT)) rotate.y += 1.0f;
		if (MyFirstEngine::Input::IsKeyPressed(MFE_KEY_DOWN)) rotate.x -= 1.0f;
		if (MyFirstEngine::Input::IsKeyPressed(MFE_KEY_UP)) rotate.x += 1.0f;

		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
			gameObject.m_Transform.rotation += lookSpeed * dt * glm::normalize(rotate);

		gameObject.m_Transform.rotation.x = glm::clamp(gameObject.m_Transform.rotation.x, -1.5f, 1.5f);
		gameObject.m_Transform.rotation.y = glm::mod(gameObject.m_Transform.rotation.y, glm::two_pi<float>());

		float yaw = gameObject.m_Transform.rotation.y;
		const glm::vec3 forwardDir{ sin(yaw), 0.0f, cos(yaw) };
		const glm::vec3 rightDir{ forwardDir.z, 0.0f, -forwardDir.x };
		const glm::vec3 upDir{ 0.0f, -1.0f, 0.0f };

		glm::vec3 moveDir{ 0.0f };

		if (MyFirstEngine::Input::IsKeyPressed(MFE_KEY_A)) moveDir -= rightDir;
		if (MyFirstEngine::Input::IsKeyPressed(MFE_KEY_D)) moveDir += rightDir;
		if (MyFirstEngine::Input::IsKeyPressed(MFE_KEY_S)) moveDir -= forwardDir;
		if (MyFirstEngine::Input::IsKeyPressed(MFE_KEY_W)) moveDir += forwardDir;
		if (MyFirstEngine::Input::IsKeyPressed(MFE_KEY_Q)) moveDir -= upDir;
		if (MyFirstEngine::Input::IsKeyPressed(MFE_KEY_E)) moveDir += upDir;

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
			gameObject.m_Transform.translation += moveSpeed * dt * glm::normalize(moveDir);
	}

private:
	MyFirstEngine::VulkanContext* m_GraphicsContext;
	MyFirstEngine::Camera m_Camera;
	MyFirstEngine::GameObject m_ViewerObject;
	std::unordered_map<unsigned int, MyFirstEngine::GameObject> m_GameObjects;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_CurrentTime;

	glm::vec3 m_LightColor = { 1.0, 1.0, 1.0 };
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
	MFE_PROFILE_FUNCTION();
	return new Sandbox();
}