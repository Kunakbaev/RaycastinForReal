#include <assert.h>
#include <stdlib.h>

#include "../include/scene.hpp"

const long double ROTATION_EPS = 1e-6;

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

void displayPlayer(const Player* player, sf::RenderWindow* window, int screenHeight) {
    assert(player != NULL);
    assert(window != NULL);

    float playerCircleRad = (float)player->bodyRadius;
    sf::CircleShape playerCircle(playerCircleRad);
    playerCircle.setPosition(
        (float)player->position.x - playerCircleRad,
        screenHeight - ((float)player->position.y) - 1 - playerCircleRad
    );
    playerCircle.setFillColor(sf::Color::White);
    window->draw(playerCircle);

    int sightCircleSegmentRadius = 200;
    int STEPS = 10;
    assert(STEPS >= 2);

    Vector direction = constructPoint(sightCircleSegmentRadius, 0);
    direction = rotateVectorByAngle(&direction,
                                    player->currentDirection + player->FOV / 2);
    // printf("direction : %Lg %Lg\n", direction.x, direction.y);

    long double angleDelta = player->FOV / (STEPS - 1);
    sf::VertexArray arr(sf::TrianglesFan);

    sf::Vertex vert(sf::Vector2f(player->position.x, player->position.y));
    vert.color = sf::Color::Magenta;
    arr.append(vert);
    for (int i = 0; i < STEPS; ++i) {
        Point point = addVector(&player->position, &direction);
        direction = rotateVectorByAngle(&direction, -angleDelta);

        sf::Vertex vert(sf::Vector2f(point.x, point.y));
        vert.color = sf::Color::Magenta;
        arr.append(vert);
        //if (i == 1) break;
    }
    arr.append(vert);

    for (int i = 0; i < arr.getVertexCount(); ++i)
        arr[i].position.y = screenHeight - arr[i].position.y - 1;
    window->draw(arr);
}

void displayObstacles(size_t numberOfObstacles, const Obstacle* obstacles, sf::RenderWindow* window, int screenHeight) {
    // last obstacle is bounding rect, so we don't want to display it
    for (size_t i = 0; i < numberOfObstacles - 1; ++i)
        displayObstacle(&obstacles[i], window, screenHeight);
}

void displayScene(Scene* scene, sf::RenderWindow* window) {
    assert(scene  != NULL);
    assert(window != NULL);

    displayPlayer(&scene->player, window, scene->height);
    displayObstacles(scene->numberOfObstacles, scene->obstacles, window, scene->height);
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
        return -si;
    return sign(p1->second - p2->second);
}

long double findMinDistForDirection(const Vector* direction, const Scene* scene) {
    assert(direction != NULL);
    assert(scene     != NULL);

    long double bestDist = INF;
    Point origin = scene->player.position;
    for (size_t i = 0; i < scene->numberOfObstacles; ++i) {
        Obstacle obj = scene->obstacles[i];
        for (size_t j = 0; j < obj.numberOfSides; ++j) {
            Segment segm = getSegment(&obj, j);
            long double dist = distanceToSegmByDirection(&origin, direction, &segm);
            if (sign(dist - bestDist) < 0) {
                bestDist = dist;
            }
        }
    }

    return bestDist;
}

const size_t PAIRS_ARRAY_SIZE = 300;
Pair pairsArray[PAIRS_ARRAY_SIZE] = {};

static void findDistancesToWalls(const Scene* scene, size_t* arrLen) {
    assert(scene  != NULL);
    assert(arrLen != NULL);

    long double FOVhalf  = scene->player.FOV / 2;
    long double minAngle = scene->player.currentDirection - FOVhalf;
    long double maxAngle = scene->player.currentDirection + FOVhalf;
    int cntLoops = maxAngle / (2 * PIE);
    minAngle -= cntLoops * 2 * PIE;
    maxAngle -= cntLoops * 2 * PIE;

    Vector leftRay = constructPoint(1, 0);
    leftRay = rotateVectorByAngle(&leftRay, scene->player.currentDirection + scene->player.FOV / 2);
    Vector rightRay = rotateVectorByAngle(&leftRay, -scene->player.FOV);
    // printf("left  %Lg : , %Lg : \n", leftRay.x, leftRay.y);
    // printf("right %Lg : , %Lg : \n", rightRay.x, rightRay.y);
    // printf("minAngle: %Lg, maxAngle: %Lg\n", minAngle, maxAngle);

    int numberOfPairs = 0;
    Point origin = scene->player.position;
    for (size_t i = 0; i < scene->numberOfObstacles; ++i) {
        Obstacle obj = scene->obstacles[i];
        for (size_t j = 0; j < obj.numberOfSides; ++j) {
            Point point = obj.sides[j];
            //printf("point : %Lg, %Lg\n", point.x, point.y);
            Segment segm = getSegment(&obj, j);

            Vector direction = subVector(&point, &origin);
            long double angle = getVectorAngle(&direction);

             // FIXME:not proper way to check this
            // this ray is not in our field of view
            bool isOk = false;
            for (int j = -3; j <= 3; ++j) {
                long double cur = angle + j * 2 * PIE;
                //printf("cur : %Lg\n", cur);
                if (sign(cur - minAngle) >= 0 && sign(cur - maxAngle) <= 0) {
                    isOk = true;
                    angle = cur;
                    break;
                }
            }
            if (!isOk) continue;
            //if (!i) printf("obj: %d, vert: %d\n", i, j);

            // if (!isInsideAngle(&direction, &origin, &leftRay, &rightRay))
            //     continue;

            long double dist = distanceToSegmByDirection(&origin, &direction, &segm);
            long double bestDist = findMinDistForDirection(&direction, scene);
            //printf("dist : %Lg, best: %Lg\n", dist, bestDist);

            // FIXME: this should work without check, because we will just draw on top and further objects are smaller
            // UPD: it's not working
            if (sign(bestDist - dist) < 0) {
                continue;
            }
            //if (!i) printf("OK obj: %d, vert: %d\n", i, j);
            // printf("OKKK\n");

            direction = rotateVectorByAngle(&direction, ROTATION_EPS);
            for (int isToRight = 0; isToRight <= 1; ++isToRight) {
                if (sign(getVectorLen(&direction)) == 0)
                    continue;

                dist = findMinDistForDirection(&direction, scene);
                Pair pair = {angle + (isToRight ? -ROTATION_EPS : ROTATION_EPS), dist};
                assert(numberOfPairs < PAIRS_ARRAY_SIZE);
                pairsArray[numberOfPairs++] = pair;

                direction = rotateVectorByAngle(&direction, -2 * ROTATION_EPS);
            }
        }
    }

    long double leftDist = findMinDistForDirection(&leftRay, scene);
    long double rightDist = findMinDistForDirection(&rightRay, scene);

    pairsArray[numberOfPairs++] = {maxAngle, leftDist};
    pairsArray[numberOfPairs++] = {minAngle, rightDist};

    *arrLen = numberOfPairs;
    qsort(pairsArray, numberOfPairs, sizeof(Pair), compare);
}

static int getColumnColorByDistance(long double dist) {
    //assert(sign(dist) > 0);

    long double koef = sign(dist) ? 1 / dist : 1;
    koef *= 50;
    if (sign(koef - 1) > 0)
        koef = 1.0;

    int color = 255 * koef;

    // FIXME:
    return color;
}

static int getColumnHeightByDistance(const Scene* scene, long double dist) {
    assert(scene      != NULL);
    //assert(sign(dist) > 0);

    long double koef = sign(dist) ? 1 / dist : 1;
    koef *= 100;
    if (sign(koef - 1) > 0)
        koef = 1.0;

    int height = scene->height * koef;

    // FIXME:
    if (height < 1) height = 1;
    return height;
}

static void drawTrapezoid(int start, int end, int startHeight, int endHeight, int screenHeight,
        sf::RenderWindow* screen, int colorStart, int colorEnd) {
    assert(screenHeight > 0);
    assert(screen       != NULL);

    int mid = screenHeight / 2;
    sf::VertexArray arr(sf::TrianglesFan);
    for (int i = 0; i < 4; ++i) {
        bool isLeft = i == 0 || i == 3;
        int height = (isLeft ? startHeight : endHeight) / 2;
        int colorValue = isLeft ? colorStart : colorEnd;
        sf::Color color(colorValue, colorValue, colorValue);
        if (i <= 1) height *= -1;

        sf::Vertex vert(sf::Vector2f(isLeft ? start : end,
                                     mid + height));
        vert.color = color;
        arr.append(vert);
    }

    screen->draw(arr);
}

void displayScreen(const Scene* scene, sf::RenderWindow* screen) {
    assert(scene  != NULL);
    assert(screen != NULL);

    size_t arrLen = 0;
    findDistancesToWalls(scene, &arrLen);
    //printf("arrLen : %d\n", arrLen);
    // printf("cur dir: %Lg\n", scene->player.currentDirection);
    //return;

    long double maxAngle = scene->player.currentDirection + scene->player.FOV / 2;
    int cntLoops = maxAngle / (2 * PIE);
    long double startAngle = maxAngle - cntLoops * 2 * PIE;
    //printf("--------------------\n");
    int previousEnd = 0;
    for (size_t i = 1; i < arrLen; i += 2) {
        Pair p1 = pairsArray[i - 1];
        Pair p2 = pairsArray[i];

        long double koef1 = (p1.first - p2.first) / scene->player.FOV;
        int width         = roundl(scene->width * koef1);
        long double koef2 = (startAngle - p1.first) / scene->player.FOV;
        int end           = previousEnd + width;
        int startHeight   = getColumnHeightByDistance(scene, p1.second);
        int endHeight     = getColumnHeightByDistance(scene, p2.second);
        int colorStart    = getColumnColorByDistance(p1.second);
        int colorEnd      = getColumnColorByDistance(p2.second);
        //printf("%Lg %Lg\n", p1.first, p2.first);

        drawTrapezoid(previousEnd, end, startHeight, endHeight, scene->height,
            screen, colorStart, colorEnd);
        previousEnd = end;
    }
}
