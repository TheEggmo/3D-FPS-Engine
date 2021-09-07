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
#include <QPushButton>
#include <QFileDialog>

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
    bool shadingEnabled(){
        return shadingToggle->isChecked();
    }
    void updateActorSelect(std::vector<std::string> names);
    void updateActorInfo(Actor *actor);
//    void updateActorInfo(ActorDynamic *actor);
private:
    bool active = false; // If true, the remote was started and can be used

    std::deque<float> deltaQueue; // Holds the last 60 frame deltas (for fps tracking)

    QVBoxLayout *mainLayout;

    QLabel *fpsDisplay;

    QCheckBox *colWireToggle;
    QCheckBox *modelWireToggle;
    QCheckBox *shadingToggle;

    QComboBox *actorSelect;
    QFormLayout *actorInfoLayout;

    QLineEdit *actorName;

    QCheckBox *actorModelToggle;
    QCheckBox *actorCollisionToggle;
    QCheckBox *actorLogicToggle;

    QHBoxLayout *actorPositionLayout;
    QDoubleSpinBox *posX, *posY, *posZ;

    QDoubleSpinBox *actorGravity;

//    QHBoxLayout *fileSelectContainer;
    QHBoxLayout *modelSelectContainer;
    QLineEdit *modelSelectLine;
//    QPushButton *modelSelectConfirm;
    QPushButton *modelSelectBrowse;
    QPushButton *modelSelectApply;
    QHBoxLayout *textureSelectContainer;
    QLineEdit *textureSelectLine;
//    QPushButton *textureSelectConfirm;
    QPushButton *textureSelectBrowse;
    QPushButton *textureSelectApply;

    QHBoxLayout *actorManagementLayout;
    QPushButton *duplicateActorButton;
    QPushButton *exportActorButton;
    QPushButton *importActorButton;

    QPushButton *applyButton;

    Actor *storedActor; // Pointer used to access the selected actor

    void closeEvent(QCloseEvent *event) override;


//    void updateStoredName();
//    void updateStoredToggleModel();
//    void updateStoredToggleCollision();
//    void updateStoredToggleLogic();
//    void updateStoredPosition();
//    void updateStoredGravity();

//    void updateActorDynamicInfo(ActorDynamic *actor);
public slots:
    void browseModel();
    void browseTexture();

    void remoteActorSelected(int index); // Calls updateRemoteActor(index)
//    void updateActorValues(); //
    void updateStoredActor(); // Applies all info currently in the remote to storedActor

    void updateStoredModel();
    void updateStoredModel(QString filepath);
    void updateStoredTexture();

    void duplicateStoredActor();
signals:
    void updateRemoteActor(int index); // Tells MainWindow to send data from actorList[index]
    // Tells MainWindow to add a new actor
    void addActor(ActorStatic a);
    void addActor(ActorPlayer a);
    void addActor(ActorLight a);
    void recastShadows();
};

#endif // REMOTE_H
