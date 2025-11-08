#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <memory>

#include "Heightmap.hh"
#include "Light.hh"
#include "Shader.hh"

class Terrain
{
public:
    Terrain(int size, float heightScale);
    ~Terrain();

    void Draw(std::shared_ptr<Heightmap> heightmap, const glm::mat4& view,
              const glm::mat4& proj, const glm::vec3& cameraPos,
              const Light& sun, const bool enableGrass, bool enableRock,
              const bool enableTime, const bool enableHeight);

private:
    void GenerateGrid();

    void DrawTerrain(std::shared_ptr<Heightmap> heightmap,
                     const glm::mat4& view, const glm::mat4& proj,
                     const glm::vec3& cameraPos, const Light& sun,
                     const bool enableTime, const bool enableHeight);

    void DrawGrass(std::shared_ptr<Heightmap> heightmap, const glm::mat4& view,
                   const glm::mat4& proj, const glm::vec3& cameraPos,
                   const Light& sun, const bool enableTime);

    void DrawRocks(std::shared_ptr<Heightmap> heightmap, const glm::mat4& view,
                   const glm::mat4& proj, const glm::vec3& cameraPos,
                   const Light& sun, const bool enableTime);

    void SetCommonUniforms(Shader& shader, std::shared_ptr<Heightmap> heightmap,
                           const glm::mat4& view, const glm::mat4& proj,
                           const glm::vec3& cameraPos, const Light& sun,
                           const bool enableTime,
                           const bool enableHight = true);

    int size_;
    float heightScale_;
    GLuint vao_, vbo_, ebo_;
    GLsizei indexCount_;
    GLsizei vertexCount_;
    float lastTime_;

    std::unique_ptr<Shader> terrainShader_;
    std::unique_ptr<Shader> grassShader_;
    std::unique_ptr<Shader> rocksShader_;
};
