#pragma once

#include "RendererAPI.h"

namespace MyFirstEngine
{
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void DrawObject(const GameObject& object) override;
	};
}