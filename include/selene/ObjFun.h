#pragma once

#include "BaseFun.h"
#include <string>

namespace sel {

template <typename Ret, typename... Args>
class ObjFun : public BaseFun {
private:
    using _fun_type = std::function<Ret(Args...)>;
    _fun_type _fun;

public:
    ObjFun(lua_State *l,
           const std::string &name,
           Ret(*fun)(Args...))
        : ObjFun(l, name, _fun_type{fun}) {}

    ObjFun(lua_State *l,
           const std::string &name,
           _fun_type fun) : _fun(fun) {
        lua_pushlightuserdata(l, (void *)static_cast<BaseFun *>(this));
        lua_pushcclosure(l, &detail::_lua_dispatcher, 1);
        lua_setfield(l, -2, name.c_str());
    }

    // Each application of a function receives a new Lua context so
    // this argument is necessary.
    int Apply(lua_State *l) {
        detail::_call<Args...>(l, _fun);
        return detail::_arity<Ret>::value;
    }
};
}
