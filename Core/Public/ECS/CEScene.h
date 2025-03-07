//
// Created by wanghaoyu25 on 25-3-7.
//

#ifndef CESCENE_H
#define CESCENE_H

#include <entt/entity/registry.hpp>
#include "CEUUID.h"

namespace CE {
    class CENode;
    class CEEntity;

    class CEScene {
    public:
        CEScene();
        ~CEScene();

        CEEntity* CreateEntity(const std::string &name = "");
        CEEntity* CreateEntityWithUUID(const CEUUID &id, const std::string &name = "");
        void DestroyEntity(const CEEntity *entity);
        void DestroyAllEntity();

        entt::registry &GetEcsRegistry() { return mEcsRegistry; }
        CENode *GetRootNode() const { return mRootNode.get(); }
        CEEntity *GetEntity(entt::entity enttEntity);

    private:
        std::string mName;
        entt::registry mEcsRegistry;

        std::unordered_map<entt::entity, std::shared_ptr<CEEntity>> mEntities;
        std::shared_ptr<CENode> mRootNode;

        friend class CEEntity;
    };
}


#endif //CESCENE_H
