#pragma once
#include <btBulletDynamicsCommon.h>
#include <vector>
#include <memory>

class PhysicsEngine {
public:
    PhysicsEngine() {
        collisionConfig = std::make_unique<btDefaultCollisionConfiguration>();
        dispatcher = std::make_unique<btCollisionDispatcher>(collisionConfig.get());
        broadphase = std::make_unique<btDbvtBroadphase>();
        solver = std::make_unique<btSequentialImpulseConstraintSolver>();
        dynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(
            dispatcher.get(), broadphase.get(), solver.get(), collisionConfig.get()
        );
        dynamicsWorld->setGravity(btVector3(0, 0, 0));
    }

    void stepSimulation(float deltaTime) {
        dynamicsWorld->stepSimulation(deltaTime);
    }

    btDiscreteDynamicsWorld* getDynamicsWorld() { return dynamicsWorld.get(); }

private:
    std::unique_ptr<btDefaultCollisionConfiguration> collisionConfig;
    std::unique_ptr<btCollisionDispatcher> dispatcher;
    std::unique_ptr<btDbvtBroadphase> broadphase;
    std::unique_ptr<btSequentialImpulseConstraintSolver> solver;
    std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld;
};