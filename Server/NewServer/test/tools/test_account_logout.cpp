#include "account_logout.h"
#include <array>
#include <chrono>
#include <cstdio>
#include <format>
#include <fstream>
#include <stdexcept>
#include <string_view>

namespace
{
namespace fs=std::filesystem;
int checks=0,failures=0;
void check(bool value,const char* message) { ++checks; if (!value) { ++failures; std::fprintf(stderr,"FAIL: %s\n",message); } }
struct Fixture
{
    fs::path base,root;
    explicit Fixture(const fs::path& directory)
    {
        fs::create_directories(directory); base=fs::canonical(directory);
        root=base/("account-analysis-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        if (!fs::create_directory(root)) throw std::runtime_error("fixture root must be new");
        root/=u8"测试";
        fs::create_directory(root);
    }
    ~Fixture()
    {
        std::error_code error;
        const auto owned=root.parent_path();
        if (owned.parent_path()==base && owned.filename().string().starts_with("account-analysis-") && fs::canonical(owned,error)==owned && !error)
        {
            for (const auto& entry:fs::recursive_directory_iterator(owned,error))
                if (entry.is_regular_file(error)) fs::permissions(entry.path(),fs::perms::owner_write,fs::perm_options::add,error);
            fs::remove_all(owned,error);
        }
    }
};
void write(const fs::path& file,std::string_view text)
{
    fs::create_directories(file.parent_path()); std::ofstream stream{file,std::ios::binary};
    stream.write(text.data(),static_cast<std::streamsize>(text.size())); stream.close();
    if (!stream) throw std::runtime_error("fixture write failed");
}
void inputs(const fs::path& root,std::string_view sessions="a\nb\n",std::string_view users="Alpha\nBeta\n",
    std::string_view logouts="a\n",std::string_view logins="Alpha\nBeta\nMissing\n")
{
    write(root/"db_file/session.txt",sessions); write(root/"db_file/user.txt",users);
    write(root/"db_file/logout.txt",logouts); write(root/"dl_file/AccountLogin.txt",logins);
}
const auto expected=std::string{
    "Session : a, UserName : Alpha Found!\n"
    "SessionID : b in , UserName : Beta in dl file, not found in db file?\n"
    "Can not find UserName : Missing in db login file?\n"
    "SessionID : 0 in , UserName : Missing in dl file, not found in db file?\n"};
void tests(const fs::path& root)
{
    inputs(root); auto result=account_logout::analyze(root);
    check(result && result->text==expected && result->sessions==2 && result->users==2 && result->logouts==1 && result->logins==3,
        "legacy report messages, order and counts preserved");
    for (int repeat=0;repeat<5;++repeat) check(account_logout::analyze(root)->text==expected,"analysis is repeatable without retaining list state");
    for (const auto* name:{"db_file/session.txt","db_file/user.txt","db_file/logout.txt","dl_file/AccountLogin.txt"})
        fs::permissions(root/name,fs::perms::owner_read|fs::perms::group_read|fs::perms::others_read,fs::perm_options::replace);
    result=account_logout::analyze(root); check(result && result->text==expected,"all inputs open read-only on Unicode paths");
    for (const auto* name:{"db_file/session.txt","db_file/user.txt","db_file/logout.txt","dl_file/AccountLogin.txt"})
        fs::permissions(root/name,fs::perms::owner_write,fs::perm_options::add);
    inputs(root,"a\r\nb","Alpha\r\nBeta","a","Alpha\r\nBeta\r\nMissing");
    result=account_logout::analyze(root); check(result && result->text==expected,"CRLF and missing final newline retain report bytes");
    inputs(root,"a\nb\n","Duplicate\nDuplicate\n","a\na\n","Duplicate\n");
    result=account_logout::analyze(root); check(result && result->text=="Session : a, UserName : Duplicate Found!\n" && result->logouts==2,
        "first duplicate username wins and repeated logout IDs remain harmless");
    inputs(root,"0\n","Zero\n","0\n","Zero\nMissing\n");
    result=account_logout::analyze(root); check(result && result->text==
        "Can not find UserName : Zero in db login file?\nSession : 0, UserName : Zero Found!\n"
        "Can not find UserName : Missing in db login file?\nSession : 0, UserName : Missing Found!\n",
        "legacy zero-ID sentinel reporting is retained");
    inputs(root,"FFFFFFFF\n80000000\n","Maximum\nHighBit\n","FFFFFFFF\n80000000\n","Maximum\nHighBit\n");
    result=account_logout::analyze(root); check(result && result->text=="Session : ffffffff, UserName : Maximum Found!\nSession : 80000000, UserName : HighBit Found!\n",
        "session IDs retain all 32 bits rather than saturating signed long");
    inputs(root,"\xef\xbb\xbf +0xA metadata\n0Xb\tmetadata\n","\xef\xbb\xbf Alpha extra\nBeta\tmore\n","+0xa\n","\xef\xbb\xbf Alpha meta\nBeta\nMissing\n");
    result=account_logout::analyze(root); check(result && result->text==expected,"BOM, whitespace, positive prefixes and first-token exports are handled");
    const auto gbk=std::string{"\xb2\xe2\xca\xd4"};
    inputs(root,"a\n",gbk+"\n","a\n",gbk+"\n"); result=account_logout::analyze(root);
    check(result && result->text=="Session : a, UserName : "+gbk+" Found!\n","GBK username bytes remain unchanged");
    inputs(root,"","","",""); result=account_logout::analyze(root);
    check(result && result->text.empty() && result->sessions==0,"empty exports are a valid empty report");
    for (const std::string value:{"","0x","+","-1","1g","100000000","FFFFFFFFF","ZZZ"})
    {
        inputs(root,value+"\n","Alpha\n","a\n","Alpha\n"); result=account_logout::analyze(root);
        check(!result && result.error().find("session.txt:1")!=std::string::npos,"malformed or overflowing hex values identify their file and row");
    }
    inputs(root,"a\nb\n","Alpha\n","a\n","Alpha\n"); result=account_logout::analyze(root);
    check(!result && result.error().find("row count mismatch")!=std::string::npos,"mismatched DB exports fail before a report is published");
    inputs(root,"a\n","Alpha\n","invalid\n","Alpha\n"); result=account_logout::analyze(root);
    check(!result && result.error().find("logout.txt:1")!=std::string::npos,"logout ID parsing is also checked");
    for (std::size_t size:{std::size_t{0},std::size_t{63},std::size_t{64},std::size_t{2048}})
    {
        const auto name=std::string(size,'x'); inputs(root,"a\n",name+"\n","a\n",name+"\n");
        result=account_logout::analyze(root);
        check(result.has_value()==(size==63),"usernames never silently truncate or split into extra rows");
    }
    for (const auto* name:{"db_file/session.txt","db_file/user.txt","db_file/logout.txt","dl_file/AccountLogin.txt"})
    {
        inputs(root); fs::remove(root/name); result=account_logout::analyze(root);
        check(!result && result.error().find(fs::path{name}.filename().string())!=std::string::npos,"missing file reports its path");
        fs::create_directory(root/name); result=account_logout::analyze(root);
        check(!result,"directories cannot be read as data files"); fs::remove(root/name);
        inputs(root); write(root/name,std::string_view{"a\0b\n",4}); result=account_logout::analyze(root);
        check(!result && result.error().find("embedded zero byte")!=std::string::npos,"embedded zeros are rejected rather than hiding bytes");
        const auto moved=fs::path{root/name}.concat(".moved"); fs::rename(root/name,moved); fs::rename(moved,root/name);
        check(fs::exists(root/name),"failed analysis closes input handles immediately");
    }
    std::string ids,names;
    for (unsigned i=1;i<=70000;++i) { ids+=std::format("{:x}\n",i); names+=std::format("User{}\n",i); }
    inputs(root,ids,names,"1\n11170\n","User1\nUser70000\n"); result=account_logout::analyze(root);
    check(result && result->sessions==70000 && result->text=="Session : 1, UserName : User1 Found!\nSession : 11170, UserName : User70000 Found!\n",
        "standard indexes scale beyond the legacy 65535-record pools");
}
}

int main(int argc,char** argv)
{
    try { if (argc!=2) throw std::runtime_error("fixture directory required"); Fixture fixture{argv[1]}; tests(fixture.root); }
    catch (const std::exception& error) { check(false,error.what()); }
    std::printf("account logout: %d checks, %d failures\n",checks,failures); return failures!=0;
}
