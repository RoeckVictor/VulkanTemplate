#pragma once

#include "RendererAPI.h"

namespace MyFirstEngine
{
	class VulkanRendererAPI : public RendererAPI
	{
	public:
		// virtual void Init() override;
		// virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void DrawObject(const GameObject& object) override;
	};
}