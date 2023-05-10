#pragma once

#include "window.h"

// std lib headers
#include <string>
#include <vector>

namespace VulkanTutorial 
{
	struct SwapChainSupportDetails 
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct QueueFamilyIndices 
	{
		// [COMMENT] Index of the last queue family that supports VK_QUEUE_GRAPHICS_BIT
		uint32_t graphicsFamily;
		// [COMMENT] Index of the last queue family that supports rendering to the surface
		uint32_t presentFamily;

		// [COMMENT] If we found a queue family that supports VK_QUEUE_GRAPHICS_BIT
		bool graphicsFamilyHasValue = false;
		// [COMMENT] If we found a queue family that supports rendering to the surface
		bool presentFamilyHasValue = false;

		// [COMMENT] Check if we found indexes for the required queue families
		bool IsComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
	};

	class Device 
	{
	public:
		// [COMMENT] Enables or disables validation layers depending on build mode
#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif

		Device(Window& window);
		~Device();

		// Not copyable or movable
		Device(const Device&) = delete;
		void operator=(const Device&) = delete;
		Device(Device&&) = delete;
		Device& operator=(Device&&) = delete;

		VkCommandPool GetCommandPool() { return commandPool; }
		VkDevice device() { return device_; }
		VkPhysicalDevice physicalDevice() { return physicalDevice_; }
		VkSurfaceKHR surface() { return surface_; }
		VkQueue graphicsQueue() { return graphicsQueue_; }
		VkQueue presentQueue() { return presentQueue_; }

		SwapChainSupportDetails GetSwapChainSupport() { return QuerySwapChainSupport(physicalDevice_); }
		// [COMMENT] Finds the right memory type to use by combining requirements from the buffer and our application
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		QueueFamilyIndices FindPhysicalQueueFamilies() { return FindQueueFamilies(physicalDevice_); }
		// [COMMENT] Find the right VkFormat from a list of candidates depending on support from the device
		VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		// [COMMENT] Get the maximum sample count supported by the device
		VkSampleCountFlagBits GetMaxUsableSampleCount();

		// [COMMENT] Helper function to create a buffer
		void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		// [COMMENT] Creates a temporary short lived command buffer
		VkCommandBuffer BeginSingleTimeCommands();
		// [COMMENT] Stop recording the command buffer and submit it to the GPU
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
		// [COMMENT] Copies the contents of one buffer to another
		void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		// [COMMENT] Copies the contents of a buffer to an image
		void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);
		// [COMMENT] Creates an image and binds it to memory
		void CreateImageWithInfo(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		// [COMMENT] Creates an image view used to access an image
		VkImageView CreateImageView(VkImage image, VkFormat format, uint32_t mipLevels);

		VkPhysicalDeviceProperties properties;

	private:
		void CreateInstance();
		void SetupDebugMessenger();
		// [COMMENT] Creates the surface to render to
		void CreateSurface();
		// [COMMENT] Chooses which graphics card to use
		void PickPhysicalDevice();
		// [COMMENT] Creates the logical device which will be used to interface with the GPU
		void CreateLogicalDevice();
		// [COMMENT] Creates the command pool which will be used to record commands to the GPU
		void CreateCommandPool();

		// [COMMENT] helper functions
		// [COMMENT] Checks if the graphics card is suitable for our application
		bool IsDeviceSuitable(VkPhysicalDevice device);
		std::vector<const char*> GetRequiredExtensions();
		bool CheckValidationLayerSupport();
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		void HasGflwRequiredInstanceExtensions();
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

		// [COMMENT] The Vulkan instance, which is the connection between the application and the Vulkan library
		VkInstance instance;
		// [COMMENT] The debug messenger, which is used to receive validation layer messages
		VkDebugUtilsMessengerEXT debugMessenger;
		// [COMMENT] The physical device, which is the GPU we will use
		VkPhysicalDevice physicalDevice_ = VK_NULL_HANDLE;
		// [COMMENT] The window used to display the application
		Window& window;
		// [COMMENT] The command pool, which is used to record all operations we want to perform on the GPU
		VkCommandPool commandPool;

		// [COMMENT] The logical device, which is the handle to interface with the GPU
		VkDevice device_;
		// [COMMENT] The surface, which is the connection between the window system and Vulkan
		VkSurfaceKHR surface_;
		// [COMMENT] The graphics queue, which is the queue that we will submit our drawing commands to
		VkQueue graphicsQueue_;
		// [COMMENT] The present queue, which is the queue that we will submit our presentation commands to
		VkQueue presentQueue_;

		// [COMMENT] The validation layers that we want to enable
		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
		// [COMMENT] The device extensions that we want to enable
		const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	};

}