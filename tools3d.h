#ifndef TOOLS3D_H
#define TOOLS3D_H

#include "tools.h"

using T2 = Tools;

class Tools3D{
public:
    // Structs used for 3d
    class Vector3;
    class Mat4x4;
    class Triangle;
    class Mesh;

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

        // Overloads for basic math
        Vector3 operator+(Vector3 v){
            Vector3 out;
            out.x = this->x + v.x;
            out.y = this->y + v.y;
            out.z = this->z + v.z;
            out.w = this->w + v.w;
            return out;
        }
        Vector3 operator-(Vector3 v){
            Vector3 out;
            out.x = this->x - v.x;
            out.y = this->y - v.y;
            out.z = this->z - v.z;
            out.w = this->w - v.w;
            return out;
        }
        Vector3 operator*(float k){
            Vector3 out;
            out.x = this->x * k;
            out.y = this->y * k;
            out.z = this->z * k;
            out.w = this->w * k;
            return out;
        }
        Vector3 operator/(float k){
            Vector3 out;
            out.x = this->x / k;
            out.y = this->y / k;
            out.z = this->z / k;
            out.w = this->w / k;
            return out;
        }

        // Overload for 4x4 matrices, like the projection matrix
//        Vector3 operator*(Mat4x4 m){
//            Vector3 in = *this;
//            Vector3 out;
//            out.x = in.x * m.m[0][0] + in.y * m.m[1][0] + in.z * m.m[2][0] + m.m[3][0];
//            out.y = in.x * m.m[0][1] + in.y * m.m[1][1] + in.z * m.m[2][1] + m.m[3][1];
//            out.z = in.x * m.m[0][2] + in.y * m.m[1][2] + in.z * m.m[2][2] + m.m[3][2];
//            float w = in.x * m.m[0][3] + in.y * m.m[1][3] + in.z * m.m[2][3] + m.m[3][3];

//            if (w != 0.0f)
//            {
//                out.x /= w; out.y /= w; out.z /= w;
//            }
//            return out;
//        }
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
            Vector3 in = *this;
            return in * m;
        }
    };
    // Struct representing a triangle. Triangle points must be in clockwise order to properly calculate normals
    struct Triangle{
        Vector3 p[3];

        Tools::Color8 color;

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
        std::vector<Triangle> tris;

        // Load the mesh from obj file
        bool loadFromFile(std::string filename){
            std::ifstream f(filename);
            if(!f.is_open()){
                return false;
            }

            std::vector<Vector3> verts;

            while(!f.eof()){
                char line[128];
                f.getline(line, 128);

                std::strstream s;

                s << line;

                char junk;

                if (line[0] == 'v'){
                    Vector3 v;
                    s >> junk >> v.x >> v.y >> v.z;
                    verts.push_back(v);
                }
                if(line[0] == 'f'){
                    int f[3];
                    s >> junk >> f[0] >> f[1] >> f[2];
                    tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
                }
//                if (line[0] == 'f'){
//                    int f[3];
//                    // Since some lines may contain useless(for me) data, I need to do some formatting on the fly
//                    char c;
//                    int i = 0;
//                    bool skip = true;
//                    int value = 0;
//                    while(s.get(c)){
//                        if (c == '\n') break;
//                        if (!isdigit(c)) {
//                            skip = true;
//                            f[i] = value;
//                            i++;
//                            if (i > 2){
//                                break;
//                            }
//                        }
//                        if (!skip){
//                            value = value * 10 + std::stoi(&c);
//                        }

//                        if (c == ' ') skip = false;
//                    }
//                    tris.push_back({verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1]});

//                }
            }

            return true;
        }
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
    // Add, subtract, multiply and divide vectors
//    static Vector3 addVectors(Vector3 &v1, Vector3 &v2);
//    static Vector3 subVectors(Vector3 &v1, Vector3 &v2);
//    static Vector3 mulVectors(Vector3 &v1, float k);
//    static Vector3 divVectors(Vector3 &v1, float k);

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
    static Vector3 intersectPlane(Vector3 planePoint, Vector3 planeNormal, Vector3 lineStart, Vector3 lineEnd);
    // Clips a triangle against a plane. Returns the amount of triangles produced by clipping (0-2).
    // New triangles created by clipping are placed in outTri1 and outTri2.
    static int clipTriangle(Vector3 planePoint, Vector3 planeNormal, Triangle inTri, Triangle &outTri1, Triangle &outTri2);
};

#endif // TOOLS3D_H
