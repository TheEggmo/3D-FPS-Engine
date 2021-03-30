#ifndef ACTOR_H
#define ACTOR_H

#include "tools3d.h"
#include "meshcollision.h"

class Actor{
protected:
    Tools3D::MeshCollision collision;
    Tools3D::MeshTexture model;

public:
    Tools3D::Vector3 position;

    bool visible = false; // If true, the model mesh will be drawn
    bool physics_enabled = false; // If true, the body will have physics applied every frame (gravity etc.)
    bool logic_enabled = false; // If true, the body will process its logic(pathfinding, movement etc.)
    bool collision_enabled = false; // If true, the body will have collisions

    void process_physics();
    void process_logic();
    void process_collision();

    void process();

    void set_model(Tools3D::MeshTexture);
    void set_collision(Tools3D::MeshCollision);

    Tools3D::MeshTexture get_model();

};

#endif // ACTOR_H
