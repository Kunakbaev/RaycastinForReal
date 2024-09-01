#include <assert.h>
#include <stdlib.h>

#include "../include/scene.hpp"

Scene constructScene(int height, int width, const Player* player, size_t numberOfObstacles, Obstacle* obstacles) {
    assert(height > 0);
    assert(width > 0);
    assert(player != NULL);
    //assert(numberOfObstacles >= 0);
    //assert(obstacles != NULL);

    Scene scene = {height, width, *player, numberOfObstacles, obstacles};

    Point sides[] = {
        constructPoint(0, 0),
        constructPoint(width, 0),
        constructPoint(width, height),
        constructPoint(0, height),
    };
    Obstacle bounds = constructObstacle(4, sides);

    scene.numberOfObstacles = numberOfObstacles + 1;
    scene.obstacles = (Obstacle*)calloc(numberOfObstacles + 1, sizeof(Obstacle));
    for (size_t i = 0; i <= numberOfObstacles; ++i) {
        if (i != numberOfObstacles) {
            scene.obstacles[i] = obstacles[i];
        } else {
            scene.obstacles[i] = bounds;
        }
    }

    return scene;
}

void displayPlayer(const Player* player, sf::RenderWindow* window) {
    assert(player != NULL);
    assert(window != NULL);

    float playerCircleRad = 20.0;
    sf::CircleShape playerCircle(playerCircleRad);
    //playerCircle.setOrigin(100, 100);
    playerCircle.setPosition(
        (float)player->position.x - playerCircleRad,
        (float)player->position.y - playerCircleRad
    );
    playerCircle.setFillColor(sf::Color::White);
    window->draw(playerCircle);

    int sightCircleSegmentRadius = 200;
    int STEPS = 20;
    assert(STEPS >= 2);

    Vector direction = constructPoint(0, sightCircleSegmentRadius);
    direction = rotateVectorByAngle(&direction,
                                    player->currentDirection - player->FOV / 2);

    long double angleDelta = player->FOV / (STEPS - 1);
    sf::VertexArray arr(sf::TrianglesFan);

    sf::Vertex vert(sf::Vector2f(player->position.x, player->position.y));
    vert.color = sf::Color::Magenta;
    arr.append(vert);
    for (int i = 0; i < STEPS; ++i) {
        Point point = addVector(&player->position, &direction);
        direction = rotateVectorByAngle(&direction, angleDelta);

        sf::Vertex vert(sf::Vector2f(point.x, point.y));
        vert.color = sf::Color::Magenta;
        arr.append(vert);
    }
    arr.append(vert);

    window->draw(arr);
}

void displayScene(Scene* scene, sf::RenderWindow* window) {
    assert(scene  != NULL);
    assert(window != NULL);

    displayPlayer(&scene->player, window);
}
