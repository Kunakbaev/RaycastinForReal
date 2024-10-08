#ifndef PLAYER
#define PLAYER

#include "geometryLib.hpp"

enum Directions {
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
    Point position;
    long double stepSize = 1; // how fast player moves across scene
    long double angleRotationStep = PIE / 120; // how fast player rotates
    long double currentDirection; // angle in radians
    long double FOV; // FOV = Field Of View, how much player sees
    int bodyRadius;
};

Vector getVectorByDirection(enum Directions direction);
Player constructPlayer(const Point* position, long double startingDirection, long double FOV,
        long double stepSize, long double angleRotationStep, int bodyRadius);
void movePlayerForward(Player* player);
void movePlayerBackward(Player* player);
// void movePlayerByVector(Player* player, Vector* direction);
void movePlayer(Player* player, Directions direction);
void turnPlayerByAngle(Player* player, RotationDirections direction);
long double getMinFOVangle(const Player* player);
long double getMaxFOVangle(const Player* player);
bool checkIfDirectionInsideFOV(const Player* player, Vector direction, long double* angle);

#endif
