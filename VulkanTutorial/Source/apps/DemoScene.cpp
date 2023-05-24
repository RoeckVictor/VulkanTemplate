#include "apps/DemoScene.h"

#include <stdexcept>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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

		std::shared_ptr<Model> modelFlat = Model::CreateModelFromFile(device, "Resources/Models/flat_vase.obj", "Resources/Textures/viking_room.png"); 
		GameObject gameObjFlat = GameObject::CreateGameObject();
		gameObjFlat.model = modelFlat;
		gameObjFlat.transform.translation = { -0.5f, 0.5f, 0.0f };
		gameObjFlat.transform.scale = glm::vec3(3.0f);
		gameObjects.emplace(gameObjFlat.GetId(), std::move(gameObjFlat));

		std::shared_ptr<Model> modelFloor = Model::CreateModelFromFile(device, "Resources/Models/quad.obj", "Resources/Textures/PointLight.png");
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

		uint32_t maxObjectsPerFrame = 3;
		uint32_t maxSets = SwapChain::MAX_FRAMES_IN_FLIGHT * maxObjectsPerFrame;

		globalPool = DescriptorPool::Builder(device)
			.SetMaxSets(maxSets)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, maxSets)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, maxSets)
			.Build();

		globalSetLayout = DescriptorSetLayout::Builder(device)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1)
			.Build();

		globalDescriptorSets.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++)
		{
			VkDescriptorBufferInfo bufferInfo = uniformBuffers[i]->descriptorInfo();
			VkDescriptorImageInfo imageInfo = defaultTexture.GetImageInfo();

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

			DescriptorWriter descriptorWriter(*globalSetLayout, *globalPool);
			FrameInfo frameInfo
			{
				frameIndex,
				frameTime,
				commandBuffer,
				camera,
				globalDescriptorSets[frameIndex],
				descriptorWriter,
				gameObjects,
				*globalSetLayout,
				*globalPool,
				*uniformBuffers[frameIndex]
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
			renderer.EndSwapChainRenderPass(commandBuffer);
			renderer.EndFrame();
		}
	}

	void DemoScene::Cleanup()
	{
		for (auto renderSys : renderSystems)
			delete renderSys;
		vkDeviceWaitIdle(device.device());
	}
}