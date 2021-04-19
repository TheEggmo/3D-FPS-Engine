#ifndef TOOLS3D_H
#define TOOLS3D_H

#include "tools.h"
//#include "actor.h"
//#include "Mesh.h"

#include <QFile>
#include <QRegularExpression>

using T2 = Tools;

class Tools3D{
public:
    // Structs used for 3d
    class Vector3;
    class Mat4x4;
    class Triangle;
    class Mesh;

    struct UV{
        float u, v, w;

        UV(){
            u = 0;
            v = 0;
            w = 1;
        }
        UV(float a, float b){
            u = a;
            v = b;
            w = 1;
        }
    };

    // Struct repsresenting a 3d vector
    struct Vector3{
        float x, y, z, w;

        Vector3(){
            x = y = z = 0;
            w = 1;
        }
        Vector3(float a, float b, float c){
            x = a;
            y = b;
            z = c;
            w = 1;
        }
        Vector3(float a, float b, float c, float d){
            x = a;
            y = b;
            z = c;
            w = d;
        }

        // Overloads for basic math
        Vector3 operator+(Vector3 v){
            Vector3 out;
            out.x = this->x + v.x;
            out.y = this->y + v.y;
            out.z = this->z + v.z;
            out.w = this->w + v.w;
            return out;
        }
        void operator+=(Vector3 v){
            *this = *this + v;
        }
        Vector3 operator-(Vector3 v){
            Vector3 out;
            out.x = this->x - v.x;
            out.y = this->y - v.y;
            out.z = this->z - v.z;
//            out.w = this->w - v.w;
            return out;
        }
        Vector3 operator-=(Vector3 v){
            *this = *this - v;
        }
        Vector3 operator*(float k){
            Vector3 out;
            out.x = this->x * k;
            out.y = this->y * k;
            out.z = this->z * k;
//            out.w = this->w * k;
            return out;
        }
        Vector3 operator*=(float k){
            *this = *this * k;
        }
        Vector3 operator/(float k){
            Vector3 out;
            out.x = this->x / k;
            out.y = this->y / k;
            out.z = this->z / k;
//            out.w = this->w / k;
            return out;
        }
        Vector3 operator/=(float k){
            *this = *this / k;
        }

        Vector3 operator*(Mat4x4 m){
            Vector3 in = *this;
            Vector3 out;
            out.x = in.x * m.m[0][0] + in.y * m.m[1][0] + in.z * m.m[2][0] + m.m[3][0];
            out.y = in.x * m.m[0][1] + in.y * m.m[1][1] + in.z * m.m[2][1] + m.m[3][1];
            out.z = in.x * m.m[0][2] + in.y * m.m[1][2] + in.z * m.m[2][2] + m.m[3][2];
            out.w = in.x * m.m[0][3] + in.y * m.m[1][3] + in.z * m.m[2][3] + m.m[3][3];

            return out;
        }
        Vector3 operator*=(Mat4x4 m){
            *this = *this * m;
        }
    };

    // Struct representing a triangle. Triangle points must be in clockwise order to properly calculate normals
    struct Triangle{
        Vector3 p[3]; // Triangle points' 3D coordinates
        UV t[3]; // Texture coordinates

        Tools::Color8 color;
        QImage *texture; // Whenever a triangle is clipped it retains a pointer to it's texture

        Triangle(){
            this->p[0] = {0,0,0};
            this->p[1] = {0,0,0};
            this->p[2] = {0,0,0};
        }
        Triangle(Vector3 v0, Vector3 v1, Vector3 v2){
            this->p[0] = v0;
            this->p[1] = v1;
            this->p[2] = v2;
        }
        Triangle(Vector3 v0, Vector3 v1, Vector3 v2, UV uv0, UV uv1, UV uv2){
            this->p[0] = v0;
            this->p[1] = v1;
            this->p[2] = v2;
            this->t[0] = uv0;
            this->t[1] = uv1;
            this->t[2] = uv2;
        }

        Triangle operator*(Mat4x4 m){
            Triangle in = *this;
            Triangle out;
            out.p[0] = in.p[0] * m;
            out.p[1] = in.p[1] * m;
            out.p[2] = in.p[2] * m;

            return out;
        }
        Triangle operator/(float k){
            Triangle in = *this;
            Triangle out;
            out.p[0] = in.p[0] / k;
            out.p[1] = in.p[1] / k;
            out.p[2] = in.p[2] / k;

            return out;
        }
    };

    // Struct representing a mesh of triangles
    struct Mesh{
        std::vector<Tools3D::Triangle> tris;

        // Load the mesh from obj file
        // Read only the vertice data
        virtual bool loadFromFile(const QString& fileName);
        // Multiply all vertices by mod
        void scale(float mod);
        // Multiply all vertices by mod, in a single dimension
        void scaleX(float mod);
        void scaleY(float mod);
        void scaleZ(float mod);
        // Adds the specified vector to all vertices within this mesh
        // Shouldn't be used, it's better to use a matrix returned by newMatTrans() (I think)
        void move(Tools3D::Vector3 v);
        // Checks if tris is empty
        bool empty();

    };

    struct MeshTexture : public Mesh{
//        bool flat = true;
        QImage *texture;

        // Load the mesh from obj file
        // Unlike Mesh, this one also reads texture data
        bool loadFromFile(const QString& fileName) override;
    };

//    struct MeshCollision : public Tools3D::Mesh{

//        // Copy MeshTexture to MeshCollision without copying UV data
//        MeshCollision operator=(MeshTexture in){
//            tris.clear();
//            for(Triangle inTri : in.tris){
//                Triangle tri;
//                tri.p[0] = inTri.p[0];
//                tri.p[1] = inTri.p[1];
//                tri.p[2] = inTri.p[2];
//                tris.push_back(tri);
//            }
//        }

////        bool loadFromFile(const QString &fileName) override;
//        // Check for collision against another collision mesh
//        // The meshes should be cuboids for this to work
//        bool checkCollision(MeshCollision other);
//    };

    // Axis-Aligned Bounding Box
    struct AABB{
    private:
        // The parent of this AABB
//        Actor parent;

        // Local space coordinates
        // These are the original coordinates and are used to calculate
        // global coordinates of the AABB during updatePosition().
        // Beggining corner
        Vector3 positionLocal;
        // Ending corner (position + size)
        Vector3 endLocal;

        // Global space coordinates
        // Used to determine collisions with other AABBs
        // Beggining corner
        Vector3 position;
        // Ending corner (position + size)
        Vector3 end;

        // Size from position to end
        Vector3 size;

    public:
        AABB(){
            positionLocal = {0, 0, 0};
            endLocal = {0, 0, 0};
            size = {0, 0, 0};
        }
        // Constructs an AABB from a position and size
        AABB(Vector3 p, Vector3 s){
            this->positionLocal = p;
            this->endLocal = p + s;
            this->size = s;
        }
        // Constructs an AABB that encloses the given mesh
        AABB(Mesh m){
            Vector3 p, e;
            p = e = {0, 0, 0};
            for(Triangle tri : m.tris){
                for(Vector3 v : tri.p){
                    p.x = std::min(p.x, v.x);
                    p.y = std::min(p.y, v.y);
                    p.z = std::min(p.z, v.z);

                    e.x = std::max(e.x, v.x);
                    e.y = std::max(e.y, v.y);
                    e.z = std::max(e.z, v.z);
                }
            }

            this->positionLocal = p;
            this->endLocal = e;
            this->size = e - p;
        }

        // Sets global coordinates to local + vec
        void updatePosition(Vector3 vec);
        // Returns true if this AABB intersects the specified AABB
        // Uses global coordinates
        bool intersects(const AABB &other);
        // Returns a mesh in the shape of this AABB
        // Useful for debugging
        Mesh toMesh();
//        // Returns an AABB with equalivent position and size,
//        // modified so that the most-negative corner is the origin and the size is positive
//        AABB abs();
//        Actor getParent() { return parent; }
    };

    // Struct representing a 4x4 matrix
    struct Mat4x4{
        float m[4][4] = {0};

        Mat4x4 operator*(Mat4x4 m){
            Mat4x4 out;
            for(int c = 0; c < 4; c++){
                for(int r = 0; r < 4; r++){
                    out.m[r][c] = this->m[r][0] * m.m[0][c] +
                            this->m[r][1] * m.m[1][c] +
                            this->m[r][2] * m.m[2][c] +
                            this->m[r][3] * m.m[3][c];
                }
            }
            return out;
        }
   };


    // Drawing functions

    // Draws a triangle in 3d space
    static void drawTri(QImage *image, Triangle tri, Tools::Color8 color);
    static void drawTri(QImage *image, Triangle tri, Tools::Color color);
    static void fillTri(QImage *image, Triangle tri, Tools::Color8 color);
    static void fillTri(QImage *image, Triangle tri, Tools::Color color);
    // Draw a textured triangle
    // texture is a pointer to the texture QImage
    // dBuffer is a pointer to the depth buffer
    static void textureTri(QImage *image, Triangle tri, QImage *texture, std::vector<float> &dBuffer);
//    static void textureTri(QImage *image, Triangle tri, QImage *texture, std::vector<std::pair<float, Tools::Color8>> &dBuffer);



    // Math functions

    // Vector functions
    // Multiply a vector by a matrix
    static Vector3 multiplyMatrix(Vector3 &in, Mat4x4 &m);
    // Calculate dot product
    static float dotProduct(Vector3 v1, Vector3 v2);
    // Calculate the length of a vector
    static float length(Vector3 v);
    // Normalize a vector
    static Vector3 normalise(Vector3 v);
    // Calculate the normal vector(cross product) of two vectors/lines
    static Vector3 crossProduct(Vector3 v1, Vector3 v2);


    // Matrix creation functions
    // Identity matrix
    static Mat4x4 newMatIdentity();
    // Rotation matrix
    static Mat4x4 newMatRotX(float angle);
    static Mat4x4 newMatRotY(float angle);
    static Mat4x4 newMatRotZ(float angle);
    // Translation matrix
    static Mat4x4 newMatTrans(float x, float y, float z);
    // Projection matrix
    static Mat4x4 newMatProj(float fovDeg, float aspectRatio, float near, float far);

    static Mat4x4 matPointAt(Vector3 pos, Vector3 target, Vector3 up);
    static Mat4x4 matQuickInverse(Mat4x4 mat);

    //
    static Vector3 intersectPlane(Vector3 planePoint, Vector3 planeNormal, Vector3 lineStart, Vector3 lineEnd, float &t);
    // Clips a triangle against a plane. Returns the amount of triangles produced by clipping (0-2).
    // New triangles created by clipping are placed in outTri1 and outTri2.
    static int clipTriangle(Vector3 planePoint, Vector3 planeNormal, Triangle inTri, Triangle &outTri1, Triangle &outTri2);
};

#endif // TOOLS3D_H
