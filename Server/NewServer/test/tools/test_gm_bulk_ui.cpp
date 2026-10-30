#include "gm_application_fixture.h"
#include "gm_ui.h"
#include "gm_reports.h"
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateTimeEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QTextEdit>
#include <fstream>
#include <limits>

namespace
{
template<class T> T* widget(QWidget& owner,const char* name)
{
    auto* value=owner.findChild<T*>(name);
    if (!value) throw std::runtime_error(std::string("Missing widget: ")+name);
    return value;
}
void select(gmtool& window,UINT32 id=0)
{
    auto* tree=widget<QTreeWidget>(window,"treeWidget_SvrList");
    for (UINT32 index=0;index<2;++index)
    {
        CSessionMgr::GetInstance().GetGMSession(index)->eState=GM_SVR_STATE_LOGIN;
        tree->topLevelItem(index)->setCheckState(0,index==id ? Qt::Checked : Qt::Unchecked);
    }
    CServerMgr::GetInstance().UpdateSelectServer();
}
void answer(gmtool& window,bool yes=true,std::function<void()> before={})
{
    QTimer::singleShot(0,&window,[&window,yes,before=std::move(before)]
    {
        const QPointer<QDialog> dialog=window.findChild<QDialog*>("gmActionConfirmation");
        check(!dialog.isNull(),"expected owned confirmation dialog");
        if (before) before();
        if (dialog) { if (yes) dialog->accept();else dialog->reject(); }
    });
}
template<class T> std::shared_ptr<T> child(INT32 kind)
{
    const auto active=CCmdManager::GetInstance().GetCurrentCmd();
    check(active && active->Kind()==CMD_PRE_QUERY,"UI owns pre-query before bulk action");
    auto target=std::static_pointer_cast<PRE_QUERY>(active)->pCmd;
    check(target && target->Kind()==kind,"pre-query retains typed bulk continuation");
    return std::static_pointer_cast<T>(target);
}
void resolve(std::string_view name,UINT32 id)
{
    auto reply=message<GM2TL_PRE_QUERY_ACK>(gm2tl_pre_query_ack);
    reply.dwRoleID=id;check(gm::copy_field(reply.szRoleName,name),"fixture role fits wire field");
    deliver(*current->net->sockets.front(),reply);CCmdManager::GetInstance().MainLoop();
}
template<class T> T request(UINT16 id)
{
    const auto found=std::find_if(current->sent.rbegin(),current->sent.rend(),[id](const auto& entry){return entry.first==id;});
    if (found==current->sent.rend() || found->second.size()<sizeof(T)) throw std::runtime_error("missing outbound packet");
    T value;std::memcpy(&value,found->second.data(),sizeof(value));return value;
}
void parsing()
{
    for (const auto input:{"", " ","1x","++1","+-1","--1","9223372036854775808"})
        check(!gm::parse_decimal<std::int64_t>(input),"malformed/overflow decimal rejected");
    check(gm::parse_decimal<std::int64_t>(" \t+42\r\n")==42,"decimal accepts whitespace and one plus");
    check(gm::parse_decimal<std::uint32_t>("4294967295")==UINT32_MAX,"unsigned protocol maximum retained");
    check(!gm::parse_decimal<std::uint32_t>("-1") && !gm::parse_decimal<std::uint32_t>("4294967296"),"unsigned range checked");
    check(!gm::parse_decimal<int>(std::string_view("1\0",2)),"embedded NUL is not a numeric terminator");
    check(gm::parse_decimal<int>(" \t",true)==0,"optional blank numeric fields retain zero compatibility");
    check(gm::minute_duration(-1,FOREVER_TIME)==FOREVER_TIME,"minus one duration is permanent");
    check(!gm::minute_duration(0,FOREVER_TIME) && !gm::minute_duration(-2,FOREVER_TIME),"zero/other negative durations rejected");
    check(gm::minute_duration(16666666,FOREVER_TIME)==999999960,"last finite legacy duration retained");
    check(gm::minute_duration(16666667,FOREVER_TIME)==FOREVER_TIME && gm::minute_duration(INT64_MAX,FOREVER_TIME)==FOREVER_TIME,"large durations saturate before multiplication");
    std::string text="a\r\nb\nc\r";text.push_back(static_cast<char>(0x80));
    gm::normalize_line_breaks(std::span<char>(text));
    check(text==std::string("a  b c\r")+static_cast<char>(0x80),"newline normalization preserves byte widths and malformed legacy bytes");
}
}

int main(int argc,char** argv)
{
    try
    {
        parsing();std::cerr<<"bulk fixture: parsing complete\n";check(argc==2,"expected private fixture directory");
        WSADATA winsock{};check(WSAStartup(MAKEWORD(2,2),&winsock)==0,"Winsock fixture starts");
        struct WinsockCleanup{~WinsockCleanup(){WSACleanup();}} winsock_cleanup;
        CLog::GetInstance().SetLevelMask(0);QApplication application(argc,argv);application.setQuitOnLastWindowClosed(false);
        const auto previous=std::filesystem::current_path();
        const auto directory=std::filesystem::absolute(argv[1]);std::filesystem::create_directories(directory);
        struct Restore{std::filesystem::path previous;~Restore(){std::error_code error;std::filesystem::current_path(previous,error);}} restore{previous};
        std::filesystem::current_path(directory);
        struct Stop{~Stop(){CGMManager::GetInstance().UnInit();}} stop_runtime;
        {
            gmtool window;loader login;gm::ApplicationViews views(window,login);start(window);select(window);
            std::cerr<<"bulk fixture: runtime ready\n";
            auto& manager=CCmdManager::GetInstance();
            auto* single=widget<QRadioButton>(window,"radioButton_6");single->setChecked(true);
            auto* role=widget<QTextEdit>(window,"textEdit_RoleID");role->setPlainText(QStringLiteral("original"));
            auto* preset=widget<QRadioButton>(window,"fc1_radioButton");preset->setChecked(true);
            auto* duration=widget<QComboBox>(window,"fc1_comboBox");
            const std::array<UINT32,5> seconds{3600,86400,259200,604800,FOREVER_TIME};
            for (int index=0;index<5;++index)
            {
                duration->setCurrentIndex(index);answer(window);window.ForBiddenChat();
                check(child<FORBIDDEN_CHAT>(CMD_FORBIDDEN_CHAT)->llTime==seconds[index],"preset duration exactly preserved");manager.CmdEnd();
                std::cerr<<"bulk fixture: preset "<<index<<" complete\n";
            }
            auto* custom=widget<QRadioButton>(window,"fc2_radioButton");custom->setChecked(true);
            auto* minutes=widget<QLineEdit>(window,"forbiddenChatTimeEdit");
            const auto initial=sent(tl2gm_pre_query);
            for (const auto input:{"0","-2","oops","1.5","9223372036854775808"})
            {minutes->setText(QString::fromLatin1(input));window.FreezeRole();check(sent(tl2gm_pre_query)==initial,"invalid duration sends no pre-query");}
            minutes->setText(QStringLiteral(" +2 "));answer(window,true,[&]{role->setPlainText(QStringLiteral("changed"));minutes->setText(QStringLiteral("99"));window.FreezeRole();});
            window.FreezeRole();auto frozen=child<FREEZE_ROLE>(CMD_FREEZE_ROLE);
            check(frozen->llTime==120 && std::string(frozen->roleList.front().szRoleName)=="original","role and duration owned before confirmation; duplicate suppressed");manager.CmdEnd();
            answer(window,false);window.KickRole();check(!manager.GetCurrentCmd(),"declined bulk action leaves no command");
            answer(window,true,[&]{select(window,1);});window.KickRole();check(!manager.GetCurrentCmd(),"peer change cancels confirmed bulk action");select(window);
            answer(window);window.UnfreezeRole();check(child<FREEZE_ROLE>(CMD_FREEZE_ROLE)->llTime==0,"revocation ignores custom duration");manager.CmdEnd();

            std::cerr<<"bulk fixture: durations complete\n";
            auto* item=widget<QLineEdit>(window,"lineEdit_mail_2");auto* count=widget<QLineEdit>(window,"lineEdit_mail_3");
            auto* subject=widget<QLineEdit>(window,"lineEdit_mail_1");auto* body=widget<QTextEdit>(window,"textEdit_mail");auto* reason=widget<QTextEdit>(window,"mailReasonEdit");
            const auto before_mail=sent(tl2gm_pre_query);
            for (const auto& pair:std::array<std::pair<const char*,const char*>,5>{{{"1","0"},{"0","1"},{"1","-1"},{"x","1"},{"1","2147483648"}}})
            {item->setText(QString::fromLatin1(pair.first));count->setText(QString::fromLatin1(pair.second));window.Mail();check(sent(tl2gm_pre_query)==before_mail,"invalid mail attachment rejected before confirmation");}
            item->clear();count->clear();subject->clear();body->clear();reason->clear();answer(window);window.Mail();
            auto empty=child<SEND_MAIL>(CMD_SEND_MAIL);check(!empty->dwItemID && !empty->nItemNum && !empty->szTitle[0] && !empty->szContent[0],"blank attachment and empty mail text preserve legacy wire values");manager.CmdEnd();
            const auto before_invalid_mail=sent(tl2gm_pre_query);
            item->setText(QStringLiteral("1"));count->setText(QStringLiteral("1"));window.Mail();
            check(sent(tl2gm_pre_query)==before_invalid_mail,"attachment requires a nonempty reason");
            reason->setPlainText(QStringLiteral("reason"));subject->setText(QString(MAIL_SUBJECT_LEN,'s'));window.Mail();
            check(sent(tl2gm_pre_query)==before_invalid_mail,"oversized mail subject rejected without truncation");subject->clear();
            body->setPlainText(QString(MAIL_CONTENT_LEN,'b'));window.Mail();check(sent(tl2gm_pre_query)==before_invalid_mail,"oversized mail body rejected");body->clear();
            reason->setPlainText(QString(GM_REASON_LEN,'r'));window.Mail();check(sent(tl2gm_pre_query)==before_invalid_mail,"oversized reason rejected");
            reason->setPlainText(QStringLiteral("reason"));subject->setText(QStringLiteral("a")+QChar(0));window.Mail();check(sent(tl2gm_pre_query)==before_invalid_mail,"mail embedded NUL rejected");
            item->setText(QStringLiteral("4294967295"));count->setText(QStringLiteral("2147483647"));
            subject->setText(QString(MAIL_SUBJECT_LEN-1,'s'));body->setPlainText(QString(MAIL_CONTENT_LEN-1,'b'));reason->setPlainText(QString(GM_REASON_LEN-1,'r'));
            answer(window,true,[&]{item->setText(QStringLiteral("1"));count->setText(QStringLiteral("1"));subject->setText(QStringLiteral("changed"));reason->clear();});window.Mail();
            const auto mail=child<SEND_MAIL>(CMD_SEND_MAIL);
            check(mail->dwItemID==UINT32_MAX && mail->nItemNum==INT32_MAX && std::strlen(mail->szReason)==GM_REASON_LEN-1,"mail maximum numeric/reason fields survive confirmation mutation");
            resolve("changed",42);const auto packet=request<TL2GM_SEND_MAIL>(tl2gm_send_mail);
            check(packet.dwRoleID==42 && packet.dwItemID==UINT32_MAX && std::strlen(packet.szSubject)==MAIL_SUBJECT_LEN-1 && std::strlen(packet.szContent)==MAIL_CONTENT_LEN-1,"actual sender retains full captured mail wire fields");manager.CmdEnd();

            std::cerr<<"bulk fixture: mail complete\n";
            std::string basename;
            for (int index=0;index<55;++index) basename+="\xe7\x8c\xab";
            const auto filename=directory/gm::utf8_path(basename+".txt");
            {std::ofstream file(filename,std::ios::binary);file<<"first\r\nsecond\r\n";}
            widget<QRadioButton>(window,"radioButton_5")->setChecked(true);
            const auto filename_bytes=filename.u8string();widget<QLineEdit>(window,"lineEdit")->setText(QString::fromUtf8(reinterpret_cast<const char*>(filename_bytes.data()),static_cast<int>(filename_bytes.size())));
            answer(window,true,[&]{widget<QLineEdit>(window,"lineEdit")->clear();std::ofstream file(filename,std::ios::binary);file<<"mutated\n";});window.KickRole();
            const auto kick=child<KICK_ROLE>(CMD_KICK_ROLE);
            check(kick->report_path==filename && kick->roleList.size()==2 && std::string(kick->roleList.front().szRoleName)=="first","checked file input owns native Unicode path and role list before confirmation");
            check(filename_bytes.size()>256,"fixture exceeds removed UTF-8 report-path buffer");
            resolve("first",43);auto kicked=message<GM2TL_KICK_ROLE_ACK>(gm2tl_kick_role_ack);kicked.dwRoleID=43;kicked.nResult=errSuccess;deliver(*current->net->sockets.front(),kicked);manager.MainLoop();
            check(child<KICK_ROLE>(CMD_KICK_ROLE)->roleList.size()==1,"bulk ACK advances to next captured role");resolve("second",44);
            kicked.dwRoleID=44;deliver(*current->net->sockets.front(),kicked);manager.MainLoop();
            check(kick->roleList.empty() && !manager.GetCurrentCmd(),"both captured roles finish and retire through production receive/sender paths");
            auto report_path=filename;report_path+="_kickrole_result";
            std::ifstream report(report_path,std::ios::binary);
            const std::string report_bytes{std::istreambuf_iterator<char>(report),std::istreambuf_iterator<char>()};
            check(report_bytes=="\xEF\xBB\xBFRoleName\tRoleID\tResult \r\nfirst\t43\tsuccess \r\nsecond\t44\tsuccess \r\n","full native path publishes exact legacy report bytes after both replies");

            std::cerr<<"bulk fixture: file batch complete\n";
            auto* groups=widget<QComboBox>(window,"comboBox");groups->clear();groups->addItems({QStringLiteral("all"),QStringLiteral("group")});groups->setCurrentIndex(0);
            auto* interval=widget<QLineEdit>(window,"lineEdit_announce_1");auto* repeats=widget<QLineEdit>(window,"lineEdit_announce_2");
            auto* announcement=widget<QTextEdit>(window,"textEdit_announce");auto* date=widget<QDateTimeEdit>(window,"dateTimeEdit_announce");
            date->setDateTime(QDateTime::fromSecsSinceEpoch(1700000039));interval->setText(QStringLiteral("0"));repeats->setText(QStringLiteral("1"));announcement->setPlainText(QStringLiteral("hello"));
            const auto announce_count=sent(tl2gm_announce);window.AnnounceFunc();check(sent(tl2gm_announce)==announce_count,"zero announcement interval rejected");
            interval->setText(QStringLiteral("1"));
            date->setMinimumDateTime(QDateTime::fromSecsSinceEpoch(-100));date->setMaximumDateTime(QDateTime::fromSecsSinceEpoch(4294967395LL));
            for (const auto epoch:std::array<qint64,2>{-1,4294967296LL})
            {date->setDateTime(QDateTime::fromSecsSinceEpoch(epoch));window.AnnounceFunc();check(sent(tl2gm_announce)==announce_count,"out-of-protocol announcement timestamp rejected");}
            date->setDateTime(QDateTime::fromSecsSinceEpoch(1700000039));
            for (const auto value:{"0","-1","2147483648","1x"})
            {repeats->setText(QString::fromLatin1(value));window.AnnounceFunc();check(sent(tl2gm_announce)==announce_count,"invalid repeat count rejected before confirmation");}
            repeats->setText(QStringLiteral("1"));announcement->setPlainText(QString(GM_ANNOUNCE_LENGTH,'x'));window.AnnounceFunc();
            check(sent(tl2gm_announce)==announce_count,"oversized announcement rejected without truncation");
            announcement->setPlainText(QStringLiteral("a")+QChar(0));window.AnnounceFunc();check(sent(tl2gm_announce)==announce_count,"announcement NUL rejected");
            interval->setText(QStringLiteral(" +2 "));repeats->setText(QStringLiteral("2147483647"));announcement->setPlainText(QStringLiteral("first\nsecond"));
            answer(window,true,[&]{announcement->setPlainText(QStringLiteral("changed"));interval->setText(QStringLiteral("99"));window.AnnounceFunc();});window.AnnounceFunc();
            const auto announce=request<TL2GM_ANNOUNCE>(tl2gm_announce);
            check(announce.dwBeginTime==1699999980 && announce.dwIntervalTime==2 && announce.nRepeatNum==INT32_MAX,"announcement minute boundary and captured numeric fields retained");
            const auto& wire=current->sent.back().second;
            check(std::string(wire.data()+sizeof(TL2GM_ANNOUNCE))=="first second","captured announcement line breaks normalized without byte expansion");manager.CmdEnd();
            answer(window,true,[&]{groups->setCurrentIndex(1);});window.AnnounceFunc();check(sent(tl2gm_announce)==announce_count+1,"group change during confirmation cancels announcement");groups->setCurrentIndex(0);
            TL_ANNOUNCE_DATA row{};row.nSvrID=1;row.dwSerial=UINT32_MAX;std::memcpy(row.szText,"fixture",8);
            check(CDataMgr::GetInstance().ReplaceAnnounceList(std::span(&row,1)) && window.UpdateAnnounceList(),"announcement record published with unsigned serial and server identity");
            auto* announcements=widget<QTreeWidget>(window,"announceWidget");announcements->setCurrentItem(announcements->topLevelItem(0));
            check(manager.SendGMStrCmd("busy"),"busy announcement-query fixture starts");window.GetAnnounceList();
            check(announcements->topLevelItemCount()==1 && CDataMgr::GetInstance().GetAnnounceList().size()==1,"busy announcement query preserves published rows and cached records");manager.CmdEnd();
            answer(window);window.DelAnnounce();check(request<TL2GM_DEL_ANNOUNCE>(tl2gm_del_announce).dwAnnounceSerial==UINT32_MAX,"UI deletion retains full unsigned announcement serial");manager.CmdEnd();
            select(window,1);const auto deletion_count=sent(tl2gm_del_announce);window.DelAnnounce();check(sent(tl2gm_del_announce)==deletion_count,"stale other-server announcement row cannot be deleted");
            select(window);
            current->send_hook=[&](Socket& socket,UINT16 id)
            {
                if (id!=tl2gm_get_announce_list) return;
                GM2TL_GET_ANNOUNCE_LIST_ACK header;std::memset(&header,0,sizeof(header));header.wMsg=gm2tl_get_announce_list_ack;header.nCount=1;
                GMTL_ANNOUNCE_DATA reply{};reply.dwSerial=7;std::memcpy(reply.szText,"synchronous",12);
                std::vector<char> bytes(sizeof(header)+sizeof(reply));std::memcpy(bytes.data(),&header,sizeof(header));std::memcpy(bytes.data()+sizeof(header),&reply,sizeof(reply));
                auto* packet=current->allocator->Alloc(static_cast<INT32>(bytes.size()));std::memcpy(packet,bytes.data(),bytes.size());
                CMessageHandler::GetInstance().OnRecv(&socket,packet,static_cast<UINT32>(bytes.size()));
            };
            window.GetAnnounceList();current->send_hook={};
            check(announcements->topLevelItemCount()==1 && announcements->topLevelItem(0)->data(0,Qt::UserRole).toUInt()==7,"synchronous announcement query remains visible after submission returns");
            stop();
        }
        std::cout<<checks<<" GM bulk-input checks passed\n";return 0;
    }
    catch(const std::exception& error){CGMManager::GetInstance().UnInit();std::cerr<<error.what()<<'\n';return 1;}
}
