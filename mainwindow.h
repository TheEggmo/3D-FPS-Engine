#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QTimer>
#include <QKeyEvent>
#include <QCursor>

#include <string>
#include <chrono>
#include <ctime>
#include <utility>

#include <tools.h>
#include <tools3d.h>
#include <inputmap.h>
#include <actor.h>

using string = std::string;

class MainWindow : public QWidget{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    void paintEvent(QPaintEvent *event) override;

    int sWidth, sHeight;

    QTimer *processTimer; // Used to call the process() function every frame

    InputMap Input;

protected:
//    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
//    void mouseReleaseEvent(QMouseEvent *event) override;

    // Keyboard keys are caputed during these events and stored in the inputmap
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    // Functions called every frame by process()
    // They update the game state, i.e. visuals, positions, inputs
    void movePlayer(); // Updates player position based on input
    void screenUpdate(); // Updates the screen by transforming the 3D world
    void processActors();

    float clamp(float in, float lo, float hi);
    float lerp(float from, float to, float mod);

    Tools3D::MeshTexture meshCube; // DELETE LATER, ONLY FOR DEBBUGING
    Tools3D::Mat4x4 matProj; // Projection matrix

    Tools3D::Vector3 camera; // Vector representing the camera
    Tools3D::Vector3 lookDir;
    float yaw, pitch;

    float fTheta = 0;

    // Used to calculate delta time (time since last process() call)
    std::chrono::time_point<std::chrono::system_clock> lastFrameTime;
    float delta; // Delta time

    Tools::Vector2 lastMousePos = {0, 0};
    QCursor cursor;

//    float *depthBuffer;
//    std::vector<std::pair<float, Tools::Color8>> depthBuffer;
    std::vector<float> depthBuffer;

    std::vector<Actor*> actorList;
    void addActor(Actor a);
    void addActor(ActorDynamic a);


    // Apply projection based on input parameters.
    // tri is the triangle being projected.
    // transformMatrix is a transformation that will be applied to the triangle.
    // camera is the player camera
    // viewMatrix is a transformation based on player camera
    // All output triangles are put in outputQueue.
    void projectTriangle(Tools3D::Triangle tri, Tools3D::Mat4x4 transformMatrix,
                         Tools3D::Vector3 camera, Tools3D::Mat4x4 viewMatrix,
                         std::vector<Tools3D::Triangle> *outputQueue, QImage *texture);
    // Same as above, but ignore texture and UV data
    void projectTriangle(Tools3D::Triangle tri, Tools3D::Mat4x4 transformMatrix,
                         Tools3D::Vector3 camera, Tools3D::Mat4x4 viewMatrix,
                         std::vector<Tools3D::Triangle> *outputQueue);

public slots:
    void process(); // The "frame" function, processes logic, graphics etc.
};
#endif // MAINWINDOW_H
