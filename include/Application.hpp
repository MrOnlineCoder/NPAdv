#ifndef APPLICATION_H
#define APPLICATION_H

#include <GameContext.hpp>

struct ApplicationLaunchOptions {
    int fps_limit;
    bool is_debug;
};

class Application {
    public:
        Application(ApplicationLaunchOptions options);

        void run();
    
    private:
        void loadAssets();
        void loadStates();

        GameContext m_gameContext;
};

#endif