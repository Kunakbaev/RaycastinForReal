#include <assert.h>
#include <stdio.h>

#include "../include/player.hpp"

Vector getVectorByDirection(enum Directions direction) {
    // FIXME: where do we have origin of coordinates
    switch (direction) {
        case DIRECTION_UP   :   return constructPoint(0.0, 1.0);
        case DIRECTION_RIGHT:   return constructPoint(1.0, 0.0);
        case DIRECTION_DOWN :   return constructPoint(0.0, -1.0);
        case DIRECTION_LEFT :   return constructPoint(-1.0, 0.0);
        default             :   return constructPoint(0.0, 0.0); // error
    }
}

Player constructPlayer(const Point* position, long double startingDirection, long double FOV,
                       long double stepSize, long double angleRotationStep, int bodyRadius) {
    assert(position != NULL);

    Player result = { *position, stepSize, angleRotationStep, startingDirection, FOV, bodyRadius };
    return result;
}

void movePlayer(Player* player, Directions direction) {
    Vector deltaVector = getVectorByDirection(direction);
    deltaVector = vectorMultByConst(&deltaVector, player->stepSize);
    player->position = addVector(&player->position, &deltaVector);
}

void movePlayerForward(Player* player) {
    Vector deltaVector = constructPoint(player->stepSize, 0);
    deltaVector = rotateVectorByAngle(&deltaVector, player->currentDirection);
    player->position = addVector(&player->position, &deltaVector);
}

void movePlayerBackward(Player* player) {
    Vector deltaVector = constructPoint(player->stepSize, 0);
    deltaVector = rotateVectorByAngle(&deltaVector, player->currentDirection);
    deltaVector = vectorMultByConst(&deltaVector, -1);
    player->position = addVector(&player->position, &deltaVector);
}

void turnPlayerByAngle(Player* player, RotationDirections direction) {
    assert(player != NULL);

    long double delta = player->angleRotationStep;
    if (direction == TURN_RIGHT)
        delta *= -1;
    player->currentDirection += delta;
}

long double getMaxFOVangle(const Player* player) {
    assert(player != NULL);

    long double maxAngle = player->currentDirection + player->FOV / 2;
    int cntLoops = (int)(maxAngle / (2 * PIE));
    maxAngle -= cntLoops * 2 * PIE;
    return maxAngle;
}

long double getMinFOVangle(const Player* player) {
    assert(player != NULL);

    long double maxAngle = player->currentDirection + player->FOV / 2;
    long double minAngle = player->currentDirection - player->FOV / 2;
    int cntLoops = (int)(maxAngle / (2 * PIE));
    // printf("maxAngle : %Lg\n", maxAngle);
    // printf("cntLoops : %d\n", cntLoops);
    minAngle -= cntLoops * 2 * PIE;
    return minAngle;
}

// Vector is small, so copy
bool checkIfDirectionInsideFOV(const Player* player, Vector direction, long double* validAngle) {
    assert(player     != NULL);
    assert(validAngle != NULL);

    long double minAngle = getMinFOVangle(player);
    long double maxAngle = getMaxFOVangle(player);
    *validAngle = getVectorAngle(&direction);

    // FIXME: not proper way to check this
    // this ray is not in our field of view
    for (int j = -3; j <= 3; ++j) {
        long double cur = *validAngle + j * 2 * PIE;
        if (0 <= sign(cur - minAngle) && sign(cur - maxAngle) <= 0) {
            *validAngle = cur;
            return true;
        }
    }

    return false;
}
