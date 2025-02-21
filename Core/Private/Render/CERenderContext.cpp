//
// Created by Chillstep on 25-2-19.
//

#include "Render/CERenderContext.h"

namespace CE{
	CERenderContext::CERenderContext(CEWindow* window)
	{
		mGraphicContext = CE::CEGraphicContext::Create(window);
		CEVulkanGraphicContext* vkContext = dynamic_cast<CEVulkanGraphicContext*>(mGraphicContext.get());
		mDevice = std::make_shared<CE::CEVulkanLogicDevice>(vkContext, 1, 1);
		mSwapchain = std::make_shared<CE::CESwapchain>(vkContext, mDevice.get());
	}

	CERenderContext::~CERenderContext()
	{

	}
}