#pragma once

#include <glm/glm.hpp>
#include "GameObject.h"

namespace MyFirstEngine
{
	class RendererAPI
	{
	public:
		enum class SelectedAPI
		{
			None = 0,
			Vulkan = 1
		};

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void DrawObject(const GameObject& object) = 0;

		inline static SelectedAPI GetSelectedAPI() { return m_SelectedAPI; }
	private:
		static SelectedAPI m_SelectedAPI;
	};
}