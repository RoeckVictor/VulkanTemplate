#pragma once

#include "RendererAPI.h"

namespace MyFirstEngine
{
	class RenderCommand
	{
	public:
		inline static void SetClearColor(const glm::vec4& color) { m_RendererAPI->SetClearColor(color); }

		inline static void DrawObject(const GameObject& object) { m_RendererAPI->DrawObject(object); }

	private:
		static RendererAPI* m_RendererAPI;
	};
}