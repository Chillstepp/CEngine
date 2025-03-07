//
// Created by wanghaoyu25 on 25-3-7.
//

#ifndef CENODE_H
#define CENODE_H

#include "CEUUID.h"

namespace CE {
    class CENode {
    public:
        CENode() = default;
        virtual ~CENode() = default;

        CEUUID GetId() const;
        void SetId(const CEUUID& uuid);
        const std::string& GetName() const;
        void SetName(const std::string& name);

        bool HasParent();
        bool HasChildren();
        void SetParent(CENode* node);
        CENode* GetParent();
        void AddChild(CENode* node);
        void RemoveChild(CENode* node);

    private:
        CEUUID mUUID;
        std::string mName;
        CENode* mParent = nullptr;
        std::vector<CENode*> mChildren;

    };
}

#endif //CENODE_H
