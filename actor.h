#ifndef ACTOR_H
#define ACTOR_H

#include "tools3d.h"
#include <string>
#include <QString>

class Actor{
protected:
    Tools3D::AABB collision;
    Tools3D::MeshTexture model;

public:
    Tools3D::Vector3 position;
//    std::string name = "";
    QString name = "";

    bool visible = false; // If true, the model mesh will be drawn
    bool physicsEnabled = false; // If true, the body will have physics applied every frame (gravity etc.)
    bool logicEnabled = false; // If true, the body will process its logic(pathfinding, movement etc.)
    bool collisionEnabled = false; // If true, the body will have collisions

    void process(std::vector<Tools3D::AABB> colliders);

    void processPhysics();
    void processLogic();
    void processCollision(std::vector<Tools3D::AABB> colliders);
    void processCollision(Tools3D::AABB aabb);

    void setCollision(Tools3D::AABB);
    void setModel(Tools3D::MeshTexture);

    Tools3D::AABB getCollider() { return collision; }
    Tools3D::MeshTexture getModel() { return model; }

};

#endif // ACTOR_H
