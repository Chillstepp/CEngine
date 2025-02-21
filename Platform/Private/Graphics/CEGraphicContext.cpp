//
// Created by Chillstep on 2025/2/5.
//

#include "Graphics/CEGraphicContext.h"
#include "Graphics/CEVulkanGraphicContext.h"


namespace CE{

	std::unique_ptr<CEGraphicContext> CEGraphicContext::Create(CEWindow* Window)
	{
#ifdef CE_ENGINE_GRAPHIC_API_VULKAN
		return std::make_unique<CEVulkanGraphicContext>(Window);
#endif
	}
}