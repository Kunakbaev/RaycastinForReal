#include <stdio.h>
#include <SFML/Graphics.hpp>

#include "../include/scene.hpp"

const int WIDTH = 800, HEIGHT = 800;

int main() {

//     Vector a = constructPoint(1, 0);
//     a = rotateVectorByAngle(&a, PIE / 6);
//     printf("%Lg %Lg\n", a.x, a.y);
//
//     return 0;

    // Point startingPosition = constructPoint(WIDTH / 2, HEIGHT / 2);
    Point startingPosition = constructPoint(30, 30);
    Player player = constructPlayer(&startingPosition, 0, PIE / 3, 0.2, 0.002, 0.1);

    Obstacle obstacles[] = {
        constructRectObstacle(100, 100, 500, 300),
        constructRectObstacle(300, 350, 600, 550),
    };
    Scene scene = constructScene(WIDTH, HEIGHT, &player, 2, obstacles);

    sf::RenderWindow sceneWindow(sf::VideoMode(WIDTH, HEIGHT), "Scene");
    sf::RenderWindow screenWindow(sf::VideoMode(WIDTH, HEIGHT), "Screen");

    while (sceneWindow.isOpen()) {
        sf::Event event;
        while (sceneWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                sceneWindow.close();
        }
        while (screenWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                screenWindow.close();
        }




        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
            sceneWindow.close();
            screenWindow.close();
        }





        Point previousPlayerPosition = scene.player.position;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            movePlayer(&scene.player, DIRECTION_UP);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            movePlayer(&scene.player, DIRECTION_RIGHT);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            movePlayer(&scene.player, DIRECTION_DOWN);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            movePlayer(&scene.player, DIRECTION_LEFT);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            turnPlayerByAngle(&scene.player, TURN_RIGHT);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            turnPlayerByAngle(&scene.player, TURN_LEFT);
        }

        // new player position is not valid, so we don't move
        if (!isPlayerPositionGood(&scene)) {
            scene.player.position = previousPlayerPosition;
        }

        sceneWindow.clear();
        screenWindow.clear();

        displayScene(&scene, &sceneWindow);
        displayScreen(&scene, &screenWindow);

        sceneWindow.display();
        screenWindow.display();
    }

    while (screenWindow.isOpen()) {
        sf::Event event;
        while (screenWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                screenWindow.close();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
            screenWindow.close();
        }
        screenWindow.clear();

        displayScreen(&scene, &screenWindow);

        screenWindow.display();
    }

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
