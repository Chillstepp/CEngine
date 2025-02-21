//
// Created by Chillstep on 2025/2/5.
//
#include "Graphics/CEVulkanGraphicContext.h"
#include "Window/CEGlfwWindow.h"

namespace CE{

	const DeviceFeature RequestedLayers[] = {
		{"VK_LAYER_KHRONOS_validation", true}
	};

	const DeviceFeature RequestedExtensions[] = {
		{VK_KHR_SURFACE_EXTENSION_NAME, true},
#ifdef CE_ENGINE_PLATFORM_WIN32
		{ VK_KHR_WIN32_SURFACE_EXTENSION_NAME, true },
#elif CE_ENGINE_PLATFORM_MACOS
		{ VK_MVK_MACOS_SURFACE_EXTENSION_NAME, true },
#elif CE_ENGINE_PLATFORM_LINUX
		{ VK_KHR_XCB_SURFACE_EXTENSION_NAME, true },
#endif
	};

	CEVulkanGraphicContext::CEVulkanGraphicContext(CEWindow* Window)
	{
		CreateInstance();
		CreateSurface(Window);
		SelectPhyDevice();
	}

	CEVulkanGraphicContext::~CEVulkanGraphicContext()
	{
		vkDestroySurfaceKHR(VulkanInstance, VulkanSurface, nullptr);
		vkDestroyInstance(VulkanInstance, nullptr);
	}

	static VkBool32 VkDebugReportCallback(VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT                  objectType,
		uint64_t                                    object,
		size_t                                      location,
		int32_t                                     messageCode,
		const char*                                 pLayerPrefix,
		const char*                                 pMessage,
		void*                                       pUserData)
	{
		if(flags & VK_DEBUG_REPORT_ERROR_BIT_EXT){
			LOG_E("{0}", pMessage);
		}
		if(flags & VK_DEBUG_REPORT_WARNING_BIT_EXT || flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT){
			LOG_W("{0}", pMessage);
		}
		return VK_TRUE;
	}

	void CEVulkanGraphicContext::CreateInstance()
	{
		//1.构建Layers
		uint32_t AvailableLayerCount;
		CALL_VK(vkEnumerateInstanceLayerProperties(&AvailableLayerCount, nullptr));
		std::vector<VkLayerProperties>AvailableLayers(AvailableLayerCount);
		CALL_VK(vkEnumerateInstanceLayerProperties(&AvailableLayerCount, AvailableLayers.data()));
		uint32_t enableLayerCount = 0;
		const char *enableLayers[32];
		if(bShouldValidate){
			if(!checkDeviceFeatures("Instance Layers", false, AvailableLayerCount, AvailableLayers.data(),
				ARRAY_LENGTH(RequestedLayers), RequestedLayers, &enableLayerCount, enableLayers)){
				return;
			}
		}

		// 2. 构建extension
		uint32_t availableExtensionCount;
		CALL_VK(vkEnumerateInstanceExtensionProperties("", &availableExtensionCount, nullptr));
		VkExtensionProperties availableExtensions[availableExtensionCount];
		CALL_VK(vkEnumerateInstanceExtensionProperties("", &availableExtensionCount, availableExtensions));

		uint32_t glfwRequestedExtensionCount;
		const char ** glfwRequestedExtensions = glfwGetRequiredInstanceExtensions(&glfwRequestedExtensionCount);
		std::unordered_set<std::string> allRequestedExtensionSet;
		std::vector<DeviceFeature> allRequestedExtensions;
		for (const auto &item: RequestedExtensions){
			if(allRequestedExtensionSet.find(item.name) == allRequestedExtensionSet.end()){
				allRequestedExtensionSet.insert(item.name);
				allRequestedExtensions.push_back(item);
			}
		}
		for (int i = 0; i < glfwRequestedExtensionCount; i++){
			const char *extensionName = glfwRequestedExtensions[i];
			if(allRequestedExtensionSet.find(extensionName) == allRequestedExtensionSet.end()){
				allRequestedExtensionSet.insert(extensionName);
				allRequestedExtensions.push_back({ extensionName, true });
			}
		}

		uint32_t enableExtensionCount;
		const char *enableExtensions[32];
		if(!checkDeviceFeatures("Instance Extension", true, availableExtensionCount, availableExtensions,
			allRequestedExtensions.size(), allRequestedExtensions.data(), &enableExtensionCount, enableExtensions)){
			return;
		}

		VkApplicationInfo applicationInfo = {
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = nullptr,
			.pApplicationName = "ChillEngine",
			.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
			.pEngineName = "None",
			.engineVersion = VK_MAKE_VERSION(1, 0, 0),
			.apiVersion = VK_API_VERSION_1_3
		};

		VkDebugReportCallbackCreateInfoEXT debugReportCallbackInfoExt = {
			.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
			.pNext = nullptr,
			.flags = VK_DEBUG_REPORT_WARNING_BIT_EXT
				| VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT
				| VK_DEBUG_REPORT_ERROR_BIT_EXT,
			.pfnCallback = VkDebugReportCallback
		};

		// 3. create instance
		VkInstanceCreateInfo instanceInfo = {
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pNext = bShouldValidate ? &debugReportCallbackInfoExt : nullptr,
			.flags = 0,
			.pApplicationInfo = &applicationInfo,
			.enabledLayerCount = enableLayerCount,
			.ppEnabledLayerNames = enableLayerCount > 0 ? enableLayers : nullptr,
			.enabledExtensionCount = enableExtensionCount,
			.ppEnabledExtensionNames = enableExtensionCount > 0 ? enableExtensions : nullptr
		};
		CALL_VK(vkCreateInstance(&instanceInfo, nullptr, &VulkanInstance));
		LOG_T("{0} : instance : {1}", __FUNCTION__, (void*)VulkanInstance);
	}

	void CEVulkanGraphicContext::CreateSurface(CEWindow* Window)
	{
		if(!Window){
			LOG_E("Window is not exist.");
			return;
		}

		CEGlfwWindow* GlfwWindow = dynamic_cast<CEGlfwWindow*>(Window);
		if(!GlfwWindow)
		{
			LOG_E("This is not a Glfw Window.");
			return;
		}
		CALL_VK(glfwCreateWindowSurface(VulkanInstance, GlfwWindow->GetWindowHandle(), nullptr, &VulkanSurface));
		LOG_T("{0} : Surface : {1}", __FUNCTION__, (void*)VulkanSurface);
	}
	void CEVulkanGraphicContext::SelectPhyDevice()
	{
		uint32_t PhyDeviceCount = 0;
		CALL_VK(vkEnumeratePhysicalDevices(VulkanInstance, &PhyDeviceCount, nullptr));
		std::vector<VkPhysicalDevice> PhyDevices(PhyDeviceCount);
		CALL_VK(vkEnumeratePhysicalDevices(VulkanInstance, &PhyDeviceCount, PhyDevices.data()));

		uint32_t MaxScore = 0;
		int32_t MaxScorePhyDeviceIndex = -1;
		//For each device
		for(int i = 0; i < PhyDeviceCount; i++)
		{
			VkPhysicalDeviceProperties Props;
			vkGetPhysicalDeviceProperties(PhyDevices[i], &Props);
			PrintPhyDeviceInfo(Props);

			uint32_t Score = DeviceScoreEvaluate(Props);

			//  Device Format contribute to score
			uint32_t FormatCount;
			CALL_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(PhyDevices[i], VulkanSurface, &FormatCount, nullptr));
			std::vector<VkSurfaceFormatKHR> Formats(FormatCount);
			CALL_VK(vkGetPhysicalDeviceSurfaceFormatsKHR(PhyDevices[i], VulkanSurface, &FormatCount, Formats.data()));
			for(int j = 0; j < FormatCount; j++)
			{
				if(Formats[j].format == VK_FORMAT_B8G8R8A8_UNORM && Formats[j].colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
				{
					Score += 10;
					break;
				}
			}

			//Query queue family
			uint32_t QueueFamilyCount;
			vkGetPhysicalDeviceQueueFamilyProperties(PhyDevices[i], &QueueFamilyCount, nullptr);
			std::vector<VkQueueFamilyProperties> QueueFamilys(QueueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(PhyDevices[i], &QueueFamilyCount, QueueFamilys.data());

			LOG_D("score    --->    : {0}", Score);
			LOG_D("queue family count    : {0}", QueueFamilyCount);

			for(int j = 0; j < QueueFamilyCount; j++)
			{
				if(QueueFamilys[j].queueCount == 0){
					continue;
				}

				//1. graphic family
				if(QueueFamilys[j].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					GraphicQueueFamily.QueueFamilyIndex = j;
					GraphicQueueFamily.QueueCount = QueueFamilys[j].queueCount;
				}

				//这里我们希望 呈现队列和图形队列最好不是一个队列
				if(GraphicQueueFamily.QueueFamilyIndex >= 0 && PresentQueueFamily.QueueFamilyIndex >= 0
					&& GraphicQueueFamily.QueueFamilyIndex != PresentQueueFamily.QueueFamilyIndex)
				{
					break;
				}

				//2. present family
				VkBool32 bSupportSurface;
				vkGetPhysicalDeviceSurfaceSupportKHR(PhyDevices[i], j, VulkanSurface, &bSupportSurface);
				if(bSupportSurface){
					PresentQueueFamily.QueueFamilyIndex = j;
					PresentQueueFamily.QueueCount = QueueFamilys[j].queueCount;
				}
			}

			if(GraphicQueueFamily.QueueFamilyIndex >= 0 && PresentQueueFamily.QueueFamilyIndex >= 0 && Score > MaxScore){
				MaxScorePhyDeviceIndex = i;
				MaxScore = Score;
			}
		}
		LOG_D("-----------------------------");


		if(MaxScorePhyDeviceIndex < 0){
			LOG_W("Maybe can not find a suitable physical device, will 0.");
			MaxScorePhyDeviceIndex = 0;
		}

		VulkanPhyDevice = PhyDevices[MaxScorePhyDeviceIndex];
		vkGetPhysicalDeviceMemoryProperties(VulkanPhyDevice, &PhyDeviceMemProperties);
		LOG_T("{0} : physical device Index:{1}, score:{2}, graphic queue: Index: {3} , Count: {4}, present queue: Index: {5} , Count: {6}", __FUNCTION__, MaxScorePhyDeviceIndex, MaxScore,
			GraphicQueueFamily.QueueFamilyIndex, GraphicQueueFamily.QueueCount,
			PresentQueueFamily.QueueFamilyIndex, PresentQueueFamily.QueueCount);


	}

	void CEVulkanGraphicContext::PrintPhyDeviceInfo(const VkPhysicalDeviceProperties& Props)
	{
		const char *deviceType = Props.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU ? "integrated gpu" :
								 Props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ? "discrete gpu" :
								 Props.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU ? "virtual gpu" :
								 Props.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU ? "cpu" : "others";

		uint32_t driverVersionMajor = VK_VERSION_MAJOR(Props.driverVersion);
		uint32_t driverVersionMinor = VK_VERSION_MINOR(Props.driverVersion);
		uint32_t driverVersionPatch = VK_VERSION_PATCH(Props.driverVersion);

		uint32_t apiVersionMajor = VK_VERSION_MAJOR(Props.apiVersion);
		uint32_t apiVersionMinor = VK_VERSION_MINOR(Props.apiVersion);
		uint32_t apiVersionPatch = VK_VERSION_PATCH(Props.apiVersion);

		LOG_D("-----------------------------");
		LOG_D("deviceName       : {0}", Props.deviceName);
		LOG_D("deviceType       : {0}", deviceType);
		LOG_D("vendorID         : {0}", Props.vendorID);
		LOG_D("deviceID         : {0}", Props.deviceID);
		LOG_D("driverVersion    : {0}.{1}.{2}", driverVersionMajor, driverVersionMinor, driverVersionPatch);
		LOG_D("apiVersion       : {0}.{1}.{2}", apiVersionMajor, apiVersionMinor, apiVersionPatch);
	}

	uint32_t CEVulkanGraphicContext::DeviceScoreEvaluate(const VkPhysicalDeviceProperties& Props)
	{
		VkPhysicalDeviceType deviceType = Props.deviceType;
		uint32_t score = 0;
		switch (deviceType)
		{
			case VK_PHYSICAL_DEVICE_TYPE_OTHER:
				break;
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
				score += 20;
				break;
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
				score += 40;
				break;
			case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
				score += 30;
				break;
			case VK_PHYSICAL_DEVICE_TYPE_CPU:
				score += 10;
				break;
			case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
				break;
		}
		return score;
	}

}