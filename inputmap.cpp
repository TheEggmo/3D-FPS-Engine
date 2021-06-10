#include "inputmap.h"
#include <QDebug>

//using string = std::string;


int InputMap::has(string targetName, int key, int start){
    start = std::max(0, start); // No negative indexes

    if(key != -1){
        // If key is given, search for the key index inside an action
        int idx = has(targetName);
        for(int i = start; i < InputMap::actionMap[idx].keys.size(); i++){
            if(InputMap::actionMap[idx].keys[i] == key){
                return i;
            }
        }
    }else{
        // If key is NOT given (key = -1), search for the index of the specified action
        for(int i = start; i < InputMap::actionMap.size(); i++){
            if (InputMap::actionMap[i].name.compare(targetName) == 0){
                return i;
            }
        }
    }

    return -1;
}

void InputMap::addAction(string name, int key){
    if(has(name) == -1){
        action newAction;
        newAction.name = name;
        if(key != -1){
            newAction.keys.push_back(key);
        }
        actionMap.push_back(newAction);
    }
}

void InputMap::removeAction(string name){
    int idx = has(name);
    if(idx != -1){
        actionMap.erase(actionMap.begin() + idx);
    }
}

void InputMap::addKey(string action, int key){
    int idx = has(action);
    if(idx == -1){qDebug("ADD KEY FAILED, ACTION %s NOT FOUND", action); return;}

    actionMap[idx].keys.push_back(key);
}

void InputMap::removeKey(string action, int key){
    int idx = has(action);
    if(idx == -1){qDebug("REMOVE KEY FAILED, ACTION %s NOT FOUND", action); return;}

    int keyIdx = has(action, key);
    actionMap[idx].keys.erase(actionMap[idx].keys.begin() + keyIdx);
}

void InputMap::pressKey(int key){
    pressQueue.push_back(key);
}

void InputMap::releaseKey(int key){
    releaseQueue.push_back(key);
}

void InputMap::processInput(){
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


bool InputMap::isActionJustPressed(string action){
    int idx = has(action);
    if(idx == -1){
        qDebug("ACTION %s NOT FOUND", action.c_str());
        return false;
    }
    return actionMap[idx].state == PRESS;
}

bool InputMap::isActionPressed(string action){
    int idx = has(action);
    if(idx == -1){
        qDebug("ACTION %s NOT FOUND", action.c_str());
    }
    return actionMap[idx].state != NONE;
}

bool InputMap::isActionJustReleased(string action){
    int idx = has(action);
    if(idx == -1){
        qDebug("ACTION %s NOT FOUND", action.c_str());
    }
    return actionMap[idx].state == RELEASE;
}
