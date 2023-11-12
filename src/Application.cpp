#include <Application.hpp>
#include <Common/Logger.hpp>
#include <State/MenuState.hpp>
#include <Common/FilesystemUtils.hpp>
#include <State/IntroState.hpp>
#include <State/PlayState.hpp>
#include <State/CreditsState.hpp>

Application::Application(ApplicationLaunchOptions options)
{
    m_gameContext.window.create(
        sf::VideoMode(1920, 1080), "NPTavern", sf::Style::Fullscreen);
    m_gameContext.window.setFramerateLimit(
        options.fps_limit);
    GetLogger().tag("Application") << "Created game window";

    loadAssets();
    loadStates();
}

void Application::loadAssets()
{
    m_gameContext.assetManager = new AssetManager();
    m_gameContext.assetManager->init();

    // Fonts
    m_gameContext.assetManager->loadFont("main", "fonts/OpenSans.ttf");
    m_gameContext.assetManager->loadFont("epic", "fonts/Volja.ttf");

    // Backgrounds, textures, icons
    m_gameContext.assetManager->loadAllTexturesInFolder("backgrounds");
    m_gameContext.assetManager->loadAllTexturesInFolder("images");
    m_gameContext.assetManager->loadAllTexturesInFolder("targets");

    // Sounds
    m_gameContext.assetManager->loadAllSoundsInFolder("sounds");
}

void Application::loadStates()
{
    m_gameContext.stateManager = new StateManager();

    IntroState *introState = new IntroState(&m_gameContext);
    m_gameContext.stateManager->addState(introState);

    MenuState *menuState = new MenuState(&m_gameContext);
    m_gameContext.stateManager->addState(menuState);

    PlayState *playState = new PlayState(&m_gameContext);
    m_gameContext.stateManager->addState(playState);

    CreditsState *creditsState = new CreditsState(&m_gameContext);
    m_gameContext.stateManager->addState(creditsState);

    introState->onEnter();
}

void Application::run()
{
    if (m_gameContext.assetManager->hasError())
    {
        GetLogger().tag("Application") << "Assets loading failed, not starting application!";
        return;
    }

    GetLogger().tag("Application") << "Entering event loop...";

    sf::Clock frameClock;
    float frameDelta = 0;

    while (m_gameContext.window.isOpen())
    {
        sf::Event ev;

        while (m_gameContext.window.pollEvent(ev))
        {
            if (ev.type == sf::Event::Closed)
            {
                m_gameContext.window.close();
                continue;
            }

            m_gameContext.stateManager->getCurrentState()->onInput(ev);
        }

        m_gameContext.stateManager->getCurrentState()->onUpdate(
            frameDelta);

        m_gameContext.window.clear();

        m_gameContext.stateManager->getCurrentState()->onRender();

        m_gameContext.window.display();

        m_gameContext.stateManager->updateStatePointers();

        frameDelta = frameClock.restart().asSeconds();
    }

    m_gameContext.stateManager->release();

    m_gameContext.assetManager->release();

    delete m_gameContext.assetManager;
    delete m_gameContext.stateManager;

    GetLogger()
            .tag("Application")
        << "Event loop finished. Exiting...";
}
