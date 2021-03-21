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
//    void mouseMoveEvent(QMouseEvent *event) override;
//    void mouseReleaseEvent(QMouseEvent *event) override;

    // Keyboard keys are caputed during these events and stored in the inputmap
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    // Functions called every frame by process()
    // They update the game state, i.e. visuals, positions, inputs
    void movePlayer(); // Updates player position based on input
    void screenUpdate(); // Updates the screen by transforming the 3D world

    Tools3D::Mesh meshCube;
    Tools3D::Mat4x4 matProj; // Projection matrix

    Tools3D::Vector3 camera; // Vector representing the camera
    Tools3D::Vector3 lookDir;
    float yaw = 0;

    float fTheta = 0;

public slots:
    void process(); // The "frame" function, processes logic, graphics etc
};
#endif // MAINWINDOW_H
