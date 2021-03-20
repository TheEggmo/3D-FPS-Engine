#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <tools.h>
#include <tools3d.h>
#include <QTimer>
#include <QKeyEvent>
#include <string>

using string = std::string;

class MainWindow : public QWidget{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    void paintEvent(QPaintEvent *event) override;

    int sWidth, sHeight;

    QTimer *sUpdateTimer;

    class InputMap{
    private:
        enum KeyState{NONE, PRESS, HOLD, RELEASE};

        // Actions store button states.
        // An action state can be accessed by name.
        // Action state is changed whenever any of its keys state changes.
        // Using this you can have an action(e.g. jump) that has two different inputs(e.g. "space" and "w").
        // Whether you press space or w the state of jump will be set to pressed.
        // Then you can check the state of the jump action to make the player jump.
        struct action{
            string name; // The name of the action
            std::vector<int> keys;
            KeyState state = NONE;
        };
        // Stores the list of all actions
        std::vector<action> actionMap;

        // Searches the actionMap for the specified value and returns its index if found,
        // returns -1 if value was not found.
        // If the key is NOT given, will search the actionMap for the specified targetName action.
        // If the key is given, will search the specified action for the key.
        // start specifies the starting index of the search
        int has(string targetName, int key = -1, int start = 0){
            start = std::max(0, start); // No negative indexes

            if(key != -1){
                int idx = has(targetName);
                for(int i = start; i < actionMap[idx].keys.size(); i++){
                    if(actionMap[idx].keys[i] == key){
                        return i;
                    }
                }
            }else{
                for(int i = start; i < actionMap.size(); i++){
                    if (actionMap[i].name.compare(targetName) == 0){
                        return i;
                    }
                }
            }

            return -1;
        }


        // Input is queued whenever keyboard events happen.
        // Then, at the start of every process() call (every frame) the input is resolved
        std::vector<int> pressQueue;
        std::vector<int> releaseQueue;
    public:
        // Adds an action to the InputMap with the specified name.
        // If key is specified, also attaches that key to the action.
        // Doesn't allow duplicate names
        void addAction(string name, int key = -1){
            if(has(name) == -1){
                action newAction;
                newAction.name = name;
                if(key != -1){
                    newAction.keys.push_back(key);
                }
                actionMap.push_back(newAction);
            }
        }
        // Removes an action from the InputMap along with its keys
        void removeAction(string name){
            int idx = has(name);
            if(idx != -1){
                actionMap.erase(actionMap.begin() + idx);
            }
        }
        // Adds the specified key to an action in the input map
        void addKey(int key, string action){
            int idx = has(action);
            if(idx == -1){qDebug("ADD KEY FAILED, ACTION %s NOT FOUND", action); return;}

            actionMap[idx].keys.push_back(key);
        }
        // Removes the specified key from the specified action
        void removeKey(int key, string action){
            int idx = has(action);
            if(idx == -1){qDebug("REMOVE KEY FAILED, ACTION %s NOT FOUND", action); return;}

            int keyIdx = has(action, key);
            actionMap[idx].keys.erase(actionMap[idx].keys.begin() + keyIdx);
        }
        // Adds the specified key to pressQueue
        // All actions with this key will change state to PRESS during processInput()
        void pressKey(int key){
            pressQueue.push_back(key);
        }
        // Adds the specified key to releaseQueue
        // All actions with this key will change state to RELEASE during processInput()
        void releaseKey(int key){
            releaseQueue.push_back(key);
        }
        // Update the state of actions in the input map
        // This function is called every frame
        void processInput(){
            // Change all actions marked as RELEASE to NONE
            for(int i = 0; i < actionMap.size(); i++){
                if(actionMap[i].state == RELEASE){
                    actionMap[i].state = NONE;
                }
            }

            // Process releaseQueue inputs
            while(!releaseQueue.empty()){
                int input = releaseQueue.back();
                releaseQueue.pop_back();
                for(int i = 0; i < actionMap.size(); i++){
                    if(has(actionMap[i].name, input) != -1){
                        actionMap[i].state = RELEASE;
                    }
                }
            }

            // Change all actions marked as PRESS to HOLD
            for(int i = 0; i < actionMap.size(); i++){
                if(actionMap[i].state == PRESS){
                    actionMap[i].state = HOLD;
                }
            }

            // Process pressQueue
            while(!pressQueue.empty()){
                int input = pressQueue.back();
                pressQueue.pop_back();
                for(int i = 0; i < actionMap.size(); i++){
                    if(has(actionMap[i].name, input) != -1){
                        actionMap[i].state = PRESS;
                    }
                }
            }
        }

        // Return true if the action state is PRESS
        bool isActionJustPressed(string action){
            int idx = has(action);
            if(idx == -1){
                qDebug("ACTION %s NOT FOUND", action);
                return false;
            }
            return actionMap[idx].state == PRESS;
        }
        // Return true if the action state is not NONE
        bool isActionPressed(string action){
            int idx = has(action);
            if(idx == -1){
                qDebug("ACTION %s NOT FOUND", action);
            }
            return actionMap[idx].state != NONE;
        }
        // Return true if the action state is RELEASE
        bool isActionJustReleased(string action){
            int idx = has(action);
            if(idx == -1){
                qDebug("ACTION %s NOT FOUND");
            }
            return actionMap[idx].state == RELEASE;
        }
    };
    InputMap Input;

protected:
//    void mousePressEvent(QMouseEvent *event) override;
//    void mouseMoveEvent(QMouseEvent *event) override;
//    void mouseReleaseEvent(QMouseEvent *event) override;

    // Keyboard keys are caputed during these events and stored in the inputmap
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    void movePlayer(); // Called every frame, updates player position based on input

    Tools3D::Mesh meshCube;
    Tools3D::Mat4x4 matProj; // Projection matrix

    Tools3D::Vector3 camera; // Vector representing the camera
    Tools3D::Vector3 lookDir;
    float yaw = 0;

    float fTheta = 0;

public slots:
    void process();
};
#endif // MAINWINDOW_H
