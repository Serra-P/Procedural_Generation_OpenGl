#include "Utils.hh"

#include <algorithm>
#include <cmath>
#include <ctime>
#include <random>
#include <vector>

// Des fonctions utils
// Du moins peut etre
// De toute facon une fois cree pourquoi la supprimer
// Et oui quel phrase phylosophique

float Fade(float t)
{
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float Lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

float Grad(int hash, float x, float y)
{
    int h = hash & 7;
    float u = h < 4 ? x : y;
    float v = h < 4 ? y : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
}

void InitPerm()
{
    static bool inited = false;
    if (inited)
        return;
    inited = true;

    std::vector<unsigned char> p(256);
    for (int i = 0; i < 256; ++i)
        p[i] = static_cast<unsigned char>(i);

    std::mt19937 rng(time(nullptr));
    std::shuffle(p.begin(), p.end(), rng);

    for (int i = 0; i < 512; ++i)
        perm[i] = p[i & 255];
}

float Perlin2D(float x, float y)
{
    int xi = static_cast<int>(std::floor(x)) & 255;
    int yi = static_cast<int>(std::floor(y)) & 255;

    float xf = x - std::floor(x);
    float yf = y - std::floor(y);

    float u = Fade(xf);
    float v = Fade(yf);

    int aa = perm[perm[xi] + yi];
    int ab = perm[perm[xi] + yi + 1];
    int ba = perm[perm[xi + 1] + yi];
    int bb = perm[perm[xi + 1] + yi + 1];

    float x1 = Lerp(Grad(aa, xf, yf), Grad(ba, xf - 1.0f, yf), u);
    float x2 = Lerp(Grad(ab, xf, yf - 1.0f), Grad(bb, xf - 1.0f, yf - 1.0f), u);
    float result = Lerp(x1, x2, v);

    return result * 0.5f + 0.5f;
}

float FBm(float x, float y, int octaves, float lacunarity, float gain)
{
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float sum = 0.0f;
    float norm = 0.0f;

    for (int i = 0; i < octaves; ++i)
    {
        sum += amplitude * Perlin2D(x * frequency, y * frequency);
        norm += amplitude;
        amplitude *= gain;
        frequency *= lacunarity;
    }

    return (norm > 0.0f) ? (sum / norm) : 0.0f;
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            void* ptr = glfwGetWindowUserPointer(window);
            if (ptr)
            {
                auto cam = reinterpret_cast<class Camera*>(ptr);
                cam->ProcessMouse(0, 0);
            }

            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            s_mouseCaptured = true;
        }
        else if (action == GLFW_RELEASE)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            s_mouseCaptured = false;
        }
    }
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (!s_mouseCaptured)
        return;
    void* ptr = glfwGetWindowUserPointer(window);
    if (!ptr)
        return;
    auto cam = reinterpret_cast<class Camera*>(ptr);
    cam->ProcessMouse(xpos, ypos);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    void* ptr = glfwGetWindowUserPointer(window);
    if (!ptr)
        return;
    auto cam = reinterpret_cast<class Camera*>(ptr);
    cam->ProcessScroll(yoffset);
}
