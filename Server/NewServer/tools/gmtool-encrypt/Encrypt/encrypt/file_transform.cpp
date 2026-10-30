#include "file_transform.h"
#include "base.h"
#include "zlib.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <format>
#include <fstream>
#include <limits>
#include <span>
#include <string_view>
#include <vector>
#ifdef _WIN32
#include <Windows.h>
#endif

namespace
{
namespace fs=std::filesystem;
bool valid_path(const fs::path& value)
{
    return !value.empty() && value.native().find(fs::path::value_type{})==fs::path::string_type::npos;
}

std::expected<std::string,std::string> legacy_basename(const fs::path& path)
{
    auto name=path.filename().native();
    const auto dot=name.find(static_cast<fs::path::value_type>('.'));
    if (dot!=name.npos) name.resize(dot);
#ifdef _WIN32
    if (name.empty()) return std::string{};
    if (name.size()>static_cast<std::size_t>((std::numeric_limits<int>::max)()))
        return std::unexpected("Filename exceeds the legacy encoding limit.");
    const auto utf8=GetACP()==CP_UTF8;
    BOOL substituted=FALSE;
    const auto flags=utf8 ? WC_ERR_INVALID_CHARS : WC_NO_BEST_FIT_CHARS;
    const auto length=WideCharToMultiByte(CP_ACP,flags,name.data(),static_cast<int>(name.size()),nullptr,0,nullptr,utf8 ? nullptr : &substituted);
    if (!length || substituted) return std::unexpected("Filename cannot be represented in the legacy filename encoding.");
    std::string bytes(static_cast<std::size_t>(length),'\0');
    if (WideCharToMultiByte(CP_ACP,flags,name.data(),static_cast<int>(name.size()),bytes.data(),length,nullptr,utf8 ? nullptr : &substituted)!=length || substituted)
        return std::unexpected("Filename cannot be represented in the legacy filename encoding.");
    return bytes;
#else
    return name;
#endif
}

std::expected<std::vector<char>,std::string> read_file(const fs::path& input,gm_file::Operation operation)
{
    std::error_code error;
    if (!fs::is_regular_file(input,error)) return std::unexpected("Select an existing regular file.");
    std::ifstream stream(input,std::ios::binary|std::ios::ate);
    if (!stream) return std::unexpected("Cannot open the selected file for reading.");
    const auto position=stream.tellg();
    if (position<0) return std::unexpected("Cannot determine the selected file size.");
    const auto length=static_cast<std::uint64_t>(position);
    const auto limit=static_cast<std::uint64_t>((std::numeric_limits<std::int32_t>::max)())-
        (operation==gm_file::Operation::encrypt ? 7 : 0);
    if (length>limit) return std::unexpected("File exceeds the legacy 32-bit transform format limit.");
    if (operation==gm_file::Operation::decrypt && (length<4 || length%4!=0))
        return std::unexpected("Encrypted file has an invalid size.");
    // A non-null input address is required by the legacy transform even for an empty file.
    std::vector<char> bytes(static_cast<std::size_t>((std::max)(length,std::uint64_t{1})));
    stream.seekg(0);
    if (length && !stream.read(bytes.data(),static_cast<std::streamsize>(length)))
        return std::unexpected("Cannot read the complete selected file.");
    if (stream.peek()!=std::char_traits<char>::eof() || stream.bad())
        return std::unexpected("Selected file changed while it was being read.");
    bytes.resize(static_cast<std::size_t>(length));
    return bytes;
}

std::expected<void,std::string> publish(const fs::path& output,std::span<const char> bytes)
{
    static std::atomic<std::uint64_t> serial{};
    const auto tick=std::chrono::steady_clock::now().time_since_epoch().count();
    fs::path stage;
    std::ofstream stream;
    for (unsigned attempt=0;attempt<16;++attempt)
    {
        stage=output.parent_path()/std::format(".cqyh-transform-{}-{}.tmp",tick,serial.fetch_add(1,std::memory_order_relaxed));
        stream.open(stage,std::ios::binary|std::ios::out|std::ios::noreplace);
        if (stream) break;
        std::error_code error;
        if (!fs::exists(stage,error)) return std::unexpected("Cannot create a temporary output file.");
        stream.clear();
    }
    if (!stream) return std::unexpected("Cannot reserve a temporary output file.");
    struct Cleanup
    {
        fs::path path;
        std::ofstream& stream;
        ~Cleanup(){if(stream.is_open())stream.close();std::error_code error;fs::remove(path,error);}
    } cleanup{std::move(stage),stream};
    if (!bytes.empty()) stream.write(bytes.data(),static_cast<std::streamsize>(bytes.size()));
    stream.flush();
    if (!stream) return std::unexpected("Cannot write the complete temporary output file.");
    stream.close();
    if (!stream) return std::unexpected("Cannot close the temporary output file.");
#ifdef _WIN32
    // std::filesystem::rename does not replace an existing destination on Windows.
    if (!MoveFileExW(cleanup.path.c_str(),output.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))
        return std::unexpected(std::format("Cannot replace the output file (Windows error {}).",GetLastError()));
#else
    std::error_code error;fs::rename(cleanup.path,output,error);
    if (error) return std::unexpected(std::format("Cannot replace the output file (error {}).",error.value()));
#endif
    return {};
}
}

std::expected<std::filesystem::path,std::string> gm_file::transform_file(
    const fs::path& input,Operation operation,const fs::path& output_directory)
{
    try
    {
        if (!valid_path(input)) return std::unexpected("Select a filename without embedded NUL characters.");
        if (operation!=Operation::encrypt && operation!=Operation::decrypt) return std::unexpected("Unknown file operation.");
        fs::path output=input;
        if (operation==Operation::encrypt)
        {
            if (!output_directory.empty() && !valid_path(output_directory))
                return std::unexpected("Output directory contains an embedded NUL character.");
            const auto name=legacy_basename(input);
            if (!name) return std::unexpected(name.error());
            const auto checksum=crc32(0,reinterpret_cast<const Bytef*>(name->data()),static_cast<uInt>(name->size()));
            output=(output_directory.empty() ? fs::current_path() : output_directory)/std::to_string(checksum);
        }
        auto bytes=read_file(input,operation);
        if (!bytes) return std::unexpected(bytes.error());
        const auto input_length=static_cast<std::int32_t>(bytes->size());
        const auto capacity=operation==Operation::encrypt ? (static_cast<std::uint64_t>(input_length)+7)&~std::uint64_t{3} : static_cast<std::uint64_t>(input_length);
        std::vector<char> transformed(static_cast<std::size_t>(capacity));
        auto written=static_cast<std::int32_t>(capacity);
        const char empty{};
        const auto* data=bytes->empty() ? &empty : bytes->data();
        const auto success=operation==Operation::encrypt ? Encrypt(input_length,data,written,transformed.data()) : Decrypt(input_length,data,written,transformed.data());
        if (!success) return std::unexpected("Selected file does not match the legacy transform format.");
        transformed.resize(static_cast<std::size_t>(written));
        std::error_code error;
        const auto destination=fs::weakly_canonical(output,error);
        if (error) return std::unexpected(std::format("Cannot resolve the output filename (error {}).",error.value()));
        const auto result=publish(destination,transformed);
        if (!result) return std::unexpected(result.error());
        return output;
    }
    catch (const std::bad_alloc&) { return std::unexpected("Not enough memory to transform the selected file."); }
    catch (const fs::filesystem_error& error) { return std::unexpected(std::format("Filesystem operation failed (error {}).",error.code().value())); }
}
