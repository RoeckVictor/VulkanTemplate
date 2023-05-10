#pragma once

#include <string>
#include <stdexcept>
#include <vulkan/vulkan.h>

#include "Device.h"
#include "Buffer.h"

namespace VulkanTutorial
{
	class Texture
	{
	public:
		Texture(const std::string& filename, Device& device);
		~Texture();

		// [COMMENT] Transition the image to the right layout to use vkCmdCopyBufferToImage
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
		// [COMMENT] Create the sampler object to sample the image in the shader
		void CreateTextureSampler();
		// [COMMENT] Generate the mipmaps for the texture
		// [TODO] When making a game engine, mip maps should be generated offline
		void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

		VkDescriptorImageInfo GetImageInfo() { return imageInfo; }

	private:
		Device& device;

		// [COMMENT] Number of mipmaps levels possible for the texture
		uint32_t mipLevels;
		// [COMMENT] The actual image
		VkImage textureImage;
		// [COMMENT] The memory used by the image
		VkDeviceMemory textureImageMemory;
		// [COMMENT] The image are accessed through an image view
		VkImageView textureImageView;
		// [COMMENT] Sampler object used to sample the image in the shader
		VkSampler textureSampler;
		// [COMMENT] The image info used to bind the image and sampler to the descriptor
		VkDescriptorImageInfo imageInfo;
	};
}