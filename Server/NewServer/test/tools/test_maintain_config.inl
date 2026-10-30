void config_refresh_tests(const fs::path& root,MaintainTool& window,Session& socket,
    SRemoteGuardSession& record,QTreeWidget& areas,QTreeWidget& processes)
{
    const auto source=(root/"reload-pending.bin").string();
    std::ofstream{source,std::ios::binary}<<"pending";
    const char* names[]{"pending.bin"};const char* paths[]{source.c_str()};
    auto& sender=CTransmitFileMgr::GetInstance();
    check(sender.StartSending(1,0x0100007f,1,names,paths,etftPatch),"live transfer is present during invalid configuration reload tests");
    const auto snapshot=std::make_unique<MAINTAIN_CONFIG>(g_MTConfig);
    auto* oldRoot=areas.topLevelItem(0);const auto closes=socket.closes;
    const auto unchanged=[&] {
        window.VersionRefresh();
        check(std::memcmp(snapshot.get(),&g_MTConfig,sizeof(g_MTConfig))==0 &&
            socket.closes==closes && socket.data==&record && record.pSocketSession==&socket &&
            sender.IsSending(1,0x0100007f) && areas.topLevelItem(0)==oldRoot && processes.topLevelItemCount()==95,
            "failed scan preserves global config, live session, transfer and existing Qt trees");
    };
    for (const auto bad: {"0_Invalid","-1_Invalid","1024_Invalid","01_Duplicate"})
    {
        fs::create_directory(root/"area_config"/bad);unchanged();fs::remove(root/"area_config"/bad);
    }
    fs::create_directory(root/"area_config/1_Test/256.1.1.1");unchanged();fs::remove(root/"area_config/1_Test/256.1.1.1");
    fs::rename(root/"area_config",root/"area_config.saved");unchanged();
    fs::rename(root/"area_config.saved",root/"area_config");
    const auto timing=g_MTConfig.nPingTimeout;g_MTConfig.nPingTimeout=-1;
    window.VersionRefresh();
    check(record.pSocketSession==&socket && socket.closes==closes && areas.topLevelItem(0)==oldRoot &&
        sender.IsSending(1,0x0100007f),"session candidate validation fails before retiring existing connections");
    g_MTConfig.nPingTimeout=timing;
    for (const auto name: {"release.zip","z.zip.backup","UPPER.ZIP"}) std::ofstream{root/"version"/name}<<"file";
    const auto unicode=fs::path{L"\u96ea.zip"};std::ofstream{root/"version"/unicode}<<"file";
    fs::create_directory(root/"version/not-a-file.zip");
    std::ofstream{root/"patch/release.pak"}<<"patch";
    fs::create_directories(root/"area_config/2_New Area/10.0.0.1");
    socket.rejectClose=true;
    socket.onClose=[&] {
        window.VersionRefresh();
        check(areas.topLevelItem(0)==oldRoot && g_MTConfig.AreaConfig[2].nAreaID==0,
            "native close callback cannot publish a nested GUI refresh");
    };
    window.VersionRefresh();socket.onClose={};socket.rejectClose=false;
    check(socket.closes==closes+1 && !socket.data && !record.pSocketSession && !sender.IsSending(1,0x0100007f) &&
        g_MTConfig.AreaConfig[2].nAreaID==0 && areas.topLevelItem(0)==oldRoot && oldRoot->child(0)->text(2)=="unconnect" &&
        processes.topLevelItemCount()==0,
        "failed native retirement keeps old config/tree and reflects detached peers without retaining their transfers");
    window.VersionRefresh();
    check(socket.closes==closes+1 && !socket.data && !record.pSocketSession && !sender.IsSending(1,0x0100007f),
        "retry does not close the already detached peer twice");
    check(g_MTConfig.AreaConfig[2].nAreaID==2 && std::string_view{g_MTConfig.AreaConfig[2].szAreaName}=="New Area" &&
        g_MTConfig.AreaConfig[2].guardList[1].nPort==8810 && areas.topLevelItemCount()==2 && processes.topLevelItemCount()==0 &&
        CSessionMgr::GetInstance().GetRemoteGDSession(2,*maintain::parse_address("10.0.0.1")),
        "successful publication uses the same validated candidate for sessions, global records and the GUI");
    auto* versions=window.findChild<QListWidget*>("listVersion");
    auto* patches=window.findChild<QListWidget*>("listPatch");
    check(versions && patches && versions->count()==3 && patches->count()==1 &&
        !versions->findItems(QString::fromStdWString(unicode.native()),Qt::MatchExactly).empty(),
        "actual Qt package lists preserve filtering and native Unicode filenames");
    if (!versions || !patches) throw std::runtime_error("missing package widgets");
    QStringList before;
    for (int i=0;i<versions->count();++i) before.push_back(versions->item(i)->text());
    fs::rename(root/"version",root/"version.saved");std::ofstream{root/"version"}<<"not a directory";
    window.VersionRefresh();
    QStringList after;
    for (int i=0;i<versions->count();++i) after.push_back(versions->item(i)->text());
    check(before==after,"failed package discovery retains the previous visible list");
    fs::remove(root/"version");fs::rename(root/"version.saved",root/"version");
    fs::remove(root/"area_config/2_New Area/10.0.0.1");fs::remove(root/"area_config/2_New Area");
    window.VersionRefresh();
    check(g_MTConfig.AreaConfig[2].nAreaID==0 && areas.topLevelItemCount()==1 &&
        !CSessionMgr::GetInstance().GetRemoteGDSession(2,*maintain::parse_address("10.0.0.1")),
        "removed area disappears coherently from config, GUI and sessions");
}
