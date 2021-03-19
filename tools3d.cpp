#include "tools3d.h"

using T2 = Tools;

// Draw a triangle using drawLine
void Tools3D::drawTri(QImage *image, Triangle tri, T2::Color8 color){
    T2::drawLine(image, tri.p[0].x, tri.p[0].y,
            tri.p[1].x, tri.p[1].y, color);
    T2::drawLine(image, tri.p[1].x, tri.p[1].y,
            tri.p[2].x, tri.p[2].y, color);
    T2::drawLine(image, tri.p[2].x, tri.p[2].y,
            tri.p[0].x, tri.p[0].y, color);
}
void Tools3D::drawTri(QImage *image, Triangle tri, T2::Color color){
    drawTri(image, tri, color.convert());
}

// Draw a filled triangle
// Taken from OneLonelyCoder game engine tutorial, modified by me
void Tools3D::fillTri(QImage *image, Triangle tri, T2::Color8 color){
    auto SWAP = [](int &x, int &y) { int t = x; x = y; y = t; }; // Swaps the two values

    // Split the triangle into points
    int x1 = tri.p[0].x;
    int x2 = tri.p[1].x;
    int x3 = tri.p[2].x;
    int y1 = tri.p[0].y;
    int y2 = tri.p[1].y;
    int y3 = tri.p[2].y;
//    int z1 = tri.p[0].z;
//    int z2 = tri.p[1].z;
//    int z3 = tri.p[2].z;

    int t1x, t2x, y, minx, maxx, t1xp, t2xp;
    bool changed1 = false;
    bool changed2 = false;
    int signx1, signx2, dx1, dy1, dx2, dy2;
    int e1, e2;
    // Sort vertices
    if (y1>y2) { SWAP(y1, y2); SWAP(x1, x2); }
    if (y1>y3) { SWAP(y1, y3); SWAP(x1, x3); }
    if (y2>y3) { SWAP(y2, y3); SWAP(x2, x3); }

    t1x = t2x = x1; y = y1;   // Starting points
    dx1 = (int)(x2 - x1); if (dx1<0) { dx1 = -dx1; signx1 = -1; }
    else signx1 = 1;
    dy1 = (int)(y2 - y1);

    dx2 = (int)(x3 - x1); if (dx2<0) { dx2 = -dx2; signx2 = -1; }
    else signx2 = 1;
    dy2 = (int)(y3 - y1);

    if (dy1 > dx1) {   // swap values
        SWAP(dx1, dy1);
        changed1 = true;
    }
    if (dy2 > dx2) {   // swap values
        SWAP(dy2, dx2);
        changed2 = true;
    }

    e2 = (int)(dx2 >> 1);
    // Flat top, just process the second half
    if (y1 == y2) goto next;
    e1 = (int)(dx1 >> 1);

    for (int i = 0; i < dx1;) {
        t1xp = 0; t2xp = 0;
        if (t1x<t2x) { minx = t1x; maxx = t2x; }
        else { minx = t2x; maxx = t1x; }
        // process first line until y value is about to change
        while (i<dx1) {
            i++;
            e1 += dy1;
            while (e1 >= dx1) {
                e1 -= dx1;
                if (changed1) t1xp = signx1;//t1x += signx1;
                else          goto next1;
            }
            if (changed1) break;
            else t1x += signx1;
        }
        // Move line
    next1:
        // process second line until y value is about to change
        while (1) {
            e2 += dy2;
            while (e2 >= dx2) {
                e2 -= dx2;
                if (changed2) t2xp = signx2;//t2x += signx2;
                else          goto next2;
            }
            if (changed2)     break;
            else              t2x += signx2;
        }
    next2:
        if (minx>t1x) minx = t1x; if (minx>t2x) minx = t2x;
        if (maxx<t1x) maxx = t1x; if (maxx<t2x) maxx = t2x;
        for(int i = minx; i <= maxx; i++){
            T2::drawPixel(image, i, y, color);
        }
//        drawline(minx, maxx, y);    // Draw line from min to max points found on the y
                                     // Now increase y
        if (!changed1) t1x += signx1;
        t1x += t1xp;
        if (!changed2) t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y == y2) break;

    }
next:
    // Second half
    dx1 = (int)(x3 - x2); if (dx1<0) { dx1 = -dx1; signx1 = -1; }
    else signx1 = 1;
    dy1 = (int)(y3 - y2);
    t1x = x2;

    if (dy1 > dx1) {   // swap values
        SWAP(dy1, dx1);
        changed1 = true;
    }
    else changed1 = false;

    e1 = (int)(dx1 >> 1);

    for (int i = 0; i <= dx1; i++) {
        t1xp = 0; t2xp = 0;
        if (t1x<t2x) { minx = t1x; maxx = t2x; }
        else { minx = t2x; maxx = t1x; }
        // process first line until y value is about to change
        while (i<dx1) {
            e1 += dy1;
            while (e1 >= dx1) {
                e1 -= dx1;
                if (changed1) { t1xp = signx1; break; }//t1x += signx1;
                else          goto next3;
            }
            if (changed1) break;
            else   	   	  t1x += signx1;
            if (i<dx1) i++;
        }
    next3:
        // process second line until y value is about to change
        while (t2x != x3) {
            e2 += dy2;
            while (e2 >= dx2) {
                e2 -= dx2;
                if (changed2) t2xp = signx2;
                else          goto next4;
            }
            if (changed2)     break;
            else              t2x += signx2;
        }
    next4:

        if (minx>t1x) minx = t1x; if (minx>t2x) minx = t2x;
        if (maxx<t1x) maxx = t1x; if (maxx<t2x) maxx = t2x;
        for(int i = minx; i <= maxx; i++){
            T2::drawPixel(image, i, y, color);
        }
//        drawline(minx, maxx, y);
        if (!changed1) t1x += signx1;
        t1x += t1xp;
        if (!changed2) t2x += signx2;
        t2x += t2xp;
        y += 1;
        if (y>y3) return;
    }
}
void Tools3D::fillTri(QImage *image, Triangle tri, T2::Color color){
    fillTri(image, tri, color.convert());
}

//Tools::Vector3 Tools::multiplyMatrix(Vector3 &in, Mat4x4 &m){
//    Tools::Vector3 out;
//    out.x = in.x * m.m[0][0] + in.y * m.m[1][0] + in.z * m.m[2][0] + in.w * m.m[3][0];
//    out.y = in.x * m.m[0][1] + in.y * m.m[1][1] + in.z * m.m[2][1] + in.w * m.m[3][1];
//    out.z = in.x * m.m[0][2] + in.y * m.m[1][2] + in.z * m.m[2][2] + in.w * m.m[3][2];
//    out.w = in.x * m.m[0][3] + in.y * m.m[1][3] + in.z * m.m[2][3] + in.w * m.m[3][3];

//    return out;
//}
//Tools3D::Vector3 Tools3D::multiplyMatrix(Vector3 &in, Mat4x4 &m){
//    Vector3 out;
//    out.x = in.x * m.m[0][0] + in.y * m.m[1][0] + in.z * m.m[2][0] + m.m[3][0];
//    out.y = in.x * m.m[0][1] + in.y * m.m[1][1] + in.z * m.m[2][1] + m.m[3][1];
//    out.z = in.x * m.m[0][2] + in.y * m.m[1][2] + in.z * m.m[2][2] + m.m[3][2];
//    float w = in.x * m.m[0][3] + in.y * m.m[1][3] + in.z * m.m[2][3] + m.m[3][3];

//    if (w != 0.0f)
//    {
//        out.x /= w; out.y /= w; out.z /= w;
//    }
//    return out;
//}
float Tools3D::dotProduct(Vector3 v1, Vector3 v2){
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
float Tools3D::length(Vector3 v){
    return sqrtf(dotProduct(v, v));
}
Tools3D::Vector3 Tools3D::normalise(Vector3 v){
    float l = length(v); // Length of the input vector
    return {v.x/l, v.y/l, v.z/l};
}
Tools3D::Vector3 Tools3D::crossProduct(Vector3 v1, Vector3 v2){
    float Nx = v1.y * v2.z - v1.z * v2.y;
    float Ny = v1.z * v2.x - v1.x * v2.z;
    float Nz = v1.x * v2.y - v1.y * v2.x;
    return {Nx, Ny, Nz};
}
//Tools::Vector3 Tools::addVectors(Vector3 &v1, Vector3 &v2){
//    return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
//}
//Tools::Vector3 Tools::subVectors(Vector3 &v1, Vector3 &v2){
//    return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z};
//}
//Tools::Vector3 Tools::mulVectors(Vector3 &v1, float k){
//    return { v1.x * k, v1.y * k, v1.z * k};
//}
//Tools::Vector3 Tools::divVectors(Vector3 &v1, float k){
//    return { v1.x / k, v1.y / k, v1.z / k};
//}

Tools3D::Mat4x4 Tools3D::newMatIdentity(){
    Mat4x4 out;
    out.m[0][0] = 1.0f;
    out.m[1][1] = 1.0f;
    out.m[2][2] = 1.0f;
    out.m[3][3] = 1.0f;
    return out;
}
// Create a matrix for rotating in x/y/z axis by the specified angle
Tools3D::Mat4x4 Tools3D::newMatRotX(float angle){
    Mat4x4 mat;
    mat.m[0][0] = 1;
    mat.m[1][1] = cosf(angle);
    mat.m[1][2] = sinf(angle);
    mat.m[2][1] = -sinf(angle);
    mat.m[2][2] = cosf(angle);
    mat.m[3][3] = 1;
    return mat;
}
Tools3D::Mat4x4 Tools3D::newMatRotY(float angle){
    Mat4x4 mat;
    mat.m[0][0] = cosf(angle);
    mat.m[0][2] = sinf(angle);
    mat.m[2][0] = -sinf(angle);
    mat.m[1][1] = 1.0f;
    mat.m[2][2] = cosf(angle);
    mat.m[3][3] = 1.0f;
    return mat;
}
Tools3D::Mat4x4 Tools3D::newMatRotZ(float angle){
    Mat4x4 mat;
    mat.m[0][0] = cosf(angle);
    mat.m[0][1] = sinf(angle);
    mat.m[1][0] = -sinf(angle);
    mat.m[1][1] = cosf(angle);
    mat.m[2][2] = 1;
    mat.m[3][3] = 1;
    return mat;
}
// Translation matrix
Tools3D::Mat4x4 Tools3D::newMatTrans(float x, float y, float z){
    Mat4x4 mat;
    mat.m[0][0] = 1.0f;
    mat.m[1][1] = 1.0f;
    mat.m[2][2] = 1.0f;
    mat.m[3][3] = 1.0f;
    mat.m[3][0] = x;
    mat.m[3][1] = y;
    mat.m[3][2] = z;
    return mat;
}
// Creates a projection matrix.
// fovDeg is the field of vision in degrees
// near/fear determine the scale of the view
Tools3D::Mat4x4 Tools3D::newMatProj(float fovDeg, float aspectRatio, float near, float far){
    Mat4x4 mat;
    float fovRad = 1.0f / tanf(fovDeg * 0.5f / 180.0f * 3.14159f);
    mat.m[0][0] = aspectRatio * fovRad;
    mat.m[1][1] = fovRad;
    mat.m[2][2] = far / (far - near);
    mat.m[3][2] = (-far * near) / (far - near);
    mat.m[2][3] = 1.0f;
    mat.m[3][3] = 0.0f;
    return mat;
}

Tools3D::Mat4x4 Tools3D::matPointAt(Vector3 pos, Vector3 target, Vector3 up){
    // Calculate the new directions
    // Forward
    Vector3 newForward = target - pos;
    newForward = normalise(newForward);

    // Up
    Vector3 a = newForward * dotProduct(up, newForward);
    Vector3 newUp = up - a;
    newUp = normalise(newUp);

    // Right
    Vector3 newRight = crossProduct(newUp, newForward);

    // Dimensioning and translation matrix
    Mat4x4 mat;
    mat.m[0][0] = newRight.x;   mat.m[0][1] = newRight.y;   mat.m[0][2] = newRight.z;   mat.m[0][3] = 0;
    mat.m[1][0] = newUp.x;      mat.m[1][1] = newUp.y;      mat.m[1][2] = newUp.z;      mat.m[1][3] = 0;
    mat.m[2][0] = newForward.x; mat.m[2][1] = newForward.y; mat.m[2][2] = newForward.z; mat.m[2][3] = 0;
    mat.m[3][0] = pos.x;        mat.m[3][1] = pos.y;        mat.m[3][2] = pos.z;        mat.m[3][3] = 1.0f;

    return mat;
}
// Works only for rotation/translation matrices, i.e. the one returned by matPointAt()
Tools3D::Mat4x4 Tools3D::matQuickInverse(Mat4x4 in){
    Mat4x4 out;
    out.m[0][0] = in.m[0][0]; out.m[0][1] = in.m[1][0]; out.m[0][2] = in.m[2][0];
    out.m[1][0] = in.m[0][1]; out.m[1][1] = in.m[1][1]; out.m[1][2] = in.m[2][1];
    out.m[2][0] = in.m[0][2]; out.m[2][1] = in.m[1][2]; out.m[2][2] = in.m[2][2];
    out.m[3][0] = -(in.m[3][0] * out.m[0][0] + in.m[3][1] * out.m[1][0] + in.m[3][2] * out.m[2][0]);
    out.m[3][1] = -(in.m[3][0] * out.m[0][1] + in.m[3][1] * out.m[1][1] + in.m[3][2] * out.m[2][1]);
    out.m[3][2] = -(in.m[3][0] * out.m[0][2] + in.m[3][1] * out.m[1][2] + in.m[3][2] * out.m[2][2]);
    out.m[3][3] = 1.0f;
    return out;
}
