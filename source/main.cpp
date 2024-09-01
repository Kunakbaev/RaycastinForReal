#include <stdio.h>
#include <SFML/Graphics.hpp>

#include "../include/scene.hpp"

const int WIDTH = 800, HEIGHT = 800;

int main() {

    Point startingPosition = constructPoint(WIDTH / 2, HEIGHT / 2);
    Player player = constructPlayer(&startingPosition, 0, PIE / 3, 0.2, 0.004);
    Scene scene = constructScene(WIDTH, HEIGHT, &player, 0, NULL);

    sf::RenderWindow sceneWindow(sf::VideoMode(WIDTH, HEIGHT), "Scene");
    // sf::RenderWindow screenWindow(sf::VideoMode(WIDTH, HEIGHT), "Screen");

    while (sceneWindow.isOpen()) {
        sf::Event event;
        while (sceneWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                sceneWindow.close();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
            sceneWindow.close();
        }
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

        sceneWindow.clear();

        displayScene(&scene, &sceneWindow);

        sceneWindow.display();
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
