#ifndef REMOTE_H
#define REMOTE_H

//#include "mainwindow.h"
#include <deque>
#include <vector>
#include <string>

#include <QWidget>
#include <QCloseEvent>
#include <QLabel>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QDoubleSpinBox>

#include <actor.h>
#include <tools3d.h>

class Remote : public QWidget{
    Q_OBJECT
public:
    explicit Remote(QWidget *parent = nullptr);

//    bool initialize(MainWindow *game);
    explicit operator bool(){
        return active;
    }

    void start();

    void trackDeltas(float delta);

    // Toggle collision and model wireframes
    bool colWireEnabled(){
        return colWireToggle->isChecked();
    }
    bool modelWireEnabled(){
        return modelWireToggle->isChecked();
    }
    void updateActorSelect(std::vector<std::string> names);
    void updateActorInfo(Actor *actor);
    void updateActorInfo(ActorDynamic *actor);
private:
    bool active = false; // If true, the remote was started and can be used

    std::deque<float> deltaQueue; // Holds the last 60 frame deltas (for fps tracking)

    QVBoxLayout *mainLayout;

    QLabel *fpsDisplay;

    QCheckBox *colWireToggle;
    QCheckBox *modelWireToggle;

    QComboBox *actorSelect;
    QFormLayout *actorInfoLayout;

    QLineEdit *actorName;

    QCheckBox *actorModelToggle;
    QCheckBox *actorCollisionToggle;
    QCheckBox *actorLogicToggle;

    QHBoxLayout *actorPositionLayout;
    QDoubleSpinBox *posX, *posY, *posZ;

    void closeEvent(QCloseEvent *event) override;

    void updateActorDynamicInfo(ActorDynamic *actor);
public slots:
    void remoteActorSelected(int index);
signals:
    void updateRemoteActor(int index);
};

#endif // REMOTE_H
