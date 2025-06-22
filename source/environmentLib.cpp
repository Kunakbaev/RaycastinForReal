#include <assert.h>

#include "../include/environmentLib.hpp"

const sf::Keyboard::Key QUIT_SYMBOL = sf::Keyboard::Q;

void constructEnvironment(
    size_t            width,
    size_t            height,
    sf::RenderWindow* sceneWindow,
    sf::RenderWindow* screenWindow,
    Environment*      env
) {
    assert(sceneWindow  != NULL);
    assert(screenWindow != NULL);

    Environment result = { 
        .width        = width,
        .height       = height,
        .sceneWindow  = sceneWindow,
        .screenWindow = screenWindow
    };
    *env = result;
}

bool isEnvOpen(const struct Environment* env) {
    assert(env               != NULL);
    assert(env->screenWindow != NULL);
    assert(env->sceneWindow  != NULL);

    return env->screenWindow->isOpen() &&
           env-> sceneWindow->isOpen();
}

#define IS_KEY_PRESSED(key) sf::Keyboard::isKeyPressed(sf::Keyboard::key)

bool isForwardMove() {
    return IS_KEY_PRESSED(W) || IS_KEY_PRESSED(Up);
}

bool isBackwardMove() {
    return IS_KEY_PRESSED(S) || IS_KEY_PRESSED(Down);
}

bool isTurnRight() {
    return IS_KEY_PRESSED(D) || IS_KEY_PRESSED(Right);
}

bool isTurnLeft() {
    return IS_KEY_PRESSED(A) || IS_KEY_PRESSED(Left);
}

bool isQuitPressed() {
    return sf::Keyboard::isKeyPressed(QUIT_SYMBOL);
}

static void windowEventsLoopHelper(sf::RenderWindow* window) {
    assert(window != NULL);

    sf::Event event = {};
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window->close();
    }
}

void windowEventsLoops(const struct Environment* env) {
    windowEventsLoopHelper(env->sceneWindow);
    windowEventsLoopHelper(env->screenWindow);
}

void drawCircle(
    Environment*      env, 
    sf::RenderWindow* window, 
    const Point*      point,
    float             radius,
    sf::Color         color
) {
    assert(env          != NULL);
    assert(window       != NULL);
    assert(point        != NULL);
    assert(sign(radius) > 0);

    sf::CircleShape playerCircle(radius);
    playerCircle.setPosition(
        (float)point->x - radius,
        (float)env->height - (float)point->y - 1 - radius
    );
    playerCircle.setFillColor(color);
    window->draw(playerCircle);
}

void drawConvexShape(
    Environment*       env,
    sf::RenderWindow*  window,
    size_t             numberOfPoints,
    const Point* const points,
    sf::Color          color
) {
    assert(env            != NULL);
    assert(window         != NULL);
    assert(numberOfPoints >= 3);
    assert(points         != NULL);

    sf::ConvexShape poly;
    poly.setPointCount(numberOfPoints);
    poly.setFillColor(color);
    for (size_t pointIndex = 0; pointIndex < numberOfPoints; ++pointIndex) {
        sf::Vector2f vert((float)points[pointIndex].x,
                          (float)env->height - (float)points[pointIndex].y - 1);
        poly.setPoint(pointIndex, vert);
    }
    window->draw(poly);
}

void drawVertexArray(
    Environment*      env,
    sf::RenderWindow* window,
    sf::VertexArray   array
) {
    assert(env    != NULL);
    assert(window != NULL);

    window->draw(array);
}

void clearWindows(Environment* env) {
    assert(env               != NULL);
    assert(env->sceneWindow  != NULL);
    assert(env->screenWindow != NULL);

    env->sceneWindow->clear();
    env->screenWindow->clear();
}

void drawWindows(Environment* env) {
    assert(env               != NULL);
    assert(env->sceneWindow  != NULL);
    assert(env->screenWindow != NULL);

    env->sceneWindow->display();
    env->screenWindow->display();
}

void closeWindows(Environment* env) {
    assert(env               != NULL);
    assert(env->sceneWindow  != NULL);
    assert(env->screenWindow != NULL);

    env->sceneWindow->close();
    env->screenWindow->close();
}
