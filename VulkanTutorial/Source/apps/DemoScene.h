#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <vector>
#include <chrono>

#include "apps/App.h"
#include "Window.h"
#include "Device.h"
#include "Renderer.h"
#include "GameObject.h"
#include "systems/DefaultRenderSystem.h"
#include "systems/BillboardSystem.h"
#include "Camera.h"
#include "Descriptors.h"
#include "Keyboard.h"
#include "Buffer.h"
#include "Texture.h"

namespace VulkanTutorial
{
	class DemoScene : public App
	{
	public:
		DemoScene();

	private:
		void LoadGameObjects();

		void Setup() override;
		void DrawFrame() override;
		void Cleanup() override;

		std::chrono::steady_clock::time_point currentTime;
		Keyboard cameraController{};
		GameObject viewerObject;
		Camera camera{};
		
		std::unique_ptr<DescriptorPool> globalPool{};
		std::unique_ptr<DescriptorSetLayout> globalSetLayout;
		std::vector<VkDescriptorSet> globalDescriptorSets;
		std::vector<std::unique_ptr<Buffer>> uniformBuffers;

		std::vector<RenderSystem*> renderSystems;

		Texture defaultTexture;
	};
}