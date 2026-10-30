#include "guard_command.h"

#include <cstdio>
#include <cstring>
#include <optional>
#include <random>
#include <string>
#include <vector>

namespace
{
int checks = 0;
int failures = 0;

void Check(bool value, const char* description)
{
    ++checks;
    if (!value)
    {
        ++failures;
        std::fprintf(stderr, "FAIL: %s\n", description);
    }
}

struct LegacyCommand
{
    std::optional<guard_command::Kind> kind;
    std::string parameters;
};

// Frozen token/match/copy behavior from the old CCmdMgr parser, using a mutable
// copy so the compatibility oracle does not write through a const input pointer.
LegacyCommand LegacyParse(std::string input)
{
    LegacyCommand result;
    if (input.empty() || input.front() == '.')
        return result;
    char* next = nullptr;
    const char* token = strtok_s(input.data(), " ", &next);
    if (token)
    {
        for (std::size_t index = 0; index < guard_command::names.size(); ++index)
        {
            const auto name = guard_command::names[index];
            if (_strnicmp(name.data(), token, name.size()) == 0)
            {
                result.kind = static_cast<guard_command::Kind>(index);
                break;
            }
        }
    }
    if (next)
        result.parameters.assign(next, (std::min)(std::strlen(next), std::size_t{255}));
    return result;
}

void Compare(std::string input)
{
    const auto original = input;
    const auto old = LegacyParse(input);
    const auto parsed = guard_command::Parse(input);
    Check(parsed.has_value() == old.kind.has_value(), "legacy command recognition");
    if (parsed && old.kind)
    {
        Check(parsed->kind == *old.kind, "legacy command selection");
        Check(parsed->parameters == old.parameters, "legacy parameter bytes");
        Check(parsed->parameters.size() <= 255, "legacy parameter length bound");
        // Views must refer to the caller's live input, never a temporary token.
        Check(parsed->parameters.empty() ||
            (parsed->parameters.data() >= input.data() &&
             parsed->parameters.data() + parsed->parameters.size() <= input.data() + input.size()),
            "parameter view lifetime");
    }
    Check(input == original, "input text remains unchanged");
}
}

int main()
{
    static_assert(guard_command::Parse("START -g")->kind == guard_command::Kind::start);
    static_assert(guard_command::Parse("reload  data")->parameters == " data");
    static_assert(!guard_command::Parse(".reload"));

    // String literals exercise the formerly invalid const-input call directly.
    const auto literal = guard_command::Parse("start -g -r");
    Check(literal && literal->parameters == "-g -r", "read-only literal input");
    Check(!guard_command::Parse(""), "empty input rejected");
    Check(!guard_command::Parse("   "), "space-only input rejected");
    Check(!guard_command::Parse("st"), "short command prefix rejected");
    Check(!guard_command::Parse("unknown -g"), "unknown command rejected");
    Check(guard_command::Parse("start-extra -g").has_value(), "legacy suffix matching retained");

    const std::vector<std::string> parameters{
        "", " ", "  ", "-a", "-g -r", "-p -c", "\t-g", "file name.lua", ".reload",
        std::string(254, 'x'), std::string(255, 'x'), std::string(256, 'x'),
        std::string(4096, 'x'), std::string("\xd6\xd0\xce\xc4", 4)};
    for (const auto name : guard_command::names)
    {
        std::string upper{name};
        std::ranges::transform(upper, upper.begin(), [](char value) { return value - ('a' - 'A'); });
        const std::vector<std::string> commands{
            std::string{name}, upper, std::string{name} + "extra", std::string{name} + "\t-a",
            std::string{name.substr(0, name.size() - 1)}, "." + std::string{name}};
        for (const auto& command : commands)
            for (const auto& argument : parameters)
                for (int leading = 0; leading < 4; ++leading)
                    for (int separator = 0; separator < 4; ++separator)
                        Compare(std::string(leading, ' ') + command + std::string(separator, ' ') + argument);
    }
    std::mt19937 random{0x47554152};
    constexpr std::string_view characters = " startopqclhekuin- .\t\r\nABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for (int iteration = 0; iteration < 10000; ++iteration)
    {
        std::string input(random() % 300, ' ');
        for (auto& value : input)
            value = characters[random() % characters.size()];
        Compare(std::move(input));
    }
    std::printf("guard command parsing: %d checks, %d failures\n", checks, failures);
    return failures != 0;
}
