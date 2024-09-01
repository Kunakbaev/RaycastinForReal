#include <assert.h>
#include <stdlib.h>

#include "../include/scene.hpp"

// --------------------------------------   CONSTRUCTING SCENE and checking if all objects are valid    --------------

// obstacles should not intersect
static bool validateObstacles(size_t numberOfObstacles, Obstacle* obstacles) {
    for (size_t i = 0; i < numberOfObstacles; ++i)
        for (size_t j = i + 1; j < numberOfObstacles; ++j)
            if (doesObstaclesIntersect(&obstacles[i], &obstacles[j]))
                return false;
    return true;
}

Scene constructScene(int height, int width, const Player* player, size_t numberOfObstacles, Obstacle* obstacles) {
    assert(height > 0);
    assert(width > 0);
    assert(player != NULL);
    //assert(numberOfObstacles >= 0);
    //assert(obstacles != NULL);

    if (!validateObstacles(numberOfObstacles, obstacles)) {
        fprintf(stderr, "obstacles are not valid\n");
        assert(false);
    }

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

     if (!isPlayerPositionGood(&scene)) {
        fprintf(stderr, "player position is not valid\n");
        assert(false);
    }

    return scene;
}

bool isPlayerPositionGood(const Scene* scene) {
    size_t arrLen = scene->numberOfObstacles;
    for (size_t i = 0; i < arrLen; ++i) {
        bool isInter = doesObstacleIntersectWithPlayer(
            &scene->obstacles[i],
            &scene->player
        );

        if ((i != arrLen - 1 && isInter) ||
            (i == arrLen - 1 && !isInter))
                return false;
    }

    return true;
}




// ---------------------------------    DISPLAYING STUFF    ---------------------------------------------

void displayPlayer(const Player* player, sf::RenderWindow* window) {
    assert(player != NULL);
    assert(window != NULL);

    float playerCircleRad = (float)player->bodyRadius;
    sf::CircleShape playerCircle(playerCircleRad);
    playerCircle.setPosition(
        (float)player->position.x - playerCircleRad,
        (float)player->position.y - playerCircleRad
    );
    playerCircle.setFillColor(sf::Color::White);
    window->draw(playerCircle);

    int sightCircleSegmentRadius = 200;
    int STEPS = 10;
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

void displayObstacles(size_t numberOfObstacles, const Obstacle* obstacles, sf::RenderWindow* window) {
    // last obstacle is bounding rect, so we don't want to display it
    for (size_t i = 0; i < numberOfObstacles - 1; ++i)
        displayObstacle(&obstacles[i], window);
}

void displayScene(Scene* scene, sf::RenderWindow* window) {
    assert(scene  != NULL);
    assert(window != NULL);

    displayObstacles(scene->numberOfObstacles, scene->obstacles, window);
    displayPlayer(&scene->player, window);
}




// --------------------------------     FINDING DISTANCES TO WALLS      ------------------------------

struct Pair {
    long double first;
    long double second;
};

int compare(const void* one, const void* two) {
    assert(one != NULL);
    assert(two != NULL);

    struct Pair* p1 = (Pair*)one;
    struct Pair* p2 = (Pair*)two;

    int si = sign(p1->first - p2->first);
    if (si != 0)
        return si;
    return sign(p1->second - p2->second);
}

bool isFirstHit(long double currentDist, const Vector* direction, const Scene* scene) {
    assert(scene != NULL);

    Point origin = scene->player.position;
    for (size_t i = 0; i < scene->numberOfObstacles; ++i) {
        Obstacle obj = scene->obstacles[i];
        for (size_t j = 0; j < obj.numberOfSides; ++j) {
            Segment segm = getSegment(&obj, j);
            long double dist = distanceToSegmByDirection(&origin, direction, &segm);
            if (sign(dist - currentDist) < 0) {
                return false;
            }
        }
    }

    return true;
}

const size_t PAIRS_ARRAY_SIZE = 100;
Pair pairsArray[PAIRS_ARRAY_SIZE] = {};

static void findDistancesToWalls(const Scene* scene, size_t* arrLen) {
    assert(scene  != NULL);
    assert(arrLen != NULL);

    long double FOVhalf  = scene->player.FOV / 2;
    long double minAngle = scene->player.currentDirection - FOVhalf;
    long double maxAngle = scene->player.currentDirection + FOVhalf;

    int cntOfPairs = 0;
    Point origin = scene->player.position;
    for (size_t i = 0; i < scene->numberOfObstacles; ++i) {
        Obstacle obj = scene->obstacles[i];
        for (size_t j = 0; j < obj.numberOfSides; ++j) {
            Point point = obj.sides[j];
            Segment segm = getSegment(&obj, j);
            Vector direction = subVector(&point, &origin);
            long double angle = getVectorAngle(&direction);

            // this ray is not in our field of view
            if (sign(minAngle - angle) > 0 ||
                sign(angle - maxAngle) > 0)
                    continue;

            long double dist = getVectorLen(&direction);
            if (!isFirstHit(dist, &direction, scene)) {
                continue;
            }

            Pair pair = {angle, dist};
            assert(cntOfPairs < PAIRS_ARRAY_SIZE);
            pairsArray[cntOfPairs++] = pair;
        }
    }

    qsort(pairsArray, cntOfPairs, sizeof(Pair), compare);
}

void displayScreen(const Scene* scene, sf::RenderWindow screen) {
    assert(scene  != NULL);
    assert(screen != NULL);

    size_t arrLen = 0;
    findDistancesToWalls(scene, &arrLen);


}
