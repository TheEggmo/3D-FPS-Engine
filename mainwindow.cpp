#include "mainwindow.h"
//#include <Tools.h>
#include <vector>
#include <QDebug>
#include <algorithm>
#include <list>


using T2 = Tools;
using T3 = Tools3D;

enum MButton{NONE, LEFT, RIGHT, MIDDLE};

QImage *mainImage, *storedImage;

MButton heldButton; // Stores which button was pressed when mousePressEvent was called

int startX, startY; // Stores the x and y coordinates of the mouse when mousePressEvent is called
int offset = 0; // Shifts the drawn image and mouse events by this offset

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
    int targetFps = 60;
    processTimer->start(1000/targetFps);

    Actor player;
    player.setCollision(T3::AABB({-1, -1, -1}, {2, 2, 2}));
    player.name = "Player";
//    actorList.push_back(&player);
    actorList.push_back(new Actor);
    *actorList[0] = player;

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
//    meshCube.loadFromFile("Assets/capsule.obj");


   T3::MeshTexture meshCapsule;
   meshCapsule.loadFromFile("Assets/capsule.obj");
   meshCapsule.texture = new QImage("Assets/capsule.jpg");
   Actor capsule;
   capsule.setModel(meshCapsule);
   capsule.visible = false;
   capsule.name = "Capsule";

   T3::MeshTexture meshWatermelon;
   meshWatermelon.loadFromFile("Assets/watermelon2.obj");
   meshWatermelon.scale(10);
   meshWatermelon.texture = new QImage("Assets/SMK_JJ0KQAO2_Watermelon_8K_Albedo.png");
   Actor watermelon;
   watermelon.setModel(meshWatermelon);
   watermelon.visible = false;
   watermelon.name = "Watermelon";



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
    meshCube.texture = new QImage("Assets/Artisans Hub.png");
//    QImage *cubeTexture = new QImage("Assets/capsule.jpg");


    meshCube.loadFromFile("Assets/cube.obj");
    Actor cubeActor;
    cubeActor.setModel(meshCube);
    cubeActor.visible = true;
    cubeActor.name = "Cube";

//    actorList.push_back(&cubeActor);
//    actorList.push_back(&capsule);
//    actorList.push_back(&hub);
//    actorList.push_back(&watermelon);
    actorList.push_back(new Actor);
    actorList.push_back(new Actor);
    actorList.push_back(new Actor);
    actorList.push_back(new Actor);
    *actorList[1] = cubeActor;
    *actorList[2] = capsule;
    *actorList[3] = hub;
    *actorList[4] = watermelon;


    // Initiate the projection Matrix
    matProj = T3::newMatProj(90.0f, (float)sHeight/(float)sWidth, 0.1f, 1000.f);

    // Initiate the InputMap
    Input.addAction("JUMP", Qt::Key_Space);
    Input.addAction("CROUCH", Qt::Key_Control);
    Input.addAction("LEFT", Qt::Key_A);
    Input.addKey("LEFT", Qt::Key_Left);
    Input.addAction("RIGHT", Qt::Key_D);
    Input.addKey("RIGHT", Qt::Key_Right);
    Input.addAction("UP", Qt::Key_W);
    Input.addKey("UP", Qt::Key_Up);
    Input.addAction("DOWN", Qt::Key_S);
    Input.addKey("DOWN", Qt::Key_Down);

    // Create the depth buffer
//    depthBuffer = new float[sWidth * sHeight];
    depthBuffer.resize(sWidth * sHeight, 0.0f);
//    depthBuffer.resize(sWidth * sHeight, std::pair<float, T2::Color8> (0.0f, T2::Color8(-1, -1, -1)));
}

void MainWindow::paintEvent(QPaintEvent*){
    QPainter p(this);
    p.drawImage(offset, offset, *mainImage);
}

void MainWindow::process(){
    // Track how long each frame takes(for physics and debugging)
    auto newFrameTime = std::chrono::system_clock::now();
    std::chrono::duration<double> frameTime = newFrameTime - lastFrameTime;
    delta = frameTime.count();
    lastFrameTime = newFrameTime;
    if(bool displayFrameTime = false) qDebug("Frame Time: %f", frameTime.count());
    if(bool displayFPS = false) qDebug("FPS: %f", 1/frameTime.count());

    mainImage->fill(defaultBg); // Clear the screen
    Input.processInput(); // Update inputs
    movePlayer(); // Move the player
    processActors(); // Calculate world physics
    screenUpdate(); // Transform the 3D space into a 2D image
    update();

//    // SPEEN
//    fTheta = 0.1;
//    T3::Mat4x4 rot = T3::newMatRotY(-fTheta);
//    for(int i = 0; i < meshCube.tris.size(); i++){
//        T3::Triangle copy = meshCube.tris[i];
//        meshCube.tris[i] = copy * rot;
//        meshCube.tris[i].t[0] = copy.t[0];
//        meshCube.tris[i].t[1] = copy.t[1];
//        meshCube.tris[i].t[2] = copy.t[2];

//    }
}

// The main engine/game loop function, called every frame
void MainWindow::screenUpdate(){
    T3::Mat4x4 matRotZ, matRotX, matTrans, matWorld;
//    std::vector<T3::Triangle> extTris; // Stores all triangles extracted from models for drawing
//    for(int i = 0; i < sWidth * sHeight; i++) depthBuffer[i] = std::pair<float, T2::Color8> (0.0f, T2::Color8(-1, -1, -1); // Clear the depth buffer
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
        // Draw the actor's textured mesh
        if(a->visible){
            T3::MeshTexture model = a->getModel();
            model.flat = false;
            for(int i = 0; i < model.tris.size(); i++){
                projectTriangle(model.tris[i], matWorld, camera, viewMatrix, &triangleQueue);
            }
        }
        // Draw a wireframe of the actor's AABB
        if(bool showColliders = true && a->collisionEnabled){
            T3::Mesh collider = a->getCollider().toMesh();
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
//            if(meshCube.flat){
//                T3::fillTri(mainImage, tri, tri.color);
//            }else{
//                T3::textureTri(mainImage, tri, meshCube.texture, depthBuffer);
//            }
            T3::textureTri(mainImage, tri, meshCube.texture, depthBuffer);
//            T3::drawTri(mainImage, tri, T2::Color8(255, 255, 255)); // Draw outline on edges (wireframe)
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

        // Finally, draw the modified triangles on the screen
        for(T3::Triangle &tri : cTriangleQueue){
            T3::drawTri(mainImage, tri, T2::Color8(255, 255, 255)); // Draw outline on edges (wireframe)
        }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event){
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
    float moveSpeed = 1;

    // Calculate directions for movement relative to camera rotation
    T3::Vector3 forward = lookDir * moveSpeed;
    forward.y = 0;
    T3::Vector3 right = forward * T3::newMatRotY(3.14/2);

    T3::Vector3 playerPosition = actorList[0]->position;

    if(Input.isActionPressed("UP")){
        playerPosition += forward;
    }
    if(Input.isActionPressed("DOWN")){
        playerPosition -= forward;
    }
    if(Input.isActionPressed("LEFT")){
        playerPosition -= right;
    }
    if(Input.isActionPressed("RIGHT")){
        playerPosition += right;
    }
    if(Input.isActionPressed("JUMP")){
        playerPosition.y += jumpSpeed;
    }
    if(Input.isActionPressed("CROUCH")){
        playerPosition.y -= jumpSpeed;
    }

    camera = playerPosition;
    actorList[0]->position = playerPosition;
}

void MainWindow::processActors(){
    float gravity = 0.1f;


    // Process collisions
    for(int i = 0; i < actorList.size(); i++){
        if(actorList[i]->collisionEnabled){
            std::vector<T3::AABB> colliders;
            colliders.clear();
            // Get colliders from all other actors
            for(int j = 0; j < actorList.size(); j++){
//                if(i == j) continue; // Don't check against self
//                colliders.push_back(actorList[j].getCollider());
                if(i != j){
                    colliders.push_back(actorList[j]->getCollider());
                }
            }
            actorList[i]->processCollision(colliders);
        }
    }

//    for(Actor a : actorList){
//        if(a.collisionEnabled){
//            std::vector<T3::AABB> colliders;
//            // Get colliders from all other actors
//            for(Actor a2 : actorList){
//                colliders.push_back(a.getCollider());
//            }
//            a.processCollision(colliders);
//        }
//    }

    // Process collisions (old)
    // First, make a list of all AABBs from Actors
    //    std::vector<T3::AABB> colliders;
    //    colliders.clear();
    //    for(Actor a : actorList){
    //        if(a.collisionEnabled){
    //            colliders.push_back(a.getCollider());
    //        }
    //    }


    //    for(Actor a : actorList){
    //        a.process(colliders);
    //    }

}

float MainWindow::clamp(float in, float lo, float hi){
    if(lo > hi){
        qDebug("CLAMP ERROR, LO > HI");
        return 0;
    }

    if(in < lo) return lo;
    if(in > hi) return hi;
    return in;
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
    normal = T3::crossProduct(line1, line2);
    normal = T3::normalise(normal);

    // If the ray from triangle to camera is aligned with the normal, the triangle is visible
    T3::Vector3 cameraRay = triTransformed.p[0] - camera;
    if(T3::dotProduct(normal, cameraRay) < 0){
        // Add a light
//            T3::Vector3 lightDirection = { 0.0f, 0.0f, -1.0f};
        T3::Vector3 lightDirection = { 0.0f, 1.0f, 0.0f};
        lightDirection = T3::normalise(lightDirection);

        // Calculate the dot product of the light source and the normal to determine the intensity of shading/illumination
        float dp = std::max(0.1f, T3::dotProduct(normal, lightDirection));
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

            // Copy texture data from old triangle to new one
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

            triProjected.color = shadedColor;
            outputQueue->push_back(triProjected);
        }
    }
}

