
#include "CEFileUtil.h"
#include "CEEntryPoint.h"
#include "Render/CERenderTarget.h"
#include "Render/CEMesh.h"
#include "Graphics/CEVulkanRenderPass.h"
#include "Graphics/CEVulkanPipeline.h"
#include "Graphics/CEVulkanCommandPool.h"
#include "Graphics/CEVulkanQueue.h"
#include "Graphics/CEDescriptorPool.h"
#include "Graphics/CEVulkanImageView.h"
#include "Render/CETexture.h"
#include "Render/CERenderer.h"
#include "ECS/System/CEBaseMaterialSystem.h"
#include "ECS/CEEntity.h"


 class SandBoxApp : public CE::CEApplication
 {
  protected:
	 void OnConfiguration(CE::AppSettings *appSettings) override {
		 appSettings->width = 1360;
		 appSettings->height = 768;
		 appSettings->title = "SandBox";
	 }

	 void OnInit() override
	 {
		 CE::CERenderContext* renderContext = CEApplication::GetAppContext()->RenderContext;
		 CE::CEVulkanLogicDevice* device = renderContext->GetDevice();
		 CE::CESwapchain* swapchain = renderContext->GetSwapchain();

		 std::vector<CE::Attachment> attachments =
		 {
			 {
				 .format = swapchain->GetSurfaceInfo().SurfaceFormat.format,
				 .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				 .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
				 .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				 .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				 .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				 .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				 .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
			 },
			 {
				 .format = device->GetSettings().DepthFormat,
				 .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				 .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
				 .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				 .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				 .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				 .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				 .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
			 }
		 };

		 std::vector<CE::RenderSubPass> subpasses = {
			 {
				 .ColorAttachments = { 0 },
				 .DepthStencilAttachments = { 1 },
				 .SampleCount = VK_SAMPLE_COUNT_4_BIT
			 }
		 };
		 mRenderPass = std::make_shared<CE::CEVulkanRenderPass>(device, attachments, subpasses);

		 mRenderTarget = std::make_shared<CE::CERenderTarget>(mRenderPass.get());
		 mRenderTarget->SetColorClearValue({0.1f, 0.2f, 0.3f, 1.f});
		 mRenderTarget->SetDepthStencilClearValue({ 1, 0 });
		 mRenderTarget->AddMaterialSystem<CE::CEBaseMaterialSystem>();

		 mRenderer = std::make_shared<CE::CERenderer>();
		 mCmdBuffers = device->GetDefaultCommandPool()->AllocateCommandBuffer(swapchain->GetImages().size());

		 //Get geometry data, Create CEMesh
		 std::vector<CE::CEVertex> vertices;
		 std::vector<uint32_t> indices;
		 CE::CEGeometryUtil::CreateCube(-0.3f, 0.3f, -0.3f, 0.3f, -0.3f, 0.3f, vertices, indices);
		 mCubeMesh = std::make_shared<CE::CEMesh>(vertices, indices);
	 }

	 void OnSceneInit(CE::CEScene* scene) override
	 {
		 {
			 CE::CEEntity *cube = scene->CreateEntity("Cube 0");
			 auto &materialComp = cube->AddComponent<CE::CEBaseMaterialComponent>();
			 materialComp.colorType = CE::BaseMaterialColor::COLOR_TYPE_TEXCOORD;
			 auto &meshComp = cube->AddComponent<CE::CEMeshComponent>();
			 meshComp.mMesh = mCubeMesh.get();
			 auto &transComp = cube->GetComponent<CE::CETransformComponent>();
			 transComp.scale = { 1.f, 1.f, 1.f };
			 transComp.position = { 0.f, 0.f, 0.0f };
			 transComp.rotation = { 17.f, 30.f, 0.f };
		 }
		 {
			 CE::CEEntity *cube = scene->CreateEntity("Cube 1");
			 auto &materialComp = cube->AddComponent<CE::CEBaseMaterialComponent>();
			 materialComp.colorType = CE::BaseMaterialColor::COLOR_TYPE_NORMAL;
			 auto &meshComp = cube->AddComponent<CE::CEMeshComponent>();
			 meshComp.mMesh = mCubeMesh.get();
			 auto &transComp = cube->GetComponent<CE::CETransformComponent>();
			 transComp.scale = { 0.5f, 0.5f, 0.5f };
			 transComp.position = { -1.f, 0.f, 0.0f };
			 transComp.rotation = { 17.f, -30.f, 0.f };
		 }
		 {
			 CE::CEEntity *cube = scene->CreateEntity("Cube 2");
			 auto &materialComp = cube->AddComponent<CE::CEBaseMaterialComponent>();
			 materialComp.colorType = CE::BaseMaterialColor::COLOR_TYPE_TEXCOORD;
			 auto &meshComp = cube->AddComponent<CE::CEMeshComponent>();
			 meshComp.mMesh = mCubeMesh.get();
			 auto &transComp = cube->GetComponent<CE::CETransformComponent>();
			 transComp.scale = { 0.5f, 0.5f, 0.5f };
			 transComp.position = { 1.f, 0.f, 0.0f };
			 transComp.rotation = { 17.f, 30.f, 0.f };
		 }
	 }

	 void OnSceneDestroy(CE::CEScene* scene) override
	 {
	 }

	 void OnUpdate(float deltaTime) override
	 {
	 }

	 void OnRender() override {
		 CE::CERenderContext *renderCxt = CEApplication::GetAppContext()->RenderContext;
		 CE::CEVulkanLogicDevice *device = renderCxt->GetDevice();
		 CE::CESwapchain *swapchain = renderCxt->GetSwapchain();

		 int32_t imageIndex;
		 if(mRenderer->Begin(&imageIndex)){
			 mRenderTarget->SetExtent({ swapchain->GetWidth(), swapchain->GetHeight() });
		 }

		 VkCommandBuffer cmdBuffer = mCmdBuffers[imageIndex];
		 CE::CEVulkanCommandPool::BeginCommandBuffer(cmdBuffer);

		 mRenderTarget->Begin(cmdBuffer);
		 mRenderTarget->RenderMaterialSystems(cmdBuffer);
		 mRenderTarget->End(cmdBuffer);


		 CE::CEVulkanCommandPool::EndCommandBuffer(cmdBuffer);

		 if(mRenderer->End(imageIndex, { cmdBuffer })){
			 mRenderTarget->SetExtent({ swapchain->GetWidth(), swapchain->GetHeight() });
		 }
	 }

	 void OnDestroy() override {
		 CE::CERenderContext *renderCxt = CE::CEApplication::GetAppContext()->RenderContext;
		 CE::CEVulkanLogicDevice *device = renderCxt->GetDevice();
		 vkDeviceWaitIdle(device->GetHandle());

		 mCubeMesh.reset();
		 mCmdBuffers.clear();
		 mRenderTarget.reset();
		 mRenderPass.reset();
		 mRenderer.reset();

	 }


  private:

		 std::shared_ptr<CE::CEVulkanRenderPass> mRenderPass;
		 std::shared_ptr<CE::CERenderTarget> mRenderTarget;
		 std::shared_ptr<CE::CERenderer> mRenderer;


		 std::vector<VkCommandBuffer> mCmdBuffers;
		 std::shared_ptr<CE::CEMesh> mCubeMesh;


 };


 CE::CEApplication* CreateApplicationEntryPoint(){
	return new SandBoxApp();
}