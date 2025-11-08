#include "PostProcessor.hh"

#include "Shader.hh"
#include "Utils.hh"

PostProcessor::PostProcessor(int width, int height)
    : width_(width)
    , height_(height)
    , fbo_(0)
    , colorTex_(0)
    , diffusedTex_(0)
    , depthRBO_(0)
    , quadVAO_(0)
    , quadVBO_(0)
    , diffusionProgram_(0)
{
    diffusionProgram_ =
        Shader::LoadComputeShader("shaders/watercolor_diffusion.comp");

    displayShader_ = std::make_unique<Shader>("shaders/display.vert",
                                              "shaders/display.frag");

    glGenFramebuffers(1, &fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    glGenTextures(1, &colorTex_);
    glBindTexture(GL_TEXTURE_2D, colorTex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_, height_, 0, GL_RGBA,
                 GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           colorTex_, 0);

    glGenTextures(1, &diffusedTex_);
    glBindTexture(GL_TEXTURE_2D, diffusedTex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_, height_, 0, GL_RGBA,
                 GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenRenderbuffers(1, &depthRBO_);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRBO_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width_,
                          height_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, depthRBO_);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    CreateFullscreenQuad();
}

PostProcessor::~PostProcessor()
{
    Shutdown();
}

void PostProcessor::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
}

void PostProcessor::Apply(std::shared_ptr<Heightmap> heightmap)
{
    if (diffusionProgram_)
    {
        glUseProgram(diffusionProgram_);

        glBindImageTexture(0, colorTex_, 0, GL_FALSE, 0, GL_READ_ONLY,
                           GL_RGBA32F);
        glBindImageTexture(1, diffusedTex_, 0, GL_FALSE, 0, GL_WRITE_ONLY,
                           GL_RGBA32F);

        heightmap->Bind(2);
        glUniform1i(glGetUniformLocation(diffusionProgram_, "uHeightmap"), 2);
        glUniform1i(glGetUniformLocation(diffusionProgram_, "uImageWidth"),
                    width_);
        glUniform1i(glGetUniformLocation(diffusionProgram_, "uImageHeight"),
                    height_);
        glUniform1f(glGetUniformLocation(diffusionProgram_, "uTime"),
                    static_cast<float>(glfwGetTime()));

        int groupsX = (width_ + 15) / 16;
        int groupsY = (height_ + 15) / 16;
        glDispatchCompute(groupsX, groupsY, 1);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT
                        | GL_TEXTURE_FETCH_BARRIER_BIT);
        glUseProgram(0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    displayShader_->Use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffusionProgram_ ? diffusedTex_ : colorTex_);
    displayShader_->SetInt("screenTexture", 0);

    glBindVertexArray(quadVAO_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void PostProcessor::Shutdown()
{
    if (fbo_)
        glDeleteFramebuffers(1, &fbo_);
    if (colorTex_)
        glDeleteTextures(1, &colorTex_);
    if (diffusedTex_)
        glDeleteTextures(1, &diffusedTex_);
    if (depthRBO_)
        glDeleteRenderbuffers(1, &depthRBO_);
    if (quadVAO_)
        glDeleteVertexArrays(1, &quadVAO_);
    if (quadVBO_)
        glDeleteBuffers(1, &quadVBO_);
    if (diffusionProgram_)
        glDeleteProgram(diffusionProgram_);
}

void PostProcessor::CreateFullscreenQuad()
{
    float quadVertices[] = { -1.f, 1.f,  0.f, 1.f, -1.f, -1.f, 0.f, 0.f,
                             1.f,  -1.f, 1.f, 0.f, -1.f, 1.f,  0.f, 1.f,
                             1.f,  -1.f, 1.f, 0.f, 1.f,  1.f,  1.f, 1.f };

    glGenVertexArrays(1, &quadVAO_);
    glGenBuffers(1, &quadVBO_);
    glBindVertexArray(quadVAO_);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices,
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}
