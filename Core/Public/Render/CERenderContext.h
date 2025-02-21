//
// Created by Chillstep on 25-2-19.
//

#ifndef CENGINE_CORE_PUBLIC_RENDER_CERENDERCONTEXT_H_
#define CENGINE_CORE_PUBLIC_RENDER_CERENDERCONTEXT_H_

#include "Graphics/CEVulkanGraphicContext.h"
#include "Graphics/CEVulkanSwapchain.h"
#include "Graphics/CEVulkanLogicDevice.h"

namespace CE{
	class CEWindow;

	class CERenderContext{
	 public:
		CERenderContext(CEWindow *window);
		~CERenderContext();

		CEGraphicContext *GetGraphicContext() const { return mGraphicContext.get(); }
		CEVulkanLogicDevice *GetDevice() const { return mDevice.get(); }
		CESwapchain *GetSwapchain() const { return mSwapchain.get(); }
	 private:
		std::shared_ptr<CEGraphicContext> mGraphicContext;
		std::shared_ptr<CEVulkanLogicDevice> mDevice;
		std::shared_ptr<CESwapchain> mSwapchain;
	};
}

#endif //CENGINE_CORE_PUBLIC_RENDER_CERENDERCONTEXT_H_
