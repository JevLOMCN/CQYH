// Actual GUI-thread delivery and timer behavior, without a native connection.
void gui_lifecycle_tests(MaintainTool& window,Net& net)
{
    auto* log=window.findChild<QListWidget*>("listLog");
    auto* filter=window.findChild<QLineEdit*>("editFliter");
    if (!log || !filter) throw std::runtime_error("missing log widgets");
    check(window.findChildren<QGraphicsScene*>().size()==10,"all main and monitor scenes are QObject children");
    std::atomic<bool> wrongThread=false;
    const auto connection=QObject::connect(log->model(),&QAbstractItemModel::rowsInserted,&window,
        [&](const QModelIndex&,int,int) { if (QThread::currentThread()!=window.thread()) wrongThread=true; });
    const auto drain=[&] { QCoreApplication::sendPostedEvents(&window,QEvent::MetaCall); };
    window.CleanLog();
    {
        std::jthread worker{[] {
            char borrowed[]{'A','\t','B','\n','C','X'};
            MaintainTool::LogOutput(lltInfo,borrowed,5);
            std::ranges::fill(borrowed,'Z');
        }};
    }
    check(log->count()==0,"worker logging queues data without touching Qt models");
    drain();
    check(log->count()==1 && log->item(0)->text()=="A  BC","queued log owns its buffer and respects its supplied length");
    MaintainTool::LogOutput(lltError,nullptr,4);MaintainTool::LogOutput(lltError,"ignored",0);
    drain();check(log->count()==1,"empty and null log callbacks are ignored");
    {
        std::vector<std::jthread> workers;
        for (int thread=0;thread<4;++thread) workers.emplace_back([thread] {
            for (int row=0;row<400;++row) {
                const auto text=std::to_string(thread)+":"+std::to_string(row);
                MaintainTool::LogOutput(lltWarning,text.data(),static_cast<INT32>(text.size()));
            }
        });
    }
    check(log->count()==1,"concurrent log flood remains queued until the GUI handles it");
    drain();
    check(log->count()==1024 && !wrongThread.load(),"pending and displayed logs remain bounded and every model insertion occurs on the GUI thread");
    filter->setText("keep");
    MaintainTool::LogOutput(lltError,"drop",4);MaintainTool::LogOutput(lltLua,"keep",4);drain();
    check(log->item(log->count()-2)->isHidden() && !log->item(log->count()-1)->isHidden(),
        "newly delivered logs respect the active filter");
    check(log->item(log->count()-1)->foreground().color()==QColor{Qt::blue},"legacy severity colors are retained");
    filter->clear();
    const auto drive=[](int milliseconds) {
        QEventLoop loop;QTimer::singleShot(milliseconds,&loop,&QEventLoop::quit);return loop.exec();
    };
    const auto before=net.polls;drive(130);
    check(net.polls==before,"window initialization does not start transport polling prematurely");
    QTimerEvent unrelated{-1},inactive{0};window.timerEvent(&unrelated);window.timerEvent(&inactive);window.timerEvent(nullptr);
    check(net.polls==before,"unrelated timer events cannot poll the transport");
    check(window.StartPolling() && window.StartPolling(),"ready window starts one idempotent polling timer");
    drive(230);check(net.polls>before,"owned timer polls the borrowed transport");
    const auto prior=net.polls;
    net.onPoll=[&] { window.UnInit();return TRUE; };
    drive(230);net.onPoll={};
    check(net.polls==prior+1,"shutdown from a poll callback kills the timer before another tick");
    const auto stoppedCount=log->count();
    MaintainTool::LogOutput(lltInfo,"stopped",7);drain();
    check(log->count()==stoppedCount && !window.StartPolling(),"stopped window rejects logging and polling");
    check(window.Init(),"scoped window can reinitialize after a completed stop");
    MaintainTool::LogOutput(lltInfo,"stale",5);
    check(window.UnInit() && window.Init(),"stop and restart invalidate queued log generations");
    MaintainTool::LogOutput(lltInfo,"fresh",5);drain();
    check(log->item(log->count()-1)->text()=="fresh" &&
        (log->count()<2 || log->item(log->count()-2)->text()!="stale"),"stale queued callback cannot drain logs into a restarted window");
    for (bool throws : {false,true})
    {
        check(window.StartPolling(),"failure fixture owns a polling timer");
        const auto failedBefore=net.polls;
        net.onPoll=[throws]() -> BOOL { if (throws) throw std::runtime_error("injected GUI poll exception");return FALSE; };
        const auto runApplication=[](int milliseconds,int code) {
            QTimer watchdog;watchdog.setSingleShot(true);
            QObject::connect(&watchdog,&QTimer::timeout,QCoreApplication::instance(),[code] { QCoreApplication::exit(code); });
            watchdog.start(milliseconds);
            return QCoreApplication::exec();
        };
        check(runApplication(1000,99)==EXIT_FAILURE,"poll failure reaches the actual application event-loop exit code");
        net.onPoll={};
        check(runApplication(130,0)==0,"stopped application can drain events without another network poll");
        check(net.polls==failedBefore+1 && !window.StartPolling(),"failed and throwing polls stop the timer and do not escape into Qt");
        check(window.Init(),"window resources remain restartable after a poll failure");
    }
    check(window.UnInit() && window.Cleanup(),"GUI cleanup is idempotent after sessions have already closed");
    QObject::disconnect(connection);
    check(window.Init(),"destruction fixture leaves the scoped window initialized with no polling timer");
    MaintainTool::LogOutput(lltInfo,"pending at destruction",22);
}
