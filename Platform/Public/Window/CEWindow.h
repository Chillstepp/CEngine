//
// Created by Chillstep on 2025/2/4.
//

#ifndef CENGINE_PLATFORM_PUBLIC_CEWINDOW_H_
#define CENGINE_PLATFORM_PUBLIC_CEWINDOW_H_

#include "CEngine.h"

namespace CE{
	class CEWindow{
	 public:
		CEWindow(const CEWindow&) = delete;
		CEWindow &operator=(const CEWindow&) = delete;
		virtual ~CEWindow() = default;

		static std::unique_ptr<CEWindow> Create(uint32_t Width, uint32_t Height, const char* Title);

		virtual bool ShouldClose() = 0;
		virtual void PollEvents() = 0;
		virtual void SwapBuffer() = 0;

	 protected:
		CEWindow() = default;
	};
}

#endif //CENGINE_PLATFORM_PUBLIC_CEWINDOW_H_
