#include <assert.h>

#include "../include/geometryLib.hpp"
#include "../include/environmentLib.hpp"

void constructEnvironment(size_t width, size_t height, const char* sceneName, const char* screenName, Environment* env) {
    assert(sceneName  != NULL);
    assert(screenName != NULL);

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow sceneWindow(sf::VideoMode(width, height), sceneName,  sf::Style::Default, settings);
    sf::RenderWindow screenWindow(sf::VideoMode(width, height), screenName, sf::Style::Default, settings);
    screenWindow.setMouseCursorVisible(false);

    Environment result = { width, height, &sceneWindow, &screenWindow };
    *env = result;
}

bool isEnvOpen(const struct Environment* env) {
    assert(env               != NULL);
    assert(env->screenWindow != NULL);
    assert(env->sceneWindow  != NULL);

    return env->screenWindow->isOpen() &&
           env->sceneWindow->isOpen();
}

KeyboardActions getKeyboardAction(int keyCode) {
    switch (keyCode) {
        case sf::Keyboard::Up   :
        case sf::Keyboard::W    :   return MOVE_PLAYER_FORWARD;
        case sf::Keyboard::Down :
        case sf::Keyboard::S    :   return MOVE_PLAYER_BACKWARD;
        case sf::Keyboard::D    :
        case sf::Keyboard::Right:   return TURN_PLAYER_RIGHT;
        case sf::Keyboard::A    :
        case sf::Keyboard::Left :   return TURN_PLAYER_LEFT;
        case sf::Keyboard::Q    :   return QUIT_COMMAND;
        default                 :   return NO_ACTION;
    }
}

static KeyboardActions windowEventsLoopHelper(sf::RenderWindow* window) {
    assert(window != NULL);

    sf::Event event = {};
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window->close();
        if (event.type == sf::Event::KeyPressed) {
            KeyboardActions action = getKeyboardAction(event.key.code);
            return action;
        }
    }

    return NO_ACTION;
}

KeyboardActions windowEventsLoops(const struct Environment* env) {
    KeyboardActions action = NO_ACTION;
    action = windowEventsLoopHelper(env->sceneWindow);
    if (action != NO_ACTION)
        return action;

    return windowEventsLoopHelper(env->screenWindow);
}

void drawCircle(Environment* env, sf::RenderWindow* window, const Point* point, float radius, sf::Color color) {
    assert(env          != NULL);
    assert(window       != NULL);
    assert(point        != NULL);
    assert(sign(radius) > 0);

    sf::CircleShape playerCircle(radius);
    playerCircle.setPosition(
        (float)point->x - radius,
        (float)(env->height - (float)point->y - 1 - radius)
    );
    playerCircle.setFillColor(color);
    window->draw(playerCircle);
}

void drawConvexShape(Environment* env, sf::RenderWindow* window, size_t numberOfPoints, const Point* const points, sf::Color color) {
    assert(env != NULL);
    assert(window != NULL);
    assert(numberOfPoints >= 3);
    assert(points != NULL);

    sf::ConvexShape poly;
    poly.setPointCount(numberOfPoints);
    poly.setFillColor(color);
    for (size_t pointIndex = 0; pointIndex < numberOfPoints; ++pointIndex) {
        sf::Vector2f vert(points[pointIndex].x,
                          env->height - points[pointIndex].y - 1);
        poly.setPoint(pointIndex, vert);
    }
    window->draw(poly);
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
