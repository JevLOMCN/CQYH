#include "stdafx.h"
#include "encrypt.h"
#include "encryptDlg.h"
#include <array>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <vector>

namespace
{
CWinApp fixture_application;
unsigned checks;
void check(bool condition,const char* message){++checks;if(!condition)throw std::runtime_error(message);}
class Dialog final:public CencryptDlg
{
public:
    std::vector<std::pair<bool,std::wstring>> results;
    std::function<void()> report_hook;
protected:
    void ShowOperationResult(bool success,const CString& message) override
    {
        results.emplace_back(success,std::wstring(message.GetString(),static_cast<std::size_t>(message.GetLength())));
        const auto hook=report_hook;if(hook)hook();
    }
};
std::vector<char> read(const std::filesystem::path& path)
{
    std::ifstream stream(path,std::ios::binary);check(stream.good(),"native dialog output exists");
    return {std::istreambuf_iterator<char>(stream),std::istreambuf_iterator<char>()};
}
void click(Dialog& dialog,UINT id)
{
    dialog.SendMessage(WM_COMMAND,MAKEWPARAM(id,BN_CLICKED),reinterpret_cast<LPARAM>(dialog.GetDlgItem(id)->GetSafeHwnd()));
}
}

int main(int argc,char** argv)
{
    try
    {
        check(argc==2,"expected native dialog private directory");
        const auto instance=GetModuleHandleW(nullptr);
        check(AfxWinInit(instance,nullptr,GetCommandLineW(),SW_HIDE)!=FALSE,"MFC fixture initializes");
        fixture_application.InitInstance();
        CShellManager shell_manager;
        CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
        const auto previous=std::filesystem::current_path();
        const auto directory=std::filesystem::absolute(argv[1]);std::filesystem::create_directories(directory);
        struct Restore{std::filesystem::path previous;~Restore(){std::error_code error;std::filesystem::current_path(previous,error);}} restore{previous};
        std::filesystem::current_path(directory);
        const auto input=directory/std::filesystem::path(u8"\u8DEF\u5F84")/"123456789.lua";
        std::filesystem::create_directories(input.parent_path());{std::ofstream stream(input,std::ios::binary);stream<<"abcd";check(stream.good(),"native Unicode input fixture written");}

        const auto resource=FindResourceW(instance,MAKEINTRESOURCEW(IDD_ENCRYPT_DIALOG),RT_DIALOG);
        check(resource!=nullptr,"production MFC dialog resource is present");
        const auto size=SizeofResource(instance,resource);
        std::vector<DWORD> resource_copy((size+sizeof(DWORD)-1)/sizeof(DWORD));
        std::memcpy(resource_copy.data(),LockResource(LoadResource(instance,resource)),size);
        const auto bytes=reinterpret_cast<std::byte*>(resource_copy.data());
        WORD signature{};std::memcpy(&signature,bytes+sizeof(WORD),sizeof(signature));
        const auto style_offset=signature==0xffff ? 12u : 0u;DWORD style{};
        std::memcpy(&style,bytes+style_offset,sizeof(style));style&=~WS_VISIBLE;std::memcpy(bytes+style_offset,&style,sizeof(style));
        // Use the actual production template and controls, suppressing visibility only in this fixture.
        Dialog dialog;
        check(dialog.CreateIndirect(reinterpret_cast<LPCDLGTEMPLATE>(resource_copy.data()))!=FALSE,"hidden production dialog and native controls create");
        check(!dialog.IsWindowVisible(),"native regression does not show a desktop window");
        dialog.SetDlgItemText(IDC_MFCEDITBROWSE1,L"");click(dialog,IDC_BUTTON1);
        check(dialog.results.size()==1 && !dialog.results.back().first,"empty native selection reports failure instead of silently returning");
        dialog.SetDlgItemText(IDC_MFCEDITBROWSE1,input.c_str());
        bool reentered=false;dialog.report_hook=[&]{reentered=true;click(dialog,IDC_BUTTON1);};click(dialog,IDC_BUTTON1);dialog.report_hook={};
        const auto output=directory/"3421780262";
        check(dialog.results.size()==2 && dialog.results.back().first && reentered,"native encryption succeeds and nested result callback cannot submit twice");
        check(dialog.results.back().second.find(L"加密成功")!=std::wstring::npos && dialog.results.back().second.find(output.native())!=std::wstring::npos,"native result contains success and full output path");
        check(read(output)==std::vector<char>({0x0d,0x2e,0x02,0x00,0x6c,0x4c,0x61,0x64}) && read(input)==std::vector<char>({'a','b','c','d'}),"actual button binding publishes golden bytes and preserves source");
        dialog.SetDlgItemText(IDC_MFCEDITBROWSE1,output.c_str());click(dialog,IDC_BUTTON2);
        check(dialog.results.back().first && dialog.results.back().second.find(L"解密成功")!=std::wstring::npos && read(output)==std::vector<char>({'a','b','c','d'}),"actual decrypt button replaces its selected file with original bytes");
        click(dialog,IDC_BUTTON2);check(!dialog.results.back().first && read(output)==std::vector<char>({'a','b','c','d'}),"invalid native decrypt reports failure and preserves selection");
        dialog.SetDlgItemText(IDC_MFCEDITBROWSE1,input.c_str());dialog.report_hook=[&]{dialog.DestroyWindow();};click(dialog,IDC_BUTTON1);dialog.report_hook={};
        check(!IsWindow(dialog.GetSafeHwnd()),"result callback may destroy the native window");
        check(dialog.CreateIndirect(reinterpret_cast<LPCDLGTEMPLATE>(resource_copy.data()))!=FALSE,"native dialog object can recreate its window");
        dialog.SetDlgItemText(IDC_MFCEDITBROWSE1,input.c_str());const auto before=dialog.results.size();click(dialog,IDC_BUTTON1);
        check(dialog.results.size()==before+1 && dialog.results.back().first,"operation state resets safely across native window destruction/recreation");dialog.DestroyWindow();
        std::cout<<checks<<" Encrypt native-dialog checks passed\n";return 0;
    }
    catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
