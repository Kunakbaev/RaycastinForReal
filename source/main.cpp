#include <stdio.h>
#include <SFML/Graphics.hpp>

#include "../include/environmentLib.hpp"
#include "../include/scene.hpp"

const int WIDTH = 800, HEIGHT = 800;

int main() {
    // Point startingPosition = constructPoint(WIDTH / 2, HEIGHT / 2);
    Point startingPosition = constructPoint(697.428, 523.2);
    int FOVinDegrees = 60;
    Player player = constructPlayer(&startingPosition, 2.6579, FOVinDegrees * (PIE / 180), 0.04, 0.0004, 8);

    Point circleCenter = constructPoint(600, 650);
    Obstacle obstacles[] = {
        constructRectObstacle(100, 100, 500, 300),
        constructRectObstacle(300, 350, 600, 550),
        constructCircleObstacle(&circleCenter, 50, 10)
    };
    Scene scene = constructScene(WIDTH, HEIGHT, &player, 3, obstacles);

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::VideoMode mode(WIDTH, HEIGHT);
    sf::RenderWindow sceneWindow(mode, "scene",  sf::Style::Default, settings);
    sf::RenderWindow screenWindow(mode, "screen", sf::Style::Default, settings);
    screenWindow.setMouseCursorVisible(false);

    Environment env = {};
    constructEnvironment(WIDTH, HEIGHT, &sceneWindow, &screenWindow, &env);

    while (isEnvOpen(&env)) {
        KeyboardActions action = windowEventsLoops(&env);
        //if (action) printf("action : %d\n", action);

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


        // switch (action) {
        //     case QUIT_COMMAND:
        //         closeWindows(&env);
        //         break;
        //     case MOVE_PLAYER_FORWARD:
        //         //printf("forward\n");
        //         movePlayerForward(&scene.player);
        //         break;
        //     case MOVE_PLAYER_BACKWARD:
        //         movePlayerBackward(&scene.player);
        //         break;
        //     case TURN_PLAYER_LEFT:
        //         turnPlayerByAngle(&scene.player, TURN_LEFT);
        //         break;
        //     case TURN_PLAYER_RIGHT:
        //         turnPlayerByAngle(&scene.player, TURN_RIGHT);
        //         break;
        //     case NO_ACTION:
        //     default:
        //         break;
        // }

        // new player position is not valid, so we don't move
        if (!isPlayerPositionGood(&scene))
            scene.player.position = previousPlayerPosition;

        //printf("pos : %Lg\n", scene.player.position.x);
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
