#ifndef INCLUDE_SCENE_HPP
#define INCLUDE_SCENE_HPP

#include <SFML/Graphics.hpp>

#include "environmentLib.hpp"
#include "obstacle.hpp"
#include "player.hpp"

struct Pair {
    long double first;
    long double second;
};

struct Scene {
    int       height;
    int       width;
    Player    player;
    size_t    numberOfObstacles;
    Obstacle* obstacles;
    size_t    pairsArraySize;
    Pair*     pairsArray;
};

Scene constructScene(
    int           height,
    int           width,
    const Player* player,
    size_t        numberOfObstacles,
    Obstacle*     obstacles
);
bool isPlayerPositionGood(const Scene* scene);
void displayScene(Scene* scene, Environment* env);
void displayScreen(Scene* scene, Environment* env);
void destructScene(Scene* scene);

#endif
