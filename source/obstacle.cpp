#include <stdlib.h>
#include <assert.h>
#include <SFML/Graphics.hpp>

#include "../include/geometryLib.hpp"
#include "../include/obstacle.hpp"

#define FREE(x)             \
    do {                    \
        free(x);            \
        x = NULL;           \
    } while(0)

Obstacle constructObstacle(size_t numberOfSides, const Point* sides) {
    assert(numberOfSides > 0);
    assert(sides         != NULL);

    Obstacle result = {};
    result.numberOfSides = numberOfSides;
    result.sides = (Point*)calloc(numberOfSides, sizeof(Point));
    assert(result.sides != NULL);

    for (size_t sideInd = 0; sideInd < numberOfSides; ++sideInd)
        result.sides[sideInd] = sides[sideInd];
    return result;
}

Obstacle constructRectObstacle(int x1, int y1, int x2, int y2) {
    Point sides[] = {
        constructPoint(x1, y1),
        constructPoint(x2, y1),
        constructPoint(x2, y2),
        constructPoint(x1, y2)
    };

    return constructObstacle(4, sides);
}

Obstacle constructCircleObstacle(const Point* center, int radius, size_t numberOfPoints) {
    assert(center         != NULL);
    assert(radius         > 0);
    assert(numberOfPoints >= 3);

    Obstacle obj = {};
    obj.sides = (Point*)calloc(numberOfPoints, sizeof(Point));
    assert(obj.sides != NULL);
    obj.numberOfSides = numberOfPoints;

    long double angleDelta = 2 * PIE / numberOfPoints;
    Vector direction = constructPoint(radius, 0);

    for (size_t pointInd = 0; pointInd < numberOfPoints; ++pointInd) {
        Point point = addVector(center, &direction);
        obj.sides[pointInd] = point;
        direction = rotateVectorByAngle(&direction, angleDelta);
    }

    return obj;
}

Segment getSegment(const Obstacle* obj, size_t pointIndex) {
    size_t nxtIndex = (pointIndex + 1) % obj->numberOfSides;
    Segment result = constructSegment(
        &obj->sides[pointIndex],
        &obj->sides[nxtIndex]
    );

    return result;
}

bool doesObstaclesIntersect(const Obstacle* obj1, const Obstacle* obj2) {
    assert(obj1 != NULL);
    assert(obj2 != NULL);

    for (size_t sideInd1 = 0; sideInd1 < obj1->numberOfSides; ++sideInd1) {
        Segment segm1 = getSegment(obj1, sideInd1);
        for (size_t sideInd2 = 0; sideInd2 < obj2->numberOfSides; ++sideInd2) {
            Segment segm2 = getSegment(obj2, sideInd2);
            if (doesSegmentsIntersect(&segm1, &segm2))
                return true;
        }
    }

    return false;
}

bool doesObstacleIntersectWithPlayer(const Obstacle* obj, const Player* player) {
    assert(obj    != NULL);
    assert(player != NULL);

    // FIXME: for now we assume that's a convex polygon, and we don't consider player's body radius
    for (size_t sideIndex = 1; sideIndex + 1 < obj->numberOfSides; ++sideIndex) {
        if (isInsideTriangle(&player->position,
            &obj->sides[0], &obj->sides[sideIndex], &obj->sides[sideIndex + 1]))
                return true;
    }
    return false;
}

void displayObstacle(const Obstacle* obj, Environment* env) {
    assert(obj != NULL);
    assert(env != NULL);

    Point points[obj->numberOfSides] = {};
    for (size_t sideIndex = 0; sideIndex < obj->numberOfSides; ++sideIndex) {
        points[sideIndex] = constructPoint(obj->sides[sideIndex].x, obj->sides[sideIndex].y);
    }

    drawConvexShape(env, env->sceneWindow, obj->numberOfSides, points, sf::Color::White);
}

void destructObstacle(Obstacle* obstacle) {
    FREE(obstacle->sides);
    obstacle->numberOfSides = 0;
}
