//
// Created by Chillstep on 2025/2/4.
//

#include "Window/CEWindow.h"
#include "Window/CEGlfwWindow.h"


namespace CE
{

	std::unique_ptr<CEWindow> CEWindow::Create(uint32_t Width, uint32_t Height, const char* Title)
	{
#ifdef CE_ENGINE_PLATFORM_WIN32
//Windows
		return std::make_unique<CEGlfwWindow>(Width, Height, Title);
#elif CE_ENGINE_PLATFORM_MACOS
//Macos
		return std::make_unique<CEGlfwWindow>(Width, Height, Title)
#elif CE_ENGINE_PLATFORM_LINUX
		return std::make_unique<CEGlfwWindow>(Width, Height, Title)
#else
	#error Unsupport this platform
#endif
	}

}