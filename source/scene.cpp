#include <assert.h>
#include <stdlib.h>

#include "../include/scene.hpp"

#define FREE(x)             \
    do {                    \
        free(x);            \
        x = NULL;           \
    } while(0)

const size_t PAIRS_ARRAY_BUFFER_SIZE = 300;
const long double ROTATION_EPS                 = EPS * 10;
const int MAGIC_COLUMN_HEIGHT_SCALE_FACTOR     = 80;
const int MAGIC_COLUMN_BRIGHTNESS_SCALE_FACTOR = 50;
const int MAX_BRIGHTNESS_VALUE                 = 255; // UINT8_MAX

// ----------------------------   CONSTRUCTING SCENE and checking if all objects are valid    --------------

// obstacles should not intersect
// FIXME:                const
static bool validateObstacles(size_t numberOfObstacles, const Obstacle* obstacles) {
    assert(obstacles != NULL);

    for (size_t obstacleIndex = 0; obstacleIndex < numberOfObstacles; ++obstacleIndex)
        for (size_t obstacleIndex2 = obstacleIndex + 1; obstacleIndex2 < numberOfObstacles; ++obstacleIndex2)
            if (doesObstaclesIntersect(&obstacles[obstacleIndex], &obstacles[obstacleIndex2]))
                return false;
    return true;
}

Scene constructScene(int height, int width, const Player* player, size_t numberOfObstacles, Obstacle* obstacles) {
    assert(height > 0);
    assert(width > 0);
    assert(player != NULL);
    //assert(numberOfObstacles >= 0);
    assert(obstacles != NULL);

    // FIXME: PUT TO DEBUG
    if (!validateObstacles(numberOfObstacles, obstacles)) {
        fprintf(stderr, "obstacles are not valid\n");
        assert(false);
    }

    Scene scene = {height, width, *player, numberOfObstacles, obstacles};

    // FIXME: const
    const Point sides[] = {
        constructPoint(0,     0),
        constructPoint(width, 0),
        constructPoint(width, height),
        constructPoint(0,     height),
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

    scene.pairsArraySize = 0;
    scene.pairsArray = (Pair*)calloc(PAIRS_ARRAY_BUFFER_SIZE, sizeof(Pair));
    assert(scene.pairsArray != NULL);

    return scene;
}

bool isPlayerPositionGood(const Scene* scene) {
    size_t arrLen = scene->numberOfObstacles;
    for (size_t obstacleIndex = 0; obstacleIndex < arrLen; ++obstacleIndex) {
        bool isInter = doesObstacleIntersectWithPlayer(
            &scene->obstacles[obstacleIndex],
            &scene->player
        );

        if ((obstacleIndex != arrLen - 1 && isInter) ||
            (obstacleIndex == arrLen - 1 && !isInter))
                return false;
    }

    return true;
}




// ---------------------------------    DISPLAYING STUFF    ---------------------------------------------

static void displayPlayer(const Scene* scene, sf::RenderWindow* window, int screenHeight) {
    assert(scene  != NULL);
    assert(window != NULL);

    float playerCircleRad = (float)scene->player.bodyRadius;
    sf::CircleShape playerCircle(playerCircleRad);
    playerCircle.setPosition(
        (float)scene->player.position.x - playerCircleRad,
        (float)(screenHeight - ((float)scene->player.position.y) - 1 - playerCircleRad)
    );
    playerCircle.setFillColor(sf::Color::White);
    window->draw(playerCircle);

//     int sightCircleSegmentRadius = 200;
//     size_t STEPS = 10;
//     assert(STEPS >= 2);
//
//     Vector direction = constructPoint(sightCircleSegmentRadius, 0);
//     direction = rotateVectorByAngle(&direction,
//                                     scene->player.currentDirection + scene->player.FOV / 2);
//     // printf("direction : %Lg %Lg\n", direction.x, direction.y);
//
//     long double angleDelta = scene->player.FOV / (STEPS - 1);

    int previousEnd = 0;
    for (size_t i = 1; i < scene->pairsArraySize; i += 2) {
        Pair p1 = scene->pairsArray[i - 1];
        Pair p2 = scene->pairsArray[i];

        Vector direction = constructPoint(1, 0);
        direction = rotateVectorByAngle(&direction,
                                        p1.first);
        vectorMultByConst(&direction, p1.second);
        Point one = addVector(&scene->player.position, &direction);

        direction = constructPoint(1, 0);
        direction = rotateVectorByAngle(&direction,
                                        p2.first);
        vectorMultByConst(&direction, p2.second);
        Point two = addVector(&scene->player.position, &direction);
        printf("---------------\n");
        printf("one : %Lg, %Lg\n", one.x, one.y);
        printf("two : %Lg, %Lg\n", two.x, two.y);
        printf("beb : %Lg, %Lg\n", scene->player.position.x, scene->player.position.y);

        sf::VertexArray arr(sf::TriangleFan);
        sf::Vertex vert(sf::Vector2f(one.x, one.y));
        vert.color = sf::Color::Magenta;
        arr.append(vert);

        vert.position = sf::Vector2f(two.x, two.y);
        arr.append(vert);

        vert.position = sf::Vector2f(scene->player.position.x, scene->player.position.y);
        arr.append(vert);

        printf("arr : \n");
        for (size_t pointInd = 0; pointInd < arr.getVertexCount(); ++pointInd) {
            arr[pointInd].position.y = screenHeight - arr[pointInd].position.y - 1;
            printf("    p : %Lf, %Lf\n", arr[pointInd].position.x, arr[pointInd].position.y);
        }

        window->draw(arr);
    }

//     sf::Vertex vert(sf::Vector2f(player->position.x, player->position.y));
//     vert.color = sf::Color::Magenta;
//     arr.append(vert);
//     for (size_t _ = 0; _ < STEPS; ++_) {
//         Point point = addVector(&player->position, &direction);
//         direction = rotateVectorByAngle(&direction, -angleDelta);
//
//         sf::Vertex vert(sf::Vector2f(point.x, point.y));
//         vert.color = sf::Color::Magenta;
//         arr.append(vert);
//     }
//     arr.append(vert);
//
    // for (size_t pointInd = 0; pointInd < arr.getVertexCount(); ++pointInd)
    //     arr[pointInd].position.y = screenHeight - arr[pointInd].position.y - 1;
//     window->draw(arr);
}

static void displayObstacles(size_t numberOfObstacles, const Obstacle* obstacles, sf::RenderWindow* window, int screenHeight) {
    // last obstacle is bounding rect, so we don't want to display it
    for (size_t i = 0; i < numberOfObstacles - 1; ++i)
        displayObstacle(&obstacles[i], window, screenHeight);
}

void displayScene(Scene* scene, sf::RenderWindow* window) {
    assert(scene  != NULL);
    assert(window != NULL);

    displayPlayer(scene, window, scene->height);
    displayObstacles(scene->numberOfObstacles, scene->obstacles, window, scene->height);
}




// --------------------------------     FINDING DISTANCES TO WALLS      ------------------------------

static int compare(const void* one, const void* two) {
    assert(one != NULL);
    assert(two != NULL);

    const struct Pair* p1 = (const Pair*)one; // FIXME: warning
    const struct Pair* p2 = (const Pair*)two;

    int si = sign(p1->first - p2->first);
    if (si != 0)
        return -si;
    return sign(p1->second - p2->second);
}

static long double findMinDistForDirection(const Vector* direction, const Scene* scene) {
    assert(direction != NULL);
    assert(scene     != NULL);

    long double bestDist = INF;
    Point origin = scene->player.position;
    //    obstInd
    for (size_t obstInd = 0; obstInd < scene->numberOfObstacles; ++obstInd) {
        Obstacle obj = scene->obstacles[obstInd];
        //         sideInd
        for (size_t sideInd = 0; sideInd < obj.numberOfSides; ++sideInd) {
            Segment segm = getSegment(&obj, sideInd);
            long double dist = distanceToSegmByDirection(&origin, direction, &segm);
            if (sign(dist - bestDist) < 0) {
                bestDist = dist;
            }
        }
    }

    return bestDist;
}

static void addMostLeftAndRightRays(Scene* scene) {
    assert(scene             != NULL);
    assert(scene->pairsArray != NULL);

    Vector leftRay = constructPoint(1, 0);
    leftRay = rotateVectorByAngle(&leftRay, scene->player.currentDirection + scene->player.FOV / 2);
    Vector rightRay = rotateVectorByAngle(&leftRay, -scene->player.FOV);
    long double leftDist = findMinDistForDirection(&leftRay, scene);
    long double rightDist = findMinDistForDirection(&rightRay, scene);

    scene->pairsArray[scene->pairsArraySize++] = {getMaxFOVangle(&scene->player), leftDist};
    scene->pairsArray[scene->pairsArraySize++] = {getMinFOVangle(&scene->player), rightDist};
}

static void findDistancesToWalls(Scene* scene) {
    assert(scene != NULL);

    scene->pairsArraySize = 0;
    Point origin = scene->player.position;
    for (size_t objInd = 0; objInd < scene->numberOfObstacles; ++objInd) {
        Obstacle obj = scene->obstacles[objInd];
        for (size_t sideInd = 0; sideInd < obj.numberOfSides; ++sideInd) {
            Point point = obj.sides[sideInd];
            Segment segm = getSegment(&obj, sideInd);

            Vector direction = subVector(&point, &origin);
            long double angle = NAN;
            bool isOk = checkIfDirectionInsideFOV(&scene->player, direction, &angle);
            if (!isOk) continue;

            long double dist = distanceToSegmByDirection(&origin, &direction, &segm);
            long double bestDist = findMinDistForDirection(&direction, scene);

            if (sign(bestDist - dist) < 0)
                continue;

            direction = rotateVectorByAngle(&direction, ROTATION_EPS);
            for (int isToRight = 0; isToRight <= 1; ++isToRight) {
                if (sign(getVectorLen(&direction)) == 0)
                    continue;

                dist = findMinDistForDirection(&direction, scene);
                Pair pair = {angle + (isToRight ? -ROTATION_EPS : ROTATION_EPS), dist};
                assert(scene->pairsArraySize < PAIRS_ARRAY_BUFFER_SIZE);
                scene->pairsArray[scene->pairsArraySize++] = pair;

                direction = rotateVectorByAngle(&direction, -2 * ROTATION_EPS);
            }
        }
    }

    addMostLeftAndRightRays(scene);
    qsort(scene->pairsArray, scene->pairsArraySize, sizeof(Pair), compare);
}

static int getColumnColorByDistance(long double dist) {
    //assert(sign(dist) > 0);

    long double koef = sign(dist) ? 1 / dist : 1;
    koef *= MAGIC_COLUMN_BRIGHTNESS_SCALE_FACTOR;
    if (sign(koef - 1) > 0)
        koef = 1.0;

    return (int)(MAX_BRIGHTNESS_VALUE * koef);
}

static int getColumnHeightByDistance(const Scene* scene, long double dist) {
    assert(scene      != NULL);
    //assert(sign(dist) > 0);

    long double koef = sign(dist) ? 1 / dist : 1;
    koef *= MAGIC_COLUMN_HEIGHT_SCALE_FACTOR;

    int height = (int)(scene->height * koef);

    // FIXME:
    if (height < 1) height = 1;
    return height;
}


// alternative codestyle
static void
drawTrapezoid(int start,
              int end,
              int startHeight,
              int endHeight,
              int screenHeight,
              sf::RenderWindow* screen,
              int colorStart,
              int colorEnd) {
    assert(screenHeight > 0);
    assert(screen       != NULL);

    int mid = screenHeight / 2;
    sf::VertexArray arr(sf::TrianglesFan);
    for (int i = 0; i < 4; ++i) {
        bool isLeft = i == 0 || i == 3;
        int height = (isLeft ? startHeight : endHeight) / 2;
        sf::Uint8 colorValue = (sf::Uint8)(isLeft ? colorStart : colorEnd);
        sf::Color color(colorValue, colorValue, colorValue);
        if (i <= 1) height *= -1;

        sf::Vertex vert(sf::Vector2f(isLeft ? start : end,
                                     mid + height));
        vert.color = color;
        arr.append(vert);
    }

    screen->draw(arr);
}

void displayScreen(Scene* scene, sf::RenderWindow* screen) {
    assert(scene  != NULL);
    assert(screen != NULL);

    findDistancesToWalls(scene);
    //printf("arrLen : %d\n", arrLen);
    // printf("cur dir: %Lg\n", scene->player.currentDirection);
    //return;

    //printf("--------------------\n");
    int previousEnd = 0;
    for (size_t i = 1; i < scene->pairsArraySize; i += 2) {
        Pair p1 = scene->pairsArray[i - 1];
        Pair p2 = scene->pairsArray[i];

        long double koef1 = (p1.first - p2.first) / scene->player.FOV;
        int width         = (int)roundl(scene->width * koef1);
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

void destructScene(Scene* scene) {
    assert(scene != NULL);

    // for (size_t obstacleInd = 0; obstacleInd < scene->numberOfObstacles; ++obstacleInd) {
    //     destructObstacle(scene->obstacles[obstacleInd]);
    // }
    FREE(scene->obstacles);
    scene->numberOfObstacles = 0;

    FREE(scene->pairsArray);
    scene->pairsArraySize = 0;
}
