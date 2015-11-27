#pragma once

#include "BaseFun.h"
#include "primitives.h"
#include <string>

namespace sel {
template <typename Ret, typename... Args>
class Fun : public BaseFun {
private:
    using _fun_type = std::function<Ret(detail::decay_primitive<Args>...)>;
    _fun_type _fun;

public:
    Fun(lua_State *&l,
        _fun_type fun) : _fun(fun) {
        lua_pushlightuserdata(l, (void *)static_cast<BaseFun *>(this));
        lua_pushcclosure(l, &detail::_lua_dispatcher, 1);
    }

    // Each application of a function receives a new Lua context so
    // this argument is necessary.
    int Apply(lua_State *l) override {
        detail::_call<Args...>(l, _fun);
        return detail::_arity<Ret>::value;
    }

};
}
