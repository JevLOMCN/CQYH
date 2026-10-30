#ifndef CQYH_GM_DATA_PACKETS_H
#define CQYH_GM_DATA_PACKETS_H

#include "data_mgr.h"
#include <cstddef>
#include <expected>
#include <span>
#include <string>

namespace gm
{
struct RoleKey
{
    UINT32 id;
    std::string name, account;
};
struct RoleKeys
{
    INT32 area;
    bool truncated;
    std::vector<RoleKey> rows;
};
std::expected<RoleKeys, std::string> decode_role_keys(std::span<const std::byte> packet);
struct ItemBatch
{
    bool clear;
    std::vector<::ITEM_TEMPLATE_DATA> items;
};
std::expected<ItemBatch, std::string> decode_item_batch(std::span<const std::byte> packet);
std::expected<std::vector<TL_ANNOUNCE_DATA>, std::string> decode_announcements(
    std::span<const std::byte> packet, INT32 server_id);
}
#endif
