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
    float ourWinWidth = 800.f, ourWinHeight = 600.f;
    sf::RenderWindow window(sf::VideoMode (ourWinWidth, ourWinHeight), "Conway's Game of Life");
        sf::View view(sf::FloatRect(0.f, 0.f, ourWinWidth, ourWinHeight));
    window.setView(view);

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
        }

        sf::VertexArray verticeArray;
        verticeArray.setPrimitiveType(sf::Lines);
        fillVerticeArray(sf::Vector2f(0.f, 0.f), sf::Vector2f(ourWinWidth, ourWinHeight), kSpacing, verticeArray);
        window.clear();
        window.draw(verticeArray);
        window.display();
    }
    return 0;
}