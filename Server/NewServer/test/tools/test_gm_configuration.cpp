#include "gm_configuration.h"

#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace
{
std::size_t checks;
void check(bool condition, const char* message)
{
    ++checks;
    if (!condition)
        throw std::runtime_error(message);
}
const std::string prefix = R"(
function LoadFile()
 local c = GetTLConfig()
 c.ServerList.nSvrNum = 1
 local s = c.ServerList.arrGMList[0]
 s.nSvrID = 7
 s.szName = 'server one'
 s.szAddr = '127.0.0.1'
 s.dwPort = 9011
)";
std::string script(const std::string& body = {}) { return prefix + body + "\nend\n"; }

struct Directory
{
    std::filesystem::path path;
    explicit Directory(const std::filesystem::path& parent)
    {
        std::filesystem::create_directories(parent);
        path = parent / ("config-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
        if (!std::filesystem::create_directory(path))
            throw std::runtime_error("cannot create isolated fixture directory");
    }
    ~Directory()
    {
        std::error_code error;
        std::filesystem::remove_all(path, error);
    }
};
}

int main(int argc, char** argv)
try
{
    check(argc == 3, "fixture arguments");
    std::memset(&g_TLConfig, 0x5a, sizeof(g_TLConfig));
    auto original = std::make_unique<TL_CONFIG>(g_TLConfig);
    auto unchanged = [&] { check(std::memcmp(original.get(), &g_TLConfig, sizeof(g_TLConfig)) == 0, "global configuration isolation"); };
    std::cerr << "configuration: initial candidate\n";
    auto candidate = gm::parse_configuration(script("s.groupList[31].szGroupName='last group'"));
    check(candidate.has_value(), candidate ? "valid direct script" : candidate.error().c_str());
    unchanged();
    check((*candidate)->ServerList.nSvrNum == 1, "server count");
    const auto& server = (*candidate)->ServerList.arrGMList[0];
    check(server.nSvrID == 7 && server.dwPort == 9011, "numeric properties");
    check(std::string(server.szName) == "server one" && std::string(server.szAddr) == "127.0.0.1", "string properties");
    check(std::string(server.groupList[31].szGroupName) == "last group", "last zero-based group");
    check((*candidate)->ServerList.arrGMList[127].nSvrID == 0, "candidate zero initialization");
    auto second = gm::parse_configuration(script("s.nSvrID=9"));
    check(second.has_value() && (*second)->ServerList.arrGMList[0].nSvrID == 9 && server.nSvrID == 7, "independent owned candidates");

    const std::vector<std::string> invalid = {
        "s.szName=string.rep('x',128)", "s.szAddr=string.rep('x',128)",
        "s.groupList[0].szGroupName=string.rep('x',33)", "s.szName='x'..string.char(0)..'y'",
        "s.nSvrID=-1", "s.nSvrID=1.25", "s.nSvrID=2147483648", "s.nSvrID=0/0",
        "s.dwPort=0", "s.dwPort=65536", "s.dwPort=-1", "s.dwPort=1.5", "s.dwPort=math.huge",
        "c.ServerList.nSvrNum=-1", "c.ServerList.nSvrNum=129", "c.ServerList.nSvrNum=1.5",
        "s.szName=''", "s.szAddr=''", "s.szAddr='127.1'", "s.szAddr='1.2.3.4.5'",
        "s.szAddr='01.2.3.4'", "s.szAddr='256.2.3.4'", "s.szAddr='255.255.255.255'",
        "s.szAddr='+1.2.3.4'", "s.szAddr='1.2.3.-4'", "s.szAddr='1.2.3.4 '",
        "s.szAddr='localhost'", "s.szAddr=12", "s.nSvrID='wrong'", "s.nSvrID=nil",
        "c.ServerList.arrGMList[128].nSvrID=1", "c.ServerList.arrGMList[-1].nSvrID=1",
        "s.groupList[32].szGroupName='x'", "s.groupList[-1].szGroupName='x'",
        "s.groupList['wrong'].szGroupName='x'", "s.groupList[0.5].szGroupName='x'",
        "c.ServerList.arrGMList['wrong'].nSvrID=1", "c.ServerList.arrGMList[0.5].nSvrID=1",
        "s.groupList[1]='wrong'", "c.ServerList.arrGMList[1]='wrong'", "c.ServerList='wrong'",
        "local d=c.ServerList.arrGMList[1];d.nSvrID=7;d.szName='two';d.szAddr='127.0.0.2';d.dwPort=1",
        "error('intentional LoadFile failure after candidate writes')",
        "local p=newproxy(true);getmetatable(p).__gc=function() GetTLConfig().ServerList.arrGMList[0].dwPort=0 end;hold=p",
        "local p=newproxy(true);getmetatable(p).__gc=function() GetTLConfig().ServerList.arrGMList[0].szName=string.rep('x',128) end;hold=p"
    };
    for (const auto& body : invalid)
    {
        std::cerr << "configuration: invalid case " << (&body - invalid.data()) << '\n';
        auto result = gm::parse_configuration(script(body));
        check(!result && !result.error().empty(), body.c_str());
        unchanged();
    }
    std::cerr << "configuration: missing script/function cases\n";
    for (const auto& bad : {std::string{}, std::string("syntax ! error"), std::string("x=1"), std::string("function LoadFile() end")})
    {
        auto result = gm::parse_configuration(bad);
        check(!result, "reject absent or invalid configuration");
        unchanged();
    }
    for (const auto& address : {"0.0.0.0", "1.2.3.4", "255.255.255.254"})
        check(gm::parse_configuration(script("s.szAddr='" + std::string(address) + "'")).has_value(), "valid IPv4 address");
    auto boundaries = gm::parse_configuration(script("s.szName=string.rep('x',127);s.groupList[31].szGroupName=string.rep('x',32);s.dwPort=65535;s.nSvrID=2147483647;c.ServerList.nSvrNum=128"));
    check(boundaries.has_value(), "maximum scalar and string fields");
    unchanged();
    auto finalizer = gm::parse_configuration(script("local p=newproxy(true);getmetatable(p).__gc=function() GetTLConfig().ServerList.arrGMList[0].nSvrID=23 end;hold=p"));
    check(finalizer.has_value() && (*finalizer)->ServerList.arrGMList[0].nSvrID == 23, "finalize candidate before validating it");
    auto full = gm::parse_configuration(script("c.ServerList.nSvrNum=128;for i=0,127 do local d=c.ServerList.arrGMList[i];d.nSvrID=i+1;d.szName='server '..i;d.szAddr='127.0.0.1';d.dwPort=9000+i end"));
    check(full.has_value() && (*full)->ServerList.arrGMList[127].nSvrID == 128, "all server array slots");
    auto copies = gm::parse_configuration(script("s.groupList[31].szGroupName='copied';s.groupList[1]=s.groupList[31];c.ServerList.arrGMList[1]=s;c.ServerList.arrGMList[1].nSvrID=8;c.ServerList=c.ServerList"));
    check(copies.has_value() && std::string((*copies)->ServerList.arrGMList[1].groupList[1].szGroupName) == "copied", "typed structure assignments");
    check(gm::parse_configuration("\xEF\xBB\xBF" + script()).has_value(), "UTF-8 BOM buffer");
    check(!gm::parse_configuration("\xEF\xBB\xBF"), "empty BOM script");
    unchanged();

    Directory fixture{std::filesystem::path(argv[1])};
    const auto file = fixture.path / L"配置 script.lua";
    { std::ofstream output(file, std::ios::binary); output << script(); check(static_cast<bool>(output), "write fixture"); }
    check(gm::load_configuration(file).has_value(), "native Unicode filename");
    { std::ofstream output(file, std::ios::binary); output << "\xEF\xBB\xBF" << script(); }
    check(gm::load_configuration(file).has_value(), "UTF-8 BOM file");
    check(!gm::load_configuration(fixture.path / "missing.lua"), "missing file");
    check(!gm::reload_configuration(fixture.path / "missing.lua", g_TLConfig), "failed reload");
    unchanged();
    check(!gm::load_configuration(fixture.path), "directory cannot be read as configuration");
    { std::ofstream output(file, std::ios::binary); output << std::string(16 * 1024 * 1024 + 1, ' '); check(static_cast<bool>(output), "write oversized fixture"); }
    check(!gm::load_configuration(file), "reject oversized file");
    check(!gm::reload_configuration(file, g_TLConfig), "oversized reload");
    check(!gm::parse_configuration(std::string(16 * 1024 * 1024 + 1, ' ')), "reject oversized buffer");
    unchanged();

    // Exercise the maintained table traversal and LoadFile implementation.
    auto shipped = gm::load_configuration(std::filesystem::path(argv[2]));
    check(shipped.has_value(), "shipped ServerList configuration");
    check((*shipped)->ServerList.nSvrNum == 2 && (*shipped)->ServerList.arrGMList[0].nSvrID == 1 &&
          (*shipped)->ServerList.arrGMList[1].nSvrID == 2, "shipped server values");
    unchanged();
    check(gm::reload_configuration(std::filesystem::path(argv[2]), g_TLConfig).has_value(), "publish valid reload");
    check(GetServer(1) && GetServer(2) && !GetServer(100), "published shared lookup");
    std::cout << checks << " GM configuration checks passed\n";
    return 0;
}
catch (const std::exception& error)
{
    std::cerr << error.what() << '\n';
    return 1;
}
