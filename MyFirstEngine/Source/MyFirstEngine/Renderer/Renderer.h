#pragma once

#include "RenderCommand.h"

namespace MyFirstEngine 
{
	class Renderer
	{
	public:
		static void BeginScene();
		static void EndScene();

		static void Submit(const GameObject& object);

		inline static RendererAPI::SelectedAPI GetSelectedAPI() { return RendererAPI::GetSelectedAPI(); }
	};
}