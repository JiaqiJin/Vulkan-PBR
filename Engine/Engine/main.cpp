#include <iostream>
#include <stdexcept>

#include "Application.h"

#include <GLFW/glfw3.h>

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
// https://interplayoflight.wordpress.com/2013/12/30/readings-on-physically-based-rendering/

