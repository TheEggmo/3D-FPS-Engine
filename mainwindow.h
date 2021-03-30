#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <tools.h>
#include <tools3d.h>
#include <QTimer>
#include <QKeyEvent>
#include <string>
#include <inputmap.h>
#include <chrono>
#include <ctime>
#include <QCursor>

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
    void processPhysics();

    float clamp(float in, float lo, float hi);

    Tools3D::MeshTexture meshCube;
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

    QImage *cubeTexture;
//    float *depthBuffer;
    std::vector<float> depthBuffer;

public slots:
    void process(); // The "frame" function, processes logic, graphics etc.
};
#endif // MAINWINDOW_H
