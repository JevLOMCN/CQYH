#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace guard_runtime
{
class ConsoleInput
{
public:
    static constexpr std::size_t capacity = 1023;
    struct Line { std::string text; bool overflow; };
    std::optional<Line> push(int key)
    {
        if (m_extended) { m_extended = false; return {}; }
        if (key == 0 || key == 224) { m_extended = true; return {}; }
        if (key == '\r' || key == '\n')
        {
            Line line{std::exchange(m_text, {}), std::exchange(m_overflow, false)};
            return line;
        }
        if (key == '\b')
        {
            // Once truncated, reject the whole line. Editing cannot turn its
            // unseen suffix into a different executable command.
            if (!m_overflow && !m_text.empty()) m_text.pop_back();
        }
        else if (key >= 32 && key <= 126 && !m_overflow)
        {
            if (m_text.size() == capacity) m_overflow = true;
            else m_text.push_back(static_cast<char>(key));
        }
        return {};
    }
    std::string_view text() const noexcept { return m_text; }
    void clear() noexcept { m_text.clear(); m_overflow = m_extended = false; }
private:
    std::string m_text;
    bool m_overflow = false, m_extended = false;
};
}
