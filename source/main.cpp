#include <stdio.h>
#include <SFML/Graphics.hpp>

#include "../include/environmentLib.hpp"
#include "../include/scene.hpp"

const int WIDTH = 800, HEIGHT = 800;

int main() {
    // Point startingPosition = constructPoint(WIDTH / 2, HEIGHT / 2);
    Point startingPosition = constructPoint(697.428, 523.2);
    int FOVinDegrees = 60;
    Player player = constructPlayer(&startingPosition, 2.6579, FOVinDegrees * (PIE / 180), 0.04, 0.0004, 15);

    Point circleCenter = constructPoint(600, 650);
    Obstacle obstacles[] = {
        constructRectObstacle(100, 100, 500, 300),
        constructRectObstacle(300, 350, 600, 550),
        constructCircleObstacle(&circleCenter, 50, 10)
    };
    Scene scene = constructScene(WIDTH, HEIGHT, &player, 3, obstacles);

    Environment env = {};
    constructEnvironment(WIDTH, HEIGHT, "scene", "screen", &env);
    while (isEnvOpen(&env)) {
        KeyboardActions action = windowEventsLoops(&env);

        Point previousPlayerPosition = scene.player.position;
        switch (action) {
            case QUIT_COMMAND:
                closeWindows(&env);
                break;
            case MOVE_PLAYER_FORWARD:
                movePlayerForward(&scene.player);
                break;
            case MOVE_PLAYER_BACKWARD:
                movePlayerBackward(&scene.player);
                break;
            case TURN_PLAYER_LEFT:
                turnPlayerByAngle(&scene.player, TURN_LEFT);
                break;
            case TURN_PLAYER_RIGHT:
                turnPlayerByAngle(&scene.player, TURN_RIGHT);
                break;
            case NO_ACTION:
            default:
                break;
        }

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

/*

if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
    movePlayer(&scene.player, DIRECTION_UP);
}
if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
    movePlayer(&scene.player, DIRECTION_RIGHT);
}
if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
    movePlayer(&scene.player, DIRECTION_DOWN);
}
if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
    movePlayer(&scene.player, DIRECTION_LEFT);
}

*/
