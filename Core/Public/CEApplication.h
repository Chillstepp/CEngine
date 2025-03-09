//
// Created by Chillstep on 2025/2/3.
//

#ifndef CENGINE_CORE_PUBLIC_CEAPPLICATION_H_
#define CENGINE_CORE_PUBLIC_CEAPPLICATION_H_

#include "Window/CEWindow.h"
#include "CEApplicationContext.h"

namespace CE{
	struct AppSettings{
		uint32_t width = 800;
		uint32_t height = 600;
		const char* title = "CEngine";
	};

	class CEApplication{
	 public:
		static CEApplicationContext *GetAppContext() {return &sAppContext;}

		void Start(int argc, char *argv[]);
		void Stop();
		void MainLoop();

		bool IsPause() const { return bPause; }
		void Pause() { bPause = true; }
		void Resume() { if(bPause) bPause = false; }

		static CEApplicationContext sAppContext;

	 protected:
		virtual void OnConfiguration(AppSettings *appSettings){}
		virtual void OnInit(){}
		virtual void OnUpdate(float deltaTime){}
		virtual void OnRender(){}
		virtual void OnDestroy(){}

		virtual void OnSceneInit(CEScene* scene) {}
		virtual void OnSceneDestroy(CEScene* scene) {}

		std::chrono::steady_clock::time_point mStartTimePoint;
		std::chrono::steady_clock::time_point mLastTimePoint;
		std::shared_ptr<CERenderContext> mRenderContext;

	 private:
		void ParseArgs(int argc, char *argv[]);
		bool LoadScene(const std::string& filePath = "");
		void UnLoadScene();

		std::unique_ptr<CEWindow> mWindow;
		std::unique_ptr<CEScene> mScene;
		AppSettings mAppSettings;

		uint64_t mFrameIndex = 0;
		bool bPause = false;
	};
}

#endif //CENGINE_CORE_PUBLIC_CEAPPLICATION_H_
