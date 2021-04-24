#ifndef REMOTE_H
#define REMOTE_H

//#include "mainwindow.h"
#include <deque>
#include <string>

#include <QWidget>
#include <QCloseEvent>
#include <QLabel>
#include <QCheckBox>


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

    bool wireframeEnabled(){
        return wireframeToggle->isChecked();
    }
private:
    bool active = false; // If true, the remote was started and can be used

    std::deque<float> deltaQueue; // Holds the last 60 frame deltas (for fps tracking)

    QLabel *fpsDisplay;
    QCheckBox *wireframeToggle;

    void closeEvent(QCloseEvent *event) override;
signals:

};

#endif // REMOTE_H
