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

    for (size_t i = 0; i < numberOfSides; ++i)
        result.sides[i] = sides[i];
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

    for (size_t i = 0; i < obj1->numberOfSides; ++i) {
        Segment segm1 = getSegment(obj1, i);
        for (size_t j = 0; j < obj2->numberOfSides; ++j) {
            Segment segm2 = getSegment(obj2, j);
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

void displayObstacle(const Obstacle* obj, sf::RenderWindow* window, int screenHeight) {
    assert(obj    != NULL);
    assert(window != NULL);

    sf::VertexArray arr(sf::TrianglesFan);
    for (size_t sideIndex = 0; sideIndex < obj->numberOfSides; ++sideIndex) {
        int x = (int)obj->sides[sideIndex].x;
        int y = screenHeight - (int)obj->sides[sideIndex].y - 1;
        sf::Vertex vert(sf::Vector2f(x, y));
        vert.color = sf::Color::White;
        arr.append(vert);
    }

    window->draw(arr);
}

void destructObstacle(Obstacle* obstacle) {
    FREE(obstacle->sides);
    obstacle->numberOfSides = 0;
}
