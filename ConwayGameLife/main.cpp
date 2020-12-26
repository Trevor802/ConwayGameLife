#include <SFML/Graphics.hpp>
#include <vector>

void fillVerticeArray(const sf::Vector2f& anOrigin, const sf::Vector2f& aSize, float aSpacing, sf::VertexArray& outArray)
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

int main()
{
    const float kSpacing = 100.f;
    const float kScrollSpeed = 0.1f;
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
                sf::FloatRect visibleArea(0.f, 0.f, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            }
            if(event.type == sf::Event::MouseWheelScrolled)
            {
                float scale = 1 - event.mouseWheelScroll.delta * kScrollSpeed;
                view.zoom(scale);
                window.setView(view);
                ourScale *= scale;
                printf("wheel scrolled delta: %.3f\n", scale);
            }
            if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right)
            {
                ourMouseRightHold = true;
                ourMousePressedX = event.mouseButton.x;
                ourMousePressedY = event.mouseButton.y;
                printf("right clicked x: %.3f, y: %.3f\n", ourMousePressedX, ourMousePressedY);
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
                printf("move x: %.3f, y: %.3f\n", deltaX, deltaY);
            }
        }
        // Draw grid
        sf::VertexArray verticeArray;
        verticeArray.setPrimitiveType(sf::Lines);
        fillVerticeArray(sf::Vector2f(0.f, 0.f), sf::Vector2f(ourWinWidth, ourWinHeight), kSpacing, verticeArray);
        
        // Core drawing
        window.clear();
        window.draw(verticeArray);
        window.display();
    }
    return 0;
}