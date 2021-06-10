#include <iostream>
#include <stdexcept>

#include "RHI/VulkanApplication.h"

#include <GLFW/glfw3.h>
// https://github.com/Jiaqidesune/Vulkan-PBR/tree/63ca341a4a2bb8f69f0395bd0e44a19fdabedb9c/Engine/Engine/RHI
int main(void)
{
	if (!glfwInit())
		return EXIT_FAILURE;

	try
	{
		Application app;
		app.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glfwTerminate();
	return EXIT_SUCCESS;
}

// https://github.com/Drawoceans/vulkan_tutorial_zhcn