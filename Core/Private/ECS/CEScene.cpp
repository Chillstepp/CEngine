//
// Created by wanghaoyu25 on 25-3-7.
//

#include "ECS/CEScene.h"
#include "ECS/CEEntity.h"
#include "ECS/CENode.h"
#include "ECS/Component/CETransformComponent.h"

namespace CE {
    CEScene::CEScene() {
        mRootNode = std::make_shared<CENode>();
    }

    CEScene::~CEScene() {
        mRootNode.reset();
        DestroyAllEntity();
        mEntities.clear();
    }

    CEEntity * CEScene::CreateEntity(const std::string &name) {
        return CreateEntityWithUUID(CEUUID(), name);
    }

    CEEntity * CEScene::CreateEntityWithUUID(const CEUUID &id, const std::string &name) {
        auto enttEntity = mEcsRegistry.create();
        mEntities.insert({ enttEntity, std::make_shared<CEEntity>(enttEntity, this) });
        mEntities[enttEntity]->SetParent(mRootNode.get());
        mEntities[enttEntity]->SetId(id);
        mEntities[enttEntity]->SetName(name.empty() ? "Entity" : name);

        // add default components
        mEntities[enttEntity]->AddComponent<CETransformComponent>();

        return mEntities[enttEntity].get();
    }

    void CEScene::DestroyEntity(const CEEntity *entity) {
    }

    void CEScene::DestroyAllEntity() {
    }

    CEEntity * CEScene::GetEntity(entt::entity enttEntity) {
    }
}
