//
// Created by Chillstep on 2025/2/3.
//
#include "CEApplication.h"
#include "Render/CERenderContext.h"
#include "CELog.h"
#include "ECS/CEEntity.h"

namespace CE{
	CEApplicationContext CEApplication::sAppContext{};

	void CEApplication::Start(int argc, char** argv)
	{
		CELog::Init();

		ParseArgs(argc, argv);
		OnConfiguration(&mAppSettings);

		mWindow = CEWindow::Create(mAppSettings.width, mAppSettings.height, mAppSettings.title);
		mRenderContext = std::make_shared<CERenderContext>(mWindow.get());

		sAppContext.App = this;
		sAppContext.RenderContext = mRenderContext.get();

		OnInit();
		LoadScene();

		mStartTimePoint = std::chrono::steady_clock::now();
	}

	void CEApplication::Stop()
	{
		UnLoadScene();
		OnDestroy();
	}

	bool CEApplication::LoadScene(const std::string& filePath)
	{
		if(mScene) UnLoadScene();
		mScene = std::make_unique<CEScene>();
		OnSceneInit(mScene.get());
		sAppContext.Scene = mScene.get();
		return true;
	}

	void CEApplication::UnLoadScene()
	{
		if(mScene)
		{
			OnSceneDestroy(mScene.get());
			mScene.reset();
			sAppContext.Scene = nullptr;
		}
	}

	void CEApplication::MainLoop()
	{
		mLastTimePoint = std::chrono::steady_clock::now();
		while (!mWindow->ShouldClose()) {
			mWindow->PollEvents();

			float deltaTime = std::chrono::duration<float>(std::chrono::steady_clock::now() - mLastTimePoint).count();
			mLastTimePoint = std::chrono::steady_clock::now();
			mFrameIndex++;

			if(!bPause){
				OnUpdate(deltaTime);
			}
			OnRender();

			mWindow->SwapBuffer();
		}
	}

	void CEApplication::ParseArgs(int argc, char** argv)
	{

	}
}