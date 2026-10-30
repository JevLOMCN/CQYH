#include "gm_reports.h"
#include "error_def.h"
#include <algorithm>
#include <array>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <thread>

namespace
{
std::size_t checks{};
void check(bool value, const char* text) { ++checks; if (!value) throw std::runtime_error(text); }
std::string read(const std::filesystem::path& path)
{
    std::ifstream input(path,std::ios::binary);
    if (!input) throw std::runtime_error("cannot read fixture report");
    return {std::istreambuf_iterator<char>(input),{}};
}
ROLE_NAME_DATA name(const char* text, UINT32 id)
{
    ROLE_NAME_DATA value{};std::memcpy(value.szRoleName,text,std::strlen(text)+1);value.dwRoleID=id;return value;
}
}
int main(int argc, char** argv)
{
    try
    {
        check(argc==2,"fixture output directory argument");
        const auto root=std::filesystem::path(argv[1])/std::filesystem::path(u8"reports \u6D4B\u8BD5");
        std::filesystem::create_directories(root);
        const auto output=root/"nameTOid.txt";
        std::map<INT32,ROLE_NAME_DATA> names{{1,name("first",1)},{7,name("second",4294967295)},{2147483647,name("last",0)}};
        check(gm::write_name_report(output,names).has_value(),"write ordered full-width name rows");
        check(read(output)=="first  1\r\nsecond  4294967295\r\nlast  0\r\n","each row uses its own record and unsigned ID");
        const auto expected=read(output);
        auto invalid=names;std::fill_n(invalid.begin()->second.szRoleName,ROLE_NAME_LEN,'x');
        check(!gm::write_name_report(output,invalid),"unterminated name rejected before publication");
        check(read(output)==expected,"invalid report preserves prior file");
        check(!gm::write_name_report(root/"missing"/"result",names),"missing parent reported");
        const auto directory=root/"directory target";std::filesystem::create_directories(directory);
        check(!gm::write_name_report(directory,names),"rename-to-directory failure reported");
        check(std::filesystem::is_directory(directory),"failed publication preserves destination");
        check(gm::write_name_report(output,{{1,name("replacement",42)}}).has_value(),"existing report replaced");
        check(read(output)=="replacement  42\r\n","replacement is complete");
        std::list<ROLE_RESULT> roles;
        for (const auto& [index,value] : names)
        {
            ROLE_RESULT role{};std::memcpy(role.szRoleName,value.szRoleName,ROLE_NAME_LEN);role.dwRoleID=value.dwRoleID;
            role.nResult=index==1 ? errSuccess : errUnknownError;roles.push_back(role);
        }
        const auto source=root/"role list.txt";
        for (const auto [kind,suffix] : {std::pair{CMD_FORBIDDEN_CHAT,"_chat_result"},
             {CMD_FREEZE_ROLE,"_freeze_result"},{CMD_KICK_ROLE,"_kickrole_result"},{CMD_SEND_MAIL,"_sendmail_result"}})
        {
            check(gm::write_role_report(source,kind,roles).has_value(),"write role report");
            auto path=source;path+=suffix;
            const std::string bytes="\xEF\xBB\xBFRoleName\tRoleID\tResult \r\nfirst\t1\tsuccess \r\nsecond\t4294967295\tfailed \r\nlast\t0\tfailed \r\n";
            check(read(path)==bytes,"role report BOM, columns, suffix, result and CRLF bytes");
            auto bad=roles;std::fill_n(bad.front().szRoleName,ROLE_NAME_LEN,'x');
            check(!gm::write_role_report(source,kind,bad) && read(path)==bytes,"invalid role report preserves previous bytes");
        }
        check(!gm::write_role_report(source,-1,roles),"unsupported report type rejected");
        std::list<ROLE_RESULT> many(80000,roles.front());
        check(gm::write_role_report(source,CMD_KICK_ROLE,many).has_value(),"report exceeds old fixed buffer safely");
        auto large_path=source;large_path+="_kickrole_result";
        const auto large=read(large_path);
        check(large.size()>1024*1024 && std::ranges::count(large,'\n')==80001,"large report contains every row beyond old buffer capacity");
        constexpr std::u8string_view unicode=u8"\u8DEF\u5F84/\u89D2\u8272.txt";
        check(gm::utf8_path({reinterpret_cast<const char*>(unicode.data()),unicode.size()})==std::filesystem::path(unicode),"UTF-8 UI paths become native paths");
        std::array<bool,8> success{};
        std::array<std::string,8> errors{};
        {
            std::vector<std::jthread> writers;
            for (std::size_t index=0;index<success.size();++index)
                writers.emplace_back([&,index]
                {
                    const auto result=gm::write_name_report(output,names);
                    success[index]=result.has_value();
                    if (!result) errors[index]=result.error();
                });
        }
        for (const auto& error : errors) if (!error.empty()) std::cerr<<error<<'\n';
        check(std::ranges::all_of(success,[](bool value){return value;}),"concurrent staging names do not collide");
        check(read(output)==expected,"concurrent publication leaves a complete report");
        for (const auto& entry : std::filesystem::directory_iterator(root))
            check(entry.path().filename().string().find(".tmp.")==std::string::npos,"all staging files cleaned");
        std::cout<<checks<<" GM report checks passed\n";return 0;
    }
    catch(const std::exception& error) {std::cerr<<error.what()<<'\n';return 1;}
}
