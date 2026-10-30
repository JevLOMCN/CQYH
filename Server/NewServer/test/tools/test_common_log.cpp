#include "common_log.h"
#include "anticheat_log.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{
int checks = 0;
void require(bool condition, const char* message)
{
    ++checks;
    if (!condition)
        throw std::runtime_error(message);
}

struct Capture : log_context
{
    std::vector<std::vector<std::string>> fields;
    int lines = 0;
    int ends = 0;
    bool reject_field = false;
    bool reject_line = false;
    bool reject_end = false;
    bool throw_field = false;

    Capture()
    {
        ld = nullptr;
        field_func = [](int column, int row, const char* value, log_context* base)
        {
            auto& self = *static_cast<Capture*>(base);
            if (self.throw_field)
                throw std::runtime_error("callback failure");
            if (self.fields.size() <= static_cast<std::size_t>(row))
                self.fields.resize(row + 1);
            require(self.fields[row].size() == static_cast<std::size_t>(column), "field order");
            self.fields[row].emplace_back(value);
            return !self.reject_field;
        };
        line_func = [](int row, log_context* base)
        {
            auto& self = *static_cast<Capture*>(base);
            require(row == self.lines, "line order");
            ++self.lines;
            return !self.reject_line;
        };
        end_func = [](log_context* base)
        {
            auto& self = *static_cast<Capture*>(base);
            ++self.ends;
            return !self.reject_end;
        };
    }
};

void parser_cases()
{
    for (const std::string ending : {"\n", "\r\n", ""})
    {
        const std::string long_field(10000, 'x');
        std::string input = "a\tb\t\r\n\r\n" + long_field + '\t' + char(0xff) + ending;
        std::istringstream stream(input);
        Capture capture;
        require(load_stream(stream, &capture), "parse line endings and long field");
        require(capture.fields == std::vector<std::vector<std::string>>{{"a", "b", ""}, {""}, {long_field, std::string(1, char(0xff))}},
            "preserve empty fields and high bytes");
        require(capture.lines == 3 && capture.ends == 1, "callbacks exactly once");
    }
    Capture capture;
    std::istringstream empty;
    require(load_stream(empty, &capture) && capture.lines == 0 && capture.ends == 1, "empty stream");
    std::istringstream invalid;
    invalid.setstate(std::ios::failbit);
    require(!load_stream(invalid, &capture), "reject stream failure");
    for (int failure = 0; failure < 3; ++failure)
    {
        Capture rejected;
        rejected.reject_field = failure == 0;
        rejected.reject_line = failure == 1;
        rejected.reject_end = failure == 2;
        std::istringstream input("a\tb\n");
        require(!load_stream(input, &rejected), "callback failure propagation");
        require(rejected.ends == (failure == 2 ? 1 : 0), "no success callback after failure");
    }
    std::istringstream input("text");
    require(!load_stream(input, nullptr) && !load_file(nullptr, &capture), "null input validation");
    Capture missing_callback;
    missing_callback.line_func = nullptr;
    require(!load_stream(input, &missing_callback), "validate callbacks before input");
    const std::string nul("a\0b", 3);
    std::istringstream embedded_nul(nul);
    require(!load_stream(embedded_nul, &capture), "reject embedded NUL");

    const auto path = std::filesystem::current_path() / "callback-throw.log";
    { std::ofstream file(path); file << "text"; }
    Capture throwing;
    throwing.throw_field = true;
    bool caught = false;
    try { load_file(path.string().c_str(), &throwing); }
    catch (const std::runtime_error&) { caught = true; }
    require(caught && std::filesystem::remove(path), "close file on callback exception");
}

void anticheat_cases()
{
    for (const char* time : {"bad", "2020-02-30 00:00:00", "2020-01-01 24:00:00", "2020-01-01 00:60:00",
        "2020-01-01 00:00:60", "2020-01-01 00:00:00extra", "999999999999999-01-01 00:00:00"})
    {
        auto* context = anticheat_init();
        require(!anticheat_process_field(ac_time, 0, time, context), "reject invalid timestamp");
    }
    std::istringstream pending("2020-01-01 00:00:00\tgenerate\tAlice");
    require(load_stream(pending, anticheat_init()), "pending event");
    std::cout << '\n';
    // Reset must clear the previous pending event and every counter.
    std::istringstream after_reset("2020-01-02 00:00:00\tfailed\tAlice\n");
    require(load_stream(after_reset, anticheat_init()), "reset anticheat state");
    std::cout << '\n';
}
}

int main(int argc, char* argv[])
{
    try
    {
        require(argc == 2, "expected work directory");
        std::filesystem::create_directories(argv[1]);
        std::filesystem::current_path(argv[1]);
        parser_cases();
        anticheat_cases();
        std::cout << checks << " common log checks passed\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
