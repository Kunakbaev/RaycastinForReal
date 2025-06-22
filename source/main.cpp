#include <stdio.h>
#include <SFML/Graphics.hpp>

#include "../include/environmentLib.hpp"
#include "../include/scene.hpp"

const int WIDTH        = 800;
const int HEIGHT       = 800;
const int FOVinDegrees = 60;

int main() {
    Point startingPosition = constructPoint(697.428, 523.2);
    Player player = constructPlayer(&startingPosition, 2.6579, FOVinDegrees * (PIE / 180), 0.04, 0.0004, 8);

    Point circleCenter = constructPoint(600, 650);
    Obstacle obstacles[] = {
        constructRectObstacle(100, 100, 500, 300),
        constructRectObstacle(300, 350, 600, 550),
        constructCircleObstacle(&circleCenter, 50, 10)
    };
    const size_t NUM_OF_OBSTACLES = sizeof(obstacles) / sizeof(*obstacles);
    Scene scene = constructScene(WIDTH, HEIGHT, &player, NUM_OF_OBSTACLES, obstacles);

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::VideoMode mode(WIDTH, HEIGHT);
    sf::RenderWindow  sceneWindow(mode, "scene",  sf::Style::Default, settings);
    sf::RenderWindow screenWindow(mode, "screen", sf::Style::Default, settings);
    screenWindow.setMouseCursorVisible(false);

    Environment env = {};
    constructEnvironment(WIDTH, HEIGHT, &sceneWindow, &screenWindow, &env);

    while (isEnvOpen(&env)) {
        windowEventsLoops(&env);

        Point previousPlayerPosition = scene.player.position;
        if (isQuitPressed()) closeWindows(&env);
        if (isForwardMove())
            movePlayerForward(&scene.player);
        if (isBackwardMove())
            movePlayerBackward(&scene.player);
        if (isTurnLeft())
            turnPlayerByAngle(&scene.player, TURN_LEFT);
        if (isTurnRight())
            turnPlayerByAngle(&scene.player, TURN_RIGHT);

        // new player position is not valid, so we don't move
        if (!isPlayerPositionGood(&scene))
            scene.player.position = previousPlayerPosition;

        clearWindows(&env);
        displayScreen(&scene, &env);
        displayScene(&scene, &env);
        drawWindows(&env);
    }
    destructScene(&scene);

    return 0;
}
