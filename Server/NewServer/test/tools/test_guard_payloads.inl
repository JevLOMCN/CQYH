// Included inside the result fixture's anonymous namespace. All peers are memory
// sockets; process registration only adds metadata or reads this test's own PID.
template<class Message>
void deliver(Runtime& runtime, Session& socket, const Message& message, std::string_view tail = {})
{
    const auto size = sizeof(message) + tail.size();
    const auto previousPackets = runtime.allocator.packets.size();
    auto* packet = runtime.allocator.Alloc(static_cast<INT32>(size));
    std::memcpy(packet, &message, sizeof(message));
    if (!tail.empty()) std::memcpy(packet + sizeof(message), tail.data(), tail.size());
    CGDMessageHandler::GetInstance().OnRecv(&socket, packet, static_cast<UINT32>(size));
    check(runtime.allocator.packets.size() == previousPackets, "receive and response packets have balanced ownership");
}

template<class Message>
void deliverText(Runtime& runtime, Session& socket, UINT16 id, std::string_view text)
{
    Message message{};
    message.wMsg = id;
    auto tail = std::string{text};
    tail += '\0';
    deliver(runtime, socket, message, tail);
}

void payloads(Runtime& runtime)
{
    auto& handler = CGDMessageHandler::GetInstance();
    Session invalid{runtime.allocator};
    const auto reject = [&](UINT16 id, std::size_t size) {
        auto* packet = runtime.allocator.Alloc(static_cast<INT32>(size));
        std::memset(packet, 0x7f, size);
        MESSAGE_HEADER header{};
        header.wMsg = id;
        std::memcpy(packet, &header, sizeof(header));
        const auto closes = invalid.closes;
        handler.OnRecv(&invalid, packet, static_cast<UINT32>(size));
        check(invalid.closes == closes + 1 && runtime.allocator.packets.empty(),
            "invalid bodies close and release before dispatch");
    };
    for (const auto [id, size] : std::array<std::pair<UINT16, std::size_t>, 6>{{
        {a2a_ping, sizeof(A2A_PING)}, {a2u_regrister, sizeof(A2U_REGRISTER)},
        {n2u_regrister, sizeof(N2U_REGRISTER)}, {a2u_notify, sizeof(A2U_NOTIFY) + 1},
        {m2u_command_ack, sizeof(M2U_COMMAND_RESPONSE) + 1}, {n2u_exec_cmd, sizeof(N2U_UPDATE_CMD) + 1}}})
    {
        for (auto length = sizeof(MESSAGE_HEADER); length < size; ++length) reject(id, length);
        if (id == a2a_ping || id == a2u_regrister || id == n2u_regrister) reject(id, size + 1);
        if (id == n2u_regrister || id == a2u_notify || id == m2u_command_ack || id == n2u_exec_cmd)
            reject(id, size); // Correct size but no terminating zero.
    }
    reject(n2u_exec_cmd, MAX_MESSAGE_SIZE + 1);
    for (const std::string content : {std::string{}, std::string{"progress 100% %s %n"}, std::string(8192, 'x')})
    {
        A2U_NOTIFY message{};
        message.wMsg = a2u_notify;
        message.bySvrType = stGS;
        message.bySvrID = 3;
        message.byLevel = 2;
        std::string tail = content;
        tail += '\0';
        deliver(runtime, runtime.ms, message, tail);
        const auto& forwarded = runtime.mt.notifications.back();
        check(forwarded.starts_with("NOTIFY_2 [GS:3] ") && forwarded.size() <= 4095,
            "notification prefix and legacy byte limit preserved");
        if (content.size() < 100) check(forwarded.ends_with(content), "notification percent signs remain literal text");
    }
    A2U_NOTIFY unknown{};
    unknown.wMsg = a2u_notify;
    unknown.bySvrType = 255;
    deliver(runtime, runtime.ms, unknown, std::string_view{"unknown\0", 8});
    check(runtime.mt.notifications.back().find("[(null):0]") != std::string::npos,
        "unknown server names never become null format arguments");
    for (const std::string result : {std::string{}, std::string{"result 100%"}, std::string(12000, 'z')})
    {
        deliverText<M2U_COMMAND_RESPONSE>(runtime, runtime.ms, m2u_command_ack, result);
        check(runtime.mt.replies.back().info == result, "variable response tails forwarded within received bounds");
    }
    for (const std::string command : {std::string{}, std::string{"not-a-command %s"}, std::string(4096, 'x')})
    {
        deliverText<N2U_UPDATE_CMD>(runtime, runtime.mt, n2u_exec_cmd, command);
        check(runtime.mt.replies.back().info.starts_with("ExecCmd [") && runtime.mt.replies.back().info.size() <= 255,
            "failed command results retain the terminated legacy byte limit");
    }
    deliverText<N2U_UPDATE_CMD>(runtime, runtime.mt, n2u_exec_cmd, "quit");
    check(runtime.mt.replies.back().info == "ExecCmd [quit] issue ok", "valid operator command uses the existing dispatcher");
    auto& ms = CGDSessionMgr::GetInstance().GetMSSession();
    ms.pSocketSession = &runtime.ms;
    runtime.ms.SetUserData(&ms);
    check(CProcessMgr::GetInstance().AddProcessInfo(stMS, GetCurrentProcessId(), 0, FALSE), "register fixture PID as metadata only");
    deliverText<N2U_UPDATE_CMD>(runtime, runtime.mt, n2u_exec_cmd, ".fixture command");
    check(runtime.ms.commands.back() == ".fixture command", "GM command forwards to the memory MS socket");
    CProcessMgr::GetInstance().UnInit();
    ms.pSocketSession = nullptr;
    runtime.ms.SetUserData(nullptr);
    runtime.ms.commands.clear();
    const auto text = std::string(8192, 'x');
    check(handler.DoMTError(errCritical, "%s", text.c_str()) && runtime.mt.replies.back().info.size() == 127,
        "fixed error text cannot spill beyond its wire array");
    const auto huge = std::string(MAX_MESSAGE_SIZE * 2, 'x');
    check(handler.DoMTUpdateCmdAck(errSuccess, huge.c_str()) && runtime.mt.replies.back().info.size() ==
        MAX_MESSAGE_SIZE - sizeof(U2N_UPDATE_CMD_ACK) - 1, "outgoing flexible result fits the transport size bound");
    for (bool throwing : {false, true})
    {
        runtime.mt.fail_send = !throwing;
        runtime.mt.throw_send = throwing;
        check(!handler.DoMTUpdateCmdAck(errSuccess, "unsent") && runtime.allocator.packets.empty(), "failed result send releases ownership");
        check(!handler.DoMTError(errCritical, "unsent") && runtime.allocator.packets.empty(), "failed error send releases ownership");
        check(!handler.DoMTNotify(stGS, 1, 0, 0, "unsent") && runtime.allocator.packets.empty(), "failed notification send releases ownership");
        runtime.mt.fail_send = runtime.mt.throw_send = false;
    }
    runtime.allocator.fail_next = true;
    check(!handler.DoMTUpdateCmdAck(errSuccess, "allocation failure") && runtime.allocator.packets.empty(), "result allocation failure retains no packet");
    runtime.allocator.fail_next = true;
    check(!handler.DoMTError(errCritical, "allocation failure") && runtime.allocator.packets.empty(), "error allocation failure retains no packet");
    runtime.allocator.fail_next = true;
    check(!handler.DoMTNotify(stGS, 1, 0, 0, "allocation failure") && runtime.allocator.packets.empty(), "notification allocation failure retains no packet");
}

struct RegistrationPeers
{
    Runtime& runtime;
    std::vector<std::unique_ptr<Session>> peers;
    explicit RegistrationPeers(Runtime& value) : runtime(value)
    {
        check(CGDSessionMgr::GetInstance().UnInit() && CGDSessionMgr::GetInstance().Init(&runtime.net),
            "registration fixture starts with empty records");
    }
    Session& add()
    {
        auto& socket = *peers.emplace_back(std::make_unique<Session>(runtime.allocator));
        socket.on_close = [&socket] { CGDMessageHandler::GetInstance().OnDisconnected(&socket, FALSE); };
        check(CGDSessionMgr::GetInstance().AddServerSession(&socket), "accept a memory registration peer");
        return socket;
    }
    ~RegistrationPeers()
    {
        for (auto& socket : peers) socket->on_close = {};
        CGDSessionMgr::GetInstance().UnInit(); // Detach before owned memory peers die.
        CGDSessionMgr::GetInstance().Init(&runtime.net);
        auto& mt = CGDSessionMgr::GetInstance().GetMTSession();
        mt.pSocketSession = &runtime.mt;
        runtime.mt.SetUserData(&mt);
        CProcessMgr::GetInstance().UnInit();
    }
};

void registrations(Runtime& runtime)
{
    RegistrationPeers bank{runtime};
    auto& handler = CGDMessageHandler::GetInstance();
    auto& sessions = CGDSessionMgr::GetInstance();
    auto process = [&](Session& socket, UINT8 type, UINT8 id, UINT32 pid) {
        A2U_REGRISTER message{};
        message.wMsg = a2u_regrister;
        message.byType = type;
        message.bySessionID = id;
        message.dwProcessID = pid;
        deliver(runtime, socket, message);
    };
    UINT32 pid = 0x7fffff00;
    for (UINT8 type : {stMS, stDB, stBS, stLS, stGT, stGS, stGM, stDL, stMM, stMG})
    {
        auto& socket = bank.add();
        process(socket, type, 1, pid++);
        const auto* record = static_cast<ServerSession*>(socket.GetUserData());
        check(record && record->pSocketSession == &socket && record->dwType == type && record->dwSessionID == 1 &&
            record->eState == epsNormal && record->dwProcessID == pid - 1, "each server type registers its correct stable slot");
    }
    for (const auto [type, id] : std::array<std::pair<UINT8, UINT8>, 7>{{
        {stMG, MAX_MG_SESSION_COUNT}, {stGS, MAX_GS_SESSION_COUNT}, {stGT, MAX_GT_SESSION_COUNT},
        {stLS, 0}, {stMS, 2}, {stClient, 1}, {255, 1}}})
    {
        auto& socket = bank.add();
        process(socket, type, id, pid++);
        check(socket.closes == 1 && !socket.GetUserData(), "invalid type or slot closes without assignment");
    }
    auto& zeroPid = bank.add(); process(zeroPid, stGS, 2, 0);
    check(zeroPid.closes == 1 && !sessions.GetGSSession(2)->pSocketSession, "zero PID cannot partially register a slot");
    const auto timeout = g_GuardConfig.nServerPingTimeout;
    g_GuardConfig.nServerPingTimeout = -1;
    auto& invalidTimeout = bank.add(); process(invalidTimeout, stGS, 3, pid++);
    check(invalidTimeout.closes == 1 && !sessions.GetGSSession(3)->pSocketSession, "negative timeout cannot become an infinite registration deadline");
    g_GuardConfig.nServerPingTimeout = timeout;
    auto* old = sessions.GetGSSession(1)->pSocketSession;
    auto& replacement = bank.add(); process(replacement, stGS, 1, pid++);
    check(old && !old->GetUserData() && sessions.GetGSSession(1)->pSocketSession == &replacement,
        "duplicate process registration retires old socket before publishing replacement");
    handler.OnDisconnected(old, FALSE);
    check(sessions.GetGSSession(1)->pSocketSession == &replacement, "delayed old disconnect preserves replacement");
    auto& interrupted = bank.add();
    auto& nested = bank.add();
    replacement.on_close = [&] { handler.OnDisconnected(&replacement, FALSE); process(nested, stGS, 1, pid++); };
    process(interrupted, stGS, 1, pid++);
    check(sessions.GetGSSession(1)->pSocketSession == &nested && interrupted.closes == 1,
        "reentrant registration is not overwritten by a stale caller");
    nested.fail_close = true;
    auto& failedClose = bank.add(); process(failedClose, stGS, 1, pid++);
    check(!nested.GetUserData() && !sessions.GetGSSession(1)->pSocketSession && failedClose.closes == 1,
        "failed retirement cannot restore a borrowed socket after its callback");

    std::strcpy(g_GuardConfig.szMTUser, "Operator");
    std::strcpy(g_GuardConfig.szMTPwd, "Password");
    const auto login = [&](Session& socket, const char* name, const char* password) {
        N2U_REGRISTER message{};
        message.wMsg = n2u_regrister;
        std::strncpy(message.szName, name, sizeof(message.szName) - 1);
        std::strncpy(message.szPwd, password, sizeof(message.szPwd) - 1);
        deliver(runtime, socket, message);
    };
    for (const auto& [name, password] : std::array<std::pair<const char*, const char*>, 4>{{
        {"OperatorSuffix", "Password"}, {"Operator", "PasswordSuffix"}, {"Oper", "Password"}, {"Operator", "Pass"}}})
    {
        auto& socket = bank.add(); login(socket, name, password);
        check(socket.closes == 1 && !sessions.GetMTSession().pSocketSession, "credentials compare complete bounded values");
    }
    auto& first = bank.add(); login(first, "operator", "PASSWORD");
    check(sessions.GetMTSession().pSocketSession == &first && !first.replies.empty() && first.replies.back().error == errSuccess,
        "full login preserves legacy ASCII case-insensitive comparison");
    auto& second = bank.add(); login(second, "Operator", "Password");
    check(first.closes == 1 && !first.GetUserData() && sessions.GetMTSession().pSocketSession == &second,
        "new MT retires old peer and registers successfully");
    handler.OnDisconnected(&first, FALSE);
    check(sessions.GetMTSession().pSocketSession == &second, "delayed MT disconnect preserves replacement");
    std::memset(g_GuardConfig.szMTUser, 'x', sizeof(g_GuardConfig.szMTUser));
    auto& invalidConfig = bank.add(); login(invalidConfig, "Operator", "Password");
    check(invalidConfig.closes == 1 && sessions.GetMTSession().pSocketSession == &second,
        "unterminated configured name never becomes an unbounded read or retires the live peer");
    std::memset(g_GuardConfig.szMTUser, 0, sizeof(g_GuardConfig.szMTUser));
    std::memset(g_GuardConfig.szMTPwd, 0, sizeof(g_GuardConfig.szMTPwd));
    auto& disconnecting = bank.add(); process(disconnecting, stGS, 2, pid++);
    auto& duringNotify = bank.add();
    second.on_send = [&](UINT16 id) {
        if (id == n2u_update_process_info) process(duringNotify, stGS, 2, pid++);
    };
    handler.OnDisconnected(&disconnecting, FALSE);
    check(sessions.GetGSSession(2)->pSocketSession == &duringNotify && duringNotify.GetUserData() == sessions.GetGSSession(2) &&
        sessions.GetGSSession(2)->eState == epsNormal,
        "disconnect notification reentrancy cannot clear a newly registered replacement");
    handler.OnDisconnected(&disconnecting, FALSE);
    check(sessions.GetGSSession(2)->pSocketSession == &duringNotify, "repeated old disconnect remains harmless");
}
