#ifndef CQYH_BASELIB_SQL_RESULTS_H
#define CQYH_BASELIB_SQL_RESULTS_H

#include <cstddef>
#include <cstdint>
#include <list>
#include <memory>
#include <vector>

namespace cqyh
{
// Own byte arrays while exposing the legacy callback's borrowed row list.
// Row addresses remain stable as the result set grows. Clearing the view in a
// callback does not transfer ownership of its allocations.
class SqlResults
{
public:
    std::uint8_t* Append(std::size_t size)
    {
        auto row = std::make_unique_for_overwrite<std::uint8_t[]>(size);
        auto* data = row.get();
        m_storage.push_back(std::move(row));
        try
        {
            m_rows.push_back(data);
        }
        catch (...)
        {
            m_storage.pop_back();
            throw;
        }
        return data;
    }

    std::list<void*>& Rows() noexcept { return m_rows; }
    void Clear() noexcept
    {
        m_rows.clear();
        m_storage.clear();
    }

private:
    std::vector<std::unique_ptr<std::uint8_t[]>> m_storage;
    std::list<void*> m_rows;
};
}

#endif // CQYH_BASELIB_SQL_RESULTS_H
