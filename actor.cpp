#include "actor.h"

void Actor::process(std::vector<Tools3D::AABB> colliders){
    // Order can be rearranged if necessary
    if(logicEnabled) processLogic();
    if(physicsEnabled) processPhysics();
    if(collisionEnabled) processCollision(colliders);
}

void Actor::processPhysics(){

}
void Actor::processLogic(){

}
void Actor::processCollision(std::vector<Tools3D::AABB> colliders){
    collision.updatePosition(this->position);
    for(auto aabb : colliders){
        if(collision.intersects(aabb)){
//            qDebug("COLLISION DETECTED");
            qDebug("%s COLLIDED", this->name);
        }
    }
}

void Actor::processCollision(Tools3D::AABB aabb){
    collision.updatePosition(position);
    if(collision.intersects(aabb)){
        qDebug("COLLISION DETECTED");
    }
}

void Actor::setCollision(Tools3D::AABB in){
    collision = in;
    collisionEnabled = true;
}
void Actor::setModel(Tools3D::MeshTexture in){
    model = in;
    visible = true;
}

