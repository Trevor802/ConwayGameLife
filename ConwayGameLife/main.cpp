#include <SFML/Graphics.hpp>

int main ()
{
	sf::RenderWindow window(sf::VideoMode (200, 200), "Hello World!");
	sf::CircleShape circle(100.f);
	circle.setFillColor(sf::Color::Red);

	while(window.isOpen())
	{
		sf::Event event;
		while(window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
			{
				window.close();
			}
		}

		window.clear();
		window.draw(circle);
		window.display();
	}
	return 0;
}