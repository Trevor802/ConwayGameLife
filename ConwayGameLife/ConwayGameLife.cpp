#include <SFML/Graphics.hpp>
#include <cmath>
#include <algorithm>
#include <set>

struct TileComparator
{
    bool operator() (const sf::Vector2i& lhs, const sf::Vector2i& rhs) const
    {
        return lhs.x < rhs.x || lhs.x == rhs.x && lhs.y < rhs.y;
    }
};

typedef std::set<sf::Vector2i, TileComparator> TileSet;

/// <summary>
/// Wrap the world up
/// </summary>
/// <param name="anInteger"></param>
/// <param name="aModulo">The half length</param>
/// <returns></returns>
static int wrapInt(int anInteger, int aModulo)
{
    return (anInteger + 3 * aModulo) % (2 * aModulo) - aModulo;
}

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

static const unsigned char kSideLength = 30;

static int getNumOfLiveNeighbors(const sf::Vector2i& aTile, const TileSet& someTiles)
{
    int result = 0;
    auto tempTile = aTile;
    if(someTiles.end() != someTiles.find(tempTile)) ++result;
    tempTile.y = wrapInt(aTile.y - 1, kSideLength);
    if(someTiles.end() != someTiles.find(tempTile)) ++result;
    tempTile.y = wrapInt(aTile.y + 1, kSideLength);
    if(someTiles.end() != someTiles.find(tempTile)) ++result;

    tempTile.x = wrapInt(aTile.x - 1, kSideLength);
    tempTile.y = aTile.y;
    if(someTiles.end() != someTiles.find(tempTile)) ++result;
    tempTile.y = wrapInt(aTile.y - 1, kSideLength);
    if(someTiles.end() != someTiles.find(tempTile)) ++result;
    tempTile.y = wrapInt(aTile.y + 1, kSideLength);
    if(someTiles.end() != someTiles.find(tempTile)) ++result;

    tempTile.x = wrapInt(aTile.x + 1, kSideLength);
    tempTile.y = aTile.y;
    if(someTiles.end() != someTiles.find(tempTile)) ++result;
    tempTile.y = wrapInt(aTile.y - 1, kSideLength);
    if(someTiles.end() != someTiles.find(tempTile)) ++result;
    tempTile.y = wrapInt(aTile.y + 1, kSideLength);
    if(someTiles.end() != someTiles.find(tempTile)) ++result;
    return result;
}

static void setTileLiveness(const sf::Vector2i& aTile, TileSet& someTiles, bool isLive)
{
    if(isLive)
    {
        someTiles.insert(aTile);
    }
    else
    {
        const auto& it = someTiles.find(aTile);
        if(someTiles.end() != it)
            someTiles.erase(it);
    }
}

/// <summary>
/// Extend the boundary to include all potential live tiles
/// </summary>
/// <returns></returns>
static void getProcessingTiles(const TileSet& someTiles, TileSet& someBoundaryTiles)
{
    for(auto& tile : someTiles)
    {
        auto tempTile = tile;
        someBoundaryTiles.insert(tempTile);
        tempTile.y = wrapInt(tile.y - 1, kSideLength);
        someBoundaryTiles.insert(tempTile);
        tempTile.y = wrapInt(tile.y + 1, kSideLength);
        someBoundaryTiles.insert(tempTile);

        tempTile.x = wrapInt(tile.x - 1, kSideLength);
        tempTile.y = tile.y;
        someBoundaryTiles.insert(tempTile);
        tempTile.y = wrapInt(tile.y - 1, kSideLength);
        someBoundaryTiles.insert(tempTile);
        tempTile.y = wrapInt(tile.y + 1, kSideLength);
        someBoundaryTiles.insert(tempTile);

        tempTile.y = tile.y;
        tempTile.x = wrapInt(tile.x + 1, kSideLength);
        tempTile.y = tile.y;
        someBoundaryTiles.insert(tempTile);
        tempTile.y = wrapInt(tile.y - 1, kSideLength);
        someBoundaryTiles.insert(tempTile);
        tempTile.y = wrapInt(tile.y + 1, kSideLength);
        someBoundaryTiles.insert(tempTile);
    }
}

enum GameState
{
    Editor, // Placing tiles
    Automata, // Automatically running with a certain speed
    StepByStep // Press once, do once
};

/// <summary>
/// Double buffering
/// </summary>
/// <param name=""></param>
/// <param name="someLastLiveTiles"></param>
static void processCore(TileSet& someLastLiveTiles, TileSet& someLiveTiles)
{
    std::swap(someLastLiveTiles, someLiveTiles);
    someLiveTiles.clear();
    TileSet boundaryTiles;
    getProcessingTiles(someLastLiveTiles, boundaryTiles);
    // Based on someLastLiveTiles, modify someLiveTiles
    for(auto& tile : boundaryTiles)
    {
        int num = getNumOfLiveNeighbors(tile, someLastLiveTiles);
        switch(num)
        {
        case 3:
            setTileLiveness(tile, someLiveTiles, true);
            break;
        case 4:
            setTileLiveness(tile, someLiveTiles, someLastLiveTiles.end() != someLastLiveTiles.find(tile));
            break;
        default:
            setTileLiveness(tile, someLiveTiles, false);
            break;
        }
    }
}

int main()
{
    const float kSpacing = 100.f;
    const float kScrollSpeed = 0.1f;
    const float kMinScale = 1.f, kMaxScale = 10.f;
    const float kAutoPeriod = 0.1f;
    const std::string kStrTitle = "Conway's Game of Life";
    static float ourWinWidth = 800.f, ourWinHeight = 600.f;
    sf::RenderWindow window(sf::VideoMode (ourWinWidth, ourWinHeight), kStrTitle);
    sf::View view(sf::FloatRect(0.f, 0.f, ourWinWidth, ourWinHeight));
    window.setView(view);
    GameState gameState = GameState::Editor;
    static bool ourMouseRightHold = false, ourMouseLeftHold = false;
    static bool ourDoNextStep = false;
    static float ourMousePressedX, ourMousePressedY;
    static float ourScale = 1.f;
    static TileComparator tileComparator;
    TileSet liveTiles;
    TileSet lastLiveTiles = liveTiles;
    bool hasPlaced = false;
    sf::Vector2i lastChangedTile;
    float autoTimestamp = 0.f;
    static float ourElapsedTime;
    sf::Clock clock;
    while(window.isOpen())
    {
        ourElapsedTime = clock.getElapsedTime().asSeconds();
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
                hasPlaced = false;
            }
            if(sf::Event::KeyPressed == event.type)
            {
                switch(event.key.code)
                {
                case sf::Keyboard::Q:
                    if(GameState::Editor == gameState)
                    {
                        lastLiveTiles.clear();
                        liveTiles.clear();
                    }
                    break;
                case sf::Keyboard::Escape:
                    window.close();
                    break;
                case sf::Keyboard::R:
                    if(GameState::Automata == gameState || GameState::StepByStep == gameState)
                        gameState = GameState::Editor;
                    break;
                case sf::Keyboard::Space:
                {
                    switch(gameState)
                    {
                    case Editor:
                    case StepByStep:
                        gameState = GameState::Automata;
                        break;
                    case Automata:
                        gameState = GameState::StepByStep;
                        break;
                    default:
                        break;
                    }
                    break;
                }
                case sf::Keyboard::N:
                    gameState = GameState::StepByStep;
                    ourDoNextStep = true;
                    break;
                default:
                    break;
                }
            }
        }
        // Display game status
        std::string strGameState;
        switch(gameState)
        {
        case Editor:
            strGameState = " (Editor)";
            break;
        case Automata:
            strGameState = " (Automata)";
            break;
        case StepByStep:
            strGameState = " (StepByStep)";
            break;
        default:
            break;
        }
        window.setTitle(kStrTitle + strGameState);
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
        if(GameState::Editor == gameState)
        {
            if(ourMouseLeftHold)
            {
                sf::Vector2f pos = view2World(win2View(sf::Vector2f(sf::Mouse::getPosition(window)),
                                              sf::Vector2f(ourWinWidth, ourWinHeight)), view);
                sf::Vector2i tile(floor(pos.x / kSpacing), floor(pos.y / kSpacing));
                if(pointSanityCheck(tile, sf::Vector2i(kSideLength, kSideLength)))
                {
                    if(tileComparator(lastChangedTile, tile) || tileComparator(tile, lastChangedTile) || !hasPlaced)
                    {
                        auto it = liveTiles.find(tile);
                        if(it != liveTiles.end())
                        {
                            liveTiles.erase(it);
                            printf("remove tile x: %d, y: %d\n", tile.x, tile.y);
                        }
                        else
                        {
                            liveTiles.insert(tile);
                            printf("new tile x: %d, y: %d\n", tile.x, tile.y);
                        }
                        hasPlaced = true;
                    }
                    lastChangedTile = tile;
                }
            }
        }
        else if(GameState::Automata == gameState)
        {
            if(ourElapsedTime - autoTimestamp > kAutoPeriod)
            {
                processCore(lastLiveTiles, liveTiles);
                autoTimestamp = ourElapsedTime;
            }
        }
        else if(GameState::StepByStep == gameState)
        {
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::N) && ourDoNextStep)
            {
                processCore(lastLiveTiles, liveTiles);
                ourDoNextStep = false;
            }
        }
        // Core drawing
        window.clear();
        window.draw(verticeArray);
        for(auto& tile : liveTiles)
        {
            sf::RectangleShape tileShape(sf::Vector2f(kSpacing, kSpacing));
            tileShape.setPosition(sf::Vector2f(tile) * kSpacing);
            window.draw(tileShape);
        }
        window.display();
    }
    return 0;
}