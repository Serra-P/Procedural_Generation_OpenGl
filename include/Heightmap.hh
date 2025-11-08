#pragma once
#include <GL/glew.h>

#include "Shader.hh"

class Heightmap
{
public:
    Heightmap(int size = 512);
    ~Heightmap();

    void Generate();
    void Bind(int unit = 0) const;

private:
    void CreateEmptyTexture();

    int size_;
    GLuint texture_;
    GLuint computeProgram_;
};
