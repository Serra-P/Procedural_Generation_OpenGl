#include "Heightmap.hh"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

#include "Utils.hh"

Heightmap::Heightmap(int size)
    : size_(size)
    , texture_(0)
    , computeProgram_(0)
{
    CreateEmptyTexture();

    computeProgram_ = Shader::LoadComputeShader("shaders/perlin.comp");
}

Heightmap::~Heightmap()
{
    if (texture_)
        glDeleteTextures(1, &texture_);
    if (computeProgram_)
        glDeleteProgram(computeProgram_);
}

void Heightmap::CreateEmptyTexture()
{
    if (texture_)
        glDeleteTextures(1, &texture_);

    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R32F, size_, size_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Heightmap::Generate()
{
    if (computeProgram_)
    {
        glUseProgram(computeProgram_);

        glUniform1i(glGetUniformLocation(computeProgram_, "uSize"), size_);
        glUniform1i(glGetUniformLocation(computeProgram_, "uOctaves"), 5);
        glUniform1f(glGetUniformLocation(computeProgram_, "uLacunarity"), 2.0f);
        glUniform1f(glGetUniformLocation(computeProgram_, "uGain"), 0.5f);
        glUniform1f(glGetUniformLocation(computeProgram_, "uScale"), 2.0f);

        std::srand(std::time(nullptr));
        glUniform2f(glGetUniformLocation(computeProgram_, "uSeed"),
                    static_cast<float>(rand() % 1000),
                    static_cast<float>(rand() % 1000));

        glBindImageTexture(0, texture_, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);

        int local = 16;
        int groups = (size_ + local - 1) / local;
        glDispatchCompute(groups, groups, 1);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT
                        | GL_TEXTURE_FETCH_BARRIER_BIT);
        glBindImageTexture(0, 0, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
        glUseProgram(0);
    }
}

void Heightmap::Bind(int unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture_);
}
