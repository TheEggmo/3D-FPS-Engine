#ifndef ACTOR_H
#define ACTOR_H

#include "tools3d.h"
#include <inputmap.h>

#include <string>
#include <QString>

using T3 = Tools3D;

// Base class. Doesn't have any logic other than colliding and preventing movement of ActorDynamic
class Actor{
protected:
    T3::AABB collision;
    T3::MeshTexture model;

public:
    virtual ~Actor() {}

    std::string name = "";
    T3::Vector3 position;
    T3::Vector3 velocity;
    float gravity = 0.1f;


    bool visible = false; // If true, the model mesh will be drawn
    bool physicsEnabled = false; // If true, the body will have physics applied every frame (gravity etc.)
    bool logicEnabled = false; // If true, the body will process its logic(pathfinding, movement etc.)
    bool collisionEnabled = false; // If true, the body will have collisions



//    virtual void processPhysics();
    virtual void processLogic() {}
    virtual void processCollision(std::vector<Tools3D::AABB> colliders) {}

    void setCollision(Tools3D::AABB);
    void setModel(Tools3D::MeshTexture);

    T3::AABB getCollider() { return collision; }
    T3::MeshTexture getModel() { return model; }
};

class ActorStatic : public Actor{
public:
    ActorStatic(){
        gravity = 0;
    }

//    void processLogic() override {}
//    void processCollision(std::vector<Tools3D::AABB> colliders) override;
};

class ActorPlayer : public Actor{
private:
    InputMap *Input; // Access MainWindow's inputmap for movement inputs
public:
//    ActorPlayer(InputMap *iMap){
//        Input = iMap;
//        logicEnabled = true;
//    }

    void processLogic() override;
    void processCollision(std::vector<Tools3D::AABB> colliders) override;
};

class ActorEnemy : public Actor{
public:
    ActorEnemy(){
        logicEnabled = true;
    }

    void processLogic() override;
    void processCollision(std::vector<Tools3D::AABB> colliders) override;
};

#endif // ACTOR_H
