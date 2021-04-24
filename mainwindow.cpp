#include "mainwindow.h"
//#include <Tools.h>

#include <vector>
#include <algorithm>
#include <list>
#include <typeinfo>

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
//    cursor = QCursor(Qt::CrossCursor);
    setCursor(cursor);
    yaw = pitch = 0;

    // Create a timer that will call process() every frame
    processTimer = new QTimer(this);
    processTimer->setSingleShot(false);
    connect(processTimer, SIGNAL(timeout()), this, SLOT(process()));
    // Refresh every 16 msec, which is aprox 60fps
    processTimer->start(1000/targetFps);

    ActorDynamic player;
    player.setCollision(T3::AABB({-1, -1, -1}, {2, 2, 2}));
    player.name = "Player";
    player.position = {0, 100, 0};
    addActor(player);
//    actorList.push_back(&player);
//    actorList.push_back(new Actor);
//    *actorList[0] = player;

    // Load debug/testing model
//    meshCube.loadFromFile("Assets/axis.obj");
//    meshCube.loadFromFile("Assets/watermelon.obj");
//    meshCube.loadFromFile("Assets/watermelon2.obj");
//    meshCube.loadFromFile("Assets/Hurricos.obj");
//    meshCube.loadFromFile("Assets/Artisans Hub.obj");
   T3::MeshTexture meshHub;
   meshHub.loadFromFile("Assets/Artisans Hub.obj");
   meshHub.texture = new QImage("Assets/Artisans Hub.png");
   Actor hub;
   hub.setModel(meshHub);
   hub.visible = true;
   hub.setCollision(T3::AABB(meshHub));
   hub.name = "Hub";
   addActor(hub);
//    meshCube.loadFromFile("Assets/capsule.obj");


//   T3::MeshTexture meshCapsule;
//   meshCapsule.loadFromFile("Assets/capsule.obj");
//   meshCapsule.texture = new QImage("Assets/capsule.jpg");
//   Actor capsule;
//   capsule.setModel(meshCapsule);
//   capsule.visible = false;
//   capsule.name = "Capsule";

//   T3::MeshTexture meshWatermelon;
//   meshWatermelon.loadFromFile("Assets/watermelon2.obj");
//   meshWatermelon.scale(10);
//   meshWatermelon.texture = new QImage("Assets/SMK_JJ0KQAO2_Watermelon_8K_Albedo.png");
//   Actor watermelon;
//   watermelon.setModel(meshWatermelon);
//   watermelon.visible = false;
//   watermelon.name = "Watermelon";



//    meshCube.tris = {

//        // SOUTH
//        { {0.0f, 0.0f, 0.0f, 1.0f},    {0.0f, 1.0f, 0.0f, 1.0f},    {1.0f, 1.0f, 0.0f, 1.0f},		{0.0f, 1.0f},   {0.0f, 0.0f},   {1.0f, 0.0f}, },
//        { {0.0f, 0.0f, 0.0f, 1.0f},    {1.0f, 1.0f, 0.0f, 1.0f},    {1.0f, 0.0f, 0.0f, 1.0f},		{0.0f, 1.0f},   {1.0f, 0.0f}, 	{1.0f, 1.0f}, },

//        // EAST
//        { {1.0f, 0.0f, 0.0f, 1.0f},    {1.0f, 1.0f, 0.0f, 1.0f},    {1.0f, 1.0f, 1.0f, 1.0f},		{0.0f, 1.0f}, 	{0.0f, 0.0f},   {1.0f, 0.0f}, },
//        { {1.0f, 0.0f, 0.0f, 1.0f},    {1.0f, 1.0f, 1.0f, 1.0f},    {1.0f, 0.0f, 1.0f, 1.0f},		{0.0f, 1.0f}, 	{1.0f, 0.0f},   {1.0f, 1.0f}, },

//        // NORTH
//        { {1.0f, 0.0f, 1.0f, 1.0f},    {1.0f, 1.0f, 1.0f, 1.0f},    {0.0f, 1.0f, 1.0f, 1.0f},		{0.0f, 1.0f}, 	{0.0f, 0.0f},   {1.0f, 0.0f}, },
//        { {1.0f, 0.0f, 1.0f, 1.0f},    {0.0f, 1.0f, 1.0f, 1.0f},    {0.0f, 0.0f, 1.0f, 1.0f},		{0.0f, 1.0f}, 	{1.0f, 0.0f}, 	{1.0f, 1.0f}, },

//        // WEST
//        { {0.0f, 0.0f, 1.0f, 1.0f},    {0.0f, 1.0f, 1.0f, 1.0f},    {0.0f, 1.0f, 0.0f, 1.0f},		{0.0f, 1.0f}, 	{0.0f, 0.0f},   {1.0f, 0.0f}, },
//        { {0.0f, 0.0f, 1.0f, 1.0f},    {0.0f, 1.0f, 0.0f, 1.0f},    {0.0f, 0.0f, 0.0f, 1.0f},		{0.0f, 1.0f}, 	{1.0f, 0.0f}, 	{1.0f, 1.0f}, },

//        // TOP
//        { {0.0f, 1.0f, 0.0f, 1.0f},    {0.0f, 1.0f, 1.0f, 1.0f},    {1.0f, 1.0f, 1.0f, 1.0f},		{0.0f, 1.0f}, 	{0.0f, 0.0f},   {1.0f, 0.0f}, },
//        { {0.0f, 1.0f, 0.0f, 1.0f},    {1.0f, 1.0f, 1.0f, 1.0f},    {1.0f, 1.0f, 0.0f, 1.0f},		{0.0f, 1.0f}, 	{1.0f, 0.0f}, 	{1.0f, 1.0f}, },

//        // BOTTOM
//        { {1.0f, 0.0f, 1.0f, 1.0f},    {0.0f, 0.0f, 1.0f, 1.0f},    {0.0f, 0.0f, 0.0f, 1.0f},		{0.0f, 1.0f}, 	{0.0f, 0.0f},   {1.0f, 0.0f}, },
//        { {1.0f, 0.0f, 1.0f, 1.0f},    {0.0f, 0.0f, 0.0f, 1.0f},    {1.0f, 0.0f, 0.0f, 1.0f},		{0.0f, 1.0f}, 	{1.0f, 0.0f}, 	{1.0f, 1.0f}, },
//    };
//    meshCube.scale(10);
//    for(int i = 0; i < meshCube.tris.size(); i++){
//        meshCube.tris[i] = meshCube.tris[i] * T3::newMatTrans(0, 50, 0);
//    }

//    meshCube.flat = true;

//    cubeTexture = new QImage("Assets/dolphin.jpg");
//    cubeTexture = new QImage("Assets/railgunIcon.png");
//    cubeTexture = new QImage("Assets/SMK_JJ0KQAO2_Watermelon_8K_Albedo.png");
//    cubeTexture = new QImage("Assets/Hurricos.png");
//    cubeTexture = new QImage("Assets/Artisans Hub.png");
//    meshCube.texture = new QImage("Assets/SMK_JJ0KQAO2_Watermelon_8K_Albedo.jpg");
//    meshCube.texture = new QImage("Assets/SMK_JJ0KQAO2_Watermelon_8K_Albedo.png");
//    meshCube.texture = new QImage("Assets/capsule.jpg");
//    meshCube.texture = new QImage("Assets/Artisans Hub.png");
//    QImage *cubeTexture = new QImage("Assets/capsule.jpg");


//    meshCube.loadFromFile("Assets/cube.obj");
//    Actor cubeActor;
//    cubeActor.setModel(meshCube);
//    cubeActor.visible = true;
//    cubeActor.name = "Cube";

//    actorList.push_back(&cubeActor);
//    actorList.push_back(&capsule);
//    actorList.push_back(&hub);
//    actorList.push_back(&watermelon);
//    actorList.push_back(new Actor);
//    actorList.push_back(new Actor);
//    actorList.push_back(new ActorStatic);
//    actorList.push_back(new Actor);
//    *actorList[1] = cubeActor;
//    *actorList[2] = capsule;
//    *actorList[3] = hub;
//    *actorList[4] = watermelon;
//    addActor(cubeActor);
//    addActor(capsule);
//    addActor(watermelon);


    // Initiate the projection Matrix
    matProj = T3::newMatProj(90.0f, (float)sHeight/(float)sWidth, 0.1f, 1000.f);

    // Initiate the InputMap
    Input.addAction("JUMP", Qt::Key_Space);
    Input.addAction("CROUCH", Qt::Key_Control);
    Input.addAction("LEFT", Qt::Key_A);
//    Input.addKey("LEFT", Qt::Key_Left);
    Input.addAction("RIGHT", Qt::Key_D);
//    Input.addKey("RIGHT", Qt::Key_Right);
    Input.addAction("UP", Qt::Key_W);
//    Input.addKey("UP", Qt::Key_Up);
    Input.addAction("DOWN", Qt::Key_S);
//    Input.addKey("DOWN", Qt::Key_Down);

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
    // When MainWindow is closed the remote might break due to pointers
    remote.close();
    event->accept();
}

void MainWindow::process(){
    mainImage->fill(defaultBg); // Clear the screen
    Input.processInput(); // Update inputs
    movePlayer(); // Move the player
    processActors(); // Calculate world physics
    screenUpdate(); // Transform the 3D space into a 2D image
    update();

    // Track how long each frame takes(for physics and debugging)
    auto newFrameTime = std::chrono::system_clock::now();
    std::chrono::duration<double> frameTime = newFrameTime - lastFrameTime;
    delta = frameTime.count();
    lastFrameTime = newFrameTime;
    if(Input.isActionJustPressed("START_REMOTE")){
        if(!remote){
            // If remote is not started, start it
            qDebug("Starting remote");
            remote.start();
        }else{
            remote.raise();
            remote.activateWindow();
        }
    }

    if(remote){
//        if(bool displayFrameTime = false) qDebug("Frame Time: %f", frameTime.count());
//        if(bool displayFPS = false) qDebug("FPS: %f", 1/frameTime.count());
        remote.trackDeltas(delta);

    }
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

    // Proces triangles of every visible actor and place them in the triangleQueue for drawin
    for(int i = 0; i < actorList.size(); i++){
        Actor * a = actorList[i];
        // Project triangles for drawing the Actor's model and texture
        if(a->visible){
            T3::MeshTexture model = a->getModel();
//            model.flat = false;
            for(int i = 0; i < model.tris.size(); i++){
                projectTriangle(model.tris[i], matWorld, camera, viewMatrix, &triangleQueue, model.texture);
            }
        }
        // Project triangles for wireframe drawing
//        if(bool showColliders = true && a->collisionEnabled){
        if(remote.wireframeEnabled() && a->collisionEnabled){
            if(camFollow && i == 0) continue;
            T3::Mesh collider = a->getCollider().toMesh(a->position);
            for(int i = 0; i < collider.tris.size(); i++){
                projectTriangle(collider.tris[i], matWorld, camera, viewMatrix, &wireframeQueue);
            }
        }
    }

    // Draw triangles
    for(auto &tri : triangleQueue){
        // Clip triangles against screen edges(walls of the view frustrum
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
        for(T3::Triangle &tri : cTriangleQueue){
            T3::textureTri(mainImage, tri, tri.texture, depthBuffer);
        }
    }

    // Draw wireframe
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

//    qDebug("%d %d", event->x(), event->y());
//    printf("%d %d", event->x(), event->y());
    T2::Vector2 sCenter = T2::Vector2(sWidth/2, sHeight/2);
    QPoint g = this->mapFromGlobal(QPoint(0, 0));
    sCenter.x -= g.x();
    sCenter.y -= g.y();
    float sensMod = 0.001; // Modifies the sensitivity of the mouse

    T2::Vector2 mouseDiff = T2::Vector2(cursor.pos().x(), cursor.pos().y()) - sCenter;
    yaw += mouseDiff.x * sensMod;
//    yaw = // Add wrapping?
    pitch += mouseDiff.y * sensMod;
    pitch = clamp(pitch, -3.14/2.0, 3.14/2.0); // Needs to be clamped so that the camera won't go upside down
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

// Change player position based on inputs
// This is basically tank controls, w/d to move forward/backward, a/d to rotate, space/ctrl to move higher/lower
void MainWindow::movePlayer(){
    float jumpSpeed = 3;
    float maxJumpSpeed = 4;
    float moveSpeed = 0.5;
    float maxMoveSpeed = 5;

    bool moving = false; // If no walk input is applied, slow down

    // Calculate directions for movement relative to camera rotation
    T3::Vector3 forward = lookDir * moveSpeed;
    forward.y = 0;
    T3::Vector3 right = forward * T3::newMatRotY(3.14/2);

//    T3::Vector3 playerPosition = actorList[0]->position;
    ActorDynamic *player = (ActorDynamic *)actorList[0];

    if(Input.isActionPressed("UP")){
        player->velocity += forward;
        moving = true;
//        playerPosition += forward;
    }
    if(Input.isActionPressed("DOWN")){
        player->velocity -= forward;
        moving = true;
//        playerPosition -= forward;
    }
    if(Input.isActionPressed("LEFT")){
        player->velocity -= right;
        moving = true;
//        playerPosition -= right;
    }
    if(Input.isActionPressed("RIGHT")){
        player->velocity += right;
        moving = true;
//        playerPosition += right;
    }
    if(Input.isActionJustPressed("JUMP")){
        player->velocity.y = jumpSpeed;
//        playerPosition.y += jumpSpeed;
    }
    if(Input.isActionPressed("CROUCH")){
        player->velocity.y -= jumpSpeed;
//        playerPosition.y -= jumpSpeed;
    }

    // Cap player horizontal speed
    player->velocity.x = clamp(player->velocity.x, -maxMoveSpeed, maxMoveSpeed);
    player->velocity.z = clamp(player->velocity.z, -maxMoveSpeed, maxMoveSpeed);


    if(moving){
        // Normalize horizontal speed
//        T3::Vector3 hor = player->velocity;
//        hor.y = 0;
//        hor = T3::normalise(hor);
//        player->velocity.x += hor.x * moveSpeed;
//        player->velocity.z += hor.z * moveSpeed;
//        T2::Vector2 hor = {player->velocity.x, player->velocity.z};
//        hor = hor.normalized();
//        player->velocity.x = hor.x;
//        player->velocity.z = hor.y;
    }//else{
        // Apply friction if not moving
        player->velocity.x = lerp(player->velocity.x, 0, 0.1);
        player->velocity.z = lerp(player->velocity.z, 0, 0.1);
    //}
//    qDebug("SPEED: %f %f", player->velocity.x, player->velocity.z);

    // Cap player air speed
    player->velocity.y = clamp(player->velocity.y, -maxJumpSpeed, maxJumpSpeed);

    player->position.y = std::max(0.0f, player->position.y); // TEMP, DELETE LATER


    // Camera stuff
    if(Input.isActionJustPressed("TOGGLE_CAMFOLLOW"))
        camFollow = !camFollow;

    if(camFollow){
        camera = player->position;
    }else{
        if(Input.isActionPressed("CAMUP")){
            camera += forward * maxMoveSpeed;
        }
        if(Input.isActionPressed("CAMDOWN")){
            camera -= forward * maxMoveSpeed;
        }
        if(Input.isActionPressed("CAMLEFT")){
            camera -= right * maxMoveSpeed;
        }
        if(Input.isActionPressed("CAMRIGHT")){
            camera += right * maxMoveSpeed;
        }
        if(Input.isActionPressed("CAMJUMP")){
            camera.y += jumpSpeed * maxJumpSpeed;
        }
        if(Input.isActionPressed("CAMCROUCH")){
            camera.y -= jumpSpeed * maxJumpSpeed;
        }
    }
}

void MainWindow::processActors(){
    float gravity = 0.1f;

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

float MainWindow::clamp(float in, float lo, float hi){
//    // Check if the args are passed properly
//    if(lo > hi){
//        qDebug("CLAMP ERROR, LO > HI");
//        return 0;
//    }

    if(in < lo) return lo;
    if(in > hi) return hi;
    return in;
}

float MainWindow::lerp(float from, float to, float mod){
    return from + (to - from) * mod;
}

void MainWindow::addActor(Actor a){
    actorList.push_back(new Actor);
    *actorList.back() = a;
}

void MainWindow::addActor(ActorDynamic a){
    actorList.push_back(new ActorDynamic);
    *actorList.back() = a;
}

void MainWindow::projectTriangle(Tools3D::Triangle tri, Tools3D::Mat4x4 transformMatrix,
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

    // Get the normal of the triangle surface and normalise it
    normal = line1.crossProduct(line2);
    normal = normal.normalize();

    // If the ray from triangle to camera is aligned with the normal, the triangle is visible
    T3::Vector3 cameraRay = triTransformed.p[0] - camera;
    if(normal.dotProduct(cameraRay) < 0){
        // Add a light
//            T3::Vector3 lightDirection = { 0.0f, 0.0f, -1.0f};
        T3::Vector3 lightDirection = { 0.0f, 1.0f, 0.0f};
        lightDirection = lightDirection.normalize();

        // Calculate the dot product of the light source and the normal to determine the intensity of shading/illumination
        float dp = std::max(0.1f, normal.dotProduct(lightDirection));
        T2::Color8 shadedColor;
        shadedColor = drawingColor * dp;


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
//            triProjected.color = shadedColor;
            triProjected.texture = texture;

            outputQueue->push_back(triProjected);
        }
    }
}

void MainWindow::projectTriangle(Tools3D::Triangle tri, Tools3D::Mat4x4 transformMatrix,
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

void MainWindow::startRemote(){

}

