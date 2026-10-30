void statistics_ui_tests(const fs::path& root,MaintainTool& window,Allocator& allocator,Session& socket,
    SRemoteGuardSession& record,QTreeWidget& processes)
{
    auto saved=std::move(record.poStatisInfo);record.poStatisInfo=std::make_unique<stProcessStatisInfo>();
    auto* table=window.findChild<QTreeWidget*>("treeStatisInfo");
    auto* monitor=window.findChild<QGraphicsView*>("monitor_1");
    auto* cpuMonitor=window.findChild<QGraphicsView*>("monitor_2");
    auto* areas=window.findChild<QTreeWidget*>("treeArea");
    if (!table || !monitor || !cpuMonitor || !areas) throw std::runtime_error("missing statistics widgets");
    const auto memRows=table->findItems("MemUsage",Qt::MatchExactly),cpuRows=table->findItems("CpuUsage",Qt::MatchExactly);
    if (memRows.empty() || cpuRows.empty()) throw std::runtime_error("missing metric rows");
    auto* memory=memRows.front();auto* cpu=cpuRows.front();
    QTreeWidgetItem* selected=nullptr;
    for (int index=0;index<processes.topLevelItemCount();++index)
        if (processes.topLevelItem(index)->text(0).endsWith("-100")) selected=processes.topLevelItem(index);
    if (!selected) throw std::runtime_error("missing selected statistics process");
    processes.clearSelection();selected->setSelected(true);memory->setSelected(true);
    auto packet=std::make_unique<U2N_UPDATE_STATIS_INFO>();packet->wMsg=u2n_update_statis_info;
    const auto sample=[&](UINT32 pid,int slot,UINT64 value) {
        std::memset(&packet->oStatisInfo,0,sizeof(packet->oStatisInfo));
        auto& process=packet->oStatisInfo.ProcessInfo[slot];process.bUsed=TRUE;process.dwProcessID=pid;
        process.qwStatisEntry[statMemUsage]=value;process.qwStatisEntry[statCpuUsage]=UINT64_MAX;
        deliver(allocator,socket,*packet);
    };
    sample(100,7,10);
    check(memory->text(1)=="10" && memory->text(3)=="10" && memory->text(4)=="10" && memory->text(5)=="10",
        "actual Qt table initializes min/max from the first sample rather than zero");
    window.OnStatisInfoItemDoubleClicked(memory,1);window.OnStatisInfoItemDoubleClicked(cpu,1);
    for (int redraw=0;redraw<10;++redraw) window.UpdateGuardStatisInfoByIp(1,0x0100007f,100);
    check(memory->text(5)=="10" && record.poStatisInfo->totals.at(100)[statMemUsage].qwTotal==10,
        "GUI redraw cannot increment lifetime totals");
    sample(100,23,30);
    check(memory->text(1)=="30" && memory->text(3)=="10" && memory->text(4)=="30" && memory->text(5)=="40",
        "Qt metrics follow PID movement across wire slots");
    const auto graphLabel=[](QGraphicsScene* scene,const QString& text) {
        for (auto* item:scene->items()) if (auto* label=qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);label && label->text()==text) return true;
        return false;
    };
    check(graphLabel(monitor->scene(),"30") && graphLabel(cpuMonitor->scene(),QString::number(static_cast<qulonglong>(UINT64_MAX))),
        "pinned graphs retain full-width maxima");
    int lines=0;
    for (auto* item:cpuMonitor->scene()->items()) if (auto* line=qgraphicsitem_cast<QGraphicsLineItem*>(item)) {
        ++lines;check(std::isfinite(line->line().y1()) && line->line().y1()>=20 && line->line().y1()<=220 &&
            line->line().y2()>=20 && line->line().y2()<=220,"full-width graph scaling stays inside its canvas");
    }
    check(lines==1,"chronological graph joins the two observed samples");
    sample(200,23,99999);
    check(memory->text(1).isEmpty() && graphLabel(monitor->scene(),"30") && !graphLabel(monitor->scene(),"99999"),
        "missing current PID clears table values without drawing another process's history");
    sample(100,0,20);
    check(memory->text(3)=="10" && memory->text(4)=="30" && memory->text(5)=="60","returning PID retains its independent lifetime metrics");
    const auto closes=socket.closes;auto* oldRoot=areas->topLevelItem(0);
    QObject scope;
    const auto changed=QObject::connect(table->model(),&QAbstractItemModel::dataChanged,&scope,[&] { window.VersionRefresh(); });
    sample(100,1,666);
    QObject::disconnect(changed);
    check(socket.closes==closes && socket.data==&record && areas->topLevelItem(0)==oldRoot && memory->text(1)=="666",
        "model callbacks cannot reenter configuration publication during rendering");
    check(window.BeginHibernateGuardStatisInfoByIp(1,0x0100007f,100,stGS,1) &&
        window.BeginHibernateGuardStatisInfoByIp(1,0x0100007f,101,stGS,2),"two process streams are owned during statistics delivery");
    sample(101,4,777);
    fs::path first,second;
    for (const auto& file:fs::directory_iterator(root/"log"))
    {
        const auto name=file.path().filename().string();
        if (name.starts_with("hibernate_") && name.find("=100@")!=name.npos) first=file.path();
        if (name.starts_with("hibernate_") && name.find("=101@")!=name.npos) second=file.path();
    }
    const auto read=[](const fs::path& path) { std::ifstream input{path,std::ios::binary};return std::string{std::istreambuf_iterator<char>{input},{}}; };
    const auto missing=read(first),present=read(second);
    check(std::ranges::count(missing,'\n')==1 && std::ranges::count(present,'\n')==2 && present.find("\t777\t")!=present.npos,
        "an absent first PID does not suppress later hibernation rows");
    window.EndHibernateGuardStatisInfoByIp();fs::remove(first);fs::remove(second);
    const auto cursor=record.poStatisInfo->nAccumuIndex;const auto rejected=socket.closes;
    packet->oStatisInfo.ProcessInfo[5]=packet->oStatisInfo.ProcessInfo[4];
    deliver(allocator,socket,*packet);
    check(socket.closes==rejected+1 && record.poStatisInfo->nAccumuIndex==cursor,"duplicate PID packet is rejected before history mutation");
    record.poStatisInfo=std::move(saved);
    window.UpdateGuardStatisInfoByIp(1,0x0100007f,100);
    const std::string longVersion(127,'x');
    auto* guard=areas->topLevelItem(0)->child(0);
    const auto ackCloses=socket.closes;auto* ackRoot=areas->topLevelItem(0);
    const auto ackChanged=QObject::connect(areas->model(),&QAbstractItemModel::dataChanged,&scope,[&] { window.VersionRefresh(); });
    check(window.DeployVersionResultByIp(1,0x0100007f,longVersion.c_str(),errSuccess) &&
        guard->text(4)==QStringLiteral("deploy ok: ")+QString::fromStdString(longVersion),
        "maximum-length deployment acknowledgement cannot overflow a GUI formatting buffer");
    check(window.HotUpdatePatchResultByIp(1,0x0100007f,longVersion.c_str(),errSuccess) &&
        guard->text(4)==QStringLiteral("hot patch ok: ")+QString::fromStdString(longVersion),
        "maximum-length patch acknowledgement retains all text without a fixed buffer");
    QObject::disconnect(ackChanged);
    check(socket.closes==ackCloses && areas->topLevelItem(0)==ackRoot,"ACK model notifications cannot replace items still being updated");
}

void connection_statistics_tests(MaintainTool& window,Allocator& allocator,Session& socket,Net& net)
{
    auto& sessions=CSessionMgr::GetInstance();auto& handler=CMTMessageHandler::GetInstance();
    auto* record=sessions.GetRemoteGDSession(1,0x0100007f);
    if (!record) throw std::runtime_error("missing reconnect record");
    record->pSocketSession=&socket;socket.SetUserData(record);
    auto* area=window.findChild<QTreeWidget*>("treeArea");
    QObject scope;bool retired=false;
    const auto sent=socket.sent.size();
    const auto changed=QObject::connect(area->model(),&QAbstractItemModel::dataChanged,&scope,[&] {
        if (!retired) { retired=true;sessions.UnInit(); }
    });
    handler.OnConnected(&socket);QObject::disconnect(changed);
    check(retired && !socket.data && socket.sent.size()==sent,"connected callback does not register a session retired by GUI notification");
    check(sessions.Init(&net),"session bindings can restart after callback shutdown");
    record=sessions.GetRemoteGDSession(1,0x0100007f);
    record->pSocketSession=&socket;socket.SetUserData(record);
    record->poStatisInfo->totals[999][statMemUsage]={1,2,3};
    handler.OnConnected(&socket);
    check(record->bConnected && record->poStatisInfo->totals.empty() && record->poStatisInfo->nAccumuIndex==0,
        "reconnect starts a fresh statistics lifetime");
    Session inbound{allocator};handler.OnAccepted(nullptr,&inbound);
    check(inbound.closes==1 && !inbound.data,"unexpected inbound connection is closed instead of left unowned");
    handler.BeginShutdown();Session late{allocator};const auto before=late.sent.size();
    handler.OnConnected(&late);
    check(late.closes==1 && late.sent.size()==before,"shutdown gate closes a late connection without registration");
}
