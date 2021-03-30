#include "actor.h"

void Actor::process(){
    // Order can be rearranged if necessary
    if(logic_enabled) process_logic();
    if(physics_enabled) process_physics();
    if(collision_enabled) process_collision();
}

void Actor::process_physics(){

}
void Actor::process_logic(){

}
void Actor::process_collision(){

}

void Actor::set_model(Tools3D::MeshTexture in){
    model = in;
    visible = true;
}
void Actor::set_collision(Tools3D::MeshCollision in){
    collision = in;
}

Tools3D::MeshTexture Actor::get_model(){
    return model;
}
