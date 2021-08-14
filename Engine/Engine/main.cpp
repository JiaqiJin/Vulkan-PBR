#include <iostream>
#include <stdexcept>

#include "Application.h"
#include "Common/Logger.h"
#include <GLFW/glfw3.h>

#include "RHI/Device.h"

int main(void)
{
	/*if (!glfwInit())
		return EXIT_FAILURE;*/

	Log::Init();
	K_INFO("Init Kawaii Vulkan");
	Application app{};
	app.Run();
}

// https://github.com/Drawoceans/vulkan_tutorial_zhcn
// https://interplayoflight.wordpress.com/2013/12/30/readings-on-physically-based-rendering/

