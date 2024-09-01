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

Scene constructScene(int height, int width, const Player* player, size_t numberOfObstacles, Obstacle* obstacles);
void displayScene(Scene* scene, sf::RenderWindow* window);

#endif
