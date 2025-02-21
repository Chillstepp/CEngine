//
// Created by Chillstep on 25-2-19.
//

#include "Render/CEMesh.h"
#include "Render/CERenderContext.h"
#include "CEApplication.h"

namespace CE{
	CEMesh::CEMesh(const std::vector<CE::CEVertex>& vertices, const std::vector<uint32_t>& indices)
	{
		if(vertices.empty()) return;
		CE::CERenderContext* renderContext = CEApplication::GetAppContext()->RenderContext;
		CE::CEVulkanLogicDevice* device = renderContext->GetDevice();

		mVertexCount = vertices.size();
		mIndexCount = indices.size();
		mVertexBuffer = std::make_shared<CE::CEVulkanBuffer>(device, VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, sizeof(vertices[0]) * vertices.size(), (void*)vertices.data());
		if(mIndexCount > 0) {
			mIndicesBuffer = std::make_shared<CE::CEVulkanBuffer>(device,VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT,sizeof(indices[0]) * indices.size(),(void*) indices.data());
		}
	}

	CEMesh::~CEMesh()
	{

	}

	void CEMesh::Draw(VkCommandBuffer cmdBuffer)
	{
		VkBuffer vertexBuffer[] = { mVertexBuffer->GetHandle() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffer, offsets);

		if(mIndexCount > 0)
		{
			vkCmdBindIndexBuffer(cmdBuffer, mIndicesBuffer->GetHandle(), 0, VkIndexType::VK_INDEX_TYPE_UINT32);
			vkCmdDrawIndexed(cmdBuffer, mIndexCount, 1, 0, 0, 0);
		}
		else
		{
			vkCmdDraw(cmdBuffer, mVertexCount, 1, 0, 0);
		}
	}
}