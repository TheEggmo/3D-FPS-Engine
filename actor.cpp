#include "actor.h"

//void Actor::process(std::vector<Tools3D::AABB> colliders){
//    // Order can be rearranged if necessary
//    if(logicEnabled) processLogic();
//    if(physicsEnabled) processPhysics();
//    if(collisionEnabled) processCollision(colliders);
//}


void Actor::processCollision(std::vector<Tools3D::AABB> colliders){
    collision.updatePosition(this->position);
    for(auto aabb : colliders){
        if(collision.intersects(aabb)){
//            qDebug("COLLISION DETECTED");
//            qDebug("%s COLLIDED", this->name);
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


void ActorDynamic::processLogic(){

}

void ActorDynamic::processPhysics()
{

}

void ActorDynamic::processCollision(std::vector<Tools3D::AABB> colliders){
    float gravity = 0.1f;
    velocity.y -= gravity;

    for(auto aabb : colliders){
        if(collision.intersects(aabb) && collisionEnabled){
            velocity = {0,0,0};
        }
//        if(collision.intersectsX(aabb)){
////            velocity.x = 0;
//            velocity.x = -velocity.x;
//        }
//        if(collision.intersectsY(aabb)){
////            velocity.y = 0;
//            velocity.y = -velocity.y;
//        }
//        if(collision.intersectsZ(aabb)){
////            velocity.z = 0;
//            velocity.z = -velocity.z;
//        }
    }

    position += velocity;
    collision.updatePosition(this->position);
}
