#ifndef EVAL_HPP
#define EVAL_HPP

#include <string>
#include <lua.hpp>

class Eval
{
public:
    Eval();

    void setVariable(std::string name);
    bool evalCondition(std::string condition);

    void reset();

private:
    lua_State *m_luaState;
};

#endif