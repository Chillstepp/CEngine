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

    void CEScene::DestroyEntity(const CEEntity *entity)
	{
		if(entity && entity->IsValid()){
			mEcsRegistry.destroy(entity->GetEcsEntity());
		}

		auto it = mEntities.find(entity->GetEcsEntity());
		if(it != mEntities.end()){
			CENode *parent = it->second->GetParent();
			if(parent){
				parent->RemoveChild(it->second.get());
			}
			mEntities.erase(it);
		}
    }

    void CEScene::DestroyAllEntity()
	{
		mEcsRegistry.clear();
		mEntities.clear();
    }

    CEEntity * CEScene::GetEntity(entt::entity enttEntity) {
		if(mEntities.find(enttEntity) != mEntities.end()){
			return mEntities.at(enttEntity).get();
		}
		return nullptr;
    }
}
