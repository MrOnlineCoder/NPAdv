#include <Application.hpp>

#include <Common/Logger.hpp>

int main(int argc, char *argv[])
{
    GetLogger().init();

    std::srand(std::time(nullptr));

    ApplicationLaunchOptions options;

    options.fps_limit = 60;
    options.is_debug = true;

    Application *app = new Application(options);

    app->run();

    delete app;

    GetLogger().release();

    return 0;
}