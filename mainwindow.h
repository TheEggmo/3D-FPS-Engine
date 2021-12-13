#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QTimer>
#include <QKeyEvent>
#include <QCursor>
#include <QCloseEvent>

#include <string>
#include <chrono>
#include <ctime>
#include <utility>
#include <vector>
#include <utility> //pair()
#include <thread>
#include <mutex>

#include <tools.h>
#include <tools3d.h>
#include <inputmap.h>
#include <actor.h>
#include <remote.h>

using string = std::string;

class MainWindow : public QWidget{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    void paintEvent(QPaintEvent *event) override;

    int sWidth, sHeight;

    InputMap Input;

    Remote remote;

    // Returns a copy of the actorList
    std::vector<Actor*> getActorList();

protected:
    QTimer *processTimer; // Used to call the process() function every frame

    // Multithreading stuff
    bool useMT = true;
    bool useShadingMT = true;
    bool useProjectionMT = true;
    bool usedDrawingMT = true;
    // Thread counts
    unsigned int overrideTC = 0; // Set other TCs to this if above 0
    unsigned int shadingTC = 100;
    unsigned int projectionTC = 100;
    unsigned int drawingTC = 1;
    // Storage vectors
    std::vector<std::thread> shadingThreads;
    std::vector<std::thread> projectionThreads;
    std::vector<std::thread> drawingThreads;
    // Mutex
    std::mutex outputQueueMutex;

    void closeEvent(QCloseEvent *event) override;
//    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
//    void mouseReleaseEvent(QMouseEvent *event) override;

    // Keyboard keys are caputed during these events and stored in the inputmap
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    // Functions called every frame by process()
    // They update the game state, i.e. visuals, positions, inputs
    void cameraUpdate(); // Updates the camera position
    void screenUpdate(); // Updates the screen by transforming the 3D world
    void processActors();

    Tools3D::MeshTexture meshCube; // DELETE LATER, ONLY FOR DEBBUGING
    Tools3D::Mat4x4 matProj; // Projection matrix

    Tools3D::Vector3 camera; // Vector representing the camera
    Tools3D::Vector3 lookDir;
    float yaw, pitch;
    bool camFollow = true;

    float fTheta = 0;

    // Used to calculate delta time (time since last process() call)
    std::chrono::time_point<std::chrono::system_clock> lastFrameTime;
    float delta; // Delta time

    Tools::Vector2 lastMousePos = {0, 0};
    QCursor cursor;

    std::vector<float> depthBuffer;

    std::vector<Actor*> actorList;

    bool showColliders = false;

    // Apply projection based on input parameters.
    // tri is the triangle being projected.
    // transformMatrix is a transformation that will be applied to the triangle.
    // camera is the player camera
    // viewMatrix is a transformation based on player camera
    // All output triangles are put in outputQueue.
    inline void projectTriangle(Tools3D::Triangle tri, Tools3D::Mat4x4 transformMatrix,
                         Tools3D::Vector3 camera, Tools3D::Mat4x4 viewMatrix,
                         std::vector<Tools3D::Triangle> *outputQueue, QImage *texture);
    // Same as above, but ignore texture and UV data
    inline void projectTriangle(Tools3D::Triangle tri, Tools3D::Mat4x4 transformMatrix,
                         Tools3D::Vector3 camera, Tools3D::Mat4x4 viewMatrix,
                         std::vector<Tools3D::Triangle> *outputQueue);
    void projectTriangles(std::vector<Tools3D::Triangle> tris, Tools3D::Mat4x4 transformMatrix,
                          Tools3D::Vector3 camera, Tools3D::Mat4x4 viewMatrix,
                          std::vector<Tools3D::Triangle> *outputQueue);
    void projectTriangleThread(unsigned int threadID,
                               std::vector<Tools3D::Triangle> tris, Tools3D::Mat4x4 transformMatrix,
                               Tools3D::Vector3 camera, Tools3D::Mat4x4 viewMatrix,
                               std::vector<Tools3D::Triangle> *outputQueue);

    int remoteActorIdx = 0;

    std::vector<T3::Triangle> trianglePool;
    std::vector<T3::Triangle*> trianglePoolPointers;
    std::vector<std::pair<ActorLight*, std::vector<Tools3D::Triangle*>>> lightPairs;

    std::vector<ActorLight*> lightPointers;
    void shadeTriangle(int triIdx, T3::Vector3 lightPoint, std::vector<T3::Triangle*> tPointers);
    void castShadows(std::vector<ActorLight*> lights);
    void castShadowsThread(unsigned int threadID);
public slots:
    void process(); // The "frame" function, processes logic, graphics etc.
    void setRemoteActorIdx(int index); // Sets which actor's data will be sent to the remote

    void remoteCastShadows();

    Actor* addActor(ActorStatic a);
    Actor* addActor(ActorPlayer a);
    Actor* addActor(ActorLight a);
};
#endif // MAINWINDOW_H
