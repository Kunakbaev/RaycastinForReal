#include <stdlib.h>
#include <assert.h>
#include <SFML/Graphics.hpp>

#include "../include/geometryLib.hpp"
#include "../include/obstacle.hpp"

Obstacle constructObstacle(size_t numberOfSides, Point* sides) {
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

            // FIXME: what about when point is on segment?
            if (doesSegmentsIntersect(&segm1, &segm2)) {
                return true;
            }
        }
    }

    return false;
}

bool doesObstacleIntersectWithPlayer(const Obstacle* obj, const Player* player) {
    assert(obj    != NULL);
    assert(player != NULL);

    // FIXME: for now we assume that's a convex polygon, and we don't consider player's body radius
    for (size_t i = 1; i + 1 < obj->numberOfSides; ++i) {
        if (isInsideTriangle(&player->position,
            &obj->sides[0], &obj->sides[i], &obj->sides[i + 1])) {
                // printf("point 0    : %Lg, %Lg\n", obj->sides[0].x, obj->sides[0].y);
                // printf("point i    : %Lg, %Lg\n", obj->sides[i].x, obj->sides[i].y);
                // printf("point i + 1: %Lg, %Lg\n", obj->sides[i + 1].x, obj->sides[i + 1].y);
                // printf("i : %d\n", i);
                return true;
            }
    }
    return false;
}

void displayObstacle(const Obstacle* obj, sf::RenderWindow* window) {
    sf::VertexArray arr(sf::TrianglesFan);
    // printf("---------------------\n");
    for (int i = 0; i < obj->numberOfSides; ++i) {
        int x = (int)obj->sides[i].x;
        int y = (int)obj->sides[i].y;
        // printf("%d %d\n", x, y);
        sf::Vertex vert(sf::Vector2f(x, y));
        vert.color = sf::Color::White;
        arr.append(vert);
    }

    window->draw(arr);
}
