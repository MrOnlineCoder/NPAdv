#ifndef GAME_CONTEXT_H
#define GAME_CONTEXT_H

#include <SFML/Graphics.hpp>
#include <Resources/AssetManager.hpp>
#include <State/StateManager.hpp>

struct GameContext
{
    sf::RenderWindow window;
    AssetManager *assetManager;
    StateManager *stateManager;
};

#endif