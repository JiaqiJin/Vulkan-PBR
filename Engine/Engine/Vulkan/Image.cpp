#include "Image.h"
#include "Device.h"
#include "Queue.h"
#include "../Common/Logger.h"
#include <set>

namespace Vulkan
{
	static inline uint32_t simple_ctz(uint32_t x) 
	{
		if (x & 0x80000000u)
			return 32u;
		uint32_t ret{ 1 };
		while (x >> ret)
			++ret;
		return ret;
	}

	Image::Image(const std::shared_ptr<Device>& device, const VkExtent2D& size,
		uint32_t mip_level, VkFormat format, VkImageUsageFlags usage, const std::vector<std::shared_ptr<Queue>>& access_queue)
		: m_device(device)
	{
		VkImageCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		create_info.imageType = VK_IMAGE_TYPE_2D;
		create_info.extent = { size.width, size.height, 1 };
		create_info.mipLevels = mip_level;
		create_info.arrayLayers = 1;
		create_info.format = format;
		create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		create_info.usage = usage;
		create_info.samples = VK_SAMPLE_COUNT_1_BIT;

		Init(VMA_MEMORY_USAGE_GPU_ONLY, create_info, access_queue);
	}

	void Image::Init(VmaMemoryUsage memory_usage, const VkImageCreateInfo& create_info, const std::vector<std::shared_ptr<Queue>>& access_queues) 
	{
		m_extent = create_info.extent;
		m_type = create_info.imageType;
		m_format = create_info.format;
		m_mip_levels = create_info.mipLevels;
		m_array_layers = create_info.arrayLayers;

		std::set<uint32_t> queue_family_set;
		for (auto& i : access_queues)
			queue_family_set.insert(i->GetFamilyIndex());
		std::vector<uint32_t> queue_families{ queue_family_set.begin(), queue_family_set.end() };

		VkImageCreateInfo new_info{ create_info };
		if (queue_families.size() <= 1)
		{
			new_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}
		else
		{
			new_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
			new_info.queueFamilyIndexCount = queue_families.size();
			new_info.pQueueFamilyIndices = queue_families.data();
		}

		VmaAllocationCreateInfo alloc_info = {};
		alloc_info.usage = memory_usage;

		if (vmaCreateImage(m_device->GetAllocatorHandle(), &new_info, &alloc_info, &m_image, &m_allocation, nullptr) != VK_SUCCESS)
			K_ERROR("Faile to create Image");
	}

	uint32_t Image::QueryMipLevel(uint32_t w) { return simple_ctz(w); }

	uint32_t Image::QueryMipLevel(const VkExtent2D& size) { return simple_ctz(size.width | size.height); }

	uint32_t Image::QueryMipLevel(const VkExtent3D& size) { return simple_ctz(size.width | size.height | size.depth); }

	Image::~Image()
	{
		if (m_image)
			vmaDestroyImage(m_device->GetAllocatorHandle(), m_image, m_allocation);
	}
}