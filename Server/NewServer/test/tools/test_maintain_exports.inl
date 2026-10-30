void export_tests(const fs::path& root,MaintainTool& window,Allocator& allocator,Session& socket,
    SRemoteGuardSession& record,QTreeWidget& processes)
{
    const auto read=[](const fs::path& path) {
        std::ifstream input{path,std::ios::binary};
        if (!input) throw std::runtime_error("missing export fixture file");
        return std::string{std::istreambuf_iterator<char>{input},{}};
    };
    for (int character=32;character<=126;++character)
    {
        if (character=='"') continue;
        std::string legacy(1,static_cast<char>(character));auto* pointer=legacy.data();
        check(SimpleEncrypt(pointer,1),"retained legacy password transform accepts printable input");
        const auto transformed=maintain::transform_security("szPwd=\""+std::string(1,static_cast<char>(character))+"\"\n");
        check(transformed && *transformed=="szPwd=\""+legacy+"\"\r\n","standard transform matches the actual retained SimpleEncrypt implementation");
    }
    auto& info=*record.poStatisInfo;
    std::memset(info.oAccumuStatis,0,sizeof(info.oAccumuStatis));
    for (int sample=0;sample<ACCUMU_MAX_TIMES;++sample)
    {
        auto& process=info.oAccumuStatis[sample].ProcessInfo[sample%MAX_STATIS_PROCESS_COUNT];
        process.bUsed=TRUE;process.dwProcessID=100;process.qwStatisEntry[statCpuUsage]=UINT64_MAX;
        if (sample<2) process.qwStatisEntry[statMemUsage]=sample ? 7000 : 5000;
    }
    info.totals[100][statMemUsage].qwMin=4096;info.totals[100][statMemUsage].qwMax=16384;
    const auto summary=window.SnapShootGuardStatisInfoByIp(1,0x0100007f,100);
    check(summary && summary->find("MemUsage\t8192\t120\t4096\t16384\n")!=std::string::npos,
        "summary correlates a moving PID across all history slots with the legacy 100-slot denominator");
    check(summary && summary->find("CpuUsage\t50\t-1\t")!=std::string::npos,"full-width history mean avoids overflowing its accumulator");
    const auto current=window.SnapShootGuardStatisInfoCurrentByIp(1,0x0100007f,100);
    check(current && current->starts_with("\t8192\t50"),"current snapshot uses owned text and preserves signed TSV ordering");
    check(!window.SnapShootGuardStatisInfoCurrentByIp(1,0x0100007f,101),"missing process statistics report failure instead of a partial row");
    record.bConnected=FALSE;
    check(!window.SnapShootGuardStatisInfoByIp(1,0x0100007f,100),"disconnected statistics cannot be exported");
    record.bConnected=TRUE;
    QTreeWidgetItem* selected=nullptr;QTreeWidgetItem* missing=nullptr;
    for (int index=0;index<processes.topLevelItemCount();++index)
    {
        auto* item=processes.topLevelItem(index);
        if (item->text(0).endsWith("-100")) selected=item;
        if (item->text(0).endsWith("-101")) missing=item;
    }
    if (!selected || !missing) throw std::runtime_error("missing snapshot process rows");
    processes.clearSelection();selected->setSelected(true);
    const auto snapshots=[&] {
        std::map<fs::path,std::string> result;
        if (fs::is_directory(root/"log")) for (const auto& file:fs::directory_iterator(root/"log"))
            if (file.path().filename().string().starts_with("snapshoot_")) result.emplace(file.path(),read(file.path()));
        return result;
    };
    check(window.SnapShootGuardStatisInfoCurrent(),"actual GUI current snapshot publishes a complete file");
    auto files=snapshots();
    check(std::ranges::any_of(files,[](const auto& file) {
        return file.second.starts_with("name\tMemUsage\tCpuUsage\t") && file.second.find(":100@127.0.0.1\t8192\t50")!=std::string::npos;
    }),"current snapshot retains header, process label, columns and LF output bytes");
    check(window.SnapShootGuardStatisInfo(),"actual GUI summary snapshot publishes a complete file");
    files=snapshots();
    check(std::ranges::any_of(files,[](const auto& file) {
        return file.second.find("name\tcurrent\taverage\tmin\tmax\nMemUsage\t8192\t120")!=std::string::npos;
    }),"detailed snapshot retains its legacy TSV layout");
    processes.clearSelection();missing->setSelected(true);
    check(!window.SnapShootGuardStatisInfoCurrent() && snapshots()==files,"failed selected-process export preserves all existing snapshot files");
    processes.clearSelection();selected->setSelected(true);
    fs::rename(root/"log",root/"log.saved");std::ofstream{root/"log"}<<"blocked";
    check(!window.SnapShootGuardStatisInfoCurrent(),"snapshot publication errors reach the GUI caller");
    fs::remove(root/"log");fs::rename(root/"log.saved",root/"log");
    auto& sender=CTransmitFileMgr::GetInstance();
    const auto acknowledge=[&](const N2U_SEND_FILES& chunk) {
        U2N_SEND_FILES_ACK ack{};ack.wMsg=u2n_send_files_ack;ack.nErrorCode=errSuccess;
        std::memcpy(ack.szFileName,chunk.szFileName,sizeof(ack.szFileName));
        ack.wBufferIndex=chunk.wBufferIndex;ack.wBufferTotal=chunk.wBufferTotal;
        ack.wFileIndex=chunk.wFileIndex;ack.wFileTotal=chunk.wFileTotal;
        deliver(allocator,socket,ack);
    };
    const auto finish=[&] {
        std::size_t observed=0;
        for (int loop=0;loop<20 && sender.IsSending(1,0x0100007f);++loop) {
            sender.MainLoop();
            const auto count=socket.chunks.size();
            for (;observed<count;++observed) if (socket.chunks[observed]->wBufferIndex+1==socket.chunks[observed]->wBufferTotal)
                acknowledge(*socket.chunks[observed]);
        }
        check(!sender.IsSending(1,0x0100007f),"real GUI-generated transfer completes through actual send/ACK handlers");
    };
    const auto nativePath=root/fs::path{L"\u76ee\u5f55.bin"};
    std::ofstream{nativePath,std::ios::binary}<<"native-path";
    const std::array<CTransmitFileMgr::Source,1> nativeSources{{{"native.bin",nativePath}}};
    socket.chunks.clear();
    check(sender.StartSending(1,0x0100007f,nativeSources,etftPatch),"native filesystem source path needs no lossy narrow conversion");
    finish();
    check(socket.chunks.size()==1 && std::string_view{socket.chunks[0]->szBuffer,socket.chunks[0]->wBufferSize}=="native-path",
        "native source bytes reach the existing fixed wire packet unchanged");
    const auto config=root/"custom config";
    fs::create_directories(config/"1_Test/127.0.0.1");
    std::ofstream{config/"1_Test/area_config.lua",std::ios::binary}<<"area\n";
    std::ofstream{config/"1_Test/127.0.0.1/guard_area_config.lua",std::ios::binary}<<"guard\n";
    std::ofstream{config/"gmsecurityconfig.lua",std::ios::binary}<<"szPwd=\"123\"\n";
    std::strcpy(g_MTConfig.szAreaConfigPath,"custom config");
    std::ofstream{root/"version/release.zip",std::ios::binary}<<"VERSION";
    std::ofstream{root/"patch/user_patch.pak",std::ios::binary}<<"PATCH";
    auto* versions=window.findChild<QListWidget*>("listVersion");auto* patches=window.findChild<QListWidget*>("listPatch");
    if (!versions || !patches) throw std::runtime_error("missing package selectors");
    versions->clear();versions->addItem("release.zip");versions->setCurrentRow(0);
    patches->clear();patches->addItem("user_patch.pak");patches->setCurrentRow(0);
    check(window.SetAreaVersion(1,"release.zip"),"current version is recorded for config transfer");
    socket.chunks.clear();window.SendConfig();check(sender.IsSending(1,0x0100007f),"GUI config action uses the configured area root");finish();
    check(socket.chunks.size()==3 && std::string_view{socket.chunks[0]->szFileName}=="guard_area_config.lua" &&
        std::string_view{socket.chunks[0]->szBuffer,socket.chunks[0]->wBufferSize}=="guard\n" &&
        std::string_view{socket.chunks[2]->szBuffer,socket.chunks[2]->wBufferSize}=="szPwd=\"\x60" "ab\"\r\n",
        "config action preserves file ordering/names and generated legacy bytes without fixed path buffers");
    socket.chunks.clear();window.SendVersion();check(sender.IsSending(1,0x0100007f),"GUI version action creates owned native paths");finish();
    check(socket.chunks.size()==4 && std::string_view{socket.chunks[0]->szFileName}=="release.zip" &&
        std::string_view{socket.chunks[1]->szFileName}=="release.zip-guard_area_config.lua" &&
        std::string_view{socket.chunks[3]->szFileName}=="release.zip-gmsecurityconfig.lua","version action preserves its four-file protocol naming");
    const auto encrypted=read(config/"gmsecurityconfig-encrypted.lua");
    std::ofstream{config/"gmsecurityconfig.lua",std::ios::binary}<<"szPwd=invalid";
    window.SendConfig();
    check(!sender.IsSending(1,0x0100007f) && read(config/"gmsecurityconfig-encrypted.lua")==encrypted,
        "invalid security input cannot publish a truncated file or start a GUI transfer");
    socket.chunks.clear();window.SendPatch();check(sender.IsSending(1,0x0100007f),"patch action is independent of security-file generation");finish();
    check(socket.chunks.size()==1 && std::string_view{socket.chunks[0]->szBuffer,socket.chunks[0]->wBufferSize}=="PATCH",
        "patch GUI action retains selected source bytes");
    versions->addItem(QString{128,'x'}+".zip");versions->setCurrentRow(1);
    std::array<char,COMMON_NAME_LEN> selectedName{};std::strcpy(selectedName.data(),"retained");
    window.SendVersion();
    check(!sender.IsSending(1,0x0100007f) && !window.GetSelectVersion(selectedName.data()) &&
        std::string_view{selectedName.data()}=="retained","overlong legacy wire names fail visibly instead of truncating");
    versions->clear();patches->clear();window.SetAreaVersion(1,"");
    fs::remove(root/"version/release.zip");fs::remove(root/"patch/user_patch.pak");
    std::strcpy(g_MTConfig.szAreaConfigPath,"area_config");
    check(allocator.packets.empty(),"export/GUI transfer checks leave no packet ownership outstanding");
}
