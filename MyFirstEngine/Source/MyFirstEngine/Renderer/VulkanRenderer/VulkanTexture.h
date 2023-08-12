#pragma once

#include <string>
#include <stdexcept>
#include <vulkan/vulkan.h>

#include "Texture.h"

#include "Device.h"
#include "Buffer.h"

namespace MyFirstEngine
{
	class VulkanTexture : public Texture
	{
	public:
		VulkanTexture(const std::string& filepath);
		~VulkanTexture();

		static std::shared_ptr<Texture> CreateTextureFromFile(const std::string& filepath);

		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
		void CreateTextureSampler();
		// [TODO] When making a game engine, mip maps should be generated offline
		void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

		VkDescriptorImageInfo GetImageInfo() { return imageInfo; }

	private:
		Device& device;

		uint32_t mipLevels;
		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageView textureImageView;
		VkSampler textureSampler;
		VkDescriptorImageInfo imageInfo;
	};
}