#ifndef SCENE
#define SCENE

#include <SFML/Graphics.hpp>

#include "player.hpp"
#include "obstacle.hpp"

struct Pair {
    long double first;
    long double second;
};

struct Scene {
    int height;
    int width;
    Player player;
    size_t numberOfObstacles;
    Obstacle* obstacles;
    size_t pairsArraySize;
    Pair* pairsArray;
};

// FIXME: pointer to return value
Scene constructScene(int height, int width, const Player* player, size_t numberOfObstacles, Obstacle* obstacles);
bool isPlayerPositionGood(const Scene* scene);
void displayScene(Scene* scene, sf::RenderWindow* window);
void displayScreen(Scene* scene, sf::RenderWindow* screen);
void destructScene(Scene* scene);

#endif
