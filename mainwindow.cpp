#include "mainwindow.h"
//#include <Tools.h>
#include <vector>
#include <QDebug>
#include <algorithm>

using T2 = Tools;
using T3 = Tools3D;

enum MButton{NONE, LEFT, RIGHT, MIDDLE};

QImage *mainImage, *storedImage;

MButton heldButton; // Stores which button was pressed when mousePressEvent was called

int startX, startY; // Stores the x and y coordinates of the mouse when mousePressEvent is called
int offset = 0; // Shifts the drawn image and mouse events by this offset

T2::Color8 drawingColor;
Qt::GlobalColor defaultBg = Qt::white;

MainWindow::MainWindow(QWidget *parent): QWidget(parent){
    setWindowTitle("3D Engine");

    // Set the screen dimensions
    sWidth = 1280;
    sHeight = 720;
    resize(sWidth + 2*offset, sHeight + 2*offset);

    mainImage = new QImage(sWidth, sHeight, QImage::Format_RGB32);
    mainImage->fill(defaultBg);

    drawingColor = T2::Color8(255, 0, 0);

    // Create a timer that will redraw the screen
    sUpdateTimer = new QTimer(this);
    sUpdateTimer->setSingleShot(false);
    connect(sUpdateTimer, SIGNAL(timeout()), this, SLOT(screenUpdate()));
    int targetFps = 60;
    sUpdateTimer->start(1000/targetFps); // Refresh every 16 msec, which is aprox 60fps


    // Initialize a cube made of triangles, made of points
    meshCube.loadFromFile("blaster.obj");
//    meshCube.tris = {
//    // SOUTH
//    { 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
//    { 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

//    // EAST
//    { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
//    { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

//    // NORTH
//    { 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
//    { 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

//    // WEST
//    { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
//    { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

//    // TOP
//    { 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
//    { 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

//    // BOTTOM
//    { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
//    { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },
//    };

    // Initiate the projection Matrix

    // Values used by the matirx
    float fNear = 0.1f;
    float fFar = 1000.0f;
    float fFov = 90.0f;
    float fAspectRatio = (float)sHeight / (float)sWidth;
    float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

    // The matrix itself
    matProj.m[0][0] = fAspectRatio * fFovRad;
    matProj.m[1][1] = fFovRad;
    matProj.m[2][2] = fFar / (fFar - fNear);
    matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    matProj.m[2][3] = 1.0f;
    matProj.m[3][3] = 0.0f;

}

void MainWindow::paintEvent(QPaintEvent*){
    QPainter p(this);
    p.drawImage(offset, offset, *mainImage);
}

//void MainWindow::mousePressEvent(QMouseEvent *event){
//    // Since the image is drawn at (10,10) position, all mouse input is offset
//    startX = event->x() - offset;
//    startY = event->y() - offset;
//}
//void MainWindow::mouseMoveEvent(QMouseEvent *event){
//    int x = event->x();
//    int y = event->y();
//}
//void MainWindow::mouseReleaseEvent(QMouseEvent *event){
//    heldButton = NONE;
//}

// Simplified drawPixel
void drawPixel(int x, int y){
    T2::drawPixel(mainImage, x, y, drawingColor);
}

void MainWindow::screenUpdate(){
    mainImage->fill(defaultBg);

    T3::Mat4x4 matRotZ, matRotX;

    fTheta += 0.025f;

    // Rotation Z
    matRotZ.m[0][0] = cosf(fTheta);
    matRotZ.m[0][1] = sinf(fTheta);
    matRotZ.m[1][0] = -sinf(fTheta);
    matRotZ.m[1][1] = cosf(fTheta);
    matRotZ.m[2][2] = 1;
    matRotZ.m[3][3] = 1;

    // Rotation X
    matRotX.m[0][0] = 1;
    matRotX.m[1][1] = cosf(fTheta * 0.5f);
    matRotX.m[1][2] = sinf(fTheta * 0.5f);
    matRotX.m[2][1] = -sinf(fTheta * 0.5f);
    matRotX.m[2][2] = cosf(fTheta * 0.5f);
    matRotX.m[3][3] = 1;


    std::vector<T3::Triangle> triangleQueue;

    // Draw Triangles
    for (auto tri : meshCube.tris) {
        T3::Triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

        // Rotate in Z-Axis
//        triRotatedZ.p[0] = Tools::multiplyMatrix(tri.p[0], matRotZ);
//        triRotatedZ.p[1] = Tools::multiplyMatrix(tri.p[1], matRotZ);
//        triRotatedZ.p[2] = Tools::multiplyMatrix(tri.p[2], matRotZ);
//        triRotatedZ.p[0] = tri.p[0] * matRotZ;
//        triRotatedZ.p[1] = tri.p[1] * matRotZ;
//        triRotatedZ.p[2] = tri.p[2] * matRotZ;
        triRotatedZ = tri * matRotZ;

        // Rotate in X-Axis
//        triRotatedZX.p[0] = Tools::multiplyMatrix(triRotatedZ.p[0], matRotX);
//        triRotatedZX.p[1] = Tools::multiplyMatrix(triRotatedZ.p[1], matRotX);
//        triRotatedZX.p[2] = Tools::multiplyMatrix(triRotatedZ.p[2], matRotX);
//        triRotatedZX.p[0] = triRotatedZ.p[0] * matRotX;
//        triRotatedZX.p[1] = triRotatedZ.p[1] * matRotX;
//        triRotatedZX.p[2] = triRotatedZ.p[2] * matRotX;
        triRotatedZX = triRotatedZ * matRotX;

        // Offset into the screen
        triTranslated = triRotatedZX;
        triTranslated.p[0].z = triRotatedZX.p[0].z + 1.0f;
        triTranslated.p[1].z = triRotatedZX.p[1].z + 1.0f;
        triTranslated.p[2].z = triRotatedZX.p[2].z + 1.0f;

        T3::Vector3 normal, line1, line2;
//        line1.x = triTranslated.p[1].x - triTranslated.p[0].x;
//        line1.y = triTranslated.p[1].y - triTranslated.p[0].y;
//        line1.z = triTranslated.p[1].z - triTranslated.p[0].z;
        line1 = triTranslated.p[1] - triTranslated.p[0];

//        line2.x = triTranslated.p[2].x - triTranslated.p[0].x;
//        line2.y = triTranslated.p[2].y - triTranslated.p[0].y;
//        line2.z = triTranslated.p[2].z - triTranslated.p[0].z;
        line2 = triTranslated.p[2] - triTranslated.p[0];

        normal = T3::crossProduct(line1, line2);
        normal = T3::normalize(normal);

//        if (normal.z< 0){
        if(T3::dotProduct(normal,
        {triTranslated.p[0].x - vCamera.x, triTranslated.p[0].y - vCamera.y, triTranslated.p[0].z - vCamera.z}) < 0){
            // Add a light
            T3::Vector3 lightDirection = { 0.0f, 0.0f, -1.0f};
            lightDirection = T3::normalize(lightDirection);

            // Calculate the dot product of the light source and the normal to determine the intensity of shading/illumination
            float dp = T3::dotProduct(normal, lightDirection);
            T2::Color8 shadedColor;
            shadedColor = drawingColor * dp;
//            shadedColor.r = drawingColor.r * dp;
//            shadedColor.g = drawingColor.g * dp;
//            shadedColor.b = drawingColor.b * dp;

            // Project from 3D to 2D
//            triProjected.p[0] = Tools::multiplyMatrix(triTranslated.p[0], matProj);
//            triProjected.p[1] = Tools::multiplyMatrix(triTranslated.p[1], matProj);
//            triProjected.p[2] = Tools::multiplyMatrix(triTranslated.p[2], matProj);
            triProjected = triTranslated * matProj;

            // Scale into view
            triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
            triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
            triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;

            triProjected.p[0].x *= 0.5f * (float)sWidth;
            triProjected.p[0].y *= 0.5f * (float)sHeight;
            triProjected.p[1].x *= 0.5f * (float)sWidth;
            triProjected.p[1].y *= 0.5f * (float)sHeight;
            triProjected.p[2].x *= 0.5f * (float)sWidth;
            triProjected.p[2].y *= 0.5f * (float)sHeight;


            triProjected.color = shadedColor;
            triangleQueue.push_back(triProjected);

        }
    }

    // Sort triangles for painter algorithm
    sort(triangleQueue.begin(), triangleQueue.end(), [](Tools3D::Triangle &t1, Tools3D::Triangle &t2){
        float z1 = (t1.p[0].z + t1.p[1].z + t1.p[2].z) / 3.0f;
        float z2 = (t2.p[0].z + t2.p[1].z + t2.p[2].z) / 3.0f;
        return z1 > z2;
    });

    for(auto &triProjected : triangleQueue){
        // Rasterize triangle
        T3::fillTri(mainImage, triProjected, triProjected.color);
        T3::drawTri(mainImage, triProjected, T2::Color8(0, 0, 0)); // Draw outline on edges
    }

    update();
}
