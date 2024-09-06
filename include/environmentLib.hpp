#ifndef ENVIRONMENT_LIB
#define ENVIRONMENT_LIB

#include <SFML/Graphics.hpp>

#include "geometryLib.hpp"

enum KeyboardActions {
    NO_ACTION,
    MOVE_PLAYER_FORWARD,
    MOVE_PLAYER_BACKWARD,
    TURN_PLAYER_RIGHT,
    TURN_PLAYER_LEFT,
    QUIT_COMMAND,
};

// enum PlayerMovementState {
//     MOVE_BACKWARD,
//     NO_MOVEMENT,
//     MOVE_FORWARD,
// };
//
// enum PlayerRotationState {
//     TURN_LEFT,
//     NO_TURN,
//     TURN_RIGHT,
// };

struct Environment {
    // PlayerMovementState movementState;
    // PlayerRotationState rotationState;
    size_t width;
    size_t height;
    sf::RenderWindow* sceneWindow;
    sf::RenderWindow* screenWindow;
};

void constructEnvironment(size_t width, size_t height, sf::RenderWindow* sceneWindow,
                          sf::RenderWindow* screenWindow, Environment* env);
bool isEnvOpen(const struct Environment* env);
KeyboardActions windowEventsLoops(const struct Environment* env);
void drawCircle(Environment* env, sf::RenderWindow* window, const Point* point, float radius, sf::Color color);
void drawConvexShape(Environment* env, sf::RenderWindow* window, size_t numberOfPoints, const Point* const points, sf::Color color);
void drawVertexArray(Environment* env, sf::RenderWindow* window, sf::VertexArray array);
void clearWindows(Environment* env);
void drawWindows(Environment* env);
void closeWindows(Environment* env);

bool isForwardMove();
bool isBackwardMove();
bool isTurnRight();
bool isTurnLeft();
bool isQuitPressed();

#endif
