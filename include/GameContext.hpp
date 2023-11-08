#ifndef GAME_CONTEXT_H
#define GAME_CONTEXT_H

#include <SFML/Graphics.hpp>
#include <Resources/AssetManager.hpp>
#include <State/StateManager.hpp>

#include <Game/Eval.hpp>

struct GameContext
{
    sf::RenderWindow window;
    AssetManager *assetManager;
    StateManager *stateManager;
    Eval eval;
};

#endif