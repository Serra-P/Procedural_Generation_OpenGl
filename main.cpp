#include "App.hh"
#include "Logger.hh"

// Je suis trop triste j'ai pas reussi a faire le rendu opengl de l'app dans un
// buffer emacs J'ai vu c possible mais pas le temps ni l'energie

int main()
{
    App app;
    Logger::Get().Log("Application Init");
    if (!app.Init(1920, 1080, "Stylized Landscape"))
        return -1;
    Logger::Get().Log("App running", Logger::Level::INFO);
    app.Run();
    app.Shutdown();
    Logger::Get().Log("App shuting down", Logger::Level::WARNING);
    return 0;
}
