#include "maintain_files.h"
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <stdexcept>

namespace
{
namespace fs=std::filesystem;
int checks=0,failures=0;
void check(bool value,const char* name) { ++checks;if (!value) { ++failures;std::fprintf(stderr,"FAIL: %s\n",name); } }
struct Fixture
{
    fs::path base,root;
    explicit Fixture(const fs::path& parent) {
        fs::create_directories(parent);base=fs::canonical(parent);
        root=base/("maintain-files-"+std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        if (!fs::create_directory(root)) throw std::runtime_error("fixture must be new");
    }
    ~Fixture() {
        std::error_code error;
        if (root.parent_path()==base && root.filename().string().starts_with("maintain-files-") &&
            fs::canonical(root,error)==root && !error) fs::remove_all(root,error);
    }
};
void run(const fs::path& parent)
{
    Fixture fixture{parent};
    const auto root=fixture.root/fs::path{L"\u76ee\u5f55"};
    fs::create_directory(root);
    const maintain::ScanLimits limits{4,4,64};
    check(maintain::read_areas(root,limits)->empty(),"empty directory produces an empty complete configuration");
    fs::create_directories(root/"3_Third/192.168.1.1");
    fs::create_directories(root/"01_Name with spaces_and_underscores/127.0.0.2");
    fs::create_directories(root/"01_Name with spaces_and_underscores/127.0.0.1");
    fs::create_directories(root/"01_Name with spaces_and_underscores/127.0.0.3");
    std::ofstream{root/"ignored.lua"}<<"file";std::ofstream{root/"3_Third/ignored.lua"}<<"file";
    const auto valid=maintain::read_areas(root,limits);
    check(valid && valid->size()==2 && valid->front().id==1 && valid->back().id==3,"areas sort by ID and preserve accepted leading-zero IDs");
    if (!valid) throw std::runtime_error(valid.error());
    check(valid->front().name=="Name with spaces_and_underscores","full area display name preserves spaces and underscores");
    check(valid->front().guards.size()==3 && valid->front().guards.front().address=="127.0.0.1" &&
        valid->front().guards.front().ip==*maintain::parse_address("127.0.0.1"),"all usable guard slots are sorted deterministically with network-order IDs");
    for (const auto name: {"-1_Bad","0_Bad","4_Bad","2147483648_Bad","2x_Bad","2_","no_id","1_Duplicate"})
    {
        fs::create_directory(root/name);
        const auto result=maintain::read_areas(root,limits);
        check(!result && !result.error().empty(),"invalid, oversized or duplicate area prevents a partial scan result");
        fs::remove(root/name);
    }
    const auto tooLong="2_"+std::string(63,'x');fs::create_directory(root/tooLong);
    check(!maintain::read_areas(root,limits),"overlong persistent folder names fail before truncation");
    fs::remove(root/tooLong);
    const auto guards=root/"3_Third";
    for (const auto name: {"256.1.1.1","1.2.3","1.2.3.4suffix","01.2.3.4","0.0.0.0","255.255.255.255","1.2.3.-1","9999999999.2.3.4"})
    {
        fs::create_directory(guards/name);
        check(!maintain::read_areas(root,limits),"invalid endpoint prevents a partial scan result");
        fs::remove(guards/name);
    }
    fs::create_directory(root/"01_Name with spaces_and_underscores/127.0.0.4");
    check(!maintain::read_areas(root,limits),"excess guards are diagnosed rather than silently omitted");
    fs::remove(root/"01_Name with spaces_and_underscores/127.0.0.4");
    check(maintain::read_areas(root,limits).has_value(),"failed scans do not mutate the source directories or later results");
    check(!maintain::read_areas(root/"missing",limits) && !maintain::read_areas(root/"ignored.lua",limits),
        "missing and non-directory roots report failures");
    check(!maintain::read_areas(root,{1,4,32}) && !maintain::read_areas(root,{4,1,32}) && !maintain::read_areas(root,{4,4,1}),
        "scan limits reject invalid capacities");
    const auto packages=fixture.root/"packages";
    check(maintain::package_files(packages,".zip")->empty(),"missing package directory is created with standard filesystem operations");
    for (const auto name: {"release.zip","release.zip.backup","patch.pak","UPPER.ZIP","plain"})
        std::ofstream{packages/name}<<"file";
    fs::create_directory(packages/"directory.zip");
    const auto unicode=fs::path{L"\u96ea.zip"};std::ofstream{packages/unicode}<<"unicode";
    auto files=maintain::package_files(packages,".zip");
    check(files && files->size()==3 && std::ranges::find(*files,unicode)!=files->end(),
        "package discovery keeps legacy substring/case behavior and supports Unicode filenames");
    check(maintain::package_files(packages,".pak")->size()==1,"patch discovery uses its own suffix filter");
    check(!maintain::package_files(packages/"plain",".zip") && !maintain::package_files({},".zip"),
        "invalid package roots report errors without manufacturing a list");
    const auto read=[](const fs::path& file) {
        std::ifstream input{file,std::ios::binary};
        if (!input) throw std::runtime_error("cannot read fixture export");
        return std::string{std::istreambuf_iterator<char>{input},{}};
    };
    const auto destination=fixture.root/fs::path{L"\u5bfc\u51fa"}/"result.tsv";
    const std::string binary{"A\0B\n",4};
    check(maintain::write_atomic(destination,binary).has_value() && read(destination)==binary,
        "atomic export creates native Unicode parents and preserves arbitrary bytes");
    check(maintain::write_atomic(destination,"replacement").has_value() && read(destination)=="replacement",
        "atomic publication replaces a completed existing file");
    check(maintain::write_atomic(destination,{}).has_value() && read(destination).empty(),"empty atomic exports remain valid files");
    check(!maintain::write_atomic(destination.parent_path(),"bad") && fs::is_directory(destination.parent_path()),
        "failed publication cannot replace a directory");
    if (!maintain::write_atomic(destination,"retained")) throw std::runtime_error("cannot prepare retained export");
    fs::permissions(destination,fs::perms::owner_read,fs::perm_options::replace);
    const auto readonly=maintain::write_atomic(destination,"replacement");
    fs::permissions(destination,fs::perms::owner_all,fs::perm_options::replace);
    check(!readonly && read(destination)=="retained","publication failure retains the previous file");
    const auto source=fixture.root/"security.lua";
    std::ofstream{source,std::ios::binary}<<"name = \"unchanged\"\n szPwd = \"123\"\n";
    check(maintain::generate_security_file(source,destination).has_value() &&
        read(destination)=="name = \"unchanged\"\r\n szPwd = \"\x60" "ab\"\r\n",
        "legacy printable transform and Windows text line endings are preserved");
    const auto prior=read(destination);
    for (const auto text: {"szPwd = value","szPwd = \"missing","szPwd = \"secret\t\"","no assignments"})
    {
        { std::ofstream stream{source,std::ios::binary};stream<<text; }
        const auto failed=maintain::generate_security_file(source,destination);
        check(!failed && read(destination)==prior && failed.error().find("secret")==std::string::npos,
            "malformed security source preserves destination and reports no password text");
    }
    check(!maintain::transform_security(std::string{"szPwd=\"123\"\0",12}),"embedded NUL is diagnosed before publication");
    const auto large=std::string(4000,' ')+"szPwd = \"123\"\n";
    const auto transformed=maintain::transform_security(large);
    check(transformed && transformed->size()==large.size()+1 && transformed->find("\x60" "ab")!=std::string::npos,
        "long security lines are handled without the old 1024-byte buffer");
    check(maintain::transform_security("szPwd=\"\"")->compare("szPwd=\"\"")==0,"empty quoted values retain legacy behavior");
    check(!maintain::generate_security_file(fixture.root/"missing-security",destination) && read(destination)==prior,
        "missing source cannot truncate a prior security export");
    for (const auto& entry:fs::directory_iterator(destination.parent_path()))
        check(!entry.path().filename().string().starts_with(".maintain-write-"),"staging directories are cleaned after success and failure");
}
}
int main(int argc,char** argv)
{
    try { if (argc!=2) throw std::runtime_error("fixture root required");run(argv[1]); }
    catch (const std::exception& error) { check(false,error.what()); }
    std::printf("maintain files: %d checks, %d failures\n",checks,failures);return failures!=0;
}
