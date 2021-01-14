#include "box2d/box2d.h"
#include "SFML/Graphics.hpp"
int main()
{
    static int ourWinHeight = 1080, ourWinWidth = 1920;
    const int kFrameRate = 60;
    const float kDeltaTime = 1.f / kFrameRate;
    sf::RenderWindow window(sf::VideoMode(ourWinWidth, ourWinHeight), "Box2D Playground");
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(kFrameRate);
    sf::View view(sf::FloatRect(0.f, 0.f, ourWinWidth, ourWinHeight));
    view.zoom(0.1f);
    window.setView(view);
    sf::Clock clock;
    static float ourCurrentTime = 0.f;
    // physics
    static b2Vec2 ourGravity(0, -9.8f);
    static b2World ourWorld(ourGravity);
    const int kVelIters = 6, kPosIters = 2;

    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0, -10.f);
    b2Body* groundBody = ourWorld.CreateBody(&groundBodyDef);
    b2PolygonShape groundBox;
    groundBox.SetAsBox(50.f, 10.f);
    groundBody->CreateFixture(&groundBox, 0.f);
    // view
    sf::RectangleShape groundView(sf::Vector2f(50.f, 10.f));
    groundView.setPosition(0 + ourWinWidth * 0.5f, -10.f + ourWinHeight * 0.5f);

    b2BodyDef boxBodyDef;
    boxBodyDef.type = b2_dynamicBody;
    boxBodyDef.position.Set(0, 4.f);
    b2Body* boxBody = ourWorld.CreateBody(&boxBodyDef);
    b2PolygonShape boxShape;
    boxShape.SetAsBox(1.f, 1.f);
    b2FixtureDef fixtureDef;
    fixtureDef.density = 1.f;
    fixtureDef.shape = &boxShape;
    fixtureDef.friction = 0.3f;
    boxBody->CreateFixture(&fixtureDef);
    // view
    sf::RectangleShape boxView(sf::Vector2f(1.f, 1.f));

    while(window.isOpen())
    {
        ourCurrentTime = clock.getElapsedTime().asSeconds();
        sf::Event event;
        while(window.pollEvent(event))
        {
            if(sf::Event::Closed == event.type)
                window.close();
        }
        ourWorld.Step(kDeltaTime, kVelIters, kPosIters);
        b2Vec2 pos = boxBody->GetPosition();
        float angle = boxBody->GetAngle();
        printf("%.3f %.3f %.3f\n", pos.x, pos.y, angle);
        boxView.setPosition(pos.x + ourWinWidth * 0.5f, pos.y + ourWinHeight * 0.5f);

        window.clear();
        window.draw(groundView);
        window.draw(boxView);
        window.display();
    }

    ourWorld.DestroyBody(groundBody);
    ourWorld.DestroyBody(boxBody);
    return 0;
}