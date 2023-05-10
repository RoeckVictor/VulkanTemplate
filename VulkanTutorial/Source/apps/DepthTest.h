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
	class DepthTest : public App
	{
	public:
		DepthTest();

	private:
		void LoadGameObjects();

		// [COMMENT] Everything that needs to be setup before the main loop starts
		void Setup() override;
		// [COMMENT] Called for each frame in the main loop
		void DrawFrame() override;
		// [COMMENT] Called after the main loop ends
		void Cleanup() override;

		// [COMMENT] The time of the frame currently being rendered
		std::chrono::steady_clock::time_point currentTime;
		// [COMMENT] Script moving the camera depending on keyboard input
		Keyboard cameraController{};
		// [COMMENT] Game object used to manage the camera in the scene
		GameObject viewerObject;
		// [COMMENT] The camera component of the viewer object
		Camera camera{};

		std::unique_ptr<DescriptorPool> globalPool{};
		// [COMMENT] The descriptor sets used to pass data to the shaders, we need one for each frame in flight
		std::vector<VkDescriptorSet> globalDescriptorSets;
		// [COMMENT] The uniform buffers that will be passed to the shaders, we need one for each frame in flight
		std::vector<std::unique_ptr<Buffer>> uniformBuffers;

		// [COMMENT] The render systems used to render every elements in our scene
		std::vector<RenderSystem*> renderSystems;

		// [TODELETE] global texture lol
		Texture texture;
	};
}