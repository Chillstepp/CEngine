//
// Created by Chillstep on 2025/2/4.
//

#ifndef CENGINE_PLATFORM_PUBLIC_CEGLFWWINDOW_H_
#define CENGINE_PLATFORM_PUBLIC_CEGLFWWINDOW_H_

#include "CEWindow.h"
#include "GLFW/glfw3.h"

namespace CE{
	class CEGlfwWindow: public CEWindow{
	 public:
		CEGlfwWindow() = delete;
		CEGlfwWindow(uint32_t Width, uint32_t Height, const char* Title);
		~CEGlfwWindow() override;

		bool ShouldClose() override;
		void PollEvents() override;
		void SwapBuffer() override;

		inline GLFWwindow* GetWindowHandle() const {return GlfwWindow;}
	 private:
		GLFWwindow* GlfwWindow;
	};
}


#endif //CENGINE_PLATFORM_PUBLIC_CEGLFWWINDOW_H_
