#ifndef MESHCOLLISION_H
#define MESHCOLLISION_H

#include "tools3d.h" // Inherit Mesh3D

struct MeshCollision : public Tools3D::Mesh3D{

//    // Copy MeshTexture to MeshCollision without copying UV data
//    MeshCollision operator=(Tools3D::MeshTexture in){
//        for(Tools3D::Triangle inTri : in){
//            Tools3D::Triangle tri;
//            tri.p[0] = inTri.p[0];
//            tri.p[1] = inTri.p[1];
//            tri.p[2] = inTri.p[2];
//        }
//    }
};
#endif // MESHCOLLISION_H
