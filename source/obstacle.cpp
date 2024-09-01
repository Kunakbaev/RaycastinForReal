#include <stdlib.h>
#include <assert.h>

#include "../include/geometryLib.hpp"
#include "../include/obstacle.hpp"

Obstacle constructObstacle(size_t numberOfSides, Point* sides) {
    assert(numberOfSides > 0);
    assert(sides         != NULL);

    Obstacle result = {};
    result.numberOfSides = numberOfSides;
    result.sides = (Point*)calloc(numberOfSides, sizeof(Point));
    assert(result.sides != NULL);

    return result;
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
