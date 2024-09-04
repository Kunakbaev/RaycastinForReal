#ifndef SCENE
#define SCENE

#include <SFML/Graphics.hpp>

#include "player.hpp"
#include "obstacle.hpp"

struct Scene {
    int height;
    int width;
    Player player;
    size_t numberOfObstacles;
    Obstacle* obstacles;
};

// FIXME: pointer to return value
Scene constructScene(int height, int width, const Player* player, size_t numberOfObstacles, Obstacle* obstacles);
bool isPlayerPositionGood(const Scene* scene);
void displayScene(Scene* scene, sf::RenderWindow* window);
void displayScreen(const Scene* scene, sf::RenderWindow* screen);

#endif
