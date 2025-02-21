//
// Created by Chillstep on 2025/2/4.
//

#include "Window/CEGlfwWindow.h"
#include "GLFW/glfw3native.h"
#include "CELog.h"

namespace CE{

	CEGlfwWindow::CEGlfwWindow(uint32_t Width, uint32_t Height, const char* Title)
	{
		if(!glfwInit())
		{
			LOG_E("Failed to init glfw.");
			return;
		}
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
		GlfwWindow = glfwCreateWindow(Width, Height, Title, nullptr, nullptr);
		if(!GlfwWindow)
		{
			LOG_E("Failed to create glfw window.");
			return;
		}

		//居中
		GLFWmonitor *primaryMonitor = glfwGetPrimaryMonitor();
		if(primaryMonitor){
			int xPos, yPos, workWidth, workHeight;
			glfwGetMonitorWorkarea(primaryMonitor, &xPos, &yPos, &workWidth, &workHeight);
			glfwSetWindowPos(GlfwWindow, workWidth / 2 - Width / 2, workHeight / 2 - Height / 2);
		}

		glfwMakeContextCurrent(GlfwWindow);

		glfwShowWindow(GlfwWindow);
	}

	CEGlfwWindow::~CEGlfwWindow()
	{
		assert(GlfwWindow);
		glfwDestroyWindow(GlfwWindow);
		glfwTerminate();
		LOG_I("The Window ends.");
	}
	bool CEGlfwWindow::ShouldClose()
	{
		return glfwWindowShouldClose(GlfwWindow);
	}
	void CEGlfwWindow::PollEvents()
	{
		glfwPollEvents();
	}
	void CEGlfwWindow::SwapBuffer()
	{
		glfwSwapBuffers(GlfwWindow);
	}
}