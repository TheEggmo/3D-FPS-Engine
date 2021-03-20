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
    connect(sUpdateTimer, SIGNAL(timeout()), this, SLOT(process()));
    int targetFps = 60;
    sUpdateTimer->start(1000/targetFps); // Refresh every 16 msec, which is aprox 60fps


    meshCube.loadFromFile("axis.obj");

    // Initiate the projection Matrix
    matProj = T3::newMatProj(90.0f, (float)sHeight/(float)sWidth, 0.1f, 1000.f);

    // Initiate the InputMap
    Input.addAction("JUMP", Qt::Key_Space);
    Input.addAction("CROUCH", Qt::Key_Control);
    Input.addAction("LEFT");
    Input.addKey("LEFT", Qt::Key_A);
    Input.addKey("LEFT", Qt::Key_Left);
    Input.addAction("RIGHT");
    Input.addKey("RIGHT", Qt::Key_D);
    Input.addKey("RIGHT", Qt::Key_Right);
    Input.addAction("UP");
    Input.addKey("UP", Qt::Key_W);
    Input.addKey("UP", Qt::Key_Up);
    Input.addAction("DOWN");
    Input.addKey("DOWN", Qt::Key_S);
    Input.addKey("DOWN", Qt::Key_Down);
}

void MainWindow::paintEvent(QPaintEvent*){
    QPainter p(this);
    p.drawImage(offset, offset, *mainImage);
}

void MainWindow::process(){
    mainImage->fill(defaultBg); // Wipe the screen
    Input.processInput(); // Update inputs
    movePlayer(); // Move the player
    screenUpdate(); // Transform the 3D space into a 2D image
    update();
}

// The main engine/game loop function, called every frame
void MainWindow::screenUpdate(){
    T3::Mat4x4 matRotZ, matRotX, matTrans, matWorld;

//    fTheta += 0.025f;

    // Create matrices
    // Rotation Z
    matRotZ = T3::newMatRotZ(fTheta * 0.5f);
    // Rotation X
    matRotX = T3::newMatRotX(fTheta);
    // Translation
    matTrans = T3::newMatTrans(0.0f, 0.0f, 16.0f);

//    matWorld = T3::newMatIdentity();
    matWorld = matRotZ * matRotX;
    matWorld = matWorld * matTrans;

    T3::Vector3 up = {0, 1, 0};
    T3::Vector3 target = {0, 0, 1};
    T3::Mat4x4 cameraRotationMatrix = T3::newMatRotY(yaw);
//    lookDir = {0, 0, 1};
    lookDir = target * cameraRotationMatrix;
    target = camera + lookDir;

    T3::Mat4x4 cameraMatrix = T3::matPointAt(camera, target, up);
    T3::Mat4x4 viewMatrix = T3::matQuickInverse(cameraMatrix);

    // Store triangles for drawing later
    std::vector<T3::Triangle> triangleQueue;

    // Transform triangles
    for (auto tri : meshCube.tris) {
        T3::Triangle triTransformed, triProjected, triViewed;

        triTransformed = tri * matWorld;

        // Use cross-product to get surface normal
        T3::Vector3 normal, line1, line2;

        // Get two lines of the triangle
        line1 = triTransformed.p[1] - triTransformed.p[0];
        line2 = triTransformed.p[2] - triTransformed.p[0];

        // Get the normal of the triangle surface and normalise it
        normal = T3::crossProduct(line1, line2);
        normal = T3::normalise(normal);

        // If the ray from triangle to camera is alighed with the normal, the triangle is visible
        T3::Vector3 cameraRay = triTransformed.p[0] - camera;
        if(T3::dotProduct(normal, cameraRay) < 0){
            // Add a light
            T3::Vector3 lightDirection = { 0.0f, 0.0f, -1.0f};
            lightDirection = T3::normalise(lightDirection);

            // Calculate the dot product of the light source and the normal to determine the intensity of shading/illumination
            float dp = std::max(0.1f, T3::dotProduct(normal, lightDirection));
            T2::Color8 shadedColor;
            shadedColor = drawingColor * dp;


            // Convert world space to view space
            triViewed = triTransformed * viewMatrix;

            // Project from 3D to 2D
//            triProjected = triTransformed * matProj;
            triProjected = triViewed * matProj;

            triProjected.p[0] = triProjected.p[0] / triProjected.p[0].w;
            triProjected.p[1] = triProjected.p[1] / triProjected.p[1].w;
            triProjected.p[2] = triProjected.p[2] / triProjected.p[2].w;

            // Fix inverted x/y
            triProjected.p[0].x *= -1.0f;
            triProjected.p[1].x *= -1.0f;
            triProjected.p[2].x *= -1.0f;
            triProjected.p[0].y *= -1.0f;
            triProjected.p[1].y *= -1.0f;
            triProjected.p[2].y *= -1.0f;

            // Scale into view
            T3::Vector3 viewOffset = {1, 1, 0};
            triProjected.p[0] = triProjected.p[0] + viewOffset;
            triProjected.p[1] = triProjected.p[1] + viewOffset;
            triProjected.p[2] = triProjected.p[2] + viewOffset;

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
        // Rasterize triangles
        T3::fillTri(mainImage, triProjected, triProjected.color);
        T3::drawTri(mainImage, triProjected, T2::Color8(0, 0, 0)); // Draw outline on edges
    }
}

// Capture keyboard inputs and store them in the InputMap
void MainWindow::keyPressEvent(QKeyEvent *event){
    if(event->isAutoRepeat()) return;
    int keyCode = event->key();
    Input.pressKey(keyCode);
}
void MainWindow::keyReleaseEvent(QKeyEvent *event){
    if(event->isAutoRepeat()) return;
    int keyCode = event->key();
    Input.releaseKey(keyCode);
}

// Change player position based on inputs
// This is basically tank controls, w/d to move forward/backward, a/d to rotate, space/ctrl to move higher/lower
void MainWindow::movePlayer(){
    float jumpSpeed = 0.4;
    float moveSpeed = 0.1;

    T3::Vector3 forward = lookDir * moveSpeed;

    if(Input.isActionPressed("UP")){
        camera = camera + forward;
    }
    if(Input.isActionPressed("DOWN")){
        camera = camera - forward;
    }
    if(Input.isActionPressed("LEFT")){
        yaw -= 0.02;
    }
    if(Input.isActionPressed("RIGHT")){
        yaw += 0.02;
    }
    if(Input.isActionPressed("JUMP")){
        camera.y += jumpSpeed;
    }
    if(Input.isActionPressed("CROUCH")){
        camera.y -= jumpSpeed;
    }
}
