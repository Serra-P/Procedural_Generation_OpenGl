#pragma once
#include <GL/glew.h>
#include <memory>

#include "Heightmap.hh"
#include "Shader.hh"
class PostProcessor
{
public:
    PostProcessor(int width, int height);
    ~PostProcessor();
    void Bind();
    void Apply(std::shared_ptr<Heightmap> heightmap);
    void Shutdown();
    void CreateFullscreenQuad();

private:
    std::unique_ptr<Shader> displayShader_;

    GLuint fbo_, colorTex_, diffusedTex_, depthRBO_;
    GLuint quadVAO_, quadVBO_;
    GLuint diffusionProgram_;
    int width_, height_;
};
