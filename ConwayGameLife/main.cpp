#include <SFML/Graphics.hpp>
#include <cmath>
#include <algorithm>
#include <set>

/// <summary>
/// Get the grid lines positions
/// </summary>
/// <param name="anOrigin"></param>
/// <param name="aSize"></param>
/// <param name="aSpacing"></param>
/// <param name="outArray">The output array of lines</param>
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

/// <summary>
/// Transfer window coordinate to view coordinate
/// </summary>
/// <param name="anOrigin"></param>
/// <param name="aSize"></param>
/// <returns></returns>
static sf::Vector2f win2View(const sf::Vector2f& aPosition, const sf::Vector2f& aWindowSize)
{
    return sf::Vector2f(aPosition.x / aWindowSize.x - 0.5f, aPosition.y / aWindowSize.y - 0.5f);
}

/// <summary>
/// Transfer view coordinate to world coordinate
/// </summary>
/// <param name="aPect2Center"></param>
/// <param name="aView"></param>
/// <returns></returns>
static sf::Vector2f view2World(const sf::Vector2f& aPect2Center, const sf::View& aView)
{
    auto center = aView.getCenter();
    auto size = aView.getSize();
    return sf::Vector2f(center.x + aPect2Center.x * size.x, center.y + aPect2Center.y * size.y);
}

/// <summary>
/// Check if point is in range
/// </summary>
/// <param name="aPoint"></param>
/// <param name="aRange"></param>
/// <param name="anOrigin"></param>
/// <returns></returns>
template<typename T>
static bool pointSanityCheck(const sf::Vector2<T>& aPoint, 
                             const sf::Vector2<T>& aHalfRange, 
                             const sf::Vector2<T>& aCenter = sf::Vector2<T>(0, 0))
{
    return aPoint.x >= aCenter.x - aHalfRange.x && aPoint.x < aCenter.x + aHalfRange.x
        && aPoint.y >= aCenter.y - aHalfRange.y && aPoint.y < aCenter.y + aHalfRange.y;
}

int main()
{
    const float kSpacing = 100.f;
    const float kScrollSpeed = 0.1f;
    const float kMinScale = 1.f, kMaxScale = 10.f;
    const unsigned char kSideLength = 10;
    float ourWinWidth = 800.f, ourWinHeight = 600.f;
    sf::RenderWindow window(sf::VideoMode (ourWinWidth, ourWinHeight), "Conway's Game of Life");
    sf::View view(sf::FloatRect(0.f, 0.f, ourWinWidth, ourWinHeight));
    window.setView(view);

    bool ourMouseRightHold = false, ourMouseLeftHold = false;
    float ourMousePressedX, ourMousePressedY;
    float ourScale = 1.f;
    auto cmp = [](const sf::Vector2i& a, const sf::Vector2i& b)
    {
        return a.x < b.x || a.x == b.x && a.y < b.y;
    };
    std::set<sf::Vector2i, decltype(cmp)> ourTiles(cmp);
    bool ourHasPlaced = false;
    sf::Vector2i ourLastChangedTile;

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
                auto p2Center = win2View(sf::Vector2f(mousePos), sf::Vector2f(ourWinWidth, ourWinHeight));
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
            if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
                ourMouseLeftHold = true;
            if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
            {
                ourMouseLeftHold = false;
                ourHasPlaced = false;
            }
        }
        // Draw grid
        sf::VertexArray verticeArray;
        verticeArray.setPrimitiveType(sf::Lines);
        float left = std::max(view.getCenter().x - view.getSize().x / 2.f, -kSideLength * kSpacing);
        float top = std::max(view.getCenter().y - view.getSize().y / 2.f, -kSideLength * kSpacing);
        float width = std::min(view.getCenter().x + view.getSize().x / 2.f + kSpacing, kSideLength * kSpacing) - left;
        float height = std::min(view.getCenter().y + view.getSize().y / 2.f + kSpacing, kSideLength * kSpacing) - top;
        fillVerticeArray(sf::Vector2f(floor(left / kSpacing) * kSpacing, floor(top / kSpacing) * kSpacing),
                         sf::Vector2f(ceil(width / kSpacing) * kSpacing, ceil(height / kSpacing) * kSpacing),
                         kSpacing, verticeArray);
        if(ourMouseLeftHold)
        {
            sf::Vector2f pos = view2World(win2View(sf::Vector2f(sf::Mouse::getPosition(window)),
                                           sf::Vector2f(ourWinWidth, ourWinHeight)), view);
            sf::Vector2i tile(floor(pos.x / kSpacing), floor(pos.y / kSpacing));
            if(pointSanityCheck(tile, sf::Vector2i(kSideLength, kSideLength)))
            {
                if(cmp(ourLastChangedTile, tile) || cmp(tile, ourLastChangedTile) || !ourHasPlaced)
                {
                    auto it = ourTiles.find(tile);
                    if(it != ourTiles.end())
                    {
                        ourTiles.erase(it);
                        printf("remove tile x: %d, y: %d\n", tile.x, tile.y);
                    }
                    else
                    {
                        ourTiles.insert(tile);
                        printf("new tile x: %d, y: %d\n", tile.x, tile.y);
                    }
                    ourHasPlaced = true;
                }
                ourLastChangedTile = tile;
            }
        }
        // Core drawing
        window.clear();
        window.draw(verticeArray);
        for(auto& tile : ourTiles)
        {
            sf::RectangleShape tileShape(sf::Vector2f(kSpacing, kSpacing));
            tileShape.setPosition(sf::Vector2f(tile) * kSpacing);
            window.draw(tileShape);
        }
        window.display();
    }
    return 0;
}