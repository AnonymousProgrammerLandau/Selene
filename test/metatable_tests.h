#pragma once

#include <selene.h>

struct Qux {
    int baz() { return 4; }
    int qux = 3;
};

static Qux qux;

Qux *GetQuxPtr() { return &qux; }
Qux &GetQuxRef() { return qux; }

bool test_metatable_registry_ptr(sel::State &state) {
    state["get_instance"] = &GetQuxPtr;
    state["Qux"].SetClass<Qux>("baz", &Qux::baz);
    state.Load("../test/test_metatable.lua");
    return state["call_method"]() == 4;
}

bool test_metatable_registry_ref(sel::State &state) {
    state["get_instance"] = &GetQuxRef;
    state["Qux"].SetClass<Qux>("baz", &Qux::baz);
    state.Load("../test/test_metatable.lua");
    return state["call_method"]() == 4;
}

bool test_metatable_ptr_member(sel::State &state) {
    state["get_instance"] = &GetQuxPtr;
    state["Qux"].SetClass<Qux>("baz", &Qux::baz, "qux", &Qux::qux);
    state.Load("../test/test_metatable.lua");
    return state["access_member"]() == 3;
}

bool test_metatable_ref_member(sel::State &state) {
    state["get_instance"] = &GetQuxRef;
    state["Qux"].SetClass<Qux>("baz", &Qux::baz, "qux", &Qux::qux);
    state.Load("../test/test_metatable.lua");
    return state["access_member"]() == 3;
}

struct Quxx {
    int baz() { return 3; }
    int quux = 2;
};

static Quxx quxx;

Quxx *GetQuxxPtr() { return &quxx; }

bool test_objects_keep_their_type(sel::State &state) {
    state["get_instance"] = &GetQuxPtr;
    state["get_instance2"] = &GetQuxxPtr;
    state["Qux"].SetClass<Qux>("baz", &Qux::baz);
    state["Quxx"].SetClass<Quxx>("baz", &Quxx::baz);
    state.Load("../test/test_metatable.lua");
    auto instances = state["return_both_instances"]().GetTuple<Qux*, Quxx*>();
    return std::get<0>(instances) != nullptr && std::get<1>(instances) != nullptr;
}
