#include "file_transform.h"
#include <array>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>
#ifdef _WIN32
#include <Windows.h>
#endif

namespace
{
namespace fs=std::filesystem;
unsigned checks;
void check(bool condition,const char* message){++checks;if(!condition)throw std::runtime_error(message);}
void write(const fs::path& path,const std::vector<char>& bytes)
{
    std::ofstream stream(path,std::ios::binary|std::ios::trunc);
    if (!bytes.empty()) stream.write(bytes.data(),static_cast<std::streamsize>(bytes.size()));
    if (!stream) throw std::runtime_error("Cannot write fixture file");
}
std::vector<char> read(const fs::path& path)
{
    std::ifstream stream(path,std::ios::binary);
    if (!stream) throw std::runtime_error("Cannot read fixture file");
    return {std::istreambuf_iterator<char>(stream),std::istreambuf_iterator<char>()};
}
void no_stages(const fs::path& directory)
{
    for (const auto& entry:fs::recursive_directory_iterator(directory))
        check(!entry.path().filename().native().starts_with(fs::path(".cqyh-transform-").native()),"temporary transform files are cleaned up");
}
}

int main(int argc,char** argv)
{
    try
    {
        check(argc==2,"expected private fixture directory");
        const auto directory=fs::absolute(argv[1]);fs::create_directories(directory);
        const auto unicode=directory/fs::path(u8"\u8DEF\u5F84");fs::create_directories(unicode);
        const auto input=unicode/"123456789.part.lua";
        const auto encoded=directory/"3421780262"; // Published CRC32 check vector, first-dot basename rule.
        for (const auto length:std::array<std::size_t,11>{0,1,3,4,5,7,8,1048575,1048576,1048577,1048611})
        {
            std::vector<char> original(length);
            for (std::size_t index=0;index<length;++index) original[index]=static_cast<char>((index*131+length)&255);
            if (length==4) original={'a','b','c','d'};
            write(input,original);write(encoded,{'o','l','d'});
            const auto result=gm_file::transform_file(input,gm_file::Operation::encrypt,directory);
            check(result && *result==encoded,"native Unicode directory and legacy first-dot CRC filename retained");
            check(read(input)==original,"encryption leaves the selected source unchanged");
            const auto bytes=read(encoded);check(bytes.size()==((length+7)&~std::size_t{3}),"on-disk encoded size retains length header and legacy padding");
            if (length==4)
                check(bytes==std::vector<char>({0x0d,0x2e,0x02,0x00,0x6c,0x4c,0x61,0x64}),"file workflow publishes the legacy abcd golden bytes");
            const auto decoded=gm_file::transform_file(encoded,gm_file::Operation::decrypt);
            check(decoded && *decoded==encoded && read(encoded)==original,"decryption replaces the selected file with the exact original bytes");
        }
        for (const auto length:std::array<std::size_t,5>{0,1,3,5,8})
        {
            const std::vector<char> invalid(length,0);write(input,invalid);
            check(!gm_file::transform_file(input,gm_file::Operation::decrypt) && read(input)==invalid,"malformed input is rejected before replacing the original");
        }
        write(input,{'a','b','c','d'});write(encoded,{'p','r','i','o','r'});
        check(!gm_file::transform_file(directory/"missing",gm_file::Operation::encrypt,directory) && read(encoded)==std::vector<char>({'p','r','i','o','r'}),"missing source cannot replace prior output");
        check(!gm_file::transform_file(unicode,gm_file::Operation::decrypt),"directory input is rejected");
        auto nul=input.native();nul.push_back(fs::path::value_type{});nul+=fs::path("ignored").native();
        check(!gm_file::transform_file(fs::path(nul),gm_file::Operation::encrypt,directory),"embedded-NUL input path is rejected without truncation");
        check(!gm_file::transform_file(input,static_cast<gm_file::Operation>(99),directory),"invalid operation is rejected");
        const auto failure_directory=directory/"output-is-a-file";write(failure_directory,{'s','a','f','e'});
        check(!gm_file::transform_file(input,gm_file::Operation::encrypt,failure_directory) && read(failure_directory)==std::vector<char>({'s','a','f','e'}),"invalid output parent remains unchanged");
        const auto blocked_directory=directory/"blocked";fs::create_directories(blocked_directory/"3421780262");
        check(!gm_file::transform_file(input,gm_file::Operation::encrypt,blocked_directory) && fs::is_directory(blocked_directory/"3421780262"),"publication failure preserves an existing destination directory");
#ifdef _WIN32
        {
            const auto handle=CreateFileW(encoded.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
            check(handle!=INVALID_HANDLE_VALUE,"locked output fixture opens");
            struct Close{HANDLE handle;~Close(){CloseHandle(handle);}} close{handle};
            check(!gm_file::transform_file(input,gm_file::Operation::encrypt,directory),"sharing violation reports publication failure");
        }
        check(read(encoded)==std::vector<char>({'p','r','i','o','r'}),"locked destination preserves previous bytes");
        check(gm_file::transform_file(input,gm_file::Operation::encrypt,directory).has_value(),"valid encoded input prepared for locked decryption");
        const auto before=read(encoded);
        {
            const auto handle=CreateFileW(encoded.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
            check(handle!=INVALID_HANDLE_VALUE,"locked decrypt fixture opens");
            struct Close{HANDLE handle;~Close(){CloseHandle(handle);}} close{handle};
            check(!gm_file::transform_file(encoded,gm_file::Operation::decrypt),"decryption publication failure is reported");
        }
        check(read(encoded)==before,"failed in-place decryption preserves the complete encrypted input");
#endif
        no_stages(directory);
        std::cout<<checks<<" Encrypt file-workflow checks passed\n";return 0;
    }
    catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
