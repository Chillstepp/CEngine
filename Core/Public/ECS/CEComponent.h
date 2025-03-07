//
// Created by wanghaoyu25 on 25-3-7.
//

#ifndef CECOMPONENT_H
#define CECOMPONENT_H

#include "CEEntity.h"

namespace CE {
    class CEComponent {
    public:
        void SetOwner(CEEntity* owner) { mOwner = owner; }
        CEEntity* GetOwner() const { return mOwner; }
    private:
        CEEntity* mOwner = nullptr;
    };
}

#endif //CECOMPONENT_H
