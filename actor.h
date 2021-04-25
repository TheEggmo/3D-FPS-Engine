#ifndef ACTOR_H
#define ACTOR_H

#include "tools3d.h"
#include <string>
#include <QString>

using T3 = Tools3D;

// Base class. Doesn't have any logic other than colliding and preventing movement of ActorDynamic
class Actor{
protected:
    T3::AABB collision;
    T3::MeshTexture model;

    bool DYNAMIC = false;
public:
    T3::Vector3 position;
    std::string name = "";
//    QString name = "";

    bool visible = false; // If true, the model mesh will be drawn
//    bool physicsEnabled = false; // If true, the body will have physics applied every frame (gravity etc.)
//    bool logicEnabled = false; // If true, the body will process its logic(pathfinding, movement etc.)
    bool collisionEnabled = false; // If true, the body will have collisions

//    void process(std::vector<Tools3D::AABB> colliders);

//    void processPhysics();
//    void processLogic();
    virtual void processCollision(std::vector<Tools3D::AABB> colliders);
    virtual void processCollision(Tools3D::AABB aabb);

    void setCollision(Tools3D::AABB);
    void setModel(Tools3D::MeshTexture);

    T3::AABB getCollider() { return collision; }
    T3::MeshTexture getModel() { return model; }
    bool isDynamic() { return DYNAMIC; }
//    QImage* getTexture() { return model.texture; }

};

// Used for actors that move, like the player or enemies
class ActorDynamic : public Actor{
public:
    ActorDynamic(){
        DYNAMIC = true;
    }
    T3::Vector3 velocity = {0, 0, 0};

    bool logicEnabled = false; // If true, the body will process its logic(pathfinding, movement etc.)

    void processLogic();
    void processPhysics();
    void processCollision(std::vector<Tools3D::AABB> colliders) override;
};

#endif // ACTOR_H
