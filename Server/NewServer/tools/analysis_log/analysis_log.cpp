#include "stdafx.h"
#include "iocp_mgr.h"

#include <algorithm>
#include <charconv>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string_view>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: analysis_log log-directory mode\nModes: 1 errors, 2 network flow\n";
        return EXIT_FAILURE;
    }
    try
    {
        int mode = 0;
        const std::string_view argument(argv[2]);
        const auto [end, error] = std::from_chars(argument.data(), argument.data() + argument.size(), mode);
        if (error != std::errc{} || end != argument.data() + argument.size() || mode < 1 || mode > 2)
            throw std::runtime_error("Invalid analysis mode");
        std::vector<std::filesystem::path> paths;
        for (const auto& entry : std::filesystem::directory_iterator(argv[1]))
        {
            const auto name = entry.path().filename().string();
            auto extension = entry.path().extension().string();
            std::ranges::transform(extension, extension.begin(), [](char c)
            { return c >= 'A' && c <= 'Z' ? static_cast<char>(c - 'A' + 'a') : c; });
            if (!name.starts_with('.') && extension == ".log" && entry.is_regular_file())
                paths.push_back(entry.path());
        }
        if (paths.empty())
            return EXIT_SUCCESS;
        std::ranges::sort(paths);
        log_analysis::Workers workers;
        if (!workers.Start(std::min<std::size_t>(32, paths.size())))
            throw std::runtime_error("Cannot start analysis workers");
        log_analysis::ErrorCounts errors;
        bool success = true;
        auto fetch = [&]
        {
            auto task = workers.WaitResult();
            if (!task)
                throw std::runtime_error("Analysis stopped before all results were received");
            if (task->failed)
            {
                success = false;
                std::cerr << task->path.string() << ": " << (task->diagnostic.empty() ? "Analysis failed" : task->diagnostic) << '\n';
                return;
            }
            for (const auto& [message, count] : task->errors)
                errors[message] += count;
            std::cout << task->output;
        };
        for (const auto& path : paths)
        {
            auto task = std::make_unique<log_analysis::Task>();
            task->path = path;
            task->mode = static_cast<log_analysis::Mode>(mode);
            if (!workers.Submit(std::move(task)))
                throw std::runtime_error("Cannot submit analysis task");
            while (workers.Pending() > 16)
                fetch();
        }
        while (workers.Pending() != 0)
            fetch();
        for (const auto& [message, count] : errors)
            std::cout << message << '\t' << count << '\n';
        return success ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    catch (const std::exception& error)
    {
        std::cerr << "analysis_log: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
