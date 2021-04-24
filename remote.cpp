#include "remote.h"


Remote::Remote(QWidget *parent) : QWidget(parent){
    setWindowTitle("Remote Access");
    resize(300, 300);

    fpsDisplay = new QLabel("X FPS", this);
    fpsDisplay->move(10, 10);
    fpsDisplay->resize(100, 10);

    wireframeToggle = new QCheckBox("Enable wireframe", this);
    wireframeToggle->setChecked(false);
    wireframeToggle->move(10, 40);
}

void Remote::start(){
    active = true;
    raise();
    activateWindow();

    show();
    qDebug("Remote started");
}

void Remote::trackDeltas(float delta){
    // Track the deltas over the last second and display average fps
    // This assumes that the target fps are 60
    deltaQueue.push_back(delta);
    if(deltaQueue.size() > 60){
        deltaQueue.pop_front();
    }

    float sum = 0;
    for(float delta : deltaQueue){
        sum += delta;
    }
    float averageDelta = sum / (float)deltaQueue.size();
    int averageFps = 1.0/averageDelta;
//    qDebug("%.2f FPS", 1/averageDelta);
    fpsDisplay->setText((std::to_string(averageFps) + " FPS").c_str());
}

void Remote::closeEvent(QCloseEvent *event){
    active = false;
    event->accept();
}

