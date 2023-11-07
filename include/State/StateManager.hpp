#ifndef STATE_MANAGER_HPP
#define STATE_MANAGER_HPP

#include <vector>

#include <SFML/System/Clock.hpp>

enum class StateType
{
    INTRO = 0,
    MENU = 1,
    GAME = 2
};

class State;
class StateManager
{
public:
    StateManager();

    void addState(State *state);
    void changeState(StateType type);

    void updateStatePointers();

    void release();

    State *getCurrentState();
    State *getState(StateType type);

private:
    std::vector<State *> m_states;

    std::size_t m_currentState;
    std::size_t m_nextState;
};

#endif