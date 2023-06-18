#include <MyFirstEngine.h>

class ExampleLayer : public MyFirstEngine::Layer
{
public:
	ExampleLayer()
	: Layer("Example")
	{
	}

	void OnUpdate() override
	{
	}

	void OnEvent(MyFirstEngine::Event& event) override
	{
		MFE_TRACE("{0}", event);
	}
};

class Sandbox : public MyFirstEngine::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new MyFirstEngine::ImGuiLayer());
	}

	~Sandbox()
	{
	}
};

MyFirstEngine::Application* MyFirstEngine::CreateApplication()
{
	return new Sandbox();
}