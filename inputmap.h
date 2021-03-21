#ifndef INPUTMAP_H
#define INPUTMAP_H

#include <string>
#include <vector>

using string = std::string;

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
        std::vector<int> keys; // List of keys corresponding to this action
        KeyState state = NONE; // Current state of this action
    };
    // Stores the list of all actions
    std::vector<action> actionMap;

    // Searches the actionMap for the specified value and returns its index if found,
    // returns -1 if value was not found.
    // If the key is -1(default/can't be triggered by any keypress), will search the actionMap for the specified targetName action.
    // If the key is given, will search the specified action for the key.
    // (UNUSED) start specifies the starting index of the search
    int has(string targetName, int key = -1, int start = 0);

    // Input is queued whenever Qt keyboard events happen.
    // Then, at the start of every process() call (every frame) the input is resolved
    std::vector<int> pressQueue;
    std::vector<int> releaseQueue;

public:
    // Adds an action to the InputMap with the specified name.
    // If key is specified, also attaches that key to the action.
    // Doesn't allow duplicate names
    void addAction(string name, int key = -1);
    // Removes an action from the InputMap along with its keys
    void removeAction(string name);

    // Adds the specified key to the specified action in the input map
    void addKey(string action, int key);
    // Removes the specified key from the specified action
    void removeKey(string action, int key);

    // Adds the specified key to pressQueue
    // All actions with this key will change state to PRESS during processInput()
    void pressKey(int key);
    // Adds the specified key to releaseQueue
    // All actions with this key will change state to RELEASE during processInput()
    void releaseKey(int key);

    // Update the state of actions in the input map
    // This function is called every frame
    void processInput();

    // Return true if the action state is PRESS
    bool isActionJustPressed(string action);
    // Return true if the action state is not NONE
    bool isActionPressed(string action);
    // Return true if the action state is RELEASE
    bool isActionJustReleased(string action);
};

#endif // INPUTMAP_H
