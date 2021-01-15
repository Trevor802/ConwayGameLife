#include "box2d/box2d.h"
#include "SFML/Graphics.hpp"

const static float kScaleFactor = 64.f;
static sf::Vector2f b2s(const b2Vec2& b)
{
    return sf::Vector2f(b.x * kScaleFactor * 2.f,
                        b.y * kScaleFactor * 2.f);
}

static sf::Vector2f b2s(const b2Vec2& b, const sf::Vector2f& size, const b2Vec2& halfSize)
{
    return sf::Vector2f(size.x * 0.5f + (b.x - halfSize.x) * kScaleFactor,
                        size.y * 0.5f - (b.y + halfSize.y) * kScaleFactor);
}

int main()
{
    static int ourWinHeight = 1080, ourWinWidth = 1920;
    static sf::Vector2f ourWinSize(ourWinWidth, ourWinHeight);
    const int kFrameRate = 60;
    const float kDeltaTime = 1.f / kFrameRate;
    sf::RenderWindow window(sf::VideoMode(ourWinWidth, ourWinHeight), "Box2D Playground");
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(kFrameRate);
    sf::View view(sf::FloatRect(0.f, 0.f, ourWinWidth, ourWinHeight));
    //view.zoom(0.1f);
    window.setView(view);
    sf::Clock clock;
    static float ourCurrentTime = 0.f;
    // physics
    static b2Vec2 ourGravity(0, -9.8f);
    static b2World ourWorld(ourGravity);
    const int kVelIters = 6, kPosIters = 2;

    b2BodyDef groundBodyDef;
    b2Vec2 gPos(0, -2.f);
    b2Vec2 gSize(5.f, 2.f);
    groundBodyDef.position = gPos;
    b2Body* groundBody = ourWorld.CreateBody(&groundBodyDef);
    b2PolygonShape groundBox;
    groundBox.SetAsBox(gSize.x, gSize.y);
    groundBody->CreateFixture(&groundBox, 0.f);
    // view
    sf::RectangleShape groundView(b2s(gSize));
    groundView.setPosition(b2s(gPos, ourWinSize, gSize));

    b2BodyDef boxBodyDef;
    boxBodyDef.type = b2_dynamicBody;
    b2Vec2 bPos(0, 4.f);
    b2Vec2 bSize(1.f, 1.f);
    boxBodyDef.position = bPos;
    b2Body* boxBody = ourWorld.CreateBody(&boxBodyDef);
    b2PolygonShape boxShape;
    boxShape.SetAsBox(bSize.x, bSize.y);
    b2FixtureDef fixtureDef;
    fixtureDef.density = 1.f;
    fixtureDef.shape = &boxShape;
    fixtureDef.friction = 0.3f;
    boxBody->CreateFixture(&fixtureDef);
    // view
    sf::RectangleShape boxView(b2s(bSize));

    b2Vec2 testPos(0.f, 1.f);
    b2Vec2 testSize(1.f, 1.f);
    sf::RectangleShape testBox(b2s(testSize));
    testBox.setPosition(b2s(testPos, ourWinSize, testSize));
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
        sf::Vector2f viewPos = b2s(pos, ourWinSize, bSize);
        boxView.setPosition(viewPos);

        window.clear();
        window.draw(groundView);
        window.draw(boxView);
        //window.draw(testBox);
        window.display();
    }

    ourWorld.DestroyBody(groundBody);
    ourWorld.DestroyBody(boxBody);
    return 0;
}