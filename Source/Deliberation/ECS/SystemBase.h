#pragma once

#include <unordered_map>
#include <unordered_set>

#include <Deliberation/Core/SparseVector.h>

#include <Deliberation/ECS/ComponentFilter.h>

namespace deliberation
{
class EventListener;
class World;

class SystemBase
{
  public:
    SystemBase(World & world, const ComponentFilter & filter);
    virtual ~SystemBase();

    World &                 world();
    const ComponentFilter & filter() const;

    virtual std::size_t index() const = 0;
    virtual std::string name() const = 0;

    bool accepts(const Entity & entity);

    void addEntity(Entity & entity);
    void removeEntity(Entity & entity);

    void frameBegin();
    void beforeUpdate();
    void update(float seconds);
    void prePhysicsUpdate(float seconds);
    void postPhysicsUpdate(float seconds);
    void frameComplete();

    virtual void onCreated() {}
    virtual void onRemoved() {}

  protected:
    virtual void onEntityAdded(Entity & entity);
    virtual void onEntityRemoved(Entity & entity);
    virtual void onEntityUpdate(Entity & entity, float seconds);
    virtual void
                 onEntityPrePhysicsUpdate(Entity & entity, float physicsTimestep);
    virtual void
                 onEntityPostPhysicsUpdate(Entity & entity, float physicsTimestep) {}
    virtual void onFrameBegin();
    virtual void onUpdate(float seconds);
    virtual void onPrePhysicsUpdate(float seconds);
    virtual void onPostPhysicsUpdate(float seconds) {}
    virtual void onFrameComplete();

protected:
    struct EntityEntry
    {
        EntityId id;
        bool     active;
    };

protected:
    World &                                     m_world;
    SparseVector<EntityEntry>                   m_entities;
    std::vector<std::shared_ptr<EventListener>> m_eventListeners;


  private:
    ComponentFilter                             m_filter;
    std::unordered_map<EntityId, std::size_t>   m_entityIndexByID;
};
};
