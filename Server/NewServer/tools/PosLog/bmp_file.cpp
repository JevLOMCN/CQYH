#include "bmp_file.h"
#include <atomic>
#include <chrono>
#include <format>
#include <fstream>
#ifdef _WIN32
#include <Windows.h>
#endif

namespace
{
void put(std::vector<char>& bytes,std::size_t offset,std::uint32_t value,unsigned count)
{
    for(unsigned index=0;index<count;++index)bytes[offset+index]=static_cast<char>((value>>(index*8))&255);
}
}
std::expected<std::vector<char>,std::string> poslog::make_bitmap(std::span<const Position> positions,std::uint32_t side)
{
    if(side!=512 && side!=1024) return std::unexpected("Choose a 512 or 1024 pixel bitmap size.");
    try
    {
        const auto image_size=side*side*3;
        std::vector<char> bytes(54+image_size,0);
        bytes[0]='B';bytes[1]='M';put(bytes,2,static_cast<std::uint32_t>(bytes.size()),4);put(bytes,10,54,4);
        put(bytes,14,40,4);put(bytes,18,side,4);put(bytes,22,side,4);put(bytes,26,1,2);put(bytes,28,24,2);put(bytes,34,image_size,4);
        for(const auto point:positions)
            if(const auto location=pixel(point,side,side))
                bytes[54+(static_cast<std::size_t>(location->row)*side+location->column)*3+2]=static_cast<char>(255);
        return bytes;
    }
    catch(const std::bad_alloc&) {return std::unexpected("Not enough memory to create the bitmap.");}
}
std::expected<void,std::string> poslog::write_bitmap(const std::filesystem::path& filename,
    std::span<const Position> positions,std::uint32_t side)
{
    namespace fs=std::filesystem;
    if(filename.empty() || filename.native().find(fs::path::value_type{})!=filename.native().npos)
        return std::unexpected("Choose a filename without embedded NUL characters.");
    const auto bytes=make_bitmap(positions,side);if(!bytes)return std::unexpected(bytes.error());
    try
    {
        std::error_code error;const auto destination=fs::weakly_canonical(filename,error);
        if(error)return std::unexpected("Cannot resolve the bitmap filename.");
        static std::atomic<std::uint64_t> serial{};
        const auto tick=std::chrono::steady_clock::now().time_since_epoch().count();
        fs::path stage;std::ofstream stream;
        for(unsigned attempt=0;attempt<16;++attempt)
        {
            stage=destination.parent_path()/std::format(".poslog-bmp-{}-{}.tmp",tick,serial.fetch_add(1,std::memory_order_relaxed));
            stream.open(stage,std::ios::binary|std::ios::out|std::ios::noreplace);
            if(stream)break;
            if(!fs::exists(stage,error))return std::unexpected("Cannot create the temporary bitmap file.");
            stream.clear();
        }
        if(!stream)return std::unexpected("Cannot reserve the temporary bitmap file.");
        struct Cleanup
        {
            fs::path path;std::ofstream& stream;
            ~Cleanup(){if(stream.is_open())stream.close();std::error_code error;fs::remove(path,error);}
        } cleanup{std::move(stage),stream};
        stream.write(bytes->data(),static_cast<std::streamsize>(bytes->size()));stream.flush();
        if(!stream)return std::unexpected("Cannot write the complete bitmap file.");
        stream.close();if(!stream)return std::unexpected("Cannot close the bitmap file.");
#ifdef _WIN32
        if(!MoveFileExW(cleanup.path.c_str(),destination.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH))
            return std::unexpected(std::format("Cannot replace the bitmap file (Windows error {}).",GetLastError()));
#else
        fs::rename(cleanup.path,destination,error);
        if(error)return std::unexpected(std::format("Cannot replace the bitmap file (error {}).",error.value()));
#endif
        return {};
    }
    catch(const std::bad_alloc&){return std::unexpected("Not enough memory to save the bitmap.");}
    catch(const fs::filesystem_error&){return std::unexpected("Cannot access the bitmap output directory.");}
}
