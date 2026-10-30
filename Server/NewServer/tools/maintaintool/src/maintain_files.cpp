#include "maintain_files.h"
#include <algorithm>
#include <format>
#include <set>
#include <stdexcept>
#include <atomic>
#include <chrono>
#include <fstream>
#include <iterator>
#include <limits>
#include <array>

namespace maintain
{
namespace fs = std::filesystem;
std::expected<std::vector<AreaFolder>, std::string> read_areas(const fs::path& root, ScanLimits limits)
try
{
    if (limits.area_count < 2 || limits.guards_per_area < 2 || limits.name_bytes < 2)
        return std::unexpected("invalid configuration scan limits");
    std::vector<AreaFolder> areas;
    std::set<int> ids;
    for (const auto& entry : fs::directory_iterator(root))
    {
        if (!entry.is_directory()) continue;
        AreaFolder area;
        area.folder = entry.path().filename().string();
        const auto separator = area.folder.find('_');
        if (separator == std::string::npos || separator == 0 || separator + 1 == area.folder.size() ||
            area.folder.size() >= limits.name_bytes)
            return std::unexpected("invalid or oversized area folder: " + area.folder);
        const std::string_view prefix{area.folder.data(), separator};
        const auto parsed = std::from_chars(prefix.data(), prefix.data() + prefix.size(), area.id);
        if (parsed.ec != std::errc{} || parsed.ptr != prefix.data() + prefix.size() || area.id <= 0 || area.id >= limits.area_count)
            return std::unexpected("area folder ID is out of range: " + area.folder);
        if (!ids.insert(area.id).second) return std::unexpected(std::format("duplicate area ID {}", area.id));
        area.name = area.folder.substr(separator + 1);
        for (const auto& guard : fs::directory_iterator(entry.path()))
        {
            if (!guard.is_directory()) continue;
            auto address = guard.path().filename().string();
            const auto ip = parse_address(address);
            if (!ip || address.size() >= limits.name_bytes)
                return std::unexpected("invalid guard folder in " + area.folder + ": " + address);
            if (area.guards.size() >= limits.guards_per_area - 1)
                return std::unexpected("too many guards in " + area.folder);
            area.guards.push_back({std::move(address), *ip});
        }
        std::ranges::sort(area.guards, {}, &GuardFolder::address);
        areas.push_back(std::move(area));
    }
    std::ranges::sort(areas, {}, &AreaFolder::id);
    return areas;
}
catch (const std::exception& error) { return std::unexpected(std::string{"cannot scan area configuration: "} + error.what()); }

std::expected<std::vector<fs::path>, std::string> package_files(const fs::path& root, std::string_view extension)
try
{
    if (root.empty()) return std::unexpected("empty package directory");
    fs::create_directories(root);
    std::vector<fs::path> files;
    for (const auto& entry : fs::directory_iterator(root))
    {
        // Retain the legacy case-sensitive substring filter (.zip/.pak).
        if (entry.is_regular_file() && entry.path().filename().native().find(fs::path{extension}.native()) != fs::path::string_type::npos)
            files.push_back(entry.path().filename());
    }
    std::ranges::sort(files, std::greater{});
    return files;
}
catch (const std::exception& error) { return std::unexpected(std::string{"cannot scan package directory: "} + error.what()); }

namespace
{
struct TemporaryDirectory
{
    fs::path path;
    explicit TemporaryDirectory(const fs::path& parent)
    {
        static std::atomic<std::uint64_t> sequence{0};
        for (int attempt=0; attempt<128; ++attempt)
        {
            const auto name=std::format(".maintain-write-{}-{}", std::chrono::steady_clock::now().time_since_epoch().count(), sequence++);
            auto candidate=parent/name;
            if (fs::create_directory(candidate)) { path=std::move(candidate); return; }
        }
        throw std::runtime_error("cannot reserve export staging directory");
    }
    TemporaryDirectory(const TemporaryDirectory&)=delete;
    TemporaryDirectory& operator=(const TemporaryDirectory&)=delete;
    ~TemporaryDirectory() {
        try { std::error_code error; if (!path.empty()) fs::remove_all(path,error); } catch (...) {}
    }
};
}
std::expected<void, std::string> write_atomic(const fs::path& destination, std::string_view bytes)
try
{
    if (destination.empty() || destination.filename().empty()) return std::unexpected("empty export filename");
    if (bytes.size()>static_cast<std::size_t>(std::numeric_limits<std::streamsize>::max()))
        return std::unexpected("export exceeds stream size");
    const auto target=fs::absolute(destination).lexically_normal();
    fs::create_directories(target.parent_path());
    TemporaryDirectory temporary{target.parent_path()};
    const auto staged=temporary.path/"content";
    std::ofstream output{staged,std::ios::binary|std::ios::out|std::ios::noreplace};
    if (!output) return std::unexpected("cannot open staged export");
    if (!bytes.empty()) output.write(bytes.data(),static_cast<std::streamsize>(bytes.size()));
    output.close();
    if (!output) return std::unexpected("cannot finish staged export");
    fs::rename(staged,target);
    return {};
}
catch (const std::exception& error) { return std::unexpected(std::string{"cannot publish export: "}+error.what()); }

std::expected<std::string, std::string> transform_security(std::string_view source)
try
{
    if (source.contains('\0')) return std::unexpected("embedded NUL in security configuration");
    std::string output;
    std::size_t lineNumber=0, transformed=0, position=0;
    while (position<source.size())
    {
        const auto newline=source.find('\n',position);
        auto view=source.substr(position,newline==source.npos ? source.size()-position : newline-position);
        if (newline!=source.npos && view.ends_with('\r')) view.remove_suffix(1);
        std::string line{view};++lineNumber;
        if (const auto field=line.find("szPwd"); field!=line.npos)
        {
            const auto equals=line.find('=',field+5);
            if (equals!=line.npos)
            {
                const auto first=line.find('"',equals+1);
                const auto last=first==line.npos ? line.npos : line.find('"',first+1);
                if (first==line.npos || last==line.npos)
                    return std::unexpected(std::format("missing password quotes on line {}",lineNumber));
                for (auto index=first+1;index<last;++index)
                {
                    const auto value=static_cast<unsigned char>(line[index]);
                    if (value<' ' || value>'~')
                        return std::unexpected(std::format("invalid password character on line {}",lineNumber));
                    // Preserve the existing on-disk transform used by SimpleEncrypt.
                    line[index]=static_cast<char>((value-' '+47)%95+' ');
                }
                ++transformed;
            }
        }
        output+=line;
        if (newline==source.npos) break;
        output+="\r\n"; // Legacy Windows text-mode output.
        position=newline+1;
    }
    if (!transformed) return std::unexpected("security configuration contains no quoted password assignments");
    return output;
}
catch (const std::exception&) { return std::unexpected("cannot transform security configuration"); }

std::expected<void, std::string> generate_security_file(const fs::path& source,const fs::path& destination)
try
{
    std::ifstream input{source,std::ios::binary};
    if (!input) return std::unexpected("cannot open security source file");
    std::string bytes;
    std::array<char,65536> buffer{};
    while (input)
    {
        input.read(buffer.data(),static_cast<std::streamsize>(buffer.size()));
        bytes.append(buffer.data(),static_cast<std::size_t>(input.gcount()));
    }
    if (input.bad() || !input.eof()) return std::unexpected("cannot read security source file");
    auto transformed=transform_security(bytes);
    if (!transformed) return std::unexpected(transformed.error());
    return write_atomic(destination,*transformed);
}
catch (const std::exception&) { return std::unexpected("cannot generate security configuration file"); }
}
