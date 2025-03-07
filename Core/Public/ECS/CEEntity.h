//
// Created by wanghaoyu25 on 25-3-7.
//

#ifndef CEENTITY_H
#define CEENTITY_H

#include "CENode.h"
#include "CEScene.h"

namespace CE {
    class CEEntity : public CENode {
    public:
        CEEntity(const entt::entity &ecsEntity, CEScene* scene) : mEcsEntity(ecsEntity), mScene(scene){}
        ~CEEntity() override = default;

        //Valid Check Func
        static bool IsValid(CEEntity* entity) {
            return entity && entity->IsValid();
        }
        bool IsValid() const { return mScene && mScene->mEcsRegistry.valid(mEcsEntity); }


        //Has Component Check Func
        template<typename T>
        static bool HasComponent(CEEntity *entity){
            return IsValid(entity) && entity->HasComponent<T>();
        }
        template<typename T>
        bool HasComponent() const {
            return mScene->mEcsRegistry.any_of<T>(mEcsEntity);
        }

        //Operator overload
        bool operator==(const CEEntity& other) const{
            return mEcsEntity == other.mEcsEntity && mScene == other.mScene;
        }
        bool operator!=(const CEEntity& other) const{
            return !(*this == other);
        }


        const entt::entity &GetEcsEntity() const { return mEcsEntity; }

        template<typename T, typename... Args>
        T& AddComponent(Args &&...args){
            T &component = mScene->mEcsRegistry.emplace<T>(mEcsEntity, std::forward<Args>(args)...);
            component.SetOwner(this);
            return component;
        }

        template<typename... T>
        bool HasAnyComponent() const {
            return mScene->mEcsRegistry.any_of<T...>(mEcsEntity);
        }

        template<typename... T>
        bool HasAllComponent() const {
            return mScene->mEcsRegistry.all_of<T...>(mEcsEntity);
        }

        template<typename T>
        T& GetComponent() {
            assert(HasComponent<T>() && "Entity does not have component!");
            return mScene->mEcsRegistry.get<T>(mEcsEntity);
        }

        template<typename T>
        void RemoveComponent(){
            assert(HasComponent<T>() && "Entity does not have component!");
            mScene->mEcsRegistry.remove<T>(mEcsEntity);
        }
    private:
        entt::entity mEcsEntity;
        CEScene *mScene;
    };

}

#endif //CEENTITY_H
