"""Run every generated fixed-array Lua getter/setter with release checks disabled."""
import re


FILES = ["tolua_data_manager.cpp", "tolua_global_config.cpp", "tolua_guild_config.cpp"]


def harness(client, source, function):
    parts = [source(client / "Common/utility/fixed_string.h"), "\n#define TOLUA_RELEASE\n"]
    total = 0
    for index, filename in enumerate(FILES):
        text = source(client / "Common/fusion/src/script" / filename)
        methods = [function(text[match.start():], "static int ") for match in
                   re.finditer(r"(?m)^static int \w+\(lua_State\* tolua_S\)", text)]
        setters, getters = {}, {}
        types = {}
        globals_ = {}
        for method in methods:
            owner_match = re.search(r"\b((?:\w+::)*\w+)\s*\*\s*self\s*=", method)
            owner = owner_match[1] if owner_match else ""
            setter = re.search(r"ClientFixedString::Assign\((self->\w+|\w+),", method)
            getter = re.search(r"ClientFixedString::Length\((self->\w+|\w+)\)", method)
            if not setter and not getter:
                continue
            target = (setter or getter)[1]
            key = (owner, target)
            name = re.search(r"static int (\w+)\(", method)[1]
            if setter:
                setters[key] = (name, method)
            if getter:
                getters[key] = (name, method)
        assert setters.keys() == getters.keys(), filename
        total += len(setters)
        for number, (owner, target) in enumerate(setters):
            capacity = 7 + number % 19
            if owner:
                types.setdefault(owner, {})[target.removeprefix("self->")] = capacity
            else:
                globals_[target] = capacity
        parts.append("namespace LuaFixed%d {\n" % index)
        parts.append(r'''
struct lua_State { void* self = nullptr; const char* input = nullptr; std::string output; };
void* tolua_tousertype(lua_State* state, int, int) { return state->self; }
const char* tolua_tostring(lua_State* state, int, int) { return state->input; }
void lua_pushlstring(lua_State* state, const char* bytes, size_t length) {
    state->output.assign(bytes, length);
}
''')
        tree = {}
        for owner, fields in types.items():
            node = tree
            for component in owner.split("::"):
                node = node.setdefault(component, {})
            node["@fields"] = fields

        def emit(tree):
            for name, children in tree.items():
                if name == "@fields":
                    parts.extend("char %s[%d];\n" % field for field in children.items())
                else:
                    parts.append("struct " + name + " {\n")
                    emit(children)
                    parts.append("};\n")

        emit(tree)
        parts.extend("char %s[%d];\n" % (name, size) for name, size in globals_.items())
        parts.extend(method for name, method in setters.values())
        parts.extend(method for name, method in getters.values())
        parts.append(r'''
template<size_t Size>
void Check(lua_State& state, char (&field)[Size], int (*set)(lua_State*), int (*get)(lua_State*), bool member) {
    memset(field, 'X', Size);
    assert(get(&state) == 1 && state.output == std::string(Size, 'X'));
    for (size_t length : {size_t(0), size_t(1), Size - 2, Size - 1, Size, Size + 1, Size * 2, size_t(4096)}) {
        std::string input(length, 'I');
        state.input = input.c_str();
        memset(field, 'X', Size);
        assert(set(&state) == 0);
        size_t copied = std::min(length, Size - 1);
        for (size_t i = 0; i < Size; ++i) assert(field[i] == (i < copied ? 'I' : 0));
        assert(get(&state) == 1 && state.output == std::string(copied, 'I'));
    }
    state.input = nullptr;
    assert(set(&state) == 0);
    for (char byte : field) assert(!byte);
    if (member) {
        void* saved = state.self;
        state.self = nullptr;
        assert(set(&state) == 0 && get(&state) == 0);
        state.self = saved;
    }
}
void Run() {
    lua_State state;
''')
        for owner in types:
            parts.append(owner + " value_" + owner.replace("::", "_") + ";\n")
        for key, (setter_name, _) in setters.items():
            owner, target = key
            if owner:
                field = "value_" + owner.replace("::", "_") + "." + target.removeprefix("self->")
                parts.append("state.self = &value_" + owner.replace("::", "_") + ";\n")
            else:
                field = target
            parts.append("Check(state, %s, %s, %s, %s);\n" % (
                field, setter_name, getters[key][0], "true" if owner else "false"))
        parts.append("}\n}\n")
    assert total == 115, total
    parts.append(r'''
void TestLuaFixedStrings() {
    LuaFixed0::Run(); LuaFixed1::Run(); LuaFixed2::Run();
    char alias[8] = "abcdef";
    ClientFixedString::Assign(alias, alias + 1);
    assert(!strcmp(alias, "bcdef"));
    puts("Lua fixed-string setter/getter regressions passed (115 pairs, release mode)");
}
#undef TOLUA_RELEASE
''')
    return "\n".join(parts)
