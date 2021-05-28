#ifndef ACTOR_H
#define ACTOR_H

#include "tools3d.h"
#include <inputmap.h>

#include <string>
#include <QString>

using T3 = Tools3D;

enum ActorType{Base, Static, Player, Enemy, Light};

// Base class
class Actor{
protected:
    ActorType type = Base;

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
    virtual void processLogic() =0;
    virtual void processCollision(std::vector<Tools3D::AABB> colliders) =0;

    void setCollision(Tools3D::AABB);
    void setModel(Tools3D::MeshTexture);
    void setTexture(const QString &filepath);

    T3::AABB getCollider() { return collision; }
    T3::MeshTexture getModel() { return model; }
    ActorType getType() { return type; }
};

class ActorStatic : public Actor{
private:
//    ActorType type = Static;
public:
    ActorStatic(){
        type = Static;
        gravity = 0;
    }

    void processLogic() override {}
    void processCollision(std::vector<Tools3D::AABB> colliders) override {}
};

class ActorPlayer : public Actor{
private:
//    ActorType type = Player;

    InputMap *Input; // Access MainWindow's inputmap for movement inputs
    T3::Vector3 *lookDir; // Access MainWIndow's camera direction
public:
    ActorPlayer() {
        type = Player;
    }
    ActorPlayer(InputMap *iMap, T3::Vector3 *vLook){ // vLook is the camera direction
        type = Player;
        Input = iMap;
        lookDir = vLook;
        logicEnabled = true;
    }

    void processLogic() override;
    void processCollision(std::vector<Tools3D::AABB> colliders) override;
};

class ActorEnemy : public Actor{
private:
//    ActorType type = Enemy;
public:
    ActorEnemy(){
        type = Enemy;
        logicEnabled = true;
    }

    void processLogic() override;
    void processCollision(std::vector<Tools3D::AABB> colliders) override;
};

class ActorLight : public Actor{
public:
    ActorLight(){
        type = Light;
    }
    ActorLight(Tools3D::Vector3 pos){
        type = Light;
        position = pos;
    }

    void processLogic() override {}
    void processCollision(std::vector<Tools3D::AABB> colliders) override {}
};

#endif // ACTOR_H
