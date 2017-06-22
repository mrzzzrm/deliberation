#pragma once

#include <memory>

#include <Deliberation/ECS/ComponentPrototypeBase.h>

namespace deliberation
{

template<typename ComponentT>
class ComponentPrototype:
    public ComponentPrototypeBase
{
public:
    virtual void updateComponent(ComponentT & component) = 0;

    void updateEntity(Entity entity) override;
};

}

#include <Deliberation/ECS/ComponentPrototype.inl>