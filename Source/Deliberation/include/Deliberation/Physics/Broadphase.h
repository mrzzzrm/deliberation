#pragma once

#include <memory>
#include <vector>

#include <Deliberation/Core/SparseVector.h>

#include <Deliberation/Deliberation_API.h>

namespace deliberation
{

class AABB;
class BroadphaseProxy;
class CollisionObject;
class Narrowphase;
class ProximityCache;
class RigidBody;

class DELIBERATION_API Broadphase
{
public:
    Broadphase(Narrowphase & narrowphase);
    virtual ~Broadphase();

    std::shared_ptr<BroadphaseProxy> createProxy(const CollisionObject & object);
    void removeProxy(const BroadphaseProxy & proxy);

    virtual void setProxyBounds(BroadphaseProxy & proxy, const AABB & bounds) = 0;

    virtual void checkProximities() = 0;

protected:
    Narrowphase &                                   m_narrowphase;
    SparseVector<std::shared_ptr<BroadphaseProxy>>  m_proxies;
};

}
