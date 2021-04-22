#ifndef REMOTE_H
#define REMOTE_H

#include "mainwindow.h"

#include <QWidget>


class Remote : public QWidget{
    Q_OBJECT
public:
    explicit Remote(QWidget *parent = nullptr);

    bool initialize(MainWindow *game);

signals:

};

#endif // REMOTE_H
