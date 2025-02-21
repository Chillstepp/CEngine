//
// Created by Chillstep on 25-2-18.
//

#ifndef CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANBUFFER_H_
#define CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANBUFFER_H_

#include "Graphics/CEVulkanCommon.h"

namespace CE{
	class CEVulkanLogicDevice;

	class CEVulkanBuffer{
	 public:
		//bool bHostVisible : Can cpu access or not
		CEVulkanBuffer(CEVulkanLogicDevice *device, VkBufferUsageFlags usage, size_t size, void *data, bool bHostVisible = false);
		~CEVulkanBuffer();
		VkBuffer GetHandle() const { return mHandle; }

		//Create a Buffer
		static void CreateBufferInternal(CEVulkanLogicDevice *device, VkMemoryPropertyFlags memProps, VkBufferUsageFlags usage, size_t size, VkBuffer *outBuffer, VkDeviceMemory *outMemory);
		////send a copy command: Copy src buffer to dst buffer
		static void CopyToBuffer(CEVulkanLogicDevice *device, VkBuffer srcBuffer, VkBuffer dstBuffer, size_t size);

		VkResult WriteData(void *data);
	 private:
		void CreateBuffer(VkBufferUsageFlags usage, void *data);

		VkBuffer mHandle = VK_NULL_HANDLE;
		VkDeviceMemory mMemory = VK_NULL_HANDLE;

		CEVulkanLogicDevice *mDevice;
		size_t mSize;
		bool bHostVisible;
	};

}

#endif //CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEVULKANBUFFER_H_
