#pragma once
#include <btBulletDynamicsCommon.h>
#include <memory>

class PhysicsEngine {
public:
    PhysicsEngine();
    ~PhysicsEngine() = default;
    void stepSimulation(float deltaTime);
    btDiscreteDynamicsWorld* getDynamicsWorld();

private:
    std::unique_ptr<btDefaultCollisionConfiguration> m_pCollisionConfig;
    std::unique_ptr<btCollisionDispatcher> m_pDispatcher;
    std::unique_ptr<btDbvtBroadphase> m_pBroadphase;
    std::unique_ptr<btSequentialImpulseConstraintSolver> m_pSolver;
    std::unique_ptr<btDiscreteDynamicsWorld> m_pDynamicsWorld;
};