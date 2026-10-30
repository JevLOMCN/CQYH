#include "gm_role_files.h"
#include "error_def.h"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace
{
std::size_t checks{};
void check(bool value, const char* message) { ++checks; if (!value) throw std::runtime_error(message); }
void write(const std::filesystem::path& path, std::string_view bytes)
{
    std::ofstream output(path,std::ios::binary);output.write(bytes.data(),bytes.size());output.close();
    check(static_cast<bool>(output),"fixture file written");
}
void names()
{
    for (const std::string bom : {std::string{},std::string("\xEF\xBB\xBF")})
        for (const std::string newline : {std::string("\n"),std::string("\r\n"),std::string("\r")})
        {
            const auto result=gm::parse_role_names(bom+"first"+newline+newline+"second"+newline+"last");
            check(result && *result==std::vector<std::string>{"first","second","last"},"optional BOM, whole lines, blanks and final unterminated line");
        }
    for (const auto text : {"", "\n\r\n", "\xEF\xBB\xBF"}) check(!gm::parse_role_names(text),"empty name input rejected");
    check(!gm::parse_role_names(std::string("first\ninvalid\0name",18)),"embedded NUL rejected");
    for (std::size_t size=1;size<=ROLE_NAME_LEN+4;++size)
    {
        const auto result=gm::parse_role_names(std::string(size,'x'));
        check(result.has_value()==(size<ROLE_NAME_LEN),"role-name byte capacity");
        if (result) check(result->front()==std::string(size,'x'),"maximum names are never truncated");
    }
    const auto bytes=std::string("\xE4\xB8\xAD\xE6\x96\x87");
    check(gm::parse_role_names(bytes)->front()==bytes,"UTF-8 content bytes preserved");
    check(gm::parse_role_names(" name with spaces ")->front()==" name with spaces ","non-newline name bytes preserved");
    check(gm::parse_role_names("abc")->front()=="abc","BOM-free first three bytes retained");
    std::string large;
    for (INT32 index=0;index<4000;++index) large+="role\n";
    check(gm::parse_role_names(large)->size()==4000,"all lines retained without fixed line buffer");
}
void ids()
{
    const auto result=gm::parse_role_ids("\xEF\xBB\xBF 0 1\t2147483648\r\n4294967295\v\f00042 ");
    check(result && *result==std::list<UINT32>{0,1,2147483648u,4294967295u,42},"unsigned IDs and ASCII whitespace");
    for (const auto text : {"", " \r\n\t", "-1", "+1", "4294967296", "999999999999999999999999", "1,2", "1.0", "0x10", "1x", "1 -2"})
        check(!gm::parse_role_ids(text),"invalid number rejected without partial results");
    check(!gm::parse_role_ids(std::string("1\0 2",5)),"embedded NUL cannot truncate ID input");
    for (const UINT32 value : {UINT32(0x7FFFFFFF),UINT32(0x80000000),UINT32(0xFFFFFFFE),UINT32(0xFFFFFFFF)})
        check(gm::parse_role_ids(std::to_string(value))->front()==value,"boundary IDs preserve every bit");
    check(*gm::parse_role_ids("42 42")==std::list<UINT32>{42,42},"duplicate IDs retain input order");
}
void files(const std::filesystem::path& root)
{
    std::filesystem::create_directories(root);
    const auto path=root/std::filesystem::path(u8"\u89D2\u8272\u540D\u5355.txt");
    write(path,"\xEF\xBB\xBF" "first\r\nsecond\nlast");
    const auto roles=gm::read_role_batch(path);
    check(roles && roles->size()==3,"native Unicode role input path");
    auto role=roles->begin();
    for (const auto name : {"first","second","last"})
    {
        check(std::string(role->szRoleName)==name && role->dwRoleID==0 && role->nResult==errUnknownError,"owned initialized role batch record");++role;
    }
    const auto queries=gm::read_name_queries(path);
    check(queries && queries->size()==3 && std::string(queries->at(1).szRoleName)=="first" &&
        std::string(queries->at(3).szRoleName)=="last" && queries->at(2).dwRoleID==0,"query map has contiguous one-based indices and clean names");
    write(path,"4294967295\r\n42\t0");
    check(*gm::read_role_ids(path)==std::list<UINT32>{4294967295u,42,0},"whole-file ID parsing");
    write(path,"valid\n"+std::string(ROLE_NAME_LEN,'x'));
    check(!gm::read_role_batch(path) && !gm::read_name_queries(path),"invalid trailing row prevents publishing a partial file result");
    write(path,"1 2 invalid");
    check(!gm::read_role_ids(path),"invalid trailing ID prevents partial results");
    write(path,"");
    check(!gm::read_role_batch(path) && !gm::read_role_ids(path),"empty files rejected");
    check(!gm::read_role_batch(root/"missing.txt") && !gm::read_role_ids(root/"missing.txt"),"missing files reported");
    check(!gm::read_role_batch(root),"directory cannot be treated as role data");
    {
        std::ofstream output(path,std::ios::binary);
        output<<"role\n";
        const std::string block(64*1024,'\n');
        std::size_t remaining=gm::max_role_file_size-5;
        while (remaining)
        {
            const auto count=std::min(remaining,block.size());output.write(block.data(),count);remaining-=count;
        }
        output.close();check(static_cast<bool>(output),"boundary input file written");
    }
    check(gm::read_role_batch(path)->size()==1,"exact 10-MiB input limit accepted without truncation");
    {std::ofstream output(path,std::ios::binary|std::ios::app);output.put('\n');}
    check(!gm::read_role_batch(path) && !gm::read_name_queries(path) && !gm::read_role_ids(path),"oversized files explicitly rejected");
    write(path,"after failure");
    check(gm::read_role_batch(path)->front().szRoleName==std::string("after failure"),"streams close on failures and file can be replaced");
}
}
int main(int argc,char** argv)
{
    try
    {
        check(argc==2,"fixture output directory argument");names();ids();files(argv[1]);
        std::cout<<checks<<" GM role-file checks passed\n";return 0;
    }
    catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
