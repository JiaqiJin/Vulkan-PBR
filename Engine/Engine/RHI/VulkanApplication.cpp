#define VK_USE_PLATFORM_WIN32_KHR
#include "VulkanApplication.h"
#include "VulkanRenderer.h"
#include "VulkanUtils.h"

#include "RenderScene.h"

#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

#include <array>
#include <iostream>
#include <set>
#include <limits>

static std::string vertex_shader_path = "Assert/Shader/shader.vert";
static std::string fragment_shader_path = "Assert/Shader/shader.frag";
static std::string albedoTexturePath = "Assert/Texture/Default_albedo.jpg";
static std::string normalTexturePath = "Assert/Texture/Default_normal.jpg";
static std::string aoTexturePath = "Assert/Texture/Default_AO.jpg";
static std::string shadingTexturePath = "Assert/Texture/Default_metalRoughness.jpg";
static std::string emissionTexturePath = "Assert/Texture/Default_emissive.jpg";
static std::string model_path = "Assert/Model/DamagedHelmet.fbx";