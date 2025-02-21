//
// Created by Chillstep on 25-2-18.
//

#ifndef CENGINE_CORE_PUBLIC_CEENTRYPOINT_H_
#define CENGINE_CORE_PUBLIC_CEENTRYPOINT_H_

#include "CEngine.h"
#include "CEApplication.h"

extern CE::CEApplication* CreateApplicationEntryPoint();

int main(int argc, char *argv[]){

	std::cout<< "Engine starting..." << std::endl;

	CE::CEApplication* app = CreateApplicationEntryPoint();
	// start
	app->Start(argc, argv);
	// main loop
	app->MainLoop();
	// stop
	app->Stop();
	// delete

	delete app;

	return EXIT_SUCCESS;
}


#endif //CENGINE_CORE_PUBLIC_CEENTRYPOINT_H_
