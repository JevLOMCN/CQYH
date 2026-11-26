"""All generated fixed-array Lua accessors with TOLUA_RELEASE enabled."""
import re


def harness(client, source, function):
    parts = [source(client / 'Common/utility/lua_array_index.h'),
             '#include <limits>\n#include <type_traits>\n#define TOLUA_RELEASE']
    total = 0
    for number, filename in enumerate(['tolua_data_manager.cpp', 'tolua_global_config.cpp', 'tolua_guild_config.cpp']):
        text = source(client / 'Common/fusion/src/script' / filename)
        methods = [function(text[match.start():], 'static int ') for match in
                   re.finditer(r'(?m)^static int \w+\(lua_State\* tolua_S\)', text)]
        methods = [method for method in methods if 'int tolua_index;' in method]
        pairs, tree = {}, {}

        def node(name):
            value = tree
            for component in name.split('::'):
                value = value.setdefault(component, {})
            return value

        for method in methods:
            assert 'ClientLuaArray::Index(' in method
            owner = re.search(r'self = \(([\w:]+)\*\)', method)[1]
            field = re.search(r'self->(\w+)\[tolua_index\]', method)[1]
            name = re.search(r'static int (\w+)', method)[1]
            entry = pairs.setdefault((owner, field), {})
            entry['set' if name.startswith('tolua_set_') else 'get'] = (name, method)
            value_type = re.search(r'self->\w+\[tolua_index\] = \*\(\(([\w:]+)\*\)', method)
            if value_type:
                entry['type'] = value_type[1]
                node(value_type[1])
            node(owner)
        total += len(methods)
        for i, ((owner, field), entry) in enumerate(pairs.items()):
            assert 'set' in entry and 'get' in entry
            node(owner).setdefault('@fields', {})[field] = (entry.get('type', 'int'), 3 + i % 7)

        parts.append('namespace LuaArray%d {' % number)
        parts.append(r'''
using lua_Number = double;
struct lua_State {
    void* self = nullptr;
    void* input = nullptr;
    void* pointer = nullptr;
    double index = 0, output = 0;
    INT pushes = 0;
};
void lua_pushstring(lua_State*, const char*) {}
void lua_rawget(lua_State*, INT) {}
void* lua_touserdata(lua_State* state, INT) { return state->self; }
void* tolua_tousertype(lua_State* state, INT, INT) { return state->input; }
double tolua_tonumber(lua_State* state, INT position, INT) { return position == 2 ? state->index : 17; }
void tolua_pushnumber(lua_State* state, double value) { ++state->pushes; state->output = value; }
void tolua_pushusertype(lua_State* state, void* value, const char*) { ++state->pushes; state->pointer = value; }
''')

        def emit(nodes, path=()):
            emitted = set()

            def types(children):
                for ctype, _ in children.get('@fields', {}).values():
                    yield ctype
                for key, value in children.items():
                    if not key.startswith('@'):
                        yield from types(value)

            def one(name):
                if name in emitted:
                    return
                children = nodes[name]
                for ctype in types(children):
                    components = tuple(ctype.split('::'))
                    if components[:len(path)] == path and len(components) > len(path):
                        dependency = components[len(path)]
                        if dependency in nodes and dependency != name:
                            one(dependency)
                emitted.add(name)
                parts.append('struct ' + name + ' {')
                emit(children, path + (name,))
                parts.append('int marker;')
                for field, (ctype, size) in children.get('@fields', {}).items():
                    parts.append(ctype + ' ' + field + '[' + str(size) + '];')
                parts.append('};')

            for name in nodes:
                if not name.startswith('@'):
                    one(name)

        emit(tree)
        parts.extend(method for entry in pairs.values() for _, method in [entry['set'], entry['get']])
        parts.append(r'''
template<class T, size_t Size>
void Check(lua_State& state, T (&array)[Size], INT (*set)(lua_State*), INT (*get)(lua_State*)) {
    T input;
    if constexpr (std::is_arithmetic<T>::value) input = 17;
    else memset(&input, 7, sizeof(input));
    state.input = &input;
    memset(array, 0x4f, sizeof(array));
    BYTE before[sizeof(array)]; memcpy(before, array, sizeof(array));
    for (double invalid : {-1e300, -2147483648.0, -1.0, -0.1, double(Size), double(Size + 1),
                           2147483647.0, 1e300, std::numeric_limits<double>::infinity(),
                           -std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN()}) {
        state.index = invalid; state.pushes = 0;
        assert(set(&state) == 0 && get(&state) == 0 && state.pushes == 0);
        assert(memcmp(before, array, sizeof(array)) == 0);
    }
    for (size_t i = 0; i < Size; ++i) {
        state.index = double(i); state.pushes = 0;
        assert(set(&state) == 0 && get(&state) == 1 && state.pushes == 1);
        assert(memcmp(&array[i], &input, sizeof(T)) == 0);
        if constexpr (std::is_arithmetic<T>::value) assert(state.output == 17);
        else assert(state.pointer == &array[i]);
    }
    void* owner = state.self; state.self = nullptr; state.index = 0; state.pushes = 0;
    assert(set(&state) == 0 && get(&state) == 0 && state.pushes == 0);
    state.self = owner;
}
void Run() {
    lua_State state;
''')
        for (owner, field), entry in pairs.items():
            parts.extend(['{', owner + ' object = {};', 'state.self = &object;',
                          'Check(state, object.' + field + ', ' + entry['set'][0] + ', ' + entry['get'][0] + ');', '}'])
        parts.append('}\n}')
    assert total == 274, total
    parts.append(r'''
void RunLuaArrayBoundsTests() {
    LuaArray0::Run(); LuaArray1::Run(); LuaArray2::Run();
    puts("All 137 generated Lua array pairs reject invalid/nonfinite indexes and null owners in release builds");
}
''')
    return '\n'.join(parts)
