#include "apps/SimpleModelApp.h"

#include <stdexcept>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace VulkanTutorial
{
	SimpleModelApp::SimpleModelApp()
		: App("Demo Scene"),
		viewerObject(GameObject::CreateGameObject()),
		texture("Resources/Textures/viking_room.png", device)
	{
		globalPool = DescriptorPool::Builder(device)
			.SetMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.Build();
	}

	void SimpleModelApp::LoadGameObjects()
	{
		std::shared_ptr<Model> viking = Model::CreateModelFromFile(device, "Resources/Models/viking_room.obj");
		GameObject vikingObject = GameObject::CreateGameObject();
		vikingObject.model = viking;
		vikingObject.transform.translation = { 0.0f, 1.5f, 4.0f };
		vikingObject.transform.rotation = glm::vec3(glm::radians(90.0f), glm::radians(90.0f), 0.0f);
		vikingObject.transform.scale = glm::vec3(3.0f);
		gameObjects.emplace(vikingObject.GetId(), std::move(vikingObject));

		std::vector<glm::vec3> lightColors = {
			glm::vec3(1.0f, 1.0f, 1.0f),
			glm::vec3(1.0f, 1.0f, 1.0f)
		};

		for (int i = 0; i < lightColors.size(); i++)
		{
			auto pointLight = GameObject::MakePointLight();
			pointLight.color = lightColors[i];
			if(i == 0)
				pointLight.transform.translation = { -1.2f, -0.6f, 4.3f };
			else if (i == 1)
				pointLight.transform.translation = { 1.5f, -0.6f, 4.3f };

			gameObjects.emplace(pointLight.GetId(), std::move(pointLight));
		}
	}

	void SimpleModelApp::Setup()
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

		globalSetLayout = DescriptorSetLayout::Builder(device)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1)
			.Build();

		globalDescriptorSets.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++)
		{
			VkDescriptorBufferInfo bufferInfo = uniformBuffers[i]->descriptorInfo();

			VkDescriptorImageInfo imageInfo = texture.GetImageInfo();

			DescriptorWriter(*globalSetLayout, *globalPool)
				.WriteBuffer(0, &bufferInfo)
				.WriteImage(1, &imageInfo)
				.Build(globalDescriptorSets[i]);
		}

		renderSystems.push_back(new DefaultRenderSystem(device, renderer.GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout()));
		renderSystems.push_back(new BillboardSystem(device, renderer.GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout()));

		camera.SetViewTarget(glm::vec3(-1.0f, -2.0f, -0.5f), glm::vec3(0.0f, 0.0f, 0.0f));

		currentTime = std::chrono::high_resolution_clock::now();
	}

	void SimpleModelApp::DrawFrame()
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
			DescriptorWriter descriptorWriter(*globalSetLayout, *globalPool);
			FrameInfo frameInfo
			{
				frameIndex,
				frameTime,
				commandBuffer,
				camera,
				globalDescriptorSets[frameIndex],
				descriptorWriter,
				gameObjects
			};

			UniformBufferObject ubo{};
			ubo.projection = camera.GetProjectionMatrix();
			ubo.view = camera.GetViewMatrix();
			ubo.inverseView = camera.GetInverseViewMatrix();
			ubo.ambientColor = glm::vec4(0.3f, 0.3f, 0.3f, 1.0f);

			int lightIndex = 0;
			for (auto& kv : frameInfo.gameObjects)
			{
				auto& obj = kv.second;
				if (obj.pointLight == nullptr)
					continue;

				assert(lightIndex < MAX_LIGHTS && "Too many point lights");

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
			renderer.EndSwapChainRenderPass(commandBuffer);
			renderer.EndFrame();
		}
	}

	void SimpleModelApp::Cleanup()
	{
		for (auto renderSys : renderSystems)
			delete renderSys;
		vkDeviceWaitIdle(device.device());
	}
}