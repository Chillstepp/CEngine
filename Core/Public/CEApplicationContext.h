//
// Created by Chillstep on 25-2-19.
//

#ifndef CENGINE_CORE_PUBLIC_CEAPPLICATIONCONTEXT_H_
#define CENGINE_CORE_PUBLIC_CEAPPLICATIONCONTEXT_H_

namespace CE{

	class CEApplication;
	class CEScene;
	class CERenderContext;

	struct CEApplicationContext{
		CEApplication* App;
		CEScene* Scene;
		CERenderContext* RenderContext;
	};
}

#endif //CENGINE_CORE_PUBLIC_CEAPPLICATIONCONTEXT_H_
