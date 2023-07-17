#pragma once

#include "RendererAPI.h"

namespace MyFirstEngine
{
	class RenderCommand
	{
	public:
		inline static void SetClearColor(const glm::vec4& color) { rendererAPI->SetClearColor(color); }

		inline static void DrawObject(const GameObject& object) { rendererAPI->DrawObject(object); }

	private:
		static RendererAPI* rendererAPI;
	};
}