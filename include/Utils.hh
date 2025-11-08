#pragma once

#include <GL/glew.h>

#include "Camera.hh"
#include "Window.hh"

static const int PERM_SIZE = 512;
static unsigned char perm[PERM_SIZE];
static bool s_mouseCaptured = false;

float Fade(float t);

float Lerp(float a, float b, float t);

float Grad(int hash, float x, float y);

void InitPerm();

float Perlin2D(float x, float y);

float FBm(float x, float y, int octaves, float lacunarity, float gain);

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

void MouseCallback(GLFWwindow* window, double xpos, double ypos);

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
