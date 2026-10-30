void send_path_tests(const fs::path& root,MaintainTool& window,Allocator& allocator,Session& socket,SRemoteGuardSession& record)
{
    MaintainUrlHandler sink;
    QDesktopServices::setUrlHandler("file",&sink,"receive");
    struct Reset { ~Reset() { QDesktopServices::unsetUrlHandler("file"); } } reset;
    auto* areas=window.findChild<QTreeWidget*>("treeArea");
    window.OnAreaItemDoubleClicked(nullptr,0);window.OnAreaItemDoubleClicked(areas->topLevelItem(0),0);
    check(sink.urls.empty(),"invalid folder targets do not launch an application");
    window.OnAreaItemDoubleClicked(areas->topLevelItem(0)->child(0),0);
    window.OnVesionWidgetDoubleClicked({});window.OnPatchWidgetDoubleClicked({});
    check(sink.urls.size()==3,"native file URL requests are intercepted without launching Explorer");
    const std::array<fs::path,3> expected{root/"area_config/1_Test/127.0.0.1",root/"version",root/"patch"};
    for (std::size_t index=0;index<sink.urls.size() && index<expected.size();++index)
        check(fs::equivalent(fs::path{sink.urls[index].toLocalFile().toStdWString()},expected[index]),
            "GUI folder URL preserves the complete native path including spaces");
    auto& handler=CMTMessageHandler::GetInstance();
    const auto senders=std::array<std::function<BOOL()>,5>{
        [&] { return handler.DoPing(&record); },
        [&] { return handler.DoGDRegristerGD(&record,"fixture-user","fixture-password"); },
        [&] { return handler.DoGDExecCmd(1,0x0100007f,"fixture-command"); },
        [&] { return handler.DoGDDeployServerVersion(1,0x0100007f,"fixture.zip"); },
        [&] { return handler.DoGDHotUpdatePatch(1,0x0100007f,"fixture.pak"); }
    };
    for (const auto& send:senders)
    {
        check(send() && allocator.packets.empty(),"successful fixed/flexible send transfers packet ownership once");
        socket.rejectSend=true;check(!send() && allocator.packets.empty(),"rejected send frees its owned packet");socket.rejectSend=false;
        socket.throwSend=true;check(!send() && allocator.packets.empty(),"throwing send frees its owned packet");socket.throwSend=false;
        allocator.rejectAllocation=true;check(!send() && allocator.packets.empty(),"allocation failure returns without sending or leaking");allocator.rejectAllocation=false;
        allocator.throwAllocation=true;check(!send() && allocator.packets.empty(),"allocation exception is contained at the send boundary");allocator.throwAllocation=false;
    }
    const std::string large(128,'x'),credential(32,'x'),command(MAX_MESSAGE_SIZE,'x');
    check(!handler.DoGDDeployServerVersion(1,0x0100007f,large.c_str()) &&
        !handler.DoGDHotUpdatePatch(1,0x0100007f,large.c_str()) &&
        !handler.DoGDRegristerGD(&record,credential.c_str(),"fixture") &&
        !handler.DoGDExecCmd(1,0x0100007f,command.c_str()) && allocator.packets.empty(),
        "oversized protocol fields fail before allocation instead of truncating");
    const std::string longCommand(512,'x');
    check(window.ExecCmd(longCommand.c_str()) && window.UpdateCmdResult(0x0100007f,errSuccess,"fixture result"),
        "GUI command tracking owns text beyond the former 64-byte buffer");
    check(!window.ExecCmd(nullptr),"null GUI command is rejected");
}
void login_tests(MaintainTool& window)
{
    const auto priorQuit=QApplication::quitOnLastWindowClosed();
    QApplication::setQuitOnLastWindowClosed(false);
    struct Restore { bool value;~Restore() { QApplication::setQuitOnLastWindowClosed(value); } } restore{priorQuit};
    QObject scope;
    QTimer driver;bool tooLong=false,warning=false,accepted=false;
    QObject::connect(&driver,&QTimer::timeout,&scope,[&] {
        auto* dialog=qobject_cast<QDialog*>(QApplication::activeModalWidget());
        if (!dialog) return;
        auto* name=dialog->findChild<QLineEdit*>("loginUser");auto* password=dialog->findChild<QLineEdit*>("loginPassword");
        auto* feedback=dialog->findChild<QLabel*>("loginError");auto* buttons=dialog->findChild<QDialogButtonBox*>();
        if (!name || !password || !feedback || !buttons) return;
        if (!tooLong) { name->setText(QString{40,'x'});password->setText("fixture");tooLong=true;buttons->button(QDialogButtonBox::Ok)->click(); }
        else if (!feedback->text().isEmpty()) {
            check(dialog->grab().save(QString::fromStdWString((fs::current_path().parent_path()/"maintain-login-validation.png").native())),
                "inline validation dialog renders to a reviewable offscreen image");
            warning=true;name->setText("fixture-user");password->setText("fixture-password");
            accepted=true;driver.stop();buttons->button(QDialogButtonBox::Ok)->click();
        }
    });
    driver.start(1);window.Login();driver.stop();
    check(tooLong && warning && accepted && std::string_view{window.GetUserName()}=="fixture-user" &&
        std::string_view{window.GetPassword()}=="fixture-password","modal login rejects truncation, retains its fields and accepts valid credentials");
    window.Logout();
    check(!*window.GetUserName() && !*window.GetPassword(),"logout clears credential storage");
    QTimer::singleShot(0,&scope,[] {
        if (auto* dialog=qobject_cast<QDialog*>(QApplication::activeModalWidget())) dialog->reject();
    });
    window.Login();
    check(!*window.GetUserName() && !*window.GetPassword(),"cancelled login does not publish credentials");
}
