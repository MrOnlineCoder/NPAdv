#include <Game/Eval.hpp>

#include <Common/Logger.hpp>

Eval::Eval()
{
    reset();
}

void Eval::setVariable(std::string name)
{
    lua_pushboolean(m_luaState, true);
    lua_setglobal(m_luaState, name.c_str());

    GetLogger().tag("Eval") << "Set variable " << name;
}

bool Eval::evalCondition(std::string condition)
{
    std::string code = "return " + condition;

    if (luaL_dostring(m_luaState, code.c_str()) != 0)
    {
        GetLogger().tag("Eval") << "Error while evaluating condition: " << condition << ", error = " << lua_tostring(m_luaState, -1);
        return false;
    }

    bool result = lua_toboolean(m_luaState, -1);
    lua_pop(m_luaState, 1);

    GetLogger().tag("Eval") << "Evaluated condition: " << condition << ", result = " << result;

    return result;
}

void Eval::reset()
{
    if (m_luaState)
    {
        lua_close(m_luaState);
        delete m_luaState;
    }

    m_luaState = luaL_newstate();
}