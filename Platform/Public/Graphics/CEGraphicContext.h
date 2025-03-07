//
// Created by Chillstep on 2025/2/5.
//

#ifndef CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEGRAPHICCONTEXT_H_
#define CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEGRAPHICCONTEXT_H_

#include "CEngine.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace CE{
	class CEWindow;

	class CEGraphicContext{
	 public:
		CEGraphicContext(const CEGraphicContext&) = delete;
		CEGraphicContext &operator=(const CEGraphicContext&) = delete;
		virtual ~CEGraphicContext() = default;

		static std::unique_ptr<CEGraphicContext> Create(CEWindow* Window);
	 protected:
		CEGraphicContext() = default;
	};
}

#endif //CENGINE_PLATFORM_PUBLIC_GRAPHICS_CEGRAPHICCONTEXT_H_
