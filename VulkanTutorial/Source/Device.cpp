#include "Device.h"

// std headers
#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>

namespace VulkanTutorial 
{
	// [COMMENT] Callback used to send debug messages from the validation layers to the console
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) 
	{
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}

	// [COMMENT] Creates the VkDebugUtilsMessengerEXT object
	VkResult CreateDebugUtilsMessengerEXT(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger) 
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		else
			return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	// [COMMENT] Destroys the VkDebugUtilsMessengerEXT object, called when cleaning
	void DestroyDebugUtilsMessengerEXT(
		VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator) 
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
			func(instance, debugMessenger, pAllocator);
	}

	// class member functions
	Device::Device(Window& window)
		: window{ window }
	{
		CreateInstance();
		SetupDebugMessenger();
		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();
		CreateCommandPool();
	}

	Device::~Device() 
	{
		// [COMMENT] Destroy the command pool used for rendering
		vkDestroyCommandPool(device_, commandPool, nullptr);
		// [COMMENT] Destroy the logical device (VkDevice)
		vkDestroyDevice(device_, nullptr);

		// [COMMENT] Destroy validation layer stuff
		if (enableValidationLayers)
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

		// [COMMENT] Destroy the VkSurfaceKHR
		vkDestroySurfaceKHR(instance, surface_, nullptr);
		// [COMMENT] Destroy the VkInstance
		vkDestroyInstance(instance, nullptr);
	}

	void Device::CreateInstance() 
	{
		// [COMMENT] Makes sures that every validation layers we want to use are supported
		if (enableValidationLayers && !CheckValidationLayerSupport()) 
			throw std::runtime_error("validation layers requested, but not available!");

		// [COMMENT] Struct containing useful information to optimize the application (optional)
		VkApplicationInfo appInfo = {};
		// [COMMENT] This struct needs the user to explicitly specify it's type
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		// [COMMENT] Name of the application
		appInfo.pApplicationName = "Vulkan Tutorial App";
		// [COMMENT] Version of the application (the one we are writting)
		appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
		// [COMMENT] Name of the engine used to create the application (we are making our own)
		appInfo.pEngineName = "No Engine";
		// [COMMENT] Version of said engine
		appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
		// [COMMENT] highest version of Vulkan that the application is designed to use
		appInfo.apiVersion = VK_API_VERSION_1_0;
		// [COMMENT] 
		appInfo.pNext = nullptr;
		
		// [COMMENT] Struct telling Vulkan which global extensions and validation layers we want to use
		VkInstanceCreateInfo createInfo = {};
		// [COMMENT] This struct needs the user to explicitly specify it's type
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		// [COMMENT] Optional application info
		createInfo.pApplicationInfo = &appInfo;

		// [COMMENT] Get a list of every extensions we need to run the application
		std::vector<const char*> extensions = GetRequiredExtensions();
		// [COMMENT] Number of extensions
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		// [COMMENT] Names of the extensions we need to enable
		createInfo.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		if (enableValidationLayers) 
		{
			// [COMMENT] Number of enabled validation layers
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			// [COMMENT] The validation layers we want to enable
			createInfo.ppEnabledLayerNames = validationLayers.data();

			// [COMMENT] Allows the validation layer callbacks to be used during vkCreateInstance and vkDestroyInstance
			PopulateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else 
		{
			// [COMMENT] No validation layers
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		// [COMMENT] Create the Vulkan instance with the specified infos
		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
			throw std::runtime_error("failed to create instance!");

		// [COMMENT] Makes sures that the extensions required by GLFW are supported
		HasGflwRequiredInstanceExtensions();
	}

	// [COMMENT] Chooses which graphics card to use
	void Device::PickPhysicalDevice() 
	{
		// [COMMENT] Get the number of physical devices available
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
		// [COMMENT] If no GPU are found, our application can't run
		if (deviceCount == 0)
			throw std::runtime_error("failed to find GPUs with Vulkan support!");

		// [COMMENT] Optional info sent to the console
		std::cout << "Device count: " << deviceCount << std::endl;

		// [COMMENT] Now that we know how many devices are available, we can allocate an array to hold them
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		// [COMMENT] Pick the first device that is suitable for our needs
		// [COMMENT] [TODO] Make a better selection process instead of selecting the first that is suitable
		for (const auto& device : devices) 
		{
			if (IsDeviceSuitable(device)) 
			{
				physicalDevice_ = device;
				break;
			}
		}

		// [COMMENT] If no device is suitable, our application can't run
		if (physicalDevice_ == VK_NULL_HANDLE) 
			throw std::runtime_error("failed to find a suitable GPU!");

		// [COMMENT] Print the name of the GPU we are using
		vkGetPhysicalDeviceProperties(physicalDevice_, &properties);
		std::cout << "physical device: " << properties.deviceName << std::endl;
	}

	void Device::CreateLogicalDevice() 
	{
		// [COMMENT] Get the physical device's queue families
		QueueFamilyIndices indices = FindQueueFamilies(physicalDevice_);

		// [COMMENT] We need to give the logical device the queue families we want to use
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		// [COMMENT] Indices of the queue families we want to use
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

		// [COMMENT] We can specify a priority for each queue, which will be used by the scheduler to determine which queue to assign a new task to
		float queuePriority = 1.0f;
		// [COMMENT] Create a VkDeviceQueueCreateInfo for each queue family we want to use
		for (uint32_t queueFamily : uniqueQueueFamilies) 
		{
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			// [COMMENT] The index of the queue family in the array of queue families returned by vkGetPhysicalDeviceQueueFamilyProperties
			queueCreateInfo.queueFamilyIndex = queueFamily;
			// [COMMENT] We can create multiple queues in the same family, but we only need one
			queueCreateInfo.queueCount = 1;
			// [COMMENT] Set the priority of the queue, we should use an array if we want to set multiple priorities
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		// [COMMENT] We use a VkPhysicalDeviceFeatures struct to tell Vulkan what features we want to use
		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		// [COMMENT] Fill in the information needed to create the logical device
		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		// [COMMENT] The queue create infos we created earlier
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		// [COMMENT] The features we want to use, contained in the VkPhysicalDeviceFeatures struct we created earlier
		createInfo.pEnabledFeatures = &deviceFeatures;

		// [COMMENT] The number of extension we want to use
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		// [COMMENT] The extensions we want to use
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		// [COMMENT] might not really be necessary anymore because device specific validation layers have been deprecated
		// [COMMENT] Defines how many and wich validation layers we want to enable, if any
		if (enableValidationLayers) 
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
			createInfo.enabledLayerCount = 0;

		// [COMMENT] Create the logical device
		if (vkCreateDevice(physicalDevice_, &createInfo, nullptr, &device_) != VK_SUCCESS)
			throw std::runtime_error("failed to create logical device!");

		// [COMMENT] Get a handle to the graphics queue and store it in the graphicsQueue variable
		vkGetDeviceQueue(device_, indices.graphicsFamily, 0, &graphicsQueue_);
		// [COMMENT] Get a handle to the present queue and store it in the presentQueue variable
		vkGetDeviceQueue(device_, indices.presentFamily, 0, &presentQueue_);
	}

	void Device::CreateCommandPool() 
	{
		// [COMMENT] Get the queue families of the physical device
		QueueFamilyIndices queueFamilyIndices = FindPhysicalQueueFamilies();

		// [COMMENT] Fill in the information needed to create the command pool
		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		// [COMMENT] Command buffers are submitted to queues, here we record commands for drawing, so we use the graphics queue
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
		// [COMMENT] Set the command pool flags
		// [COMMENT] VK_COMMAND_POOL_CREATE_TRANSIENT_BIT : Hint that command buffers are rerecorded with new commands very often
		// [COMMENT] VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : Allow command buffers to be rerecorded individually
		// [COMMENT] without this flag they all have to be reset together
		poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		// [COMMENT] Create the command pool and check for errors
		if (vkCreateCommandPool(device_, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) 
			throw std::runtime_error("failed to create command pool!");
	}

	void Device::CreateSurface() { window.CreateWindowSurface(instance, &surface_); }

	// [COMMENT] Checks if the graphics card is suitable for our application
	bool Device::IsDeviceSuitable(VkPhysicalDevice device) 
	{
		// [COMMENT] Get the device's queue families, a suitable device should have IsComplete return true
		QueueFamilyIndices indices = FindQueueFamilies(device);

		// [COMMENT] Check if the device supports the required extensions
		bool extensionsSupported = CheckDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionsSupported) 
		{
			SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		// [COMMENT] Check support for optional features like texture compression, 64 bit floats and multi viewport rendering 
		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		// [COMMENT] Check if the device has the required queue families and extensions
		return indices.IsComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
	}

	// [COMMENT] Setup a VkDebugUtilsMessengerCreateInfoEXT default parameters
	void Device::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) 
	{
		createInfo = {};
		// [COMMENT] This struct needs the user to explicitly specify it's type
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		// [COMMENT] Specify all types of severities we want the callback to be called for
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		// [COMMENT] Specify all types of messages we want the callback to be called for
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
								 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
								 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		// [COMMENT] The callback function to call
		createInfo.pfnUserCallback = DebugCallback;
		// [COMMENT] Can be used to pass a pointer to the pUserData parameter of the callback function (optional)
		createInfo.pUserData = nullptr;
	}

	// [COMMENT] Set a VkDebugUtilsMessengerEXT to recieve validation layer message insinde a callback function
	void Device::SetupDebugMessenger() 
	{
		if (!enableValidationLayers) 
			return;

		// [COMMENT] Set which message severity/type should be send to the callback function and whick callback function to call
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		PopulateDebugMessengerCreateInfo(createInfo);

		// [COMMENT] Creates the VkDebugUtilsMessengerEXT object
		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) 
			throw std::runtime_error("failed to set up debug messenger!");
	}

	bool Device::CheckValidationLayerSupport() 
	{
		// [COMMENT] Get the number of validation layer
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		// [COMMENT] Now that we know the validation layer count, get the validation layers properties
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		// [COMMENT] Makes sures that all layers in validationLayers exist in the availableLayers list
		for (const char* layerName : validationLayers) 
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) 
			{
				if (strcmp(layerName, layerProperties.layerName) == 0) 
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
				return false;
		}

		return true;
	}

	std::vector<const char*> Device::GetRequiredExtensions() 
	{
		// [COMMENT] GLFW has a built-in function that returns the extensions it needs
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		// [COMMENT] List of all extensions required for the application
		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		// [COMMENT] If we need the validation layers, add the vulkan debug utils extension to the list
		if (enableValidationLayers)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		return extensions;
	}

	void Device::HasGflwRequiredInstanceExtensions() 
	{
		// [COMMENT] 1st argument of vkEnumerateInstanceExtensionProperties is used to filter the extensions by a specific validation layer
		// [COMMENT] Get the number of supported extensions
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		// [COMMENT] Now that we know how many extension are supported, get the extensions properties
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		// [COMMENT] Each VkExtensionProperties contains the name and version of an extension
		// [COMMENT] Here we are printing every available extensions
		std::cout << "available extensions:" << std::endl;
		std::unordered_set<std::string> available;
		for (const auto& extension : extensions) 
		{
			std::cout << "\t" << extension.extensionName << std::endl;
			available.insert(extension.extensionName);
		}

		// [COMMENT] Here we are printing every required extensions, and checking if they are available
		std::cout << "required extensions:" << std::endl;
		auto requiredExtensions = GetRequiredExtensions();
		for (const auto& required : requiredExtensions) 
		{
			std::cout << "\t" << required << std::endl;
			if (available.find(required) == available.end()) 
			{
				throw std::runtime_error("Missing required glfw extension");
			}
		}
	}

	bool Device::CheckDeviceExtensionSupport(VkPhysicalDevice device) 
	{
		// [COMMENT] Get the number of device extensions
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		// [COMMENT] Get the device extensions properties
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		// [COMMENT] This is a list representing all the extensions we need
		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		// [COMMENT] Remove from the list all the extensions that are available
		for (const auto& extension : availableExtensions)
			requiredExtensions.erase(extension.extensionName);

		// [COMMENT] If the list is empty, all the extensions we need are available
		return requiredExtensions.empty();
	}

	QueueFamilyIndices Device::FindQueueFamilies(VkPhysicalDevice device) 
	{
		QueueFamilyIndices indices;

		// [COMMENT] Get the number of queue families
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		// [COMMENT] Get the queue families properties
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) 
		{
			// [COMMENT] Check if the queue family supports the VK_QUEUE_GRAPHICS_BIT
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
			{
				indices.graphicsFamily = i;
				indices.graphicsFamilyHasValue = true;
			}
			// [COMMENT] Check if the queue family rendering to the surface
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &presentSupport);
			if (queueFamily.queueCount > 0 && presentSupport) 
			{
				indices.presentFamily = i;
				indices.presentFamilyHasValue = true;
			}
			// [COMMENT] If we found all the queue families we need, we can stop looking
			if (indices.IsComplete()) 
				break;

			i++;
		}

		return indices;
	}

	SwapChainSupportDetails Device::QuerySwapChainSupport(VkPhysicalDevice device) 
	{
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(
				device,
				surface_,
				&presentModeCount,
				details.presentModes.data());
		}
		return details;
	}

	VkFormat Device::FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) 
	{
		for (VkFormat format : candidates) 
		{
			// [COMMENT] Query the the support of the format
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice_, format, &props);

			// [COMMENT] If the format supports TILING_LINEAR or TILING_OPTIMAL, we will use it
			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
				return format;
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
				return format;
		}
		// [COMMENT] If we cannot find a suitable format, we throw an error
		throw std::runtime_error("failed to find supported format!");
	}

	VkSampleCountFlagBits Device::GetMaxUsableSampleCount()
	{
		// [COMMENT] Get the properties of the physical device
		VkPhysicalDeviceProperties physicalDeviceProperties;
		vkGetPhysicalDeviceProperties(physicalDevice_, &physicalDeviceProperties);

		// [COMMENT] Get the max sample count of both the color and depth buffer
		VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts &
							        physicalDeviceProperties.limits.framebufferDepthSampleCounts;

		// [COMMENT] Return the max sample count usable by the physical device
		if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
		if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
		if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
		if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
		if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
		if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

		// [COMMENT] If the physical device does not support multisampling, we return VK_SAMPLE_COUNT_1_BIT
		return VK_SAMPLE_COUNT_1_BIT;
	}

	uint32_t Device::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) 
	{
		// [COMMENT] Query info about the available types of memory
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice_, &memProperties);

		// [COMMENT] (typeFilter & (1 << i)) checks if the memory type i is suitable for the buffer
		// [COMMENT] (memProperties.memoryTypes[i].propertyFlags & properties) checks if the memory type i has the required properties
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
				return i;

		// [COMMENT] If we cannot find a suitable memory type, we throw an error
		throw std::runtime_error("failed to find suitable memory type!");
	}

	void Device::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) 
	{
		// [COMMENT] Structure used to create a buffer
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		// [COMMENT] Size of the buffer in bytes
		bufferInfo.size = size;
		// [COMMENT] For which purpose the buffer will be used
		bufferInfo.usage = usage;
		// [COMMENT] The buffer will be used by only one queue at the time (graphics queue in our case)
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		// [COMMENT] Create the buffer and check for errors
		if (vkCreateBuffer(device_, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
			throw std::runtime_error("failed to create vertex buffer!");

		// [COMMENT] The buffer has been created, but we still need to allocate memory for it
		// [COMMENT] The first step is to get the memory requirements for the buffer
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device_, buffer, &memRequirements);

		// [COMMENT] Structure used to allocate memory for the buffer
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		// [COMMENT] Size of the memory allocation
		allocInfo.allocationSize = memRequirements.size;
		// [COMMENT] The memory type that is suitable for the buffer
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

		// [COMMENT] Allocate memory for the buffer and check for errors
		if (vkAllocateMemory(device_, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
			throw std::runtime_error("failed to allocate vertex buffer memory!");

		// [COMMENT] If allocation was successfull, associate the buffer with the allocated memory
		vkBindBufferMemory(device_, buffer, bufferMemory, 0);
	}

	VkCommandBuffer Device::BeginSingleTimeCommands() 
	{
		// [COMMENT] Commande buffer allocation info
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		// [COMMENT] Allocate the command buffer with the given info
		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device_, &allocInfo, &commandBuffer);

		// [COMMENT] Imediatly start recording the command buffer
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		// [COMMENT] Tells the driver that we will only submit the command buffer once
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		// [COMMENT] Begine the command buffer execution
		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void Device::EndSingleTimeCommands(VkCommandBuffer commandBuffer) 
	{
		// [COMMENT] Stop recording the command buffer
		vkEndCommandBuffer(commandBuffer);

		// [COMMENT] Structure to submit the command buffer to the queue
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		// [COMMENT] The command buffer to submit
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		// [COMMENT] Submit the command buffer to the queue
		vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
		// [COMMENT] Wait for the queue to finish executing the command buffer
		vkQueueWaitIdle(graphicsQueue_);

		// [COMMENT] Free the command buffer
		vkFreeCommandBuffers(device_, commandPool, 1, &commandBuffer);
	}

	void Device::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) 
	{
		// [COMMENT] Creates a temporary command buffer to copy the buffer
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		// [COMMENT] Structure used to copy the buffer
		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;  // Optional
		copyRegion.dstOffset = 0;  // Optional
		copyRegion.size = size;
		// [COMMENT] Copy the buffer from srcBuffer to dstBuffer
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		EndSingleTimeCommands(commandBuffer);
	}

	void Device::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount) 
	{
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

		// [COMMENT] Structure used to copy the buffer to the image
		VkBufferImageCopy region{};
		// [COMMENT] Offset in the buffer where the pixel values start
		region.bufferOffset = 0;
		// [COMMENT] Specifies how the pixels are laid out in memory
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		// [COMMENT] Subresource layers to copy to
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = layerCount;

		// [COMMENT] Which part of the image to copy to
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width, height, 1 };

		// [COMMENT] Copy the buffer to the image
		vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		EndSingleTimeCommands(commandBuffer);
	}

	void Device::CreateImageWithInfo(const VkImageCreateInfo& imageInfo, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) 
	{
		// [COMMENT] Create the image and check for errors
		if (vkCreateImage(device_, &imageInfo, nullptr, &image) != VK_SUCCESS)
			throw std::runtime_error("failed to create image!");

		// [COMMENT] Get the memory requirements for the image
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device_, image, &memRequirements);

		// [COMMENT] Struct used to allocate memory for the image
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		// [COMMENT] Size of the memory allocation
		allocInfo.allocationSize = memRequirements.size;
		// [COMMENT] The memory type that is suitable for the image
		allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

		// [COMMENT] Allocate memory for the image and check for errors
		if (vkAllocateMemory(device_, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
			throw std::runtime_error("failed to allocate image memory!");

		// [COMMENT] Associate the image with the allocated memory
		if (vkBindImageMemory(device_, image, imageMemory, 0) != VK_SUCCESS)
			throw std::runtime_error("failed to bind image memory!");
	}

	VkImageView Device::CreateImageView(VkImage image, VkFormat format, uint32_t mipLevels)
	{
		// [COMMENT] Fill in the infos required to create the image view
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		// [COMMENT] Specify the image to create the view for
		viewInfo.image = image;
		// [COMMENT] Specify how the image data should be interpreted
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		// [COMMENT] Describes what the image's purpose is and which part of the image should be accessed
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		// [COMMENT] Number of mip levels
		viewInfo.subresourceRange.levelCount = mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		// [COMMENT] Creates the image view and makes sure it worked successfully
		if (vkCreateImageView(device(), &viewInfo, nullptr, &imageView) != VK_SUCCESS)
			throw std::runtime_error("failed to create texture image view!");

		return imageView;
	}

}