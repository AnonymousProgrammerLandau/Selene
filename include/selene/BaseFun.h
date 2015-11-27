#pragma once

#include "function.h"
#include <exception>
#include "ExceptionHandler.h"
#include <functional>
#include "primitives.h"
#include "util.h"

namespace sel {
struct BaseFun {
    virtual ~BaseFun() {}
    virtual int Apply(lua_State *state) = 0;
};

namespace detail {

inline int _lua_dispatcher(lua_State *l) {
    BaseFun *fun = (BaseFun *)lua_touserdata(l, lua_upvalueindex(1));
    _lua_check_get raiseParameterConversionError = nullptr;
    const char * wrong_meta_table = nullptr;
    int erroneousParameterIndex = 0;
    try {
        return fun->Apply(l);
    } catch (GetParameterFromLuaTypeError & e) {
        raiseParameterConversionError = e.checked_get;
        erroneousParameterIndex = e.index;
    } catch (GetUserdataParameterFromLuaTypeError & e) {
        wrong_meta_table = lua_pushlstring(
            l, e.metatable_name.c_str(), e.metatable_name.length());
        erroneousParameterIndex = e.index;
    } catch (std::exception & e) {
        lua_pushstring(l, e.what());
        Traceback(l);
        store_current_exception(l, lua_tostring(l, -1));
    } catch (...) {
        lua_pushliteral(l, "<Unknown exception>");
        Traceback(l);
        store_current_exception(l, lua_tostring(l, -1));
    }

    if(raiseParameterConversionError) {
        raiseParameterConversionError(l, erroneousParameterIndex);
    }
    else if(wrong_meta_table) {
        luaL_checkudata(l, erroneousParameterIndex, wrong_meta_table);
    }

    return lua_error(l);
}

struct PushResults{};
struct DontPushResults{};

template<
    typename Indices,
    typename... Args
>
struct _get_args;

template<
    std::size_t... idx,
    typename... Args
>
class _get_args<_indices<idx...>, Args...> {
    lua_State *_l;

    template<typename Callable, typename... ExplicitParams>
    void _call(DontPushResults, Callable&& callable, ExplicitParams&&... explicitParams) {
        callable(
            std::forward<ExplicitParams>(explicitParams)...,
            _check_get(_id<Args>{}, _l, idx + 1)...);
    }

    template<typename Callable, typename... ExplicitParams>
    void _call(PushResults, Callable&& callable, ExplicitParams&&... explicitParams) {
        _push(_l, callable(
            std::forward<ExplicitParams>(explicitParams)...,
            _check_get(_id<Args>{}, _l, idx + 1)...));
    }

public:
    _get_args(lua_State *l) : _l(l) {}

    template<typename Callable, typename... ExplicitParams>
    void Call(Callable&& callable, ExplicitParams&&... explicitParams) {
        constexpr bool has_results =
            !std::is_void<
                decltype(callable(
                    std::forward<ExplicitParams>(explicitParams)...,
                    _check_get(_id<Args>{}, _l, idx + 1)...))
            >::value;

        _call(
            typename std::conditional<
                has_results,
                PushResults,
                DontPushResults
            >::type{},
            std::forward<Callable>(callable),
            std::forward<ExplicitParams>(explicitParams)...
        );
    }

};

template<
    typename... Args,
    typename Callable,
    typename... ExplicitParams
>
void _call(
    lua_State *l,
    Callable &&callable,
    ExplicitParams&&... explicitParams
) {
    using arg_indices = typename _indices_builder<sizeof...(Args)>::type;
    _get_args<arg_indices, decay_primitive<Args>...>{l}
    .Call(
        std::forward<Callable>(callable),
        std::forward<ExplicitParams>(explicitParams)...
    );
}
}
}
