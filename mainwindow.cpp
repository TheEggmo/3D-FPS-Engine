#include "mainwindow.h"
//#include <Tools.h>

#include <vector>
#include <algorithm>
#include <list>
#include <typeinfo>
#include <cstdlib> // rand()

#include <QDebug>

using T2 = Tools;
using T3 = Tools3D;

enum MButton{NONE, LEFT, RIGHT, MIDDLE};

QImage *mainImage, *storedImage;

MButton heldButton; // Stores which button was pressed when mousePressEvent was called

int startX, startY; // Stores the x and y coordinates of the mouse when mousePressEvent is called
int offset = 0; // Shifts the drawn image and mouse events by this offset

const int targetFps = 60;

T2::Color8 drawingColor;
Qt::GlobalColor defaultBg = Qt::black;

MainWindow::MainWindow(QWidget *parent): QWidget(parent){
    setWindowTitle("3D Engine");

    // Set the screen dimensions
    sWidth = 1280;
    sHeight = 720;
    resize(sWidth + 2*offset, sHeight + 2*offset);

    mainImage = new QImage(sWidth, sHeight, QImage::Format_RGB32);
    mainImage->fill(defaultBg);

    drawingColor = T2::Color8(255, 0, 0);

    // Setup mouse tracking/camera movement
    setMouseTracking(true);
    cursor = QCursor(Qt::BlankCursor);
    setCursor(cursor);
    yaw = pitch = 0.1f;

    // Create a timer that will call process() every frame
    processTimer = new QTimer(this);
    processTimer->setSingleShot(false);
    connect(processTimer, SIGNAL(timeout()), this, SLOT(process()));
    // Refresh every 16 msec, which is aprox 60fps
    processTimer->start(1000/targetFps);

    // Multithreading
    if(overrideTC > 0){
        shadingTC = overrideTC;
        projectionTC = overrideTC;
        drawingTC = overrideTC;
    }

    // Create actors
    // Player
    ActorPlayer player = ActorPlayer(&Input, &lookDir);
    player.setCollision(T3::AABB({-1, -1, -1}, {2, 2, 2}));
    player.name = "Player";
    player.position = {0, 100, 0};
    addActor(player);

    // Terrain
    T3::MeshTexture meshHub;
    meshHub.loadFromFile("Assets/Artisans Hub.obj");
    meshHub.texture = new QImage("Assets/Artisans Hub.png");
    ActorStatic hub;
    hub.setModel(meshHub);
    hub.visible = true;
    hub.setCollision(T3::AABB(meshHub));
    hub.name = "Hub";
    addActor(hub);


//    T3::MeshTexture meshCapsule;
//    meshCapsule.loadFromFile("Assets/capsule.obj");
//    meshCapsule.texture = new QImage("Assets/debugWhite.png");
//   meshCapsule.texture = new QImage("Assets/capsule.jpg");
//    meshCapsule.scaleX(100);
//    meshCapsule.scaleY(10);
//    meshCapsule.scaleZ(100);
//    ActorStatic capsule;
//    capsule.setModel(meshCapsule);
//    capsule.name = "Capsule";
//    capsule.position = {0, -10, 70};
//    addActor(capsule);

//    T3::MeshTexture meshCube;
//    meshCube.loadFromFile("Assets/cube.obj");
//    meshCube.texture = new QImage("Assets/debugWhite.png");
//    meshCube.scaleX(30);
//    meshCube.scaleZ(30);
//    ActorStatic cube;
//    cube.setModel(meshCube);
//    cube.name = "Cube";
//    cube.position = {0, 30, 0};
//    addActor(cube);


   // Add randomly placed light actors
    int lightCount = 1;
    srand(time(NULL));
    for(int i = 0; i < lightCount; i++){
       ActorLight light;
       light.name = "Light" + std::to_string(i);
//       light.position = {0, 60, 0};
       light.position = {float(rand() % 1000) / 10.0f,
                          float(rand() % 1000) / 10.0f + 10,
                          float(rand() % 1000) / 10.0f - 100};
       // Lights don't actually have collisions, only for debugging purposes
       light.setCollision(T3::AABB({-0.5, -0.5, -0.5}, {1, 1, 1}));
       lightPointers.push_back((ActorLight*)addActor(light));
    }

    castShadows(lightPointers);

    // Create the projection Matrix
    matProj = T3::newMatProj(90.0f, (float)sHeight/(float)sWidth, 0.1f, 1000.f);

    // Create the InputMap
    Input.addAction("JUMP", Qt::Key_Space);
    Input.addAction("SPRINT", Qt::Key_Control);
    Input.addAction("LEFT", Qt::Key_A);
    Input.addAction("RIGHT", Qt::Key_D);
    Input.addAction("UP", Qt::Key_W);
    Input.addAction("DOWN", Qt::Key_S);

    Input.addAction("TOGGLE_CAMFOLLOW", Qt::Key_Shift);
    Input.addAction("CAMJUMP", Qt::Key_PageUp);
    Input.addAction("CAMCROUCH", Qt::Key_PageDown);
    Input.addAction("CAMUP", Qt::Key_Up);
    Input.addAction("CAMDOWN", Qt::Key_Down);
    Input.addAction("CAMLEFT", Qt::Key_Left);
    Input.addAction("CAMRIGHT", Qt::Key_Right);

    Input.addAction("START_REMOTE", Qt::Key_F1);

    // Create the depth buffer
    depthBuffer.resize(sWidth * sHeight, 0.0f);

    // Prepare threads
    projectionThreads.resize(projectionTC);

}

void MainWindow::paintEvent(QPaintEvent*){
    QPainter p(this);
    p.drawImage(offset, offset, *mainImage);
}

std::vector<Actor*> MainWindow::getActorList(){
    std::vector<Actor*> out = actorList;
    return out;
}

void MainWindow::closeEvent(QCloseEvent *event){
//    // Wait for threads
//    for(int i = 0; i < shadingTC; i++){
//        shadingThreads[i].join();
//    }

    // Close remote
    remote.close();

    event->accept();
}

void MainWindow::process(){
    mainImage->fill(defaultBg); // Clear the screen
    Input.processInput(); // Update inputs
    cameraUpdate(); // Move the player
    processActors(); // Process actor logic and collisions
    screenUpdate(); // Transform the 3D space into a 2D image
    update();

    // Track how long each frame takes(for physics and debugging)
    auto newFrameTime = std::chrono::system_clock::now();
    std::chrono::duration<double> frameTime = newFrameTime - lastFrameTime;
    delta = frameTime.count();
    lastFrameTime = newFrameTime;
    if(Input.isActionJustPressed("START_REMOTE")){
        // MOVE THIS TO A SEPARATE FUNCTION
        if(!remote){
            // If remote is not started, start it
            qDebug("Starting remote");
            remote.start();
            connect(&remote, SIGNAL(updateRemoteActor(int)), this, SLOT(setRemoteActorIdx(int)));
            connect(&remote, SIGNAL(recastShadows()), this, SLOT(remoteCastShadows()));

            connect(&remote, SIGNAL(addActor(ActorStatic)), this, SLOT(addActor(ActorStatic)));
            connect(&remote, SIGNAL(addActor(ActorPlayer)), this, SLOT(addActor(ActorPlayer)));
            connect(&remote, SIGNAL(addActor(ActorLight)), this, SLOT(addActor(ActorLight)));
        }else{
            remote.raise();
            remote.activateWindow();
        }
    }

    if(remote){
        remote.trackDeltas(delta);

        // Send actor information to the remote
        // If MainWindow loses focus, stop sending data
        // This creates a snapshot for easier editing/saving of actor data
        if(isActiveWindow()){
            remote.updateActorInfo(actorList[remoteActorIdx]);
            remote.update();
        }
    }

//    if(bool lightFollowPlayer = false){
//        light1->position = actorList[0]->position;
//    }
}

void MainWindow::setRemoteActorIdx(int index){
    remoteActorIdx = index;
    remote.updateActorInfo(actorList[remoteActorIdx]);
}

// The main engine/game loop function, called every frame
void MainWindow::screenUpdate(){
    T3::Mat4x4 matRotZ, matRotX, matTrans, matWorld;

    for(int i = 0; i < sWidth * sHeight; i++) depthBuffer[i] = 0.0f; // Clear the depth buffer

    // Store triangles for drawing later
    std::vector<T3::Triangle> triangleQueue;
    triangleQueue.clear();
    // Wireframe triangles, for debugging purposes
    std::vector<T3::Triangle> wireframeQueue;
    wireframeQueue.clear();

    T3::Vector3 up = {0, 1, 0};
    T3::Vector3 target = {0, 0, 1};

    matWorld = T3::newMatIdentity();

    T3::Mat4x4 cameraRotationMatrixY = T3::newMatRotY(yaw);
    T3::Mat4x4 cameraRotationMatrixX = T3::newMatRotX(pitch);
    lookDir = target * cameraRotationMatrixX;
    lookDir = lookDir * cameraRotationMatrixY;
    target = camera + lookDir;

    T3::Mat4x4 cameraMatrix = T3::matPointAt(camera, target, up);
    T3::Mat4x4 viewMatrix = T3::matQuickInverse(cameraMatrix);

    // Process triangles from trianglePool and place them in triangleQueue for drawing
    // Prepare threads
    if(useMT && useProjectionMT){
        projectionThreads.resize(shadingTC);
        for(int threadID = 0; threadID < projectionTC; threadID++){
            projectionThreads[threadID] = std::thread(&MainWindow::projectTriangleThread, this,
                                                      threadID, trianglePool, matWorld,
                                                      camera, viewMatrix, &triangleQueue);
        }
        for(int threadID = 0; threadID < projectionTC; threadID++){
            projectionThreads[threadID].join();
            //qDebug("Projection thread %d finished", threadID);
        }
    }else{
        for(T3::Triangle tri : trianglePool){
            projectTriangle(tri, matWorld, camera, viewMatrix, &triangleQueue, tri.texture);
        }
    }
    // END PROJECTION MULTITHREADING

    // Project collision meshes' triangles for wireframe drawing
    if(remote.colWireEnabled()){
        for(int i = 0; i < actorList.size(); i++){
            Actor *a = actorList[i];
            if(a->collisionEnabled){
                if(camFollow && i == 0) continue; // Don't render player collider when camera is attached
                T3::Mesh collider = a->getCollider().toMesh(a->position);
                for(int i = 0; i < collider.tris.size(); i++){
                    projectTriangle(collider.tris[i], matWorld, camera, viewMatrix, &wireframeQueue);
                }
            }
        }
    }

    // START DRAWING MULTITHREADING
    // Draw models
    if(useMT && useDrawingMT){
        drawingThreads.resize(drawingTC);
        for(int threadID = 0; threadID < drawingTC; threadID++){
            drawingThreads[threadID] = std::thread(&MainWindow::processDrawingQueueThread, this,
                                                  &triangleQueue, threadID);
        }
        for(int threadID = 0; threadID < drawingTC; threadID++){
            drawingThreads[threadID].join();
        }
    }else{
        processDrawingQueue(&triangleQueue);
    }
    // END DRAWING MULTITHREADING

    // Draw collision wireframes, if enabled
    for(auto &tri : wireframeQueue){
        // Clip triangles against screen edges(walls of the view frustrum)
        T3::Triangle clipped[2];
        std::list<T3::Triangle> cTriangleQueue; // Queue of triangles for clipping
        cTriangleQueue.push_back(tri);
        int newTriangles = 1;

        for(int p = 0; p < 4; p++){
            int trianglesToAdd = 0;
            while(newTriangles > 0){
                T3::Triangle test = cTriangleQueue.front();
                cTriangleQueue.pop_front();
                newTriangles--;

                switch(p){
                case 0:
                    trianglesToAdd = T3::clipTriangle({0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
                                                      test, clipped[0], clipped[1]);
                    break;
                case 1:
                    trianglesToAdd = T3::clipTriangle({0.0f, (float)sHeight - 1, 0.0f}, {0.0f, -1.0f, 0.0f},
                                                      test, clipped[0], clipped[1]);
                    break;
                case 2:
                    trianglesToAdd = T3::clipTriangle({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
                                                      test, clipped[0], clipped[1]);
                    break;
                case 3:
                    trianglesToAdd = T3::clipTriangle({(float)sWidth - 1, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},
                                                      test, clipped[0], clipped[1]);
                    break;
                }

                // Add newly created triangles to the queue for clipping against next planes
                for(int w = 0; w < trianglesToAdd; w++){
                    cTriangleQueue.push_back(clipped[w]);
                }
            }
            newTriangles = cTriangleQueue.size();
        }

        // Draw triangles
        for(T3::Triangle &tri : cTriangleQueue){
            T3::drawTri(mainImage, tri, T2::Color8(255, 255, 255));
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
    if(!isActiveWindow()) return;

    T2::Vector2 sCenter = T2::Vector2(sWidth/2, sHeight/2);
    QPoint g = this->mapFromGlobal(QPoint(0, 0));
    sCenter.x -= g.x();
    sCenter.y -= g.y();
    float sensMod = 0.001; // Modifies the sensitivity of the mouse

    T2::Vector2 mouseDiff = T2::Vector2(cursor.pos().x(), cursor.pos().y()) - sCenter;
    yaw += mouseDiff.x * sensMod;
//    yaw = // Add wrapping?
    pitch += mouseDiff.y * sensMod;
    pitch = T2::clamp(pitch, -3.14/2.0, 3.14/2.0); // Needs to be clamped so that the camera won't go upside down
    cursor.setPos(sCenter.x, sCenter.y); // Center the cursor
}

// Capture keyboard inputs and store them in the InputMap
void MainWindow::keyPressEvent(QKeyEvent *event){
    if(event->isAutoRepeat()) return;
    int keyCode = event->key();
    Input.pressKey(keyCode);

    // Toggle mouse tracking
    if(event->key() == Qt::Key_Escape) setMouseTracking(!hasMouseTracking());
    // Toggle player noclip
    if(event->key() == Qt::Key_Q)
        actorList[0]->collisionEnabled = !actorList[0]->collisionEnabled;
}
void MainWindow::keyReleaseEvent(QKeyEvent *event){
    if(event->isAutoRepeat()) return;
    int keyCode = event->key();
    Input.releaseKey(keyCode);
}

void MainWindow::cameraUpdate(){
    // Camera can be attached to an Actor or controlled directly
    // If camFollow is true, the camera will follow the player

    if(Input.isActionJustPressed("TOGGLE_CAMFOLLOW"))
        camFollow = !camFollow;

    if(camFollow){
        ActorPlayer *player = (ActorPlayer *)actorList[0];
        camera = player->position;
    }else{
        // Calculate directions for movement relative to camera rotation
        float speed = 1 + int(Input.isActionPressed("SPRINT"));

        T3::Vector3 forward = lookDir * speed;
        forward.y = 0;
        T3::Vector3 right = forward * T3::newMatRotY(3.14/2);


        if(Input.isActionPressed("CAMUP")){
            camera += forward * speed;
        }
        if(Input.isActionPressed("CAMDOWN")){
            camera -= forward * speed;
        }
        if(Input.isActionPressed("CAMLEFT")){
            camera -= right * speed;
        }
        if(Input.isActionPressed("CAMRIGHT")){
            camera += right * speed;
        }
        if(Input.isActionPressed("CAMJUMP")){
            camera.y += speed;
        }
        if(Input.isActionPressed("CAMCROUCH")){
            camera.y -= speed;
        }
    }
}

void MainWindow::processActors(){
    // Process logic
    for(int i = 0; i < actorList.size(); i++){
        if(actorList[i]->logicEnabled){
            actorList[i]->processLogic();
        }
    }

    // Process collisions
    for(int i = 0; i < actorList.size(); i++){
        // For every actor with enabled collisions gather other actors with enabled collisions,
        // put them on a queue, and call the actor processCollision()
        // Actors at the beggining will have their collisions resolved first
        // Since player is at index 0, they will always have priority
        std::vector<T3::AABB> colliders;
        colliders.clear();
        // Get colliders from all other actors
        for(int j = 0; j < actorList.size(); j++){
            if(i != j && actorList[j]->collisionEnabled){
                colliders.push_back(actorList[j]->getCollider());
            }
        }
        actorList[i]->processCollision(colliders);
    }
}

Actor* MainWindow::addActor(ActorStatic a){
    actorList.push_back(new ActorStatic);
    *(ActorStatic*)actorList.back() = a;

    std::vector<string> names;
    for(Actor *a : actorList){
        names.push_back(a->name);
    }
    remote.updateActorSelect(names);

    return actorList.back();
}

Actor* MainWindow::addActor(ActorPlayer a){
    actorList.push_back(new ActorPlayer);
    *(ActorPlayer*)actorList.back() = a;

    std::vector<string> names;
    for(Actor *a : actorList){
        names.push_back(a->name);
    }
    remote.updateActorSelect(names);

    return actorList.back();
}

Actor* MainWindow::addActor(ActorLight a){
    actorList.push_back(new ActorLight);
    *(ActorLight*)actorList.back() = a;

    std::vector<string> names;
    for(Actor *a : actorList){
        names.push_back(a->name);
    }
    remote.updateActorSelect(names);

    return actorList.back();
}

void MainWindow::projectTriangleThread(unsigned int threadID,
                                       std::vector<Tools3D::Triangle> tris, Tools3D::Mat4x4 transformMatrix,
                                       Tools3D::Vector3 camera, Tools3D::Mat4x4 viewMatrix,
                                       std::vector<Tools3D::Triangle> *outputQueue){
    std::vector<Tools3D::Triangle> outputQueueTemp;
    // Use textures
    for(int i = threadID; i < tris.size(); i += projectionTC){
        projectTriangle(tris[i], transformMatrix, camera, viewMatrix, &outputQueueTemp, tris[i].texture);
    }
    std::lock_guard<std::mutex> guard(outputQueueMutex);
    outputQueue->insert(outputQueue->end(), outputQueueTemp.begin(), outputQueueTemp.end());
}

inline void MainWindow::projectTriangle(Tools3D::Triangle tri, Tools3D::Mat4x4 transformMatrix,
                                 Tools3D::Vector3 camera, Tools3D::Mat4x4 viewMatrix,
                                 std::vector<Tools3D::Triangle> *outputQueue, QImage *texture){

    T3::Triangle triTransformed, triViewed, triProjected;

    triTransformed = tri * transformMatrix;
    triTransformed.t[0] = tri.t[0];
    triTransformed.t[1] = tri.t[1];
    triTransformed.t[2] = tri.t[2];

    // Use cross-product to get surface normal
    T3::Vector3 normal, line1, line2;

    // Get two lines of the triangle
    line1 = triTransformed.p[1] - triTransformed.p[0];
    line2 = triTransformed.p[2] - triTransformed.p[0];

    // Get the normal of the triangle surface and normalize it
    normal = line1.crossProduct(line2);
    normal = normal.normalize();

    // If the ray from triangle to camera is aligned with the normal, the triangle is visible
    T3::Vector3 cameraRay = triTransformed.p[0] - camera;
    if(normal.dotProduct(cameraRay) < 0){

        // Convert world space to view space
        triViewed = triTransformed * viewMatrix;
        triViewed.t[0] = triTransformed.t[0];
        triViewed.t[1] = triTransformed.t[1];
        triViewed.t[2] = triTransformed.t[2];


        // Clip viewed triangle against the near plane
        // This could form two additional triangles
        int clippedTriangles = 0;
        T3::Triangle clipped[2];
        clippedTriangles = T3::clipTriangle({0.0f, 0.0f, 0.1f}, {0.0f, 0.0f, 1.0f}, triViewed, clipped[0], clipped[1]); // TODO: REPLACE THE VECTOR CONSTRUCTOR WITH A VARIABLE

        for(int i = 0; i < clippedTriangles; i++){

            // Project from 3D to 2D
            triProjected = clipped[i] * matProj;

            // Copy uv data from old triangle to new one
            triProjected.t[0] = clipped[i].t[0];
            triProjected.t[1] = clipped[i].t[1];
            triProjected.t[2] = clipped[i].t[2];

            // Apply perspective to texture coordinates
            triProjected.t[0].u = triProjected.t[0].u / triProjected.p[0].w;
            triProjected.t[1].u = triProjected.t[1].u / triProjected.p[1].w;
            triProjected.t[2].u = triProjected.t[2].u / triProjected.p[2].w;

            triProjected.t[0].v = triProjected.t[0].v / triProjected.p[0].w;
            triProjected.t[1].v = triProjected.t[1].v / triProjected.p[1].w;
            triProjected.t[2].v = triProjected.t[2].v / triProjected.p[2].w;

            triProjected.t[0].w = 1.0f / triProjected.p[0].w;
            triProjected.t[1].w = 1.0f / triProjected.p[1].w;
            triProjected.t[2].w = 1.0f / triProjected.p[2].w;

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

            // Copy texture data to new triangle
            triProjected.shading = tri.shading;
            triProjected.texture = texture;

            outputQueue->push_back(triProjected);
        }
    }
}

inline void MainWindow::processDrawingQueue(std::vector<Tools3D::Triangle> *input){
    for(auto &tri : *input){
        // Clip triangles against screen edges(walls of the view frustrum)
        T3::Triangle clipped[2];
        std::list<T3::Triangle> cTriangleQueue; // Queue of triangles for clipping
        cTriangleQueue.push_back(tri);
        int newTriangles = 1;

        for(int p = 0; p < 4; p++){
            int trianglesToAdd = 0;
            while(newTriangles > 0){
                T3::Triangle test = cTriangleQueue.front();
                cTriangleQueue.pop_front();
                newTriangles--;

                switch(p){
                case 0:
                    trianglesToAdd = T3::clipTriangle({0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
                                                      test, clipped[0], clipped[1]);
                    break;
                case 1:
                    trianglesToAdd = T3::clipTriangle({0.0f, (float)sHeight - 1, 0.0f}, {0.0f, -1.0f, 0.0f},
                                                      test, clipped[0], clipped[1]);
                    break;
                case 2:
                    trianglesToAdd = T3::clipTriangle({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
                                                      test, clipped[0], clipped[1]);
                    break;
                case 3:
                    trianglesToAdd = T3::clipTriangle({(float)sWidth - 1, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},
                                                      test, clipped[0], clipped[1]);
                    break;
                }

                // Add newly created triangles to the queue for clipping against next planes
                for(int w = 0; w < trianglesToAdd; w++){
                    cTriangleQueue.push_back(clipped[w]);
                }
            }
            newTriangles = cTriangleQueue.size();
        }

        // Finally, draw the modified triangles on the screen
        if(remote.modelWireEnabled()){
            for(T3::Triangle &tri : cTriangleQueue){
                T3::drawTri(mainImage, tri, drawingColor);
            }
        }else{
            for(T3::Triangle &tri : cTriangleQueue){
                T3::textureTri(mainImage, tri, tri.texture, depthBuffer);
            }
        }
    }
}

inline void MainWindow::processDrawingQueueThread(std::vector<Tools3D::Triangle> *input, int threadID){
    for(int i = threadID; i < input->size(); i += drawingTC){
        T3::Triangle tri = input->at(i);

        // Clip triangles against screen edges(walls of the view frustrum)
        T3::Triangle clipped[2];
        std::list<T3::Triangle> cTriangleQueue; // Queue of triangles for clipping
        cTriangleQueue.push_back(tri);
        int newTriangles = 1;

        for(int p = 0; p < 4; p++){
            int trianglesToAdd = 0;
            while(newTriangles > 0){
                T3::Triangle test = cTriangleQueue.front();
                cTriangleQueue.pop_front();
                newTriangles--;

                switch(p){
                case 0:
                    trianglesToAdd = T3::clipTriangle({0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
                                                      test, clipped[0], clipped[1]);
                    break;
                case 1:
                    trianglesToAdd = T3::clipTriangle({0.0f, (float)sHeight - 1, 0.0f}, {0.0f, -1.0f, 0.0f},
                                                      test, clipped[0], clipped[1]);
                    break;
                case 2:
                    trianglesToAdd = T3::clipTriangle({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
                                                      test, clipped[0], clipped[1]);
                    break;
                case 3:
                    trianglesToAdd = T3::clipTriangle({(float)sWidth - 1, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},
                                                      test, clipped[0], clipped[1]);
                    break;
                }

                // Add newly created triangles to the queue for clipping against next planes
                for(int w = 0; w < trianglesToAdd; w++){
                    cTriangleQueue.push_back(clipped[w]);
                }
            }
            newTriangles = cTriangleQueue.size();
        }

        // Finally, draw the modified triangles on the screen
        if(remote.modelWireEnabled()){
            for(T3::Triangle &tri : cTriangleQueue){
                T3::drawTri(mainImage, tri, drawingColor);
            }
        }else{
            for(T3::Triangle &tri : cTriangleQueue){
                std::lock_guard<std::mutex> guard(dBufferMutex);
                T3::textureTri(mainImage, tri, tri.texture, depthBuffer);
            }
        }
    }
}

inline void MainWindow::projectTriangle(Tools3D::Triangle tri, Tools3D::Mat4x4 transformMatrix,
                                 Tools3D::Vector3 camera, Tools3D::Mat4x4 viewMatrix,
                                 std::vector<Tools3D::Triangle> *outputQueue){
    T3::Triangle triTransformed, triViewed, triProjected;

    triTransformed = tri * transformMatrix;
    triTransformed.t[0] = tri.t[0];
    triTransformed.t[1] = tri.t[1];
    triTransformed.t[2] = tri.t[2];

    // Use cross-product to get surface normal
    T3::Vector3 normal, line1, line2;

    // Get two lines of the triangle
    line1 = triTransformed.p[1] - triTransformed.p[0];
    line2 = triTransformed.p[2] - triTransformed.p[0];

    // Get the normal of the triangle surface and normalise it
    normal = line1.crossProduct(line2);
    normal = normal.normalize();

    // If the ray from triangle to camera is aligned with the normal, the triangle is visible
    T3::Vector3 cameraRay = triTransformed.p[0] - camera;
    if(normal.dotProduct(cameraRay) < 0){

        // Convert world space to view space
        triViewed = triTransformed * viewMatrix;

        // Clip viewed triangle against the near plane
        // This could form two additional triangles
        int clippedTriangles = 0;
        T3::Triangle clipped[2];
        clippedTriangles = T3::clipTriangle({0.0f, 0.0f, 0.1f}, {0.0f, 0.0f, 1.0f}, triViewed, clipped[0], clipped[1]); // TODO: REPLACE THE VECTOR CONSTRUCTOR WITH A VARIABLE

        for(int i = 0; i < clippedTriangles; i++){

            // Project from 3D to 2D
            triProjected = clipped[i] * matProj;

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

            outputQueue->push_back(triProjected);
        }
    }
}

void MainWindow::shadeTriangle(int triIdx, T3::Vector3 lightPoint, std::vector<T3::Triangle*> tPointers){
    T3::Triangle* tri = tPointers[triIdx];
    T3::Vector3 iPoint; // Intersection point, unused
    bool visible = true;

    // Check every triangle to see if it blocks the light
    for(int otherIdx = 0; otherIdx < tPointers.size(); otherIdx++){
        if (triIdx == otherIdx) continue;

        // If the ray from the triangle to the light
        // is stopped by the other triangle, mark as not visible
        T3::Vector3 centroid = tri->getCentroid();
        if(T3::rayIntersectsTriangle(centroid,
                                     (lightPoint-centroid),
                                     tPointers[otherIdx],
                                     iPoint)){
            visible = false;
            break;
        }
    }
    // If triangle wasn't discarded, apply shading to it
    if(visible){
        int lightIntensity = 150;
        T3::Vector3 lightDirection = lightPoint.directionTo(tri->getCentroid()) * -1;
        float angle = tri->getNormal().dotProduct(lightDirection);
        float distance = lightPoint.distanceTo(tri->getCentroid());
        tri->shading += std::max(0.0f, angle
                                 * lightIntensity
                                 / distance);

    }
}

void MainWindow::castShadowsThread(unsigned int threadID){
    // For each light...
    for(int i = 0; i < lightPairs.size(); i++){
        // ...shade triangles corresponding to threadID
        for(int triIdx = threadID; triIdx < lightPairs[i].second.size(); triIdx += shadingTC){
            shadeTriangle(triIdx,
                          lightPairs[i].first->position, // position of light i
                          lightPairs[i].second); // pool of triangles paired with light i
        }
    }
}

void MainWindow::castShadows(std::vector<ActorLight *> lights){
    qDebug("Starting castShadows()");

    // Generate a list of all triangles from visible actors
    qDebug("Generating trianglePool");
    trianglePool.clear();
    for(Actor* a : actorList){
        if(a->visible){
            if(a->getType() == Light){
                qDebug("SKIPPING LIGHT");
            }else{
                T3::MeshTexture mesh = a->getModel();
                for(T3::Triangle tri : mesh.tris){
                    tri.texture = mesh.texture;
                    tri.p[0] += a->position;
                    tri.p[1] += a->position;
                    tri.p[2] += a->position;
                    trianglePool.push_back(tri);
                }
            }
        }
    }

    // Generate a list of pointer to triangles in trianglePool
    qDebug("Generating trianglePoolPointers");
    trianglePoolPointers.resize(trianglePool.size());
    for(int i = 0; i < trianglePool.size(); i++){
        trianglePoolPointers[i] = &trianglePool[i];
    }

    // If shading is disabled, apply maximum light to everything and return
    if(!remote.shadingEnabled()){
        qDebug("Shading is disabled, returning");
        for(T3::Triangle *tri : trianglePoolPointers){
            tri->shading = 1;
        }
        return;
    }

    // Create pairs of the ActorLight and a copy of the list of pointers to trianglePool triangles
    qDebug("Generating lightPairs");
    lightPairs.clear();
    for(ActorLight* light : lights){
        std::vector<T3::Triangle*> pointers = trianglePoolPointers;
        lightPairs.push_back(std::make_pair(light, pointers));
    }

    if(useMT && useShadingMT){
        // Start threads
        qDebug("Threads starting");
        shadingThreads.clear();
        shadingThreads.resize(shadingTC);
        for(int id = 0; id < shadingTC; id++){
            shadingThreads[id] = std::thread(&MainWindow::castShadowsThread, this, id);
            shadingThreads[id].detach();
        }
        qDebug("Threads started and detached");
    }else{
        auto start = std::chrono::system_clock::now();
        qDebug("Using main thread");
        // For each light...
        for(int i = 0; i < lightPairs.size(); i++){
            // ...shade triangles
            for(int triIdx = 0; triIdx < lightPairs[i].second.size(); triIdx++){
                shadeTriangle(triIdx,
                              lightPairs[i].first->position, // position of light i
                              lightPairs[i].second); // pool of triangles paired with light i
            }
        }
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<float> time = end - start;
        qDebug("Shading finished. Duration without multithreading: %f", time.count());
    }

    qDebug("castShadows() finished");
}

void MainWindow::remoteCastShadows(){
    qDebug("Remote requested castShadows()");
    castShadows(lightPointers);
}

void MainWindow::redrawScreen(unsigned int threadID){
    T3::Mat4x4 matWorld;

    if(threadID == 0)
        for(int i = 0; i < sWidth * sHeight; i++)
            depthBuffer[i] = 0.0f; // Clear the depth buffer

    std::barrier

    // Store triangles for drawing later
    std::vector<T3::Triangle> triangleQueue;
    triangleQueue.clear();
    // Wireframe triangles, for debugging purposes
    std::vector<T3::Triangle> wireframeQueue;
    wireframeQueue.clear();

    T3::Vector3 up = {0, 1, 0};
    T3::Vector3 target = {0, 0, 1};

    matWorld = T3::newMatIdentity();

    T3::Mat4x4 cameraRotationMatrixY = T3::newMatRotY(yaw);
    T3::Mat4x4 cameraRotationMatrixX = T3::newMatRotX(pitch);
    lookDir = target * cameraRotationMatrixX;
    lookDir = lookDir * cameraRotationMatrixY;
    target = camera + lookDir;

    T3::Mat4x4 cameraMatrix = T3::matPointAt(camera, target, up);
    T3::Mat4x4 viewMatrix = T3::matQuickInverse(cameraMatrix);

    // Process triangles from trianglePool and place them in triangleQueue for drawing
    // Prepare threads
    if(useMT && useProjectionMT){
        projectionThreads.resize(shadingTC);
        for(int threadID = 0; threadID < projectionTC; threadID++){
            projectionThreads[threadID] = std::thread(&MainWindow::projectTriangleThread, this,
                                                      threadID, trianglePool, matWorld,
                                                      camera, viewMatrix, &triangleQueue);
        }
        for(int threadID = 0; threadID < projectionTC; threadID++){
            projectionThreads[threadID].join();
            //qDebug("Projection thread %d finished", threadID);
        }
    }else{
        for(T3::Triangle tri : trianglePool){
            projectTriangle(tri, matWorld, camera, viewMatrix, &triangleQueue, tri.texture);
        }
    }
    // END PROJECTION MULTITHREADING

//    // Project collision meshes' triangles for wireframe drawing
//    if(remote.colWireEnabled()){
//        for(int i = 0; i < actorList.size(); i++){
//            Actor *a = actorList[i];
//            if(a->collisionEnabled){
//                if(camFollow && i == 0) continue; // Don't render player collider when camera is attached
//                T3::Mesh collider = a->getCollider().toMesh(a->position);
//                for(int i = 0; i < collider.tris.size(); i++){
//                    projectTriangle(collider.tris[i], matWorld, camera, viewMatrix, &wireframeQueue);
//                }
//            }
//        }
//    }

    // START DRAWING MULTITHREADING
    // Draw models
    if(useMT && useDrawingMT){
        drawingThreads.resize(drawingTC);
        for(int threadID = 0; threadID < drawingTC; threadID++){
            drawingThreads[threadID] = std::thread(&MainWindow::processDrawingQueueThread, this,
                                                  &triangleQueue, threadID);
        }
        for(int threadID = 0; threadID < drawingTC; threadID++){
            drawingThreads[threadID].join();
        }
    }else{
        processDrawingQueue(&triangleQueue);
    }
    // END DRAWING MULTITHREADING

//    // Draw collision wireframes, if enabled
//    for(auto &tri : wireframeQueue){
//        // Clip triangles against screen edges(walls of the view frustrum)
//        T3::Triangle clipped[2];
//        std::list<T3::Triangle> cTriangleQueue; // Queue of triangles for clipping
//        cTriangleQueue.push_back(tri);
//        int newTriangles = 1;

//        for(int p = 0; p < 4; p++){
//            int trianglesToAdd = 0;
//            while(newTriangles > 0){
//                T3::Triangle test = cTriangleQueue.front();
//                cTriangleQueue.pop_front();
//                newTriangles--;

//                switch(p){
//                case 0:
//                    trianglesToAdd = T3::clipTriangle({0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
//                                                      test, clipped[0], clipped[1]);
//                    break;
//                case 1:
//                    trianglesToAdd = T3::clipTriangle({0.0f, (float)sHeight - 1, 0.0f}, {0.0f, -1.0f, 0.0f},
//                                                      test, clipped[0], clipped[1]);
//                    break;
//                case 2:
//                    trianglesToAdd = T3::clipTriangle({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
//                                                      test, clipped[0], clipped[1]);
//                    break;
//                case 3:
//                    trianglesToAdd = T3::clipTriangle({(float)sWidth - 1, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},
//                                                      test, clipped[0], clipped[1]);
//                    break;
//                }

//                // Add newly created triangles to the queue for clipping against next planes
//                for(int w = 0; w < trianglesToAdd; w++){
//                    cTriangleQueue.push_back(clipped[w]);
//                }
//            }
//            newTriangles = cTriangleQueue.size();
//        }

//        // Draw triangles
//        for(T3::Triangle &tri : cTriangleQueue){
//            T3::drawTri(mainImage, tri, T2::Color8(255, 255, 255));
//        }
//    }
}
