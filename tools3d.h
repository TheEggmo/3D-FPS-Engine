#ifndef TOOLS3D_H
#define TOOLS3D_H

#include "tools.h"

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
        Vector3 p[3]; // Triangle points' 3D coordinates
        UV t[3]; // Texture coordinates

        Tools::Color8 color;

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
        std::vector<Triangle> tris;

        // Load the mesh from obj file
        // If fSimple is set to true, texture_index and normal_index of faces("f") are skipped
        bool loadFromFileOld(std::string filename, bool fSimple = true){
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

                // Read vertice data
//                if (line[0] == 'v'){
//                    Vector3 v;
//                    s >> junk >> v.x >> v.y >> v.z;
//                    verts.push_back(v);
//                }
                // Read face's vertice data, doesn't work if more face info is present in the file
//                if(line[0] == 'f'){
//                    int f[3];
//                    s >> junk >> f[0] >> f[1] >> f[2];
////                    qDebug("%d %d %d", f[0], f[1], f[2]);
//                    tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
//                }
//                if(line[0] == 'f'){
//                    int f[9];
//                    s >> junk >> f[0] >> f[1] >> f[2] >> f[3] >> f[4] >> f[5]>> f[6] >> f[7] >> f[8];
//                    qDebug("%d %d %d %d %d %d %d %d %d ", f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7], f[8]);
//                    tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
//                }
                if (line[0] == 'f'){
                    int f[3];
                    // Since some lines may contain useless(for me) data, I need to do some formatting on the fly
                    char c;
                    int i = 0;
                    bool skip = true;
                    int value = 0;
//                    while(line.get(c)){
                    for(int i = 0; i < 128; i++){
                        qDebug("%c", line[i]);
                        if (c == '\n') break;
                        if (!isdigit(c)) {
                            skip = true;
                            f[i] = value;
                            i++;
                            if (i > 2){
                                break;
                            }
                        }
                        if (!skip){
                            value = value * 10 + std::stoi(&c);
                        }

                        if (c == ' ') skip = false;
                    }
                    tris.push_back({verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1]});

                }
            }

            return true;
        }
        bool loadFromFile(const QString& fileName){
            tris.clear();
            QFile file(fileName);
            if(file.exists()){
                if(file.open(QFile::ReadOnly | QFile::Text)){
                    std::vector<Vector3> v, vn;
                    std::vector<T2::Vector2> vt;

                    while(!file.atEnd()){
                        QString line = file.readLine().trimmed();
                        QStringList lineParts = line.split(QRegularExpression("\\s+")); // HOW DOES THIS WORK
                        if(lineParts.count() > 0){
                            if(lineParts.at(0).compare("#", Qt::CaseInsensitive) == 0){
                                //COMMENT
                            }else if(lineParts.at(0).compare("v", Qt::CaseInsensitive) == 0){
                                //VERTICES
                                v.push_back({lineParts.at(1).toFloat(),lineParts.at(2).toFloat(),lineParts.at(3).toFloat()});
                            }else if(lineParts.at(0).compare("vn", Qt::CaseInsensitive) == 0){
                                //NORMAL
                                vn.push_back({lineParts.at(1).toFloat(),lineParts.at(2).toFloat(),lineParts.at(3).toFloat()});
                            }else if(lineParts.at(0).compare("vt", Qt::CaseInsensitive) == 0){
                                //TEXTURE
                                vt.push_back({lineParts.at(1).toFloat(), lineParts.at(2).toFloat()});
                            }else if(lineParts.at(0).compare("f", Qt::CaseInsensitive) == 0){
                                //FACE DATA
                                // Faces must be triangles
                                Triangle tri;
                                // Vertices
                                tri.p[0] = v.at(lineParts.at(1).split("/").at(0).toInt() - 1);
                                tri.p[1] = v.at(lineParts.at(2).split("/").at(0).toInt() - 1);
                                tri.p[2] = v.at(lineParts.at(3).split("/").at(0).toInt() - 1);
                                // UV coords
                                // Currently unused, replace x with triangle's uv structure
//                                x = v.at(lineParts.at(1).split("/").at(1).toInt() - 1);
//                                x = v.at(lineParts.at(2).split("/").at(1).toInt() - 1);
//                                x = v.at(lineParts.at(3).split("/").at(1).toInt() - 1);
                                // Normals
                                // Currently unused
//                                x = v.at(lineParts.at(1).split("/").at(2).toInt() - 1);
//                                x = v.at(lineParts.at(2).split("/").at(2).toInt() - 1);
//                                x = v.at(lineParts.at(3).split("/").at(2).toInt() - 1);

                                tris.push_back(tri);
                            }
                        }
                    }
                }else{
                    return false;
                }
            }else{
                return false;
            }
            return true;
        }
        void scale(float mod){
            for(int tIdx = 0; tIdx < tris.size(); tIdx++){
                for(int pIdx = 0; pIdx <= 2; pIdx++){
                    tris[tIdx].p[pIdx] = tris[tIdx].p[pIdx] * mod;
                }
            }
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
    static void textureTri(QImage *image, Triangle tri, QImage *texture);


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
