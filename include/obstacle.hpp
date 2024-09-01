#ifndef OBSTACLE
#define OBSTACLE

#include "geometryLib.hpp"

struct Obstacle {
    size_t numberOfSides;
    Point* sides;
};

Obstacle constructObstacle(size_t numberOfSides, Point* sides);
Segment getSegment(const Obstacle* obj, size_t pointIndex);
bool doesObstaclesIntersect(const Obstacle* obj1, const Obstacle* obj2);

#endif
