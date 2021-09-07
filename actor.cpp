#include "actor.h"

void Actor::setCollision(Tools3D::AABB in){
    collision = in;
    collisionEnabled = true;
    collision.updatePosition(position);
}
void Actor::setModel(Tools3D::MeshTexture in){
    model = in;
    visible = true;
}

void Actor::setTexture(const QString &filepath){
    QImage *newTexture = new QImage(filepath);
    if(!newTexture->isNull()){
        model.texture = new QImage(filepath);
    }
//    model.texture = new QImage(filepath);
}

//void ActorStatic::processCollision(std::vector<Tools3D::AABB> colliders){
//    collision.updatePosition(position);
////    if(collision.intersects(aabb)){
////        qDebug("COLLISION DETECTED");
////    }
//}

void ActorPlayer::processLogic(){
//    if(!logicEnabled) return;
    float jumpSpeed = 3;
    float maxJumpSpeed = 4;
    float moveSpeed = 0.5;
    float maxMoveSpeed = 5;

//    bool moving = false; // If no walk input is applied, slow down

    // Calculate directions for movement relative to camera rotation
    T3::Vector3 forward = *lookDir * moveSpeed;
    forward.y = 0;
    T3::Vector3 right = forward * T3::newMatRotY(3.14/2);

    if(Input->isActionPressed("UP")){
        velocity += forward;
//        moving = true;
    }
    if(Input->isActionPressed("DOWN")){
        velocity -= forward;
//        moving = true;
    }
    if(Input->isActionPressed("LEFT")){
        velocity -= right;
//        moving = true;
    }
    if(Input->isActionPressed("RIGHT")){
        velocity += right;
//        moving = true;
    }
    if(Input->isActionJustPressed("JUMP")){
        velocity.y = jumpSpeed;
    }
//    if(Input->isActionPressed("CROUCH")){
//        velocity.y -= jumpSpeed;
//    }

    // Friction
    velocity.x = T2::lerp(velocity.x, 0, 0.1);
    velocity.z = T2::lerp(velocity.z, 0, 0.1);

    // Cap player horizontal speed
    velocity.x = T2::clamp(velocity.x, -maxMoveSpeed, maxMoveSpeed);
    velocity.z = T2::clamp(velocity.z, -maxMoveSpeed, maxMoveSpeed);

    // Cap player air speed
    velocity.y = T2::clamp(velocity.y, -maxJumpSpeed, maxJumpSpeed);

    position.y = std::max(0.0f, position.y); // TEMP, DELETE LATER
}

void ActorPlayer::processCollision(std::vector<Tools3D::AABB> colliders){
    float t; // Used only for intersectPlane(), otherwise it wont work FIX THIS
    velocity.y -= gravity;

    // Create a temporary AABB offset by velocity to check if two AABBs will collide on the next frame
    T3::AABB nextCollision ;//= {collision.getPosition() + velocity, collision.getSize()};
    nextCollision.updatePosition(position + velocity);

    for(auto aabb : colliders){
        if(nextCollision.intersects(aabb) && collisionEnabled){
            T3::Vector3 oPosition = aabb.getPosition();
            T3::Vector3 oEnd = aabb.getEnd();

            // Check for collisions on the x plane
            if(velocity.x != 0){
                // Coordinates of the rectange we're checking against
                float z1, y1, z2, y2;
                z1 = oPosition.z;
                y1 = oPosition.y;
                z2 = oEnd.z;
                y2 = oEnd.y;

                // Raycast intersection point
                // Might need to use more raycasts for better accuracy
                T3::Vector3 iPoint;

                // Check from which side are we colliding with the other aabb
                // Construct the raycast accordingly
                if(position.x > oEnd.x){
                    // Entering from the end side
                    iPoint = T3::intersectPlane(oEnd, {1, 0, 0}, position, {velocity.x, 0, 0}, t);
                }else{
                    // Entering from the position side
                    iPoint = T3::intersectPlane(oPosition, {1, 0, 0}, position, {velocity.x, 0, 0}, t);
                }

                // Check if the intersection point is inside the rectangle
                // If yes, then modify velocity to prevent entering the other AABB
                if(z1 < iPoint.z && iPoint.z < z2 && y1 < iPoint.y && iPoint.y < y2){
                    if(position.x > oEnd.x){
//                        velocity.x = position.x - oEnd.x;
                        velocity.x = std::max(velocity.x, 0.0f);
                    }
                    if(position.x < oPosition.x){
//                        velocity.x = position.x - oPosition.x;
                        velocity.x = std::min(velocity.x, 0.0f);
                    }
                }
            }
            // Check for collisions on the y plane
            if(velocity.y != 0){
                float x1, z1, x2, z2;
                x1 = oPosition.x;
                z1 = oPosition.z;
                x2 = oEnd.x;
                z2 = oEnd.z;

                T3::Vector3 iPoint;

                if(position.y > oEnd.y){
                    // Entering from the top side
                    iPoint = T3::intersectPlane(oEnd, {0, 1, 0}, position, {velocity.x, 0, 0}, t);
                }else{
                    // Entering from the bottom side
                    iPoint = T3::intersectPlane(oPosition, {0, 1, 0}, position, {velocity.x, 0, 0}, t);
                }

                if(x1 < iPoint.x && iPoint.x < x2 && z1 < iPoint.z && iPoint.z < z2){
                    if(position.y > oEnd.y){
//                        velocity.y = std::min(velocity.y, position.y - oEnd.y);
//                        velocity.y = std::max(velocity.y, position.y - oEnd.y);
                        velocity.y = std::max(velocity.y, 0.0f);
                    }
                    if(position.y < oPosition.y){
//                        velocity.y = position.y - oPosition.y;
//                        velocity.y = std::min(velocity.y, position.y - oPosition.y);
                        velocity.y = std::min(velocity.y, 0.0f);
                    }
                }
            }
            // Check for collisions on the z plane
            if(velocity.z != 0){
                float x1, y1, x2, y2;
                x1 = oPosition.x;
                y1 = oPosition.y;
                x2 = oEnd.x;
                y2 = oEnd.y;

                T3::Vector3 iPoint;

                if(position.z > oEnd.z){
                    // Entering from the end side
                    iPoint = T3::intersectPlane(oEnd, {0, 0, 1}, position, {velocity.x, 0, 0}, t);
                }else{
                    // Entering from the position side
                    iPoint = T3::intersectPlane(oPosition, {0, 0, 1}, position, {velocity.x, 0, 0}, t);
                }

                if(x1 < iPoint.x && iPoint.x < x2 && y1 < iPoint.y && iPoint.y < y2){
                    if(position.z > oEnd.z){
//                        velocity.z = position.z - oEnd.z;
                        velocity.z = std::max(velocity.z, 0.0f);
                    }
                    if(position.z < oPosition.z){
//                        velocity.z = position.z - oPosition.z;
                        velocity.z = std::min(velocity.z, 0.0f);
                    }
                }
            }
        }
    }

    position += velocity;
    collision.updatePosition(this->position);
}

void ActorEnemy::processCollision(std::vector<Tools3D::AABB> colliders){

}

void ActorLight::processCollision(std::vector<Tools3D::AABB> colliders){
    collision.updatePosition(this->position);
}
