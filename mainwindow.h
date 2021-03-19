#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <tools.h>
#include <tools3d.h>
#include <QTimer>
#include <QKeyEvent>

class MainWindow : public QWidget{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    void paintEvent(QPaintEvent *event) override;

    int sWidth, sHeight;

    QTimer *sUpdateTimer;

protected:
//    void mousePressEvent(QMouseEvent *event) override;
//    void mouseMoveEvent(QMouseEvent *event) override;
//    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    Tools3D::Mesh meshCube;
    Tools3D::Mat4x4 matProj; // Projection matrix

    Tools3D::Vector3 camera; // Vector representing the camera
    Tools3D::Vector3 lookDir;
    float yaw = 0;

    float fTheta = 0;

public slots:
    void screenUpdate();
};
#endif // MAINWINDOW_H
