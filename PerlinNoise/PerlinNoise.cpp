#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <time.h>

int main()
{
    srand(time(NULL));
    static const unsigned int ourSideLength = 1024;
    static const unsigned int ourNumVertices = ourSideLength * ourSideLength;
    static int ourWinHeight = ourSideLength, ourWinWidth = ourSideLength;
    sf::RenderWindow window(sf::VideoMode(ourWinHeight, ourWinWidth), "Perlin Noise");
    //sf::View view(sf::FloatRect(0.f, 0.f, ourWinWidth, ourWinHeight));
    //window.setView(view);
    while(window.isOpen())
    {
        sf::Event windowEvent;
        while(window.pollEvent(windowEvent))
        {
            if(sf::Event::Closed == windowEvent.type)
                window.close();
        }
        sf::VertexArray vertices(sf::Points, ourWinHeight * ourWinWidth);
        for(int i = 0; i < ourSideLength; i++)
        {
            int base = i * ourSideLength;
            for(int j = 0; j < ourSideLength; j++)
            {
                int gray = rand() % 256;
                vertices[base + j].color = sf::Color(gray, gray, gray);
                vertices[base + j].position.x = i;
                vertices[base + j].position.y = j;
            }
        }
        window.clear();
        window.draw(vertices);
        window.display();
    }
    return 0;
}