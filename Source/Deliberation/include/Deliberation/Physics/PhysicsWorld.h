#pragma once

#include <memory>

#include <glm/glm.hpp>

#include <Deliberation/Deliberation_API.h>

#include <Deliberation/Core/SparseVector.h>

namespace deliberation
{

class Broadphase;
class Contact;
class Narrowphase;
class RigidBody;

class DELIBERATION_API PhysicsWorld final
{
public:
    PhysicsWorld(float timestep = 1.0f / 60.0f);
    ~PhysicsWorld();

    void addRigidBody(const std::shared_ptr<RigidBody> & body);
    void removeRigidBody(const RigidBody & body);

    void setGravity(float gravity);

    void update(float seconds);

    std::string toString() const;

private:
    void performTimestep(float seconds);
    void integrateTransforms(float seconds);
    void solve();
    void solveVelocities(Contact & contact);
    void solvePositions(Contact & contact);

private:
    float                                       m_timestep              = 1.0f / 60.0f;
    float                                       m_gravity               = 0.0f;
    unsigned int                                m_numVelocityIterations = 6;
    unsigned int                                m_numPositionIterations = 2;
    SparseVector<std::shared_ptr<RigidBody>>    m_rigidBodies;
    std::unique_ptr<Broadphase>                 m_broadphase;
    std::unique_ptr<Narrowphase>                m_narrowphase;
};

}

