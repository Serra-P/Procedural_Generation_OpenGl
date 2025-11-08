#pragma once
#include <memory>
#include <string>

#include "Camera.hh"
#include "Heightmap.hh"
#include "Input.hh"
#include "Light.hh"
#include "PostProcessor.hh"
#include "Terrain.hh"
#include "Timer.hh"
#include "Window.hh"

class App
{
public:
    App() = default;
    ~App() = default;

    bool Init(int width, int height, const std::string& title);
    void Run();
    void Shutdown();

private:
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();

    std::unique_ptr<Window> window_;
    std::unique_ptr<Camera> camera_;
    std::unique_ptr<Input> input_;
    std::unique_ptr<Timer> timer_;
    std::shared_ptr<Heightmap> heightmap_;
    std::unique_ptr<Terrain> terrain_;
    std::unique_ptr<PostProcessor> postProcessor_;
    Light sun_;
    bool running_ = false;
    bool enablePostProcessor_ = false;
    bool enableGrass_ = false;
    bool enableRock_ = false;
    bool enableTime_ = true;
    bool enableHeight_ = false;
};
