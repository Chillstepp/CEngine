//
// Created by Chillstep on 2025/2/3.
//

#ifndef CENGINE_PLATFORM_PUBLIC_CENGINE_H_
#define CENGINE_PLATFORM_PUBLIC_CENGINE_H_

#include <iostream>
#include <cassert>
#include <memory>
#include <algorithm>
#include <functional>

#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <stack>
#include <queue>
#include <deque>
#include <set>
#include <unordered_map>
#include <unordered_set>

#ifdef CE_ENGINE_PLATFORM_WIN32
//Windows
#define VK_USE_PLATFORM_WIN32_KHR
#elif CE_ENGINE_PLATFORM_MACOS
//Macos
#define VK_USE_PLATFORM_MACOS_MKV
#elif CCE_ENGINE_PLATFORM_LINUX
//Linux
#define VK_USE_PLATFORM_XCB_KHR

#else
	#error Unsupport this platform
#endif

#define CE_ENGINE_GRAPHIC_API_VULKAN

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof((arr)[0]))

#endif //CENGINE_PLATFORM_PUBLIC_CENGINE_H_
