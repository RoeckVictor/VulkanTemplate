#include <iostream>

#include "apps/App.h"
#include "apps/DemoScene.h"
#include "apps/SimpleModelApp.h"
#include "apps/DepthTest.h"

int main()
{
	VulkanTutorial::DemoScene app;
	// VulkanTutorial::SimpleModelApp app;
	// VulkanTutorial::DepthTest app;
	try
	{
		app.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}