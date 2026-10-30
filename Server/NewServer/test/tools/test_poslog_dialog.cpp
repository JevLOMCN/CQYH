#include "stdafx.h"
#include "PosLog.h"
#include "PosLogDlg.h"
#include "bmp_file.h"
#include <array>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>

namespace
{
namespace fs=std::filesystem;
CWinApp fixture_application;
unsigned checks;
void check(bool value,const char* message){++checks;if(!value)throw std::runtime_error(message);}
class Dialog final:public CPosLogDlg
{
public:
    std::optional<fs::path> input,output;
    std::function<void()> input_hook,output_hook,error_hook;
    unsigned input_calls{},output_calls{};
    std::vector<std::string> errors;
protected:
    std::optional<fs::path> ChooseLogFile() override {++input_calls;const auto result=input;const auto hook=input_hook;if(hook)hook();return result;}
    std::optional<fs::path> ChooseBitmapFile() override {++output_calls;const auto result=output;const auto hook=output_hook;if(hook)hook();return result;}
    void ReportError(const std::string& message) override {errors.push_back(message);const auto hook=error_hook;if(hook)hook();}
};
void write(const fs::path& file,std::string_view bytes){std::ofstream stream(file,std::ios::binary);stream.write(bytes.data(),static_cast<std::streamsize>(bytes.size()));if(!stream)throw std::runtime_error("Cannot write native fixture");}
std::vector<char> read(const fs::path& file){std::ifstream stream(file,std::ios::binary);check(stream.good(),"native output exists");return {std::istreambuf_iterator<char>(stream),std::istreambuf_iterator<char>()};}
void click(Dialog& dialog,UINT id){dialog.SendMessage(WM_COMMAND,MAKEWPARAM(id,BN_CLICKED),reinterpret_cast<LPARAM>(dialog.GetDlgItem(id)->GetSafeHwnd()));}
CListCtrl& list(Dialog& dialog){return *static_cast<CListCtrl*>(dialog.GetDlgItem(IDC_LIST2));}
void select(Dialog& dialog,int row)
{
    list(dialog).SetItemState(-1,0,LVIS_SELECTED);list(dialog).SetItemState(row,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
    LRESULT result{};dialog.OnLvnItemchangedList2(nullptr,&result);
}
void size(Dialog& dialog,DWORD_PTR side)
{
    auto* combo=static_cast<CComboBox*>(dialog.GetDlgItem(IDC_COMBO1));int selected=-1;
    for(int index=0;index<combo->GetCount();++index)if(combo->GetItemData(index)==side)selected=index;
    combo->SetCurSel(selected);dialog.SendMessage(WM_COMMAND,MAKEWPARAM(IDC_COMBO1,CBN_SELCHANGE),reinterpret_cast<LPARAM>(combo->GetSafeHwnd()));
}
bool save_enabled(Dialog& dialog){return dialog.GetDlgItem(IDC_SAVE_BMP)->IsWindowEnabled()!=FALSE;}
std::vector<DWORD> resource(HINSTANCE instance)
{
    const auto found=FindResourceW(instance,MAKEINTRESOURCEW(IDD_POSLOG_DIALOG),RT_DIALOG);check(found!=nullptr,"production PosLog dialog resource present");
    const auto length=SizeofResource(instance,found);std::vector<DWORD> bytes((length+3)/4);
    std::memcpy(bytes.data(),LockResource(LoadResource(instance,found)),length);
    auto* data=reinterpret_cast<std::byte*>(bytes.data());WORD signature{};std::memcpy(&signature,data+2,2);
    const auto offset=signature==0xffff ? 12u : 0u;DWORD style{};std::memcpy(&style,data+offset,4);style&=~WS_VISIBLE;std::memcpy(data+offset,&style,4);return bytes;
}
void canvas(Dialog& dialog)
{
    struct Surface
    {
        HDC dc=CreateCompatibleDC(nullptr);HBITMAP bitmap{};HGDIOBJ old{};
        ~Surface(){if(old)SelectObject(dc,old);if(bitmap)DeleteObject(bitmap);if(dc)DeleteDC(dc);}
    } surface;
    BITMAPINFO info{};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=8;info.bmiHeader.biHeight=-8;info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;
    void* pixels{};surface.bitmap=CreateDIBSection(surface.dc,&info,DIB_RGB_COLORS,&pixels,nullptr,0);check(surface.dc && surface.bitmap,"native offscreen canvas surface creates");surface.old=SelectObject(surface.dc,surface.bitmap);
    DRAWITEMSTRUCT item{};item.hDC=surface.dc;item.rcItem={0,0,8,8};
    static_cast<CPositionCanvas*>(dialog.GetDlgItem(IDC_BUTTON3))->DrawItem(&item);
    check(GetPixel(surface.dc,4,3)==RGB(255,0,0) && GetPixel(surface.dc,0,7)==RGB(255,0,0),"persistent native canvas uses the same center/origin orientation as bottom-up bitmap output");
}
}

int main(int argc,char** argv)
{
    try
    {
        check(argc==2,"expected native PosLog private directory");
        INITCOMMONCONTROLSEX controls{sizeof(controls),ICC_WIN95_CLASSES};check(InitCommonControlsEx(&controls)!=FALSE,"native list/combo controls initialize");
        const auto instance=GetModuleHandleW(nullptr);check(AfxWinInit(instance,nullptr,GetCommandLineW(),SW_HIDE)!=FALSE,"native MFC fixture initializes");fixture_application.InitInstance();AfxEnableControlContainer();
        CShellManager shell_manager;CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
        const auto directory=fs::absolute(argv[1])/fs::path(u8"\u8DEF\u5F84");fs::create_directories(directory);
        const auto input=directory/"positions.log";const auto invalid=directory/"invalid.log";
        write(input,"header\n2024-02-29 23:00:00\t1\t1\t1\t257\t1\t134217728\t0\r\n2024-03-01 23:01:00\t1\t1\t1\t257\t1\t134217728\t111219712\n2024-03-01 23:02:00\t1\t1\t1\t4294967295\t1\t134217728\t111219712");
        write(invalid,"header\ninvalid log record");
        auto template_bytes=resource(instance);Dialog dialog;
        check(dialog.CreateIndirect(reinterpret_cast<LPCDLGTEMPLATE>(template_bytes.data()))!=FALSE && !dialog.IsWindowVisible(),"production PosLog dialog and controls create without showing a desktop window");
        check(!save_enabled(dialog) && list(dialog).GetItemCount()==0,"initial model has no selected scene or export target");
        click(dialog,IDC_SAVE_BMP);check(dialog.errors.size()==1 && dialog.output_calls==0,"invalid export reports error before opening a picker");
        dialog.input.reset();click(dialog,IDC_BUTTON1);check(list(dialog).GetItemCount()==0,"cancelled first load keeps an empty view");
        dialog.input=input;dialog.input_hook=[&]{click(dialog,IDC_BUTTON1);click(dialog,IDC_SAVE_BMP);};const auto load_calls=dialog.input_calls;click(dialog,IDC_BUTTON1);dialog.input_hook={};
        check(dialog.input_calls==load_calls+1 && list(dialog).GetItemCount()==2,"actual load button stages data and suppresses nested file operations");
        check(list(dialog).GetItemText(0,1)==L"4294967295" && list(dialog).GetItemText(1,1)==L"257" && list(dialog).GetItemText(1,2)==L"2" && list(dialog).GetItemText(1,0)==L"23:00","native rows retain descending hour order, complete scene IDs and counts");
        CString label;dialog.GetDlgItemText(IDC_STATIC_FILE,label);check(std::wstring(label.GetString())==input.native(),"native file label retains complete Unicode path");
        size(dialog,512);check(!save_enabled(dialog),"bitmap size alone cannot export without a scene");select(dialog,1);check(save_enabled(dialog),"valid scene and size enable export");canvas(dialog);
        const auto prior_errors=dialog.errors.size();dialog.input=invalid;click(dialog,IDC_BUTTON1);check(dialog.errors.size()==prior_errors+1 && list(dialog).GetItemCount()==2 && save_enabled(dialog),"invalid reload retains previous model, selection and export state");
        dialog.input.reset();click(dialog,IDC_BUTTON1);check(list(dialog).GetItemCount()==2 && save_enabled(dialog),"cancelled reload preserves existing data");
        const auto bitmap=directory/fs::path(u8"\u5730\u56FE.bmp");write(bitmap,"prior");dialog.output.reset();click(dialog,IDC_SAVE_BMP);check(read(bitmap)==std::vector<char>({'p','r','i','o','r'}),"cancelled save leaves previous output untouched");
        dialog.output=bitmap;click(dialog,IDC_SAVE_BMP);const std::array<poslog::Position,2> selected_points{{{0,0},{76800,76800}}};
        check(read(bitmap)==*poslog::make_bitmap(selected_points,512) && !fs::exists(fs::path(bitmap.native()+L".bmp")),"actual save button publishes selected group to native Unicode filename without duplicate extension");
        dialog.output=directory/"captured.bmp";const auto save_calls=dialog.output_calls;
        dialog.output_hook=[&]{select(dialog,0);size(dialog,1024);click(dialog,IDC_SAVE_BMP);click(dialog,IDC_BUTTON1);};click(dialog,IDC_SAVE_BMP);dialog.output_hook={};
        check(dialog.output_calls==save_calls+1 && read(directory/"captured.bmp")==*poslog::make_bitmap(selected_points,512),"save owns selected positions and size before nested picker callbacks");
        dialog.output=directory/"follow-up";click(dialog,IDC_SAVE_BMP);const std::array<poslog::Position,1> follow_up{{{76800,76800}}};
        check(read(directory/"follow-up.bmp")==*poslog::make_bitmap(follow_up,1024),"subsequent save uses updated native selection/size and supplies a missing extension");
        size(dialog,0);check(!save_enabled(dialog),"invalid combo selection disables export");const auto before=dialog.output_calls;click(dialog,IDC_SAVE_BMP);check(dialog.output_calls==before,"invalid size cannot open export picker");size(dialog,512);
        list(dialog).SetItemData(0,(std::numeric_limits<DWORD_PTR>::max)());select(dialog,0);check(!save_enabled(dialog),"stale native row metadata cannot index outside owned keys");list(dialog).SetItemData(0,0);select(dialog,1);
        dialog.input=input;dialog.input_hook=[] {throw std::runtime_error("fixture picker failure");};const auto errors=dialog.errors.size();click(dialog,IDC_BUTTON1);dialog.input_hook={};check(dialog.errors.size()==errors+1 && list(dialog).GetItemCount()==2,"picker exception retains the published model");
        dialog.output=directory/"must-not-write.bmp";dialog.output_hook=[&]{dialog.DestroyWindow();};click(dialog,IDC_SAVE_BMP);dialog.output_hook={};
        check(!IsWindow(dialog.GetSafeHwnd()) && !fs::exists(directory/"must-not-write.bmp"),"window destruction during save picker cancels file publication");
        check(dialog.CreateIndirect(reinterpret_cast<LPCDLGTEMPLATE>(template_bytes.data()))!=FALSE && !save_enabled(dialog),"native window recreation resets owned model and operation state");
        dialog.input=input;dialog.input_hook=[&]{dialog.DestroyWindow();};const auto previous_errors=dialog.errors.size();click(dialog,IDC_BUTTON1);dialog.input_hook={};
        check(!IsWindow(dialog.GetSafeHwnd()) && dialog.errors.size()==previous_errors,"window destruction during load picker cancels publication without accessing deleted controls");
        check(dialog.CreateIndirect(reinterpret_cast<LPCDLGTEMPLATE>(template_bytes.data()))!=FALSE,"native window recreates after cancelled load");dialog.input=input;click(dialog,IDC_BUTTON1);check(list(dialog).GetItemCount()==2,"operation state permits successful retry after callback destruction");dialog.DestroyWindow();
        std::cout<<checks<<" PosLog native-dialog checks passed\n";return 0;
    }
    catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}
}
