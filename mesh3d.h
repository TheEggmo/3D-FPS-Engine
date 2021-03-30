#ifndef MESH3D_H
#define MESH3D_H

#include <vector>

#include <tools3d.h>

// Struct representing a mesh of triangles
struct Mesh3D{
    std::vector<Tools3D::Triangle> tris;

    // Load the mesh from obj file
    // Read only the vertice data
    bool loadFromFile(const QString& fileName);
    // Multiply all vertices by mod
    void scale(float mod);
    // Adds the specified vector to all vertices within this mesh
    void moveVertices(Tools3D::Vector3 v);
    // Checks if tris is empty
    bool empty();
};

#endif // MESH3D_H
