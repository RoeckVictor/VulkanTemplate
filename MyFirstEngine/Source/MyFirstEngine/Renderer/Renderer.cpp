#include "Mfepch.h"
#include "Renderer.h"

namespace MyFirstEngine
{
	void Renderer::BeginScene()
	{

	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const GameObject& object)
	{
		RenderCommand::DrawObject(object);
	}
}