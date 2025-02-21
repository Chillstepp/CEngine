//
// Created by Chillstep on 25-2-17.
//

#ifndef CENGINE_PLATFORM_PUBLIC_CEGEOMETRYUTIL_H_
#define CENGINE_PLATFORM_PUBLIC_CEGEOMETRYUTIL_H_

#include "Graphics/CEGraphicContext.h"

namespace CE{
	struct CEVertex{
		glm::vec3 position;
		glm::vec2 texcoord0;
		glm::vec3 normal;
	};

	class CEGeometryUtil{
	 public:
			/**
			 * Create cube
			 * @param leftPlane   left
			 * @param rightPlane  right
			 * @param bottomPlane bottom
			 * @param topPlane    top
			 * @param nearPlane   near
			 * @param farPlane    far
			 * @param vertices    out vertices
			 * @param indices     out indices
			 * @param bUseTextcoords
			 * @param bUseNormals
			 */
		static void CreateCube(float leftPlane, float rightPlane, float bottomPlane, float topPlane, float nearPlane, float farPlane,
			std::vector<CE::CEVertex> &vertices, std::vector<uint32_t> &indices, const bool bUseTextcoords = true,
			const bool bUseNormals = true, const glm::mat4 &relativeMat = glm::mat4(1.0f));
	};
}

#endif //CENGINE_PLATFORM_PUBLIC_CEGEOMETRYUTIL_H_
