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
    //printf("%Lg\n", player->stepSize);
    //printf("%Lg %Lg\n", deltaVector.x, deltaVector.y);
    deltaVector = vectorMultByConst(&deltaVector, player->stepSize);
    //printf("%Lg %Lg\n", deltaVector.x, deltaVector.y);
    player->position = addVector(&player->position, &deltaVector);
}

void turnPlayerByAngle(Player* player, RotationDirections direction) {
    assert(player != NULL);

    long double delta = player->angleRotationStep;
    if (direction == TURN_RIGHT)
        delta *= -1;
    player->currentDirection += delta;
    printf("cur dir: %Lg\n", player->currentDirection);
}
