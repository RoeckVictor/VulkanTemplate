#include "Texture.h"

#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

VulkanTutorial::Texture::Texture(const std::string& filename, Device& device)
	: device{ device }
{
	// [COMMENT] Load the image using stb_image
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load(filename.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	
	// [COMMENT] Calculate the image size in bytes
	uint32_t pixelSize = STBI_rgb_alpha;
	uint32_t pixelCount = texWidth * texHeight;
	VkDeviceSize imageSize = pixelSize * pixelCount;

	// [COMMENT] Calculate the number of mipmaps levels
	mipLevels = static_cast<uint32_t>(std::float_denorm_style(std::log2(std::max(texWidth, texHeight)))) + 1;

	// [COMMENT] Throw an exception if the image failed to load
	if (!pixels)
		throw std::runtime_error("Failed to load texture image!");

	// [COMMENT] Create a staging buffer to copy the pixel data to
	Buffer stagingBuffer
	{
		device, pixelSize, pixelCount,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};

	// [COMMENT] Map the staging buffer and write the pixel data to it
	stagingBuffer.map();
	stagingBuffer.writeToBuffer((void*)pixels);

	// [COMMENT] Now that we have the pixel data in a buffer, we can free the pixel data
	stbi_image_free(pixels);

	// [COMMENT] Struct used to create the image
	VkImageCreateInfo imageCreateInfo{};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	// [COMMENT] The coordinates system used for the texels (1D, 2D, 3D)
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	// [COMMENT] The number of texels in each dimension of the image
	imageCreateInfo.extent.width = texWidth;
	imageCreateInfo.extent.height = texHeight;
	imageCreateInfo.extent.depth = 1;
	// [COMMENT] The number of mipmap levels
	imageCreateInfo.mipLevels = mipLevels;
	// [COMMENT] The number of layers in the image
	imageCreateInfo.arrayLayers = 1;
	// [COMMENT] The format of the texels, should be the same as the pixels in the buffer
	imageCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	// [COMMENT] The tiling mode of the image (can be changed later)
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	// [COMMENT] The initialLayout specifies the layout of the image data on creation
	// [COMMENT] VK_IMAGE_LAYOUT_UNDEFINED: Not usable by the GPU and the very first transition will discard the texels
	// [COMMENT] VK_IMAGE_LAYOUT_PREINITIALIZED: Not usable by the GPU, but the first transition will preserve the texels
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// [COMMENT] The usage of the image (same as during buffer creation)
	imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	// [COMMENT] The image will only be used by one queue family: the one that supports graphics
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	// [COMMENT] Samples used for multisampling
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	// [COMMENT] Optional flags for sparse images
	imageCreateInfo.flags = 0;

	// [COMMENT] Create the actual image using the imageInfo struct we just filled
	device.CreateImageWithInfo(imageCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);

	// [COMMENT] Create the image view
	textureImageView = device.CreateImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, mipLevels);
	// [COMMENT] Create the texture sampler
	CreateTextureSampler();

	// [COMMENT] Transition the texture image so that it is ready to be copied to
	TransitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);

	// [COMMENT] Copy the staging buffer to the texture image
	device.CopyBufferToImage(stagingBuffer.getBuffer(), textureImage, texWidth, texHeight, 1);

	// [COMMENT] Generate the mipmaps for the texture and transition the texture image so that it is ready to be sampled from
	GenerateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels);
	// [COMMENT] Transition the texture image so that it is ready to be sampled from
	// TransitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels);

	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = textureImageView;
	imageInfo.sampler = textureSampler;

}

VulkanTutorial::Texture::~Texture()
{
	// [COMMENT] Destroy the sampler object
	vkDestroySampler(device.device(), textureSampler, nullptr);

	// [COMMENT] Destroy the image view used to access the texture image
	vkDestroyImageView(device.device(), textureImageView, nullptr);

	// [COMMENT] Destroy the texture image and free the memory
	vkDestroyImage(device.device(), textureImage, nullptr);
	vkFreeMemory(device.device(), textureImageMemory, nullptr);
}

void VulkanTutorial::Texture::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
{
	VkCommandBuffer commandBuffer = device.BeginSingleTimeCommands();

	// [COMMENT] Struct to create a memory barrier, a memory barrier is used to synchronize access to resources
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	// [COMMENT] Specifies layout for transition
	// [COMMENT] The old layout the image is currently using, can be VK_IMAGE_LAYOUT_UNDEFINED if we don't care about the existing contents
	barrier.oldLayout = oldLayout;
	// [COMMENT] The new layout to transition the image to
	barrier.newLayout = newLayout;

	// [COMMENT] If we want to transfer queue family ownership, we can specify the indices of the queue families
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

	// [COMMENT] The image to apply the barrier to
	barrier.image = image;
	// [COMMENT] The subresource range describes the image's purpose and which part of the image should be transitioned
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	// [COMMENT] The mip map levels and array layers to apply the barrier to
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = mipLevels;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	// [COMMENT] srcAccessMask specifies which types of operations involving the resource must happen before the barrier
	// [COMMENT] dstAccessMask specifies which types of operations involving the resource must wait on the barrier
	// [COMMENT] We need to set theses access masks to the right values depending on the old and new layouts
	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	// [COMMENT] If we don't care about the data and are transitioning to VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	// [COMMENT] If we are transitioning from VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	// [COMMENT] Other transitions are not supported
	// [COMMENT] We might have to put this function in Device.cpp, and possibly extend it to support more transitions
	else
		throw std::invalid_argument("unsupported layout transition!");


	// [COMMENT] All pipeline barriers are submitted using this function
	// [COMMENT] 2nd parameter specifies which pipeline stage to happen before the barrier
	// [COMMENT] 3rd parameter specifies which pipeline stage to wait on the barrier
	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	device.EndSingleTimeCommands(commandBuffer);
}

void VulkanTutorial::Texture::CreateTextureSampler()
{
	// [COMMENT] Struct to create a sampler
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	// [COMMENT] How to interpolate texels that are magnified (oversampling) or minified (undersampling)
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	// [COMMENT] How to handle texture coordinates outside the image
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	// [COMMENT] How to handle depth coordinates outside the image
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	// [COMMENT] Anisotropic filtering
	samplerInfo.anisotropyEnable = VK_TRUE;
	// [COMMENT] Get the maximum anisotropy level supported by the physical device
	samplerInfo.maxAnisotropy = device.properties.limits.maxSamplerAnisotropy;
	// [COMMENT] Which color is returned when sampling beyond the image with clamp to border addressing mode
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	// [COMMENT] Which coordinate system to use to address texels in an image
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	// [COMMENT] If true, texels will first be compared to a value, and the result of the comparison is used in filtering operations
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	// [COMMENT] Mipmap settings
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	// [COMMENT] The minimum and maximum mipmap levels to use
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = static_cast<float>(mipLevels);

	// [COMMENT] Create the sampler and check for errors
	if (vkCreateSampler(device.device(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
		throw std::runtime_error("failed to create texture sampler!");
}

void VulkanTutorial::Texture::GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
	// [COMMENT] Check if image format supports linear blitting
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(device.physicalDevice(), imageFormat, &formatProperties);
	// [COMMENT] If it does not we cannot generate mipmaps, so throw an error
	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
		throw std::runtime_error("texture image format does not support linear blitting!");

	VkCommandBuffer commandBuffer = device.BeginSingleTimeCommands();

	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = image;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	int32_t mipWidth = texWidth;
	int32_t mipHeight = texHeight;

	// [COMMENT] We will use VkCmdBlitImage for each mipmap level (we start the loop at 1 because the base level already exists by default)
	for (uint32_t i = 1; i < mipLevels; i++) 
	{
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

		vkCmdPipelineBarrier(
			commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		VkImageBlit blit{};
		blit.srcOffsets[0] = { 0, 0, 0 };
		blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;
		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;

		vkCmdBlitImage(
			commandBuffer,
			image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &blit,
			VK_FILTER_LINEAR
		);

		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;
	}

	barrier.subresourceRange.baseMipLevel = mipLevels - 1;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
		0, nullptr,
		0, nullptr,
		1, &barrier);

	device.EndSingleTimeCommands(commandBuffer);
}
