#ifndef CQYH_GM_ROLE_DATA_H
#define CQYH_GM_ROLE_DATA_H

#include "std_type.h"
#include "gm_def.h"
#include <algorithm>
#include <cstring>
#include <expected>
#include <span>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

namespace gm
{
struct RoleRecord
{
    UINT16 type{};
    UINT16 base{};
    std::vector<char> payload;

    template <class T> T value(std::size_t offset = 0) const
    {
        static_assert(std::is_trivially_copyable_v<T>);
        if (offset > payload.size() || sizeof(T) > payload.size() - offset)
            throw std::out_of_range("role value is outside its record");
        T result;
        std::memcpy(&result, payload.data() + offset, sizeof(result));
        return result;
    }
    std::string text(std::size_t offset = 0) const;
};
using RoleDocument = std::vector<RoleRecord>;
std::expected<RoleDocument, std::string> parse_role_data(std::span<const char> bytes);

struct RoleReply
{
    UINT32 role_id;
    INT32 area_id;
    bool online;
    RoleDocument document;
};
std::expected<RoleReply, std::string> decode_query_role(std::span<const std::byte> packet);
std::expected<RoleReply, std::string> decode_loaded_role(std::span<const std::byte> packet);

class RoleEdits
{
public:
    std::expected<void, std::string> set_signed(UINT16 base, INT32 value);
    std::expected<void, std::string> set_unsigned(UINT16 base, UINT32 value);
    std::expected<void, std::string> return_position();
    std::expected<void, std::string> remove(UINT16 type, UINT32 id);
    std::expected<void, std::string> take_item(UINT32 slot, UINT64 guid);
    std::span<const char> bytes() const { return bytes_; }
    void clear() { bytes_.clear(); }
private:
    std::expected<void, std::string> append(UINT16 type, UINT16 base, std::span<const char> value);
    std::vector<char> bytes_;
};
}
#endif
