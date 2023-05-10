#pragma once

#include "Camera.h"
#include "GameObject.h"

#include <vulkan/vulkan.h>

namespace VulkanTutorial
{
	#define MAX_LIGHTS 100

	struct FrameInfo 
	{
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		Camera& camera;
		VkDescriptorSet descriptorSet;
		std::unordered_map<unsigned int, GameObject> &gameObjects;
	};

	struct PointLight
	{
		glm::vec4 position{};
		glm::vec4 color{}; // w is intensity
	};

	struct PointLightPushConstants
	{
		glm::vec4 position{};
		glm::vec4 color{}; // w is intensity
		float radius;
	};

	// [COMMENT] The data we want to pass to the shaders through a uniform buffer
	struct UniformBufferObject
	{
		glm::mat4 projection{ 1.0f };
		glm::mat4 view{ 1.0f };
		glm::mat4 inverseView{ 1.0f };
		glm::vec4 ambientColor{ 1.0f, 1.0f, 1.0f, 0.02f };
		PointLight pointLights[MAX_LIGHTS];
		int numLights;
	};
}