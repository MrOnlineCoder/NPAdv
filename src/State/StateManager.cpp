#include <State/StateManager.hpp>
#include <State/State.hpp>

#include <Common/Logger.hpp>

StateManager::StateManager()
{
    m_currentState = 0;
    m_nextState = 0;
}

void StateManager::addState(State *state)
{
    m_states.push_back(state);
}

void StateManager::changeState(StateType type)
{
    int index = static_cast<int>(type);

    GetLogger().tag("StateManager") << "Switching to state " << index;

    m_nextState = index;
}

// called after each frame
// so we change states after all logic in current state is done
void StateManager::updateStatePointers()
{
    if (m_currentState != m_nextState)
    {
        m_states[m_currentState]->onLeave();

        m_currentState = m_nextState;

        m_states[m_currentState]->onEnter();
    }
}

void StateManager::release()
{
    m_states[m_currentState]->onLeave();

    for (std::vector<State *>::iterator it = m_states.begin(); it != m_states.end(); it++)
    {
        delete *it;
    }

    GetLogger().tag("StateManager") << "Destroyed all states.";
}

State *StateManager::getCurrentState()
{
    return m_states[m_currentState];
}

State *StateManager::getState(StateType type)
{
    return m_states[static_cast<int>(type)];
}