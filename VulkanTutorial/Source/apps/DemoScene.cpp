#include "apps/DemoScene.h"

#include <stdexcept>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imconfig.h>
#include <imgui/imgui_tables.cpp>
#include <imgui/imgui_internal.h>
#include <imgui/imgui.cpp>
#include <imgui/imgui_draw.cpp>
#include <imgui/imgui_widgets.cpp>
#include <imgui/imgui_demo.cpp>
#include <imgui/imgui_impl_glfw.cpp>
#include <imgui/imgui_impl_vulkan_but_better.h>

namespace VulkanTutorial
{
	DemoScene::DemoScene()
	: App("Demo Scene"),
	  viewerObject(GameObject::CreateGameObject()),
	  defaultTexture("Resources/Textures/uv_checker.png", device)
	{
	}

	void DemoScene::LoadGameObjects()
	{
		std::shared_ptr<Model> modelSmooth = Model::CreateModelFromFile(device, "Resources/Models/smooth_vase.obj", "Resources/Textures/uv_checker.png");
		GameObject gameObjSmooth = GameObject::CreateGameObject();
		gameObjSmooth.model = modelSmooth;
		gameObjSmooth.transform.translation = {0.5f, 0.5f, 0.0f};
		gameObjSmooth.transform.scale = glm::vec3(3.0f);
		gameObjects.emplace(gameObjSmooth.GetId(), std::move(gameObjSmooth));

		std::shared_ptr<Model> modelFlat = Model::CreateModelFromFile(device, "Resources/Models/flat_vase.obj", "Resources/Textures/Stylized_Bricks_001_basecolor.jpg"); 
		GameObject gameObjFlat = GameObject::CreateGameObject();
		gameObjFlat.model = modelFlat;
		gameObjFlat.transform.translation = { -0.5f, 0.5f, 0.0f };
		gameObjFlat.transform.scale = glm::vec3(3.0f);
		gameObjects.emplace(gameObjFlat.GetId(), std::move(gameObjFlat));

		std::shared_ptr<Model> modelFloor = Model::CreateModelFromFile(device, "Resources/Models/quad.obj", "Resources/Textures/Stylized_Wood_Planks_001_basecolor.jpg");
		GameObject gameObjFloor = GameObject::CreateGameObject();
		gameObjFloor.model = modelFloor;
		gameObjFloor.transform.translation = { 0.0f, 0.5f, 0.0f };
		gameObjFloor.transform.scale = glm::vec3(3.0f);
		gameObjects.emplace(gameObjFloor.GetId(), std::move(gameObjFloor));

		std::vector<glm::vec3> lightColors = {
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::vec3(0.0f, 0.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f)
		};

		for (int i = 0; i < lightColors.size(); i++) 
		{
			auto pointLight = GameObject::MakePointLight();
			pointLight.color = lightColors[i];
			auto rotateLight = glm::rotate(
				glm::mat4(1.0f), 
				(i * glm::two_pi<float>()) / lightColors.size(),
				{0.0f, -1.0f, 0.0f}
			);
			pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f));
			gameObjects.emplace(pointLight.GetId(), std::move(pointLight));
		}
	}

	void DemoScene::Setup()
	{
		LoadGameObjects();

		uniformBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < uniformBuffers.size(); i++)
		{
			uniformBuffers[i] = std::make_unique<Buffer>
				(
					device,
					sizeof(UniformBufferObject),
					1,
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				);
			uniformBuffers[i]->map();
		}

		uint32_t maxObjectsPerFrame = 4;
		uint32_t maxSets = SwapChain::MAX_FRAMES_IN_FLIGHT * maxObjectsPerFrame;

		// Create the descriptor pool
		globalPool = DescriptorPool::Builder(device)
			.SetMaxSets(maxSets+SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxSets)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxSets+SwapChain::MAX_FRAMES_IN_FLIGHT)
			.SetPoolFlags(VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT)
			.Build();
		
		// Create the descriptor set layouts
		globalSetLayouts.push_back(DescriptorSetLayout::Builder(device)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.Build());

		globalSetLayouts.push_back(DescriptorSetLayout::Builder(device)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1)
			.Build());

		// Create the descriptor sets
		globalDescriptorSets.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++)
		{
			VkDescriptorBufferInfo bufferInfo = uniformBuffers[i]->descriptorInfo();

			DescriptorWriter(*globalSetLayouts[0], *globalPool)
				.WriteBuffer(0, &bufferInfo)
				.Build(globalDescriptorSets[i]);
		}

		for (auto& keyValue : gameObjects)
		{
			GameObject& obj = keyValue.second;

			if (obj.model == nullptr) continue;

			obj.model->CreateTextureSet(*globalSetLayouts[1], *globalPool);
		}

		std::vector<VkDescriptorSetLayout> setLayouts;
		for (int i = 0; i < globalSetLayouts.size(); i++)
			setLayouts.push_back(globalSetLayouts[i]->GetDescriptorSetLayout());

		renderSystems.push_back(new DefaultRenderSystem(device, renderer.GetSwapChainRenderPass(), setLayouts));
		BillboardSystem billboardSystem(device, renderer.GetSwapChainRenderPass(), setLayouts);
		renderSystems.push_back(new BillboardSystem(device, renderer.GetSwapChainRenderPass(), setLayouts));
		static_cast<BillboardSystem*>(renderSystems[1])->CreateTextureSet(*globalSetLayouts[1], *globalPool);

		camera.SetViewTarget(glm::vec3(-1.0f, -2.0f, -0.5f), glm::vec3(0.0f, 0.0f, 0.0f));

		currentTime = std::chrono::high_resolution_clock::now();

		InitImgui();
	}

	void DemoScene::DrawFrame()
	{
		auto newTime = std::chrono::high_resolution_clock::now();
		float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
		currentTime = newTime;

		cameraController.MoveInPlaneXZ(window.GetWindow(), frameTime, viewerObject);
		camera.SetViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

		float aspectRatio = renderer.GetAspectRatio();
		camera.SetPerspectiveProjection(glm::radians(50.0f), aspectRatio, 0.1f, 1000.0f);

		if (VkCommandBuffer commandBuffer = renderer.BeginFrame())
		{
			int frameIndex = renderer.GetFrameIndex();

			FrameInfo frameInfo
			{
				frameIndex,
				frameTime,
				commandBuffer,
				camera,
				globalDescriptorSets[frameIndex],
				gameObjects
			};

			UniformBufferObject ubo{};
			ubo.projection = camera.GetProjectionMatrix();
			ubo.view = camera.GetViewMatrix();
			ubo.inverseView = camera.GetInverseViewMatrix();

			auto rotateLight = glm::rotate(glm::mat4(1.0f), frameInfo.frameTime, { 0.0f, -11.0f, 0.0f });

			int lightIndex = 0;
			for (auto& kv : frameInfo.gameObjects)
			{
				auto& obj = kv.second;
				if (obj.pointLight == nullptr)
					continue;

				assert(lightIndex < MAX_LIGHTS && "Too many point lights");

				obj.transform.translation = glm::vec3(rotateLight * glm::vec4(obj.transform.translation, 1.0f));

				ubo.pointLights[lightIndex].position = glm::vec4(obj.transform.translation, 1.0f);
				ubo.pointLights[lightIndex].color = glm::vec4(obj.color, obj.pointLight->lightIntensity);

				lightIndex++;
			}
			ubo.numLights = lightIndex;

			uniformBuffers[frameIndex]->writeToBuffer(&ubo);
			uniformBuffers[frameIndex]->flush();

			renderer.BeginSwapChainRenderPass(commandBuffer);
			for (auto renderSys : renderSystems)
				renderSys->Render(frameInfo);
			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGui::ShowDemoWindow();

			ImGui::Render();
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer, 0, nullptr);
			renderer.EndSwapChainRenderPass(commandBuffer);
			renderer.EndFrame();
		}
	}

	void DemoScene::Cleanup()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		for (auto renderSys : renderSystems)
			delete renderSys;
		vkDeviceWaitIdle(device.device());
	}

	void DemoScene::InitImgui()
	{
		ImGui::CreateContext();
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		ImGui_ImplGlfw_InitForVulkan(window.GetWindow(), true);

		ImGui_ImplVulkan_InitInfo info;
		info.DescriptorPool = globalPool->GetDescriptorPool();
		info.RenderPass = renderer.GetSwapChainRenderPass();
		info.Device = device.device();
		info.PhysicalDevice = device.physicalDevice();
		info.ImageCount = SwapChain::MAX_FRAMES_IN_FLIGHT;
		info.MsaaSamples = VK_SAMPLE_COUNT_8_BIT;
		ImGui_ImplVulkan_Init(&info);

		VkCommandBuffer commandBuffer = device.BeginSingleTimeCommands();
		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		device.EndSingleTimeCommands(commandBuffer);

		vkDeviceWaitIdle(device.device());
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}
}