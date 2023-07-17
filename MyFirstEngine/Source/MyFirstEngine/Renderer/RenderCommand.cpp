#include "Mfepch.h"
#include "RendererAPI.h"
#include "RenderCommand.h"

#include "VulkanRenderer/VulkanRendererAPI.h"

namespace MyFirstEngine
{
	RendererAPI* RenderCommand::rendererAPI = new VulkanRendererAPI();
}