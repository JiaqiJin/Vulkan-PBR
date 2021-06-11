# Vulkan Objects

## Vulkan objects diagram

![Image](https://github.com/Jiaqidesune/Vulkan-Engine/blob/main/Engine/Concept/Vulkan-Diagram-568x1024.png)

[Basic concepts](https://gpuopen.com/learn/understanding-vulkan-objects/).

## Vulkan Objects descriptions and creations

### Instance
It represents the connection from your application to the Vulkan runtime and therefore only should exist once in your application. 
```c++
VkInstance instance;
```

### PhysicalDevice
Represents a specific Vulkan-compatible device, like a graphics card.
> Note: We need enumerate the physical device and check if the device is suitable.
```c++
VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
```

### Queue 
Queue is a objects represent a queue of commands to be excute on device. Anything from drawing to uploading texture, requiered commands to be submitted to a "queue" using the function vkQueueSubmit. There are different types of queues (compute queue, graphic queue ...)that originate from different queue families and each family of queues allows only a subset of commands (computer commands ...).

```c++
VkQueue graphicsQueue;
VkQueue presentQueue; 
```

### Device
Can be thought of as a logical device. During device creation, you need to specify which features you want to enable, and some of them are fundamental like anisotropic texture filtering. You also must state all queues that will be in use, their number and their Queue Families.
```c++
VkDevice device;
```






