#include "Terrain.hh"

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

Terrain::Terrain(int size, float heightScale)
    : size_(size)
    , heightScale_(heightScale)
    , vao_(0)
    , vbo_(0)
    , ebo_(0)
    , indexCount_(0)
    , vertexCount_(0)
{
    terrainShader_ = std::make_unique<Shader>("shaders/terrain.vert",
                                              "shaders/terrain.frag");
    grassShader_ = std::make_unique<Shader>(
        "shaders/map.vert", "shaders/grass.frag", "shaders/grass.geom");
    rocksShader_ = std::make_unique<Shader>(
        "shaders/map.vert", "shaders/rocks.frag", "shaders/rocks.geom");

    GenerateGrid();
}

Terrain::~Terrain()
{
    if (vao_)
        glDeleteVertexArrays(1, &vao_);
    if (vbo_)
        glDeleteBuffers(1, &vbo_);
    if (ebo_)
        glDeleteBuffers(1, &ebo_);
}

void Terrain::GenerateGrid()
{
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    float scale = heightScale_;
    for (int z = 0; z <= size_; ++z)
    {
        for (int x = 0; x <= size_; ++x)
        {
            float fx = (x - size_ / 2.0f) * scale;
            float fz = (z - size_ / 2.0f) * scale;
            positions.emplace_back(fx, 0.0f, fz);
            normals.emplace_back(0.0f, 1.0f, 0.0f);
        }
    }

    for (int z = 0; z < size_; ++z)
    {
        for (int x = 0; x < size_; ++x)
        {
            int topLeft = z * (size_ + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (size_ + 1) + x;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    indexCount_ = static_cast<GLsizei>(indices.size());
    vertexCount_ = static_cast<GLsizei>(positions.size());

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    size_t posSize = positions.size() * sizeof(glm::vec3);
    size_t normSize = normals.size() * sizeof(glm::vec3);
    glBufferData(GL_ARRAY_BUFFER, posSize + normSize, nullptr, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, posSize, positions.data());
    glBufferSubData(GL_ARRAY_BUFFER, posSize, normSize, normals.data());

    glGenBuffers(1, &ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
                 indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                          (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                          (void*)posSize);

    glBindVertexArray(0);
}

void Terrain::Draw(std::shared_ptr<Heightmap> heightmap, const glm::mat4& view,
                   const glm::mat4& proj, const glm::vec3& cameraPos,
                   const Light& sun, const bool enableGrass,
                   const bool enableRock, const bool enableTime,
                   const bool enableHeight)
{
    DrawTerrain(heightmap, view, proj, cameraPos, sun, enableTime,
                enableHeight);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_TRUE);

    if (enableGrass)
        DrawGrass(heightmap, view, proj, cameraPos, sun, enableTime);

    if (enableRock)
        DrawRocks(heightmap, view, proj, cameraPos, sun, enableTime);

    glDisable(GL_BLEND);
}

void Terrain::DrawTerrain(std::shared_ptr<Heightmap> heightmap,
                          const glm::mat4& view, const glm::mat4& proj,
                          const glm::vec3& cameraPos, const Light& sun,
                          const bool enableTime, const bool enableHeight)
{
    terrainShader_->Use();
    SetCommonUniforms(*terrainShader_, heightmap, view, proj, cameraPos, sun,
                      enableTime, enableHeight);

    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// C'est vraiment moche
// Est ce que je vais l'ameliorer
// Non

void Terrain::DrawGrass(std::shared_ptr<Heightmap> heightmap,
                        const glm::mat4& view, const glm::mat4& proj,
                        const glm::vec3& cameraPos, const Light& sun,
                        const bool enableTime)
{
    grassShader_->Use();
    SetCommonUniforms(*grassShader_, heightmap, view, proj, cameraPos, sun,
                      enableTime);
    grassShader_->SetFloat("time", static_cast<float>(glfwGetTime()));

    glBindVertexArray(vao_);
    glDrawArrays(GL_POINTS, 0, vertexCount_);
    glBindVertexArray(0);
}

void Terrain::DrawRocks(std::shared_ptr<Heightmap> heightmap,
                        const glm::mat4& view, const glm::mat4& proj,
                        const glm::vec3& cameraPos, const Light& sun,
                        const bool enableTime)
{
    rocksShader_->Use();
    SetCommonUniforms(*rocksShader_, heightmap, view, proj, cameraPos, sun,
                      enableTime);
    rocksShader_->SetFloat("time", static_cast<float>(glfwGetTime()));

    glBindVertexArray(vao_);
    glDrawArrays(GL_POINTS, 0, vertexCount_);
    glBindVertexArray(0);
}

void Terrain::SetCommonUniforms(Shader& shader,
                                std::shared_ptr<Heightmap> heightmap,
                                const glm::mat4& view, const glm::mat4& proj,
                                const glm::vec3& cameraPos, const Light& sun,
                                const bool enableTime, const bool enableHeight)
{
    glm::mat4 model = glm::mat4(1.0f);

    shader.SetMat4("uModel", model);
    shader.SetMat4("uView", view);
    shader.SetMat4("uProj", proj);
    shader.SetVec3("uCameraPos", cameraPos);
    if (enableHeight)
        shader.SetFloat("uHeightScale", 10.0f);
    else
        shader.SetFloat("uHeightScale", 0.0f);
    shader.SetFloat("uTerrainSize", float(size_));

    shader.SetVec3("uLight.direction", sun.direction);
    shader.SetVec3("uLight.color", sun.color);
    shader.SetFloat("uLight.intensity", sun.intensity);

    shader.SetVec3("uFogColor", glm::vec3(0.53f, 0.81f, 0.92f));
    shader.SetFloat("uFogStart", 150.0f);
    shader.SetFloat("uFogEnd", 200.0f);
    if (!enableTime)
    {
        lastTime_ = (float)glfwGetTime();
        shader.SetFloat("time", lastTime_);
    }
    else
        shader.SetFloat("time", lastTime_);

    heightmap->Bind(0);
    shader.SetInt("uHeightmap", 0);
}
