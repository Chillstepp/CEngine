//
// Created by Chillstep on 25-2-19.
//

#ifndef CENGINE_CORE_PUBLIC_RENDER_CEMESH_H_
#define CENGINE_CORE_PUBLIC_RENDER_CEMESH_H_

#include "Graphics/CEVulkanBuffer.h"
#include "CEGeometryUtil.h"

namespace CE{
	class CEMesh{
	 public:
		CEMesh(const std::vector<CE::CEVertex> &vertices, const std::vector<uint32_t>& indices = {});
		~CEMesh();

		void Draw(VkCommandBuffer cmdBuffer);

	 private:
		std::shared_ptr<CEVulkanBuffer> mVertexBuffer;
		std::shared_ptr<CEVulkanBuffer> mIndicesBuffer;
		uint32_t mVertexCount;
		uint32_t mIndexCount;
	};
}

#endif //CENGINE_CORE_PUBLIC_RENDER_CEMESH_H_
