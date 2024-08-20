#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine() {
    m_pCollisionConfig = std::make_unique<btDefaultCollisionConfiguration>();
    m_pDispatcher = std::make_unique<btCollisionDispatcher>(m_pCollisionConfig.get());
    m_pBroadphase = std::make_unique<btDbvtBroadphase>();
    m_pSolver = std::make_unique<btSequentialImpulseConstraintSolver>();
    m_pDynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(
        m_pDispatcher.get(), m_pBroadphase.get(), m_pSolver.get(), m_pCollisionConfig.get()
    );
    m_pDynamicsWorld->setGravity(btVector3(0, -9.81f, 0));
}

void PhysicsEngine::stepSimulation(float deltaTime) {
    m_pDynamicsWorld->stepSimulation(deltaTime);
}

btDiscreteDynamicsWorld* PhysicsEngine::getDynamicsWorld() {
    return m_pDynamicsWorld.get();
}