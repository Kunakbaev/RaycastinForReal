#ifndef OBSTACLE
#define OBSTACLE

#include "geometryLib.hpp"
#include "player.hpp"

struct Obstacle {
    size_t numberOfSides;
    Point* sides;
};

Obstacle constructObstacle(size_t numberOfSides, Point* sides);
Obstacle constructRectObstacle(int x1, int y1, int x2, int y2);
Obstacle constructCircleObstacle(const Point* center, int radius, size_t numberOfPoints);
Segment getSegment(const Obstacle* obj, size_t pointIndex);
bool doesObstaclesIntersect(const Obstacle* obj1, const Obstacle* obj2);
bool doesObstacleIntersectWithPlayer(const Obstacle* obj, const Player* player);
void displayObstacle(const Obstacle* obj, sf::RenderWindow* window, int screenHeight);

#endif
