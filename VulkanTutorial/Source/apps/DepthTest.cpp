#include "apps/DepthTest.h"

#include <stdexcept>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace VulkanTutorial
{
	DepthTest::DepthTest()
		: App("Demo Scene"),
		viewerObject(GameObject::CreateGameObject()),
		texture("Resources/Textures/uv_checker.png", device)
	{
		globalPool = DescriptorPool::Builder(device)
			.SetMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT)
			.Build();
	}

	void DepthTest::LoadGameObjects()
	{
		std::shared_ptr<Model> quad2 = Model::CreateModelFromFile(device, "Resources/Models/quad.obj");
		GameObject quadObject2 = GameObject::CreateGameObject();
		quadObject2.model = quad2;
		quadObject2.transform.translation = { -0.8f, 0.4f, -0.8f };
		gameObjects.emplace(quadObject2.GetId(), std::move(quadObject2));

		std::shared_ptr<Model> quad1 = Model::CreateModelFromFile(device, "Resources/Models/quad.obj");
		GameObject quadObject1 = GameObject::CreateGameObject();
		quadObject1.model = quad1;
		quadObject1.transform.translation = { 0.0f, 0.2f, 0.0f };
		gameObjects.emplace(quadObject1.GetId(), std::move(quadObject1));

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

	void DepthTest::Setup()
	{
		// [COMMENT] Fill the gameObjects map with the objects in the scene
		LoadGameObjects();

		// [COMMENT] Makes sure we have a buffer for each frame in flight so we don't overrwrite the 
		// [COMMENT] current frame's buffer when preparing the next one
		uniformBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
		// [COMMENT] Create a buffer for every uniform buffer
		// [COMMENT] The buffers uses persistent mapping so it will be mapped until the program exits
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

		// [COMMENT] The descriptor set layout used to pass uniform values to the shader
		auto globalSetLayout = DescriptorSetLayout::Builder(device)
			// [COMMENT] The uniform buffer is bound to binding 0
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			// [COMMENT] The sampler is bound to binding 1
			.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1)
			.Build();

		// [COMMENT] We need to create a descriptor set for every frame in flight
		globalDescriptorSets.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++)
		{
			// [COMMENT] Get infos about the uniform buffer
			VkDescriptorBufferInfo bufferInfo = uniformBuffers[i]->descriptorInfo();

			// [COMMENT] Get infos about the texture
			VkDescriptorImageInfo imageInfo = texture.GetImageInfo();

			// [COMMENT] Create a descriptor writer
			DescriptorWriter(*globalSetLayout, *globalPool)
				// [COMMENT] Write the uniform buffer data to binding 0
				.WriteBuffer(0, &bufferInfo)
				// [COMMENT] Write the texture data to binding 1
				.WriteImage(1, &imageInfo)
				.Build(globalDescriptorSets[i]);
		}

		// [COMMENT] Create the render systems used to render different types of objects
		renderSystems.push_back(new DefaultRenderSystem(device, renderer.GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout()));
		// [COMMENT] Create the render system used to render billboards
		renderSystems.push_back(new BillboardSystem(device, renderer.GetSwapChainRenderPass(), globalSetLayout->GetDescriptorSetLayout()));

		camera.SetViewTarget(glm::vec3(-1.0f, -2.0f, -0.5f), glm::vec3(0.0f, 0.0f, 0.0f));

		// [COMMENT] Set the initial time of frame 0
		currentTime = std::chrono::high_resolution_clock::now();
	}

	void DepthTest::DrawFrame()
	{
		// [COMMENT] Get the new current time and the delta time since the last frame
		auto newTime = std::chrono::high_resolution_clock::now();
		float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
		currentTime = newTime;

		// [COMMENT] Set the camera position and rotation on the game object used to represent the camera
		cameraController.MoveInPlaneXZ(window.GetWindow(), frameTime, viewerObject);
		camera.SetViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

		// [COMMENT] Update the aspect ratio of the camera in case the window was resized
		float aspectRatio = renderer.GetAspectRatio();
		camera.SetPerspectiveProjection(glm::radians(50.0f), aspectRatio, 0.1f, 1000.0f);

		// [COMMENT] Render the frame
		if (VkCommandBuffer commandBuffer = renderer.BeginFrame())
		{
			// [COMMENT] Create the frame info structure for the current frame
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

			// [COMMENT] Update the uniform buffer
			UniformBufferObject ubo{};
			// [COMMENT] Set the projection and view matrices in the uniform buffer
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

			// [COMMENT] Update the render system
			uniformBuffers[frameIndex]->writeToBuffer(&ubo);
			uniformBuffers[frameIndex]->flush();

			// [COMMENT] Render the scene
			renderer.BeginSwapChainRenderPass(commandBuffer);
			for (auto renderSys : renderSystems)
				renderSys->Render(frameInfo);
			renderer.EndSwapChainRenderPass(commandBuffer);
			renderer.EndFrame();
		}
	}

	void DepthTest::Cleanup()
	{
		for (auto renderSys : renderSystems)
			delete renderSys;
		vkDeviceWaitIdle(device.device());
	}
}