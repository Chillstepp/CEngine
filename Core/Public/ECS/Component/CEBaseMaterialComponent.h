//
// Created by wanghaoyu25 on 25-3-7.
//

#ifndef CEBASEMATERIALCOMPONENT_H
#define CEBASEMATERIALCOMPONENT_H

#include "ECS/CEComponent.h"

namespace CE{
    enum class BaseMaterialColor{
        COLOR_TYPE_NORMAL = 0,
        COLOR_TYPE_TEXCOORD = 1
    };

    struct CEBaseMaterialComponent : public CEComponent{
        BaseMaterialColor colorType;
    };
}

#endif //CEBASEMATERIALCOMPONENT_H
