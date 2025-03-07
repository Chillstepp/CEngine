//
// Created by wanghaoyu25 on 25-3-7.
//
#include "ECS/CENode.h"

namespace CE {
    CEUUID CENode::GetId() const {
        return mUUID;
    }

    void CENode::SetId(const CEUUID &uuid) {
        mUUID = uuid;
    }

    const std::string & CENode::GetName() const {
        return mName;
    }

    void CENode::SetName(const std::string &name) {
        mName = name;
    }

    bool CENode::HasParent() {
        return mParent != nullptr;
    }

    bool CENode::HasChildren() {
        return !mChildren.empty();
    }

    void CENode::SetParent(CENode *node) {
        node->AddChild(node);
    }

    CENode * CENode::GetParent() {
        return mParent;
    }

    void CENode::AddChild(CENode *node) {
        if(node->HasParent()){
            node->GetParent()->RemoveChild(node);
        }
        node->mParent = this;
        mChildren.push_back(node);
    }

    void CENode::RemoveChild(CENode *node) {
        if(!HasChildren()) return;
        mChildren.erase(
            std::ranges::find_if(mChildren,
                                 [&](const auto& child) {
                                     return node == child;
                                 }
            ));
    }


}
