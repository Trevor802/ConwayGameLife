#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>

static void fillVerticeArray(const sf::Vector2f& anOrigin, const sf::Vector2f& aSize, float aSpacing, sf::VertexArray& outArray)
{
    int numHLines = aSize.y / aSpacing + 1, numVLines = aSize.x / aSpacing + 1;
    outArray.resize(2 * (numHLines + numVLines));
    int numoutArray = 0;
    for(int i = 0; i < numHLines; i++)
    {
        outArray[numoutArray].position.x = anOrigin.x;
        outArray[numoutArray].position.y = anOrigin.y + i * aSpacing;
        ++numoutArray;
        outArray[numoutArray].position.x = anOrigin.x + aSize.x;
        outArray[numoutArray].position.y = anOrigin.y + i * aSpacing;
        ++numoutArray;
    }
    for(int i = 0; i < numVLines; i++)
    {
        outArray[numoutArray].position.x = anOrigin.x + i * aSpacing;
        outArray[numoutArray].position.y = anOrigin.y;
        ++numoutArray;
        outArray[numoutArray].position.x = anOrigin.x + i * aSpacing;
        outArray[numoutArray].position.y = anOrigin.y + aSize.y;
        ++numoutArray;
    }
}

static sf::Vector2f pect2Center(const sf::Vector2f& anOrigin, const sf::Vector2f& aSize)
{
    return sf::Vector2f(anOrigin.x / aSize.x - 0.5f, anOrigin.y / aSize.y - 0.5f);
}

static sf::Vector2f getWorldPos(const sf::Vector2f& aPect2Center, const sf::View& aView)
{
    auto center = aView.getCenter();
    auto size = aView.getSize();
    return sf::Vector2f(center.x + aPect2Center.x * size.x, center.y + aPect2Center.y * size.y);
}

int main()
{
    const float kSpacing = 100.f;
    const float kScrollSpeed = 0.1f;
    const float kMinScale = 1.f, kMaxScale = 10.f;
    float ourWinWidth = 800.f, ourWinHeight = 600.f;
    sf::RenderWindow window(sf::VideoMode (ourWinWidth, ourWinHeight), "Conway's Game of Life");
    sf::View view(sf::FloatRect(0.f, 0.f, ourWinWidth, ourWinHeight));
    window.setView(view);

    bool ourMouseRightHold = false;
    float ourMousePressedX, ourMousePressedY;
    float ourScale = 1.f;

    while(window.isOpen())
    {
        sf::Event event;
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
            {
                window.close();
            }
            if(event.type == sf::Event::Resized)
            {
                ourWinWidth = event.size.width;
                ourWinHeight = event.size.height;
                view.setSize(ourWinWidth * ourScale, ourWinHeight * ourScale);
                window.setView(view);
            }
            if(event.type == sf::Event::MouseWheelScrolled)
            {
                float scale = 1 - event.mouseWheelScroll.delta * kScrollSpeed;
                float finalScale = ourScale * scale;
                if(finalScale < kMinScale)
                {
                    finalScale = kMinScale;
                    scale = kMinScale / ourScale;
                }
                else if(finalScale > kMaxScale)
                {
                    finalScale = kMaxScale;
                    scale = kMaxScale / ourScale;
                }
                auto mousePos = sf::Mouse::getPosition(window);
                auto p2Center = pect2Center(sf::Vector2f(mousePos), sf::Vector2f(ourWinWidth, ourWinHeight));
                auto preSize = view.getSize();
                view.zoom(scale);
                auto postSize = view.getSize();
                auto sizeOffset = preSize - postSize;
                view.setCenter(view.getCenter() + sf::Vector2f(sizeOffset.x * p2Center.x, sizeOffset.y * p2Center.y));
                window.setView(view);
                ourScale = finalScale;
                printf("scolled global scale: %.3f\n", ourScale);
            }
            if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right)
            {
                ourMouseRightHold = true;
                ourMousePressedX = event.mouseButton.x;
                ourMousePressedY = event.mouseButton.y;
                printf("right clicked pos x: %.3f, y: %.3f\n", ourMousePressedX, ourMousePressedY);
            }
            if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Right)
                ourMouseRightHold = false;
            if(event.type == sf::Event::MouseMoved && ourMouseRightHold)
            {
                float deltaX = ourMousePressedX - event.mouseMove.x;
                float deltaY = ourMousePressedY - event.mouseMove.y;
                view.move(deltaX * ourScale, deltaY * ourScale);
                window.setView(view);
                ourMousePressedX = event.mouseMove.x;
                ourMousePressedY = event.mouseMove.y;
                printf("moved center x: %.3f, y: %.3f\n", view.getCenter().x, view.getCenter().y);
            }
        }
        // Draw grid
        sf::VertexArray verticeArray;
        verticeArray.setPrimitiveType(sf::Lines);
        float left = view.getCenter().x - view.getSize().x / 2.f;
        float top = view.getCenter().y - view.getSize().y / 2.f;
        fillVerticeArray(sf::Vector2f(floor(left / kSpacing) * kSpacing, floor(top / kSpacing) * kSpacing),
                         sf::Vector2f(view.getSize().x + kSpacing, view.getSize().y + kSpacing), kSpacing, verticeArray);
        // Draw tiles
        sf::RectangleShape tile(sf::Vector2f(kSpacing, kSpacing));
        tile.setPosition(sf::Vector2f(0.f, 0.f));
        // Core drawing
        window.clear();
        window.draw(verticeArray);
        window.draw(tile);
        window.display();
    }
    return 0;
}