#include "remote.h"


Remote::Remote(QWidget *parent) : QWidget(parent){
    setWindowTitle("Remote Access");
    setWindowState(Qt::WindowMinimized);
}

bool Remote::initialize(MainWindow *game){
    std::vector<Actor*> list = game->getActorList();
    qDebug("connected");
}
