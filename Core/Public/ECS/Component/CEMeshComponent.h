//
// Created by wanghaoyu25 on 25-3-7.
//

#ifndef CEMESHCOMPONENT_H
#define CEMESHCOMPONENT_H

#include "ECS/CEComponent.h"
#include "Render/CEMesh.h"


namespace CE{
    struct CEMeshComponent : public CEComponent{

      CEMesh* mesh = nullptr;

    };
}

#endif //CEMESHCOMPONENT_H
