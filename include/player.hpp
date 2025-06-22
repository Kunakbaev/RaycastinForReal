#ifndef INCLUDE_PLAYER_HPP
#define INCLUDE_PLAYER_HPP

#include "geometryLib.hpp"

enum MovementDirections {
    DIRECTION_UP,
    DIRECTION_RIGHT,
    DIRECTION_DOWN,
    DIRECTION_LEFT,
};

enum RotationDirections {
    TURN_LEFT,
    TURN_RIGHT,
};

struct Player {
    Point       position;
    long double stepSize = 1;                  // how fast player moves across the scene
    long double angleRotationStep = PIE / 120; // how fast player rotates
    long double currentDirection;              // angle in radians
    long double FOV;                           // FOV = Field Of View, how much player sees (angle in radians)
    int         bodyRadius;
};

Vector getVectorByDirection(enum MovementDirections direction);
Player constructPlayer(
    const Point* position,
    long double  startingDirection,
    long double  FOV,
    long double  stepSize,
    long double  angleRotationStep,
    int          bodyRadius
);

void        movePlayerInViewDirection(      Player* player, bool isForward);
void        movePlayer               (      Player* player, MovementDirections direction);
void        turnPlayerByAngle        (      Player* player, RotationDirections direction);
long double getMinFOVangle           (const Player* player);
long double getMaxFOVangle           (const Player* player);
bool        isDirectionInsideFOV(
    const Player* player,
    Vector        direction,
    long double*  angle
);

#endif
