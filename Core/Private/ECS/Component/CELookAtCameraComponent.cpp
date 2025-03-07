//
// Created by wanghaoyu25 on 25-3-7.
//
#include "ECS/Component/CELookAtCameraComponent.h"
#include "ECS/Component/CETransformComponent.h"

namespace CE{
    const glm::mat4 &AdLookAtCameraComponent::GetProjMat() {
        mProjMat = glm::perspective(glm::radians(mFov), mAspect, mNearPlane, mFarPlane);
        return mProjMat;
    }

    const glm::mat4 &AdLookAtCameraComponent::GetViewMat() {
        CEEntity *owner = GetOwner();
        if(CEEntity::HasComponent<CETransformComponent>(owner)){
            auto &transComp = owner->GetComponent<CETransformComponent>();
            float yaw = transComp.rotation.x;
            float pitch = transComp.rotation.y;

            glm::vec3 direction;
            direction.x = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
            direction.y = sin(glm::radians(pitch));
            direction.z = cos(glm::radians(pitch)) * cos(glm::radians(yaw));

            transComp.position = mTarget + direction * mRadius;

            mViewMat = glm::lookAt(transComp.position, mTarget, mWorldUp);
        }
        return mViewMat;
    }

    void AdLookAtCameraComponent::SetViewMat(const glm::mat4 &viewMat) {
        // TODO
    }

}