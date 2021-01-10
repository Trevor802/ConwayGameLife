#include <SFML/Graphics.hpp>
#include <math.h>

/// <summary>
/// smoother
/// </summary>
/// <param name="t"></param>
/// <returns></returns>
static double fade(double t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

/// <summary>
/// Perlin's random array
/// TODO Proc gen one
/// </summary>
static const int arrRandInt[256] = {
    151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
    8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117
    ,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,
    71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,
    41,55,46,245,40,244,102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208,
    89,18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,
    226,250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,
    182,189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167,
    43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,
    97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,
    107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

/// <summary>
/// Get the dot product between the gradient vector and the distance vector
/// </summary>
/// <param name="hash"></param>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
/// <returns></returns>
static double grad(int hash, double x, double y, double z)
{
    double result = 0.f;
    switch(hash & 0xF)
    {
    case 0x0: result = x + y; break;
    case 0x1: result = -x + y; break;
    case 0x2: result = x - y; break;
    case 0x3: result = -x - y; break;
    case 0x4: result = x + z; break;
    case 0x5: result = -x + z; break;
    case 0x6: result = x - z; break;
    case 0x7: result = -x - z; break;
    case 0x8: result = y + z; break;
    case 0x9: result = -y + z; break;
    case 0xA: result = y - z; break;
    case 0xB: result = -y - z; break;
    case 0xC: result = y + x; break;
    case 0xD: result = -y + z; break;
    case 0xE: result = y - x; break;
    case 0xF: result = -y - z; break;
    default: break;
    }
    return result;
}

/// <summary>
/// Linear interpolation
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <param name="ratio"></param>
/// <returns></returns>
static double lerp(const double& a, const double& b, const double& ratio)
{
    return a + ratio * (b - a);
}

/// <summary>
/// Core improved perlin algo
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
/// <returns></returns>
static double perlin(double x, double y, double z)
{
    double result = 0.f;
    int xi = (int)x & 255, yi = (int)y & 255, zi = (int)z & 255;
    double xf = x - (int)x, yf = y - (int)y, zf = z - (int)z;
    double u = fade(xf), v = fade(yf), w = fade(zf);
    int arrBuffer[512];
    memcpy(arrBuffer, arrRandInt, 256 * sizeof(int));
    memcpy(arrBuffer + 256, arrRandInt, 256 * sizeof(int));
    int aaa = arrBuffer[arrBuffer[arrBuffer[xi] + yi] + zi];
    int aba = arrBuffer[arrBuffer[arrBuffer[xi] + yi+1] + zi];
    int baa = arrBuffer[arrBuffer[arrBuffer[xi+1] + yi] + zi];
    int bba = arrBuffer[arrBuffer[arrBuffer[xi+1] + yi+1] + zi];
    int aab = arrBuffer[arrBuffer[arrBuffer[xi] + yi] + zi+1];
    int abb = arrBuffer[arrBuffer[arrBuffer[xi] + yi+1] + zi+1];
    int bab = arrBuffer[arrBuffer[arrBuffer[xi+1] + yi] + zi+1];
    int bbb = arrBuffer[arrBuffer[arrBuffer[xi+1] + yi+1] + zi+1];
    double _000 = grad(aaa, xf, yf, zf);
    double _100 = grad(baa, xf - 1, yf, zf);
    double _010 = grad(aba, xf, yf - 1, zf);
    double _110 = grad(bba, xf - 1, yf - 1, zf);
    double _001 = grad(aab, xf, yf, zf - 1);
    double _101 = grad(bab, xf - 1, yf, zf - 1);
    double _011 = grad(abb, xf, yf - 1, zf - 1);
    double _111 = grad(bbb, xf - 1, yf - 1, zf - 1);
    double y0 = lerp(_000, _100, u);
    double y1 = lerp(_010, _110, u);
    double y2 = lerp(_001, _101, u);
    double y3 = lerp(_011, _111, u);
    double x0 = lerp(y0, y1, v);
    double x1 = lerp(y2, y3, v);
    result = lerp(x0, x1, w);
    result = (result + 1) / 2.f;
    return result;
}

/// <summary>
/// More natural
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
/// <param name="octaves"></param>
/// <param name="persistence"></param>
/// <returns></returns>
static double octavePerlin(double x, double y, double z, int octaves, double persistence)
{
    double result;
    double amplitude = 1.f, frequency = 1.f, total = 0.f, maxValue = 0.f;
    for(int i = 0; i < octaves; i++)
    {
        total += perlin(x * frequency, y * frequency, z * frequency)* amplitude;
        maxValue += amplitude;
        frequency *= 2.f;
        amplitude *= persistence;
    }
    result = total / maxValue;
    return result;
}

int main()
{
    static const unsigned int ourSideLength = 256;
    const double delta = 1 / (double)ourSideLength;
    static const unsigned int ourNumVertices = ourSideLength * ourSideLength;
    static int ourWinHeight = ourSideLength, ourWinWidth = ourSideLength;
    sf::RenderWindow window(sf::VideoMode(ourWinHeight, ourWinWidth), "Perlin Noise");
    sf::VertexArray vertices(sf::Points, ourWinHeight * ourWinWidth);
    sf::Clock clock;
    float lastFrameTime = 0.f;
    float currentTime = 0.f;
    float deltaTime = 0.f;
    while(window.isOpen())
    {
        currentTime = clock.getElapsedTime().asSeconds();
        sf::Event windowEvent;
        while(window.pollEvent(windowEvent))
        {
            if(sf::Event::Closed == windowEvent.type)
                window.close();
        }
        double persistence = sin(currentTime) + 1;
        //printf("persistence %.3f\n", persistence);
        for(int i = 0; i < ourSideLength; i++)
        {
            int base = i * ourSideLength;
            for(int j = 0; j < ourSideLength; j++)
            {
                int gray = octavePerlin(i * delta + currentTime, j * delta + currentTime, 0.f, 4, persistence) * 255;
                //int gray = perlin(i * delta, j * delta, 0.f) * 255;
                vertices[base + j].color = sf::Color(gray, gray, gray);
                vertices[base + j].position.x = i;
                vertices[base + j].position.y = j;
            }
        }
        window.clear();
        window.draw(vertices);
        window.display();
        lastFrameTime = clock.getElapsedTime().asSeconds();
        deltaTime = lastFrameTime - currentTime;
        printf("fps: %.3f\n", 1 / deltaTime);
    }
    return 0;
}