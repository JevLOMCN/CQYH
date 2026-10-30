#include "stdafx.h"
#include "maintaintool.h"
#include "net.h"
#include "dir.h"
#include "ifile.h"
#include "math_ex.h"
#include "time_ex.h"
#include "server_def.h"
#include "process_option.h"
#include "session_mgr.h"
#include "message_handler.h"
#include "statis_info.h"
#include "mt_config.h"
#include "transmit_file_mgr.h"
#include "maintain_tool_version.h"
#include "maintain_files.h"
#include "maintain_stats.h"
#include <QSignalBlocker>
#include <span>

#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/qtreewidget.h>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QDialogButtonBox>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>

#include <string>
#include <algorithm>
#include <bit>
#include <filesystem>
#include <format>
#include <iterator>
#include <deque>
#include <mutex>
#include <stdexcept>
#include <utility>
#include <QTimerEvent>
//#include "sdfile.h"
//#include "sddir.h"
//using namespace SGDP;

extern INet* g_pNet;

const char* g_szStatFieldName[statTotal] = {
    "",
    "MemUsage",
    "CpuUsage",
    "InternalPacketIncome",
    "InternalPacketOutgoing",
    "InternalNetflowIncome",
    "InternalNetflowOutgoing",
    "ExternalPacketIncome",
    "ExternalPacketOutgoing",
    "ExternalNetflowIncome",
    "ExternalNetflowOutgoing",
    "FindPathCount",
    "FindPathSamePolyReturnCount",
    "FindPathNormalReturnCount",
    "IsObstacledCount",
    "RoleCount",
    "NpcCount",
    "DoodadCount",
    "MissileCount",
    "SceneCount",
    "DropCount",
    "ItemCount",
};

#define MAX_MONITOR_COUNT   (9)

struct MONITOR_DATA
{
    QGraphicsView*  pGraphicsView;
    QGraphicsScene* pGraphicsScene;
    QLabel*         pLabel;
    UINT32          dwIp;
    UINT32          dwProcess;
    INT32           nIndex;
    INT32           nArea;
};

MONITOR_DATA    g_MonitorList[MAX_MONITOR_COUNT] = {0};
INT32           g_nLastSetMonitor = 0;

static const char* g_pcszConStr[emctTotal] = {
	"unconnect",
	"connecting",
	"connected",
	"normal",
};

enum enumAreaTextColumnIndex
{
	enumAreaTextColumnName = 0,
	enumAreaTextColumnAddress = 1,
	enumAreaTextColumnState,
	enumAreaTextColumnConfig,
	enumAreaTextColumnProgress,
};

enum enumDataUserRoleIndex
{
	enumDataUserRoleAreaID = 0,
	enumDataUserRoleVersion,
	enumDataUserRoleAreaName,
	enumDataUserRoleIP,
	enumDataUserRolePID,
	enumDataUserRoleType,
	enumDataUserRoleSessionID,
};

enum enumProcessTextColumnIndex
{
	enumProcessTextColumnPID = 0,
	enumProcessTextColumnType,
	enumProcessTextColumnState,
	enumProcessTextColumnMode,
};

namespace
{
constexpr int maxLogItems = 1024;
struct GuiMutation
{
    bool& flag;bool previous;
    explicit GuiMutation(bool& value):flag{value},previous{value} { flag=true; }
    ~GuiMutation() { flag=previous; }
};
std::filesystem::path path_field(std::span<const char> value);
std::string ip_text(UINT32 ip);
std::string wire_name(const QString& name);
struct LogSink
{
    std::mutex mutex;
    MaintainTool* target = nullptr; // Protected through invokeMethod scheduling and destruction.
    bool enabled = false, scheduled = false;
    std::uint64_t generation = 0;
    std::deque<std::pair<INT32, QString>> pending;
} logSink;
}
MaintainTool* MaintainTool::ms_Instance = nullptr;

//MaintainTool//
//*******************************************************************
MaintainTool::MaintainTool(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags)
{
	if (ms_Instance) throw std::logic_error("only one maintaintool window can be active");
	ui.setupUi(this);
	setWindowTitle(QString("MaintainTool msg %1 ver %2").arg(MAINTAIN_TOOLS_MSG_VERSION).arg(MAINTAIN_TOOLS_BUILD_VERSION));

    m_pGraphicsScene = new QGraphicsScene(this);
    ui.graphicsView->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    ui.graphicsView->setInteractive(false);
    ui.graphicsView->setScene(m_pGraphicsScene);

    g_MonitorList[0].pGraphicsView = ui.monitor_1;
    g_MonitorList[1].pGraphicsView = ui.monitor_2;
    g_MonitorList[2].pGraphicsView = ui.monitor_3;
    g_MonitorList[3].pGraphicsView = ui.monitor_4;
    g_MonitorList[4].pGraphicsView = ui.monitor_5;
    g_MonitorList[5].pGraphicsView = ui.monitor_6;
    g_MonitorList[6].pGraphicsView = ui.monitor_7;
    g_MonitorList[7].pGraphicsView = ui.monitor_8;
    g_MonitorList[8].pGraphicsView = ui.monitor_9;

    g_MonitorList[0].pLabel = ui.label_1;
    g_MonitorList[1].pLabel = ui.label_2;
    g_MonitorList[2].pLabel = ui.label_3;
    g_MonitorList[3].pLabel = ui.label_4;
    g_MonitorList[4].pLabel = ui.label_5;
    g_MonitorList[5].pLabel = ui.label_6;
    g_MonitorList[6].pLabel = ui.label_7;
    g_MonitorList[7].pLabel = ui.label_8;
    g_MonitorList[8].pLabel = ui.label_9;

    for (INT32 i = 0; i < MAX_MONITOR_COUNT; i++)
    {
        g_MonitorList[i].pGraphicsScene = new QGraphicsScene(this);
        g_MonitorList[i].pGraphicsView->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
        g_MonitorList[i].pGraphicsView->setInteractive(false);
        g_MonitorList[i].pGraphicsView->setScene(g_MonitorList[i].pGraphicsScene);
    }

	//ui.treeArea->setColumnWidth(enumAreaTextColumnName, 100);
	//ui.treeArea->setColumnWidth(enumAreaTextColumnAddress, 100);
	//ui.treeArea->setColumnWidth(enumAreaTextColumnState, 100);
	//ui.treeArea->setColumnWidth(enumAreaTextColumnConfig, 200);
	//ui.treeArea->setColumnWidth(enumAreaTextColumnProgress, 100);// sending progress

	ui.treeGuardProcesses->setColumnWidth(enumProcessTextColumnPID, 160);
	ui.treeGuardProcesses->setColumnWidth(enumProcessTextColumnType, 80);
	ui.treeGuardProcesses->setColumnWidth(enumProcessTextColumnState, 80);
	ui.treeGuardProcesses->setColumnWidth(enumProcessTextColumnMode, 80);

    ui.treeStatisInfo->setColumnWidth(0,150);
    ui.treeStatisInfo->setColumnWidth(1,60);
    ui.treeStatisInfo->setColumnWidth(2,60);
    ui.treeStatisInfo->setColumnWidth(3,60);
    ui.treeStatisInfo->setColumnWidth(4,60);
    ui.treeStatisInfo->setColumnWidth(5,60);

	QObject::connect(ui.actionLogin, SIGNAL(triggered()), this, SLOT(Login()));
	QObject::connect(ui.actionLogout, SIGNAL(triggered()), this, SLOT(Logout()));
	QObject::connect(ui.actionSnapShoot, SIGNAL(triggered()), this, SLOT(SnapShoot()));
	QObject::connect(ui.actionHibernate, SIGNAL(triggered()), this, SLOT(Hibernate()));

	QPushButton::connect(ui.listLog->model(), SIGNAL(rowsInserted(const QModelIndex&, int, int)), ui.listLog, SLOT(scrollToBottom()));
	QPushButton::connect(ui.listAlert->model(), SIGNAL(rowsInserted(const QModelIndex&, int, int)), ui.listAlert, SLOT(scrollToBottom()));
	QPushButton::connect(ui.ButtonClearLog, SIGNAL(clicked()), this, SLOT(CleanLog()));
	QPushButton::connect(ui.ButtonClearAlert, SIGNAL(clicked()), this, SLOT(CleanAlert()));
	QObject::connect(ui.editFliter, SIGNAL(textChanged(const QString&)), this, SLOT(LogFilter(const QString&)));
	QObject::connect(ui.editAlertFliter, SIGNAL(textChanged(const QString&)), this, SLOT(AlertFilter(const QString&)));

	QPushButton::connect(ui.buttonStart, SIGNAL(clicked()), this, SLOT(Start()));
	QPushButton::connect(ui.buttonOpen, SIGNAL(clicked()), this, SLOT(Open()));
	QPushButton::connect(ui.buttonStop, SIGNAL(clicked()), this, SLOT(Stop()));
	QPushButton::connect(ui.buttonClose, SIGNAL(clicked()), this, SLOT(Close()));
	QPushButton::connect(ui.buttonEnter, SIGNAL(clicked()), this, SLOT(ExecuteCmd()));

	QPushButton::connect(ui.ButtonLoadVersion, SIGNAL(clicked()), this, SLOT(VersionRefresh()));
	QPushButton::connect(ui.buttonSendConfig, SIGNAL(clicked()), this, SLOT(SendConfig()));
	QPushButton::connect(ui.buttonSendVersion, SIGNAL(clicked()), this, SLOT(SendVersion()));
	QPushButton::connect(ui.buttonDeployVersion, SIGNAL(clicked()), this, SLOT(DeployVersion()));
	QPushButton::connect(ui.buttonSendPatch, SIGNAL(clicked()), this, SLOT(SendPatch()));
	QPushButton::connect(ui.buttonHotUpdatePatch, SIGNAL(clicked()), this, SLOT(HotUpdatePatch()));

	QObject::connect(ui.treeStatisInfo, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),this, SLOT(OnStatisInfoItemDoubleClicked(QTreeWidgetItem *, int)));
	QObject::connect(ui.treeArea, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(OnAreaTreeClick(QTreeWidgetItem *, int)));
	QObject::connect(ui.treeArea, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(OnAreaItemDoubleClicked(QTreeWidgetItem *, int)));
	QObject::connect(ui.listVersion, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(OnVesionWidgetDoubleClicked(const QModelIndex &)));
	QObject::connect(ui.listPatch, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(OnPatchWidgetDoubleClicked(const QModelIndex &)));
	QObject::connect(ui.listLog, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(OnLogDoubleClicked(QListWidgetItem *)));
    std::scoped_lock lock(logSink.mutex);
    ms_Instance = this;
    logSink.target = this;
}

MaintainTool::~MaintainTool()
{
    UnInit();
    std::scoped_lock lock(logSink.mutex);
    logSink.target = nullptr;
    ms_Instance = nullptr;
    for (auto& monitor : g_MonitorList) monitor = {};
}

BOOL MaintainTool::Init()
try
{
    if (m_active || m_rendering) return FALSE;
    auto version = std::filesystem::absolute(path_field(g_MTConfig.szVersionPath));
    auto patch = std::filesystem::absolute(path_field(g_MTConfig.szPatchPath));
    auto versions = maintain::package_files(version, ".zip");
    auto patches = maintain::package_files(patch, ".pak");
    if (!versions) throw std::runtime_error(versions.error());
    if (!patches) throw std::runtime_error(patches.error());
    if (!_RefreshAreaInfo() || !InitGuardStatisInfo()) return FALSE;
    m_versionPath = std::move(version);
    m_patchPath = std::move(patch);
    ui.listVersion->clear();
    for (const auto& file : *versions) ui.listVersion->addItem(QString::fromStdWString(file.native()));
    ui.listVersion->sortItems(Qt::DescendingOrder);
    ui.listPatch->clear();
    for (const auto& file : *patches) ui.listPatch->addItem(QString::fromStdWString(file.native()));
    ui.listPatch->sortItems(Qt::DescendingOrder);
    m_nWaitCmdCount = 0;
    m_currentCommand.clear();
    m_bCmdResult = TRUE;
    m_bLogin = FALSE;
    m_active = true;
    {
        std::scoped_lock lock(logSink.mutex);
        ++logSink.generation;
        logSink.enabled = true;
    }
    return TRUE;
}
catch (const std::exception& error)
{
    ERR("cannot initialize maintaintool window: %s", error.what());
    return FALSE;
}

BOOL MaintainTool::UnInit()
{
    m_nWaitCmdCount=0;m_currentCommand.clear();m_bCmdResult=TRUE;
    m_active = false;
    m_bLogin = FALSE;
    if (const auto timer = std::exchange(m_timer, 0)) killTimer(timer);
    {
        std::scoped_lock lock(logSink.mutex);
        logSink.enabled = false;
        ++logSink.generation;
        logSink.pending.clear();
        logSink.scheduled = false;
    }
    std::ranges::fill(m_szUserName, '\0');
    std::ranges::fill(m_szPassword, '\0');
    return EndHibernateGuardStatisInfoByIp();
}

MaintainTool& MaintainTool::GetInstance()
{
    if (!ms_Instance) throw std::logic_error("maintaintool window is not alive");
    return *ms_Instance;
}

BOOL MaintainTool::Cleanup() { return UnInit(); }

BOOL MaintainTool::StartPolling()
{
    if (!m_active || !g_pNet) return FALSE;
    if (m_timer) return TRUE;
    m_timer = startTimer(100);
    return m_timer != 0;
}

void MaintainTool::timerEvent(QTimerEvent* event)
{
    if (!event || !m_active || !m_timer || m_refreshing || event->timerId() != m_timer || m_polling) return;
    auto* net = g_pNet;
    if (!net) return;
    m_polling = true;
    struct Reset { bool& value; ~Reset() { value = false; } } reset{m_polling};
    try
    {
        if (m_bLogin)
        {
            if (!CSessionMgr::GetInstance().MainLoop()) throw std::runtime_error("session polling failed");
            if (!m_active || g_pNet != net) return;
            if (!CTransmitFileMgr::GetInstance().MainLoop()) throw std::runtime_error("file transfer polling failed");
        }
        if (!m_active || g_pNet != net) return;
        if (!net->MainLoop()) throw std::runtime_error("network polling failed");
    }
    catch (const std::exception& error)
    {
        ERR("maintaintool polling failed: %s", error.what());
        UnInit();
        QCoreApplication::exit(EXIT_FAILURE);
    }
    catch (...)
    {
        ERR("maintaintool polling callback threw an exception");
        UnInit();
        QCoreApplication::exit(EXIT_FAILURE);
    }
}

void MaintainTool::LogOutput(INT32 level, const char* message, INT32 size)
{
    if (!message || size <= 0) return;
    try
    {
        // Copy the logger's borrowed buffer before returning to its worker thread.
        auto text = QString::fromUtf8(message, std::min(size, 65536));
        text.remove(QChar('\r')); text.remove(QChar('\n'));
        text.replace(QChar('\t'), QStringLiteral("  "));
        std::scoped_lock lock(logSink.mutex);
        auto* target = logSink.target;
        if (!target || !logSink.enabled) return;
        if (logSink.pending.size() == maxLogItems) logSink.pending.pop_front();
        logSink.pending.emplace_back(level, std::move(text));
        if (logSink.scheduled) return;
        logSink.scheduled = true;
        const auto generation = logSink.generation;
        if (!QMetaObject::invokeMethod(target, [target, generation] {
            try { target->DrainLog(generation); } catch (...) {}
        }, Qt::QueuedConnection))
        {
            logSink.pending.clear();
            logSink.scheduled = false;
        }
    }
    catch (...) {} // A failed optional UI sink must not recurse into the logger.
}

void MaintainTool::DrainLog(std::uint64_t generation)
{
    std::deque<std::pair<INT32, QString>> messages;
    {
        std::scoped_lock lock(logSink.mutex);
        if (logSink.target != this || !logSink.enabled || logSink.generation != generation) return;
        messages.swap(logSink.pending);
        logSink.scheduled = false;
    }
    for (auto& [level, text] : messages)
    {
        {
            std::scoped_lock lock(logSink.mutex);
            if (!m_active || !logSink.enabled || logSink.generation != generation) return;
        }
        Qt::GlobalColor color = Qt::black;
        switch (level)
        {
        case lltInfo: color = Qt::darkGreen; break;
        case lltWarning: color = Qt::darkYellow; break;
        case lltCritical: color = Qt::red; break;
        case lltError: color = Qt::darkRed; break;
        case lltLua: color = Qt::blue; break;
        default: break;
        }
        while (ui.listLog->count() >= maxLogItems) delete ui.listLog->takeItem(0);
        auto item = std::make_unique<QListWidgetItem>(std::move(text));
        item->setForeground(QBrush{color});
        ui.listLog->addItem(item.release());
    }
    LogFilter(ui.editFliter->text());
}

namespace
{
std::filesystem::path path_field(std::span<const char> value)
{
    const auto end = std::ranges::find(value, '\0');
    if (end == value.end() || end == value.begin()) throw std::invalid_argument("empty or unterminated configured directory");
    return std::filesystem::path{std::string_view{value.data(), static_cast<std::size_t>(end - value.begin())}};
}
QString path_name(const std::filesystem::path& value) { return QString::fromStdWString(value.native()); }
}
BOOL MaintainTool::_RefreshAreaInfo(bool reloadSessions)
try
{
    if (m_refreshing) return FALSE;
    m_refreshing = true;
    struct Reset { bool& value; ~Reset() { value = false; } } reset{m_refreshing};
    auto scanned = maintain::read_areas(path_field(g_MTConfig.szAreaConfigPath),
        {MAX_AREA_COUNT, MAX_GUARD_COUNT_PER_AREA, sizeof(g_MTConfig.AreaConfig[0].szAreaFolderName)});
    if (!scanned) throw std::runtime_error(scanned.error());
    auto candidate = std::make_unique<MAINTAIN_CONFIG>(g_MTConfig);
    std::ranges::fill(candidate->AreaConfig, MAINTAIN_CONFIG::SAreaConfig{});
    std::vector<std::unique_ptr<QTreeWidgetItem>> roots;
    roots.reserve(scanned->size());
    for (const auto& area : *scanned)
    {
        auto& config = candidate->AreaConfig[area.id];
        config.nAreaID = area.id;
        std::ranges::copy(area.name, config.szAreaName);
        std::ranges::copy(area.folder, config.szAreaFolderName);
        auto root = std::make_unique<QTreeWidgetItem>();
        const auto label = QString::number(area.id) + "_" + QString::fromLocal8Bit(area.name.c_str()) + ":";
        root->setText(enumAreaTextColumnName, label);
        root->setData(enumDataUserRoleAreaID, Qt::UserRole, area.id);
        root->setData(enumDataUserRoleVersion, Qt::UserRole, QString{});
        root->setData(enumDataUserRoleAreaName, Qt::UserRole, label);
        for (std::size_t index = 0; index < area.guards.size(); ++index)
        {
            const auto& guard = area.guards[index];
            auto& endpoint = config.guardList[index + 1];
            std::ranges::copy(guard.address, endpoint.szAddress);
            endpoint.nPort = 8810;
            auto child = std::make_unique<QTreeWidgetItem>();
            child->setText(enumAreaTextColumnAddress, QString::fromLatin1(guard.address.c_str()));
            child->setText(enumAreaTextColumnState, g_pcszConStr[emctUnConnect]);
            child->setText(enumAreaTextColumnProgress, "0/0");
            child->setData(enumDataUserRoleAreaName, Qt::UserRole, label);
            child->setData(enumDataUserRoleAreaID, Qt::UserRole, area.id);
            child->setData(enumDataUserRoleIP, Qt::UserRole, guard.ip);
            root->addChild(child.release());
        }
        roots.push_back(std::move(root));
    }
    if (reloadSessions && !CSessionMgr::GetInstance().Reload(*candidate))
    {
        // Native closes cannot be rolled back. Reflect peers already detached by
        // a failed retirement while retaining the old configuration and tree.
        bool detached = false;
        for (const auto& area : g_MTConfig.AreaConfig)
        {
            if (area.nAreaID > 0)
                for (const auto& guard : area.guardList)
                {
                    const auto end = std::ranges::find(guard.szAddress, '\0');
                    if (end == std::end(guard.szAddress)) continue;
                    const auto ip = maintain::parse_address({guard.szAddress, static_cast<std::size_t>(end - guard.szAddress)});
                    if (!ip) continue;
                    if (auto* session = CSessionMgr::GetInstance().GetRemoteGDSession(area.nAreaID, *ip);
                        session && !session->pSocketSession)
                    {
                        detached = true;
                        CTransmitFileMgr::GetInstance().StopSending(session->dwArea, session->dwIp);
                        UpdateGuardConnectInfo(session, emctUnConnect, nullptr);
                    }
                }
        }
        if (detached) UpdateGuardProcessesInfo();
        throw std::runtime_error("cannot replace guard sessions for the scanned configuration");
    }
    const QSignalBlocker areaSignals{ui.treeArea}, processSignals{ui.treeGuardProcesses};
    std::ranges::copy(candidate->AreaConfig, g_MTConfig.AreaConfig);
    ui.treeGuardProcesses->clear();
    ui.treeArea->clear();
    for (auto& root : roots) ui.treeArea->addTopLevelItem(root.release());
    m_pGraphicsScene->clear();
    for (auto& monitor : g_MonitorList)
    {
        monitor.dwIp = monitor.dwProcess = 0;
        monitor.nIndex = 0;
        monitor.nArea = 0;
        monitor.pGraphicsScene->clear();
        monitor.pLabel->clear();
    }
    return TRUE;
}
catch (const std::exception& error)
{
    ERR("cannot refresh area configuration: %s", error.what());
    return FALSE;
}

BOOL MaintainTool::_RefreshServerVersion()
{
    auto files = maintain::package_files(m_versionPath, ".zip");
    if (!files) { ERR("%s", files.error().c_str()); return FALSE; }
    ui.listVersion->clear();
    for (const auto& file : *files) ui.listVersion->addItem(path_name(file));
    ui.listVersion->sortItems(Qt::DescendingOrder);
    return TRUE;
}

BOOL MaintainTool::_RefreshServerPatch()
{
    auto files = maintain::package_files(m_patchPath, ".pak");
    if (!files) { ERR("%s", files.error().c_str()); return FALSE; }
    ui.listPatch->clear();
    for (const auto& file : *files) ui.listPatch->addItem(path_name(file));
    ui.listPatch->sortItems(Qt::DescendingOrder);
    return TRUE;
}

BOOL MaintainTool::UpdateGuardConnectInfo(SRemoteGuardSession* session,INT32 state,UINT8* counts)
{
    if (!session || state<0 || state>=emctTotal) return FALSE;
    const auto area=session->dwArea,ip=session->dwIp;
    auto* item=_GetGuardWidget(area,ip);
    if (!item) return FALSE;
    std::string config;
    if (counts)
        for (int type=stMS;type<=stDL;++type)
            if (counts[type]) config+=std::format(" [{}:{}]",GetServerShortName(type),counts[type]);
    const auto previous=m_rendering;
    m_rendering=true;
    struct Reset { bool& value; bool previous; ~Reset() { value=previous; } } reset{m_rendering,previous};
    item->setText(enumAreaTextColumnState,g_pcszConStr[state]);
    item->setText(enumAreaTextColumnConfig,QString::fromStdString(config));
    item->setText(enumAreaTextColumnProgress,{});
    return TRUE;
}

BOOL MaintainTool::UpdateGuardSendProgress(INT32 area,UINT32 ip,INT32 percent)
{
    auto* item=_GetGuardWidget(area,ip);
    if (!item) return FALSE;
    QString text;
    switch (percent)
    {
    case -1: text=QStringLiteral("send files error"); break;
    case -2: text=QStringLiteral("overwrite version config error"); break;
    case -3: text=QStringLiteral("direct move config error"); break;
    default:
        text=percent>100 ? QStringLiteral("send all ok") :
            percent<0 ? QStringLiteral("some error after sent") : QString::number(percent)+"%";
    }
    const auto previous=m_rendering;
    m_rendering=true;
    struct Reset { bool& value; bool previous; ~Reset() { value=previous; } } reset{m_rendering,previous};
    item->setText(enumAreaTextColumnProgress,text);
    return TRUE;
}

namespace
{
std::vector<std::unique_ptr<QTreeWidgetItem>> process_rows(INT32 area,UINT32 ip)
{
    const auto* session=CSessionMgr::GetInstance().GetRemoteGDSession(area,ip);
    if (!session) throw std::runtime_error("process list has no configured guard");
    std::vector<std::unique_ptr<QTreeWidgetItem>> rows;
    rows.reserve(session->lstProcess.size());
    for (const auto& process:session->lstProcess)
    {
        if (!process.dwProcessID) continue;
        auto row=std::make_unique<QTreeWidgetItem>();
        row->setText(enumProcessTextColumnPID,QString::fromStdString(ip_text(ip))+"-"+QString::number(process.dwProcessID));
        row->setText(enumProcessTextColumnType,QStringLiteral("%1-%2").arg(QString::fromLatin1(GetServerShortName(process.byType)))
            .arg(static_cast<qulonglong>(process.dwSessionID),2,10,QChar{'0'}));
        row->setText(enumProcessTextColumnState,process.nState==InternalMessage::epsUnonnect ? "unonnect" : "normal");
        row->setText(enumProcessTextColumnMode,process.bAuto ? "Auto" : "manul");
        row->setData(enumDataUserRoleAreaID,Qt::UserRole,area);
        row->setData(enumDataUserRoleIP,Qt::UserRole,ip);
        row->setData(enumDataUserRolePID,Qt::UserRole,process.dwProcessID);
        row->setData(enumDataUserRoleType,Qt::UserRole,process.byType);
        row->setData(enumDataUserRoleSessionID,Qt::UserRole,process.dwSessionID);
        rows.push_back(std::move(row));
    }
    return rows;
}
}
BOOL MaintainTool::AddGuardProcessesInfobyIp(INT32 area,UINT32 ip)
try
{
    if (m_rendering) return FALSE;
    auto rows=process_rows(area,ip);
    m_rendering=true;
    struct Reset { bool& flag; ~Reset() { flag=false; } } reset{m_rendering};
    for (auto& row:rows) ui.treeGuardProcesses->addTopLevelItem(row.release());
    return TRUE;
}
catch (const std::exception& error) { ERR("cannot append process rows: %s",error.what()); return FALSE; }

BOOL MaintainTool::UpdateGuardProcessesInfo()
try
{
    if (m_rendering) return FALSE;
    std::vector<std::unique_ptr<QTreeWidgetItem>> rows;
    for (const auto* item:GetSelectedIpItems(TRUE))
    {
        auto group=process_rows(item->data(enumDataUserRoleAreaID,Qt::UserRole).toInt(),
            item->data(enumDataUserRoleIP,Qt::UserRole).toUInt());
        for (auto& row:group) rows.push_back(std::move(row));
    }
    m_rendering=true;
    struct Reset { bool& flag; ~Reset() { flag=false; } } reset{m_rendering};
    const QSignalBlocker signalBlocker{ui.treeGuardProcesses};
    ui.treeGuardProcesses->clear();
    for (auto& row:rows) ui.treeGuardProcesses->addTopLevelItem(row.release());
    ui.treeGuardProcesses->sortItems(enumProcessTextColumnType,Qt::AscendingOrder);
    return TRUE;
}
catch (const std::exception& error) { ERR("cannot refresh process rows: %s",error.what()); return FALSE; }

void MaintainTool::_UpdateMonitor(QGraphicsScene* scene, const maintain::History& history)
{
    if (!scene) return;
    scene->clear();
    scene->addSimpleText(QString::number(static_cast<qulonglong>(history.maximum)));
    for (std::size_t index=1;index<history.values.size();++index)
    {
        if (!history.values[index-1] || !history.values[index]) continue;
        const auto percent=[&](UINT64 value) {
            return history.maximum ? static_cast<double>(static_cast<long double>(value)/history.maximum*100) : 0.0;
        };
        const auto x=40.0+4*(index-1);
        scene->addLine(x,(100-percent(*history.values[index-1]))*2+20,
            x+4,(100-percent(*history.values[index]))*2+20,
            QPen{index+1==history.values.size() ? Qt::red : Qt::black,2});
    }
}

std::expected<std::string, std::string> MaintainTool::SnapShootGuardStatisInfoByIp(INT32 area, UINT32 ip, UINT32 pid)
try
{
    const auto* session=CSessionMgr::GetInstance().GetRemoteGDSession(area,ip);
    if (!session || !session->bConnected || !session->poStatisInfo)
        return std::unexpected("selected guard has no connected statistics");
    const auto values=maintain::statistics(*session->poStatisInfo,pid);
    if (!values) return std::unexpected("selected process has no current statistics");
    std::string output="name\tcurrent\taverage\tmin\tmax\n";
    for (int field=statInvalid+1;field<statTotal;++field)
    {
        if (!g_szStatFieldName[field] || !*g_szStatFieldName[field]) continue;
        const auto& metric=(*values)[field];
        output+=std::format("{}\t{}\t{}\t{}\t{}\n",g_szStatFieldName[field],
            std::bit_cast<INT64>(metric.current),std::bit_cast<INT64>(metric.average),
            std::bit_cast<INT64>(metric.minimum),std::bit_cast<INT64>(metric.maximum));
    }
    return output;
}
catch (const std::exception& error) { return std::unexpected(error.what()); }

std::expected<std::string, std::string> MaintainTool::SnapShootGuardStatisInfoCurrentByIp(INT32 area, UINT32 ip, UINT32 pid)
try
{
    const auto* session = CSessionMgr::GetInstance().GetRemoteGDSession(area, ip);
    if (!session || !session->bConnected || !session->poStatisInfo)
        return std::unexpected("selected guard has no connected statistics");
    const auto& info = *session->poStatisInfo;
    const auto found = std::ranges::find_if(info.ProcessInfo, [pid](const auto& process) { return process.bUsed && process.dwProcessID == pid; });
    if (found == std::end(info.ProcessInfo)) return std::unexpected("selected process has no current statistics");
    std::string output;
    for (int field = statInvalid + 1; field < statTotal; ++field)
        if (g_szStatFieldName[field] && *g_szStatFieldName[field])
            output += std::format("\t{}", std::bit_cast<INT64>(found->qwStatisEntry[field]));
    return output;
}
catch (const std::exception& error) { return std::unexpected(error.what()); }

BOOL MaintainTool::EndHibernateGuardStatisInfoByIp()
{
    for (QTreeWidgetItemIterator item{ui.treeArea}; *item; ++item)
    {
        const auto area = (*item)->data(enumDataUserRoleAreaID, Qt::UserRole).toInt();
        const auto ip = (*item)->data(enumDataUserRoleIP, Qt::UserRole).toUInt();
        if (auto* session = CSessionMgr::GetInstance().GetRemoteGDSession(area, ip))
            for (auto& process : session->lstProcess) process.hibernate.reset();
    }
    return TRUE;
}

BOOL MaintainTool::ProcessHibernateGuardStatisInfoByIp(stStatisProcess* stats, SProcessInfo* process)
try
{
    if (!stats || !process || !process->hibernate) return FALSE;
    const auto now = *localtime_ex();
    auto row = std::format("{:04}-{:02}-{:02} {:02}:{:02}:{:02}.{:03}", now.tm_year, now.tm_mon, now.tm_mday,
        now.tm_hour, now.tm_min, now.tm_sec, now.tm_msec);
    for (int field = 0; field < statTotal; ++field)
        if (g_szStatFieldName[field] && *g_szStatFieldName[field])
            row += std::format("\t{}", std::bit_cast<INT64>(stats->qwStatisEntry[field]));
    row += '\n';
    process->hibernate->write(row.data(), static_cast<std::streamsize>(row.size()));
    process->hibernate->flush();
    if (*process->hibernate) return TRUE;
    process->hibernate.reset();
    return FALSE;
}
catch (const std::exception& error)
{
    ERR("cannot write hibernation statistics: %s", error.what());
    if (process) process->hibernate.reset();
    return FALSE;
}

BOOL MaintainTool::BeginHibernateGuardStatisInfoByIp(INT32 area, UINT32 ip, UINT32 pid, UINT8 type, UINT32 id)
try
{
    auto* session = CSessionMgr::GetInstance().GetRemoteGDSession(area, ip);
    if (!session || !session->poStatisInfo) return FALSE;
    auto found = std::ranges::find(session->lstProcess, pid, &SProcessInfo::dwProcessID);
    if (found == session->lstProcess.end() || found->hibernate) return FALSE;
    const auto* name = GetServerShortName(type);
    if (!name) return FALSE;
    const auto now = *localtime_ex();
    const auto stamp = std::format("{:04}_{:02}_{:02}_{:02}_{:02}_{:02}_{:03}", now.tm_year, now.tm_mon, now.tm_mday,
        now.tm_hour, now.tm_min, now.tm_sec, now.tm_msec);
    in_addr address{}; address.s_addr = ip;
    const auto path = std::filesystem::current_path() / "log" /
        std::format("hibernate_{}-{}={}@{}_{}.tsv", name, id, pid, inet_ntoa(address), stamp);
    std::filesystem::create_directories(path.parent_path());
    auto file = std::make_unique<std::ofstream>(path, std::ios::binary | std::ios::trunc);
    if (!*file) return FALSE;
    std::string header = "time";
    for (int field = statInvalid + 1; field < statTotal; ++field)
        if (g_szStatFieldName[field] && *g_szStatFieldName[field]) header += std::format("\t{}", g_szStatFieldName[field]);
    header += '\n';
    file->write(header.data(), static_cast<std::streamsize>(header.size()));
    file->flush();
    if (!*file) return FALSE;
    found->hibernate = std::move(file);
    return TRUE;
}
catch (const std::exception& error)
{
    ERR("cannot start hibernation statistics: %s", error.what());
    return FALSE;
}

BOOL MaintainTool::RenderStatistics(INT32 area, UINT32 ip, UINT32 pid)
try
{
    if (!m_active || m_refreshing || m_rendering) return FALSE;
    const auto* session=CSessionMgr::GetInstance().GetRemoteGDSession(area,ip);
    if (!session || !session->bConnected || !session->poStatisInfo) return FALSE;
    m_rendering=true;
    struct Reset { bool& flag; ~Reset() { flag=false; } } reset{m_rendering};
    const auto& info=*session->poStatisInfo;
    const auto values=session->bConnected && pid ? maintain::statistics(info,pid) : std::optional<maintain::Statistics>{};
    struct Graph { QGraphicsScene* scene; maintain::History history; };
    std::vector<Graph> graphs;
    for (const auto& monitor:g_MonitorList)
        if (monitor.nArea==area && monitor.dwIp==ip && monitor.dwProcess)
            graphs.push_back({monitor.pGraphicsScene,session->bConnected ?
                maintain::history(info,monitor.dwProcess,monitor.nIndex) : maintain::History{}});
    if (values)
        for (const auto* item:ui.treeStatisInfo->selectedItems())
        {
            const auto field=item->data(0,Qt::UserRole).toInt();
            if (field>statInvalid && field<statTotal)
                graphs.push_back({m_pGraphicsScene,maintain::history(info,pid,field)});
        }
    // All borrowed statistics are copied before Qt model/scene callbacks.
    const QSignalBlocker signalBlocker{ui.treeStatisInfo};
    if (values)
        for (int field=statInvalid+1;field<statTotal;++field)
        {
            const auto rows=ui.treeStatisInfo->findItems(QString::fromLatin1(g_szStatFieldName[field]),Qt::MatchExactly);
            if (rows.empty()) continue;
            auto* row=rows.front();
            const auto& metric=(*values)[field];
            const std::array<UINT64,5> columns{metric.current,metric.average,metric.minimum,metric.maximum,metric.total};
            for (std::size_t column=0;column<columns.size();++column)
            {
                row->setText(static_cast<int>(column+1),QString::number(static_cast<qulonglong>(columns[column])));
                if (!m_active) return FALSE;
            }
        }
    else
    {
        for (QTreeWidgetItemIterator row{ui.treeStatisInfo};*row;++row)
            for (int column=1;column<=5;++column) (*row)->setText(column,{});
        m_pGraphicsScene->clear();
    }
    for (const auto& graph:graphs)
    {
        if (!m_active) return FALSE;
        _UpdateMonitor(graph.scene,graph.history);
    }
    return !pid || values.has_value();
}
catch (const std::exception& error) { ERR("cannot display statistics: %s",error.what()); return FALSE; }

BOOL MaintainTool::UpdateGuardStatisInfoByIp(INT32 area, UINT32 ip, UINT32 pid)
{
    return RenderStatistics(area,ip,pid);
}

BOOL MaintainTool::InitGuardStatisInfo()
{
    if (m_rendering) return FALSE;
    m_rendering=true;
    struct Reset { bool& flag; ~Reset() { flag=false; } } reset{m_rendering};
    const QSignalBlocker signalBlocker{ui.treeStatisInfo};
    ui.treeStatisInfo->clear();
    for (int field=statInvalid+1;field<statTotal;++field)
    {
        if (!g_szStatFieldName[field] || !*g_szStatFieldName[field]) continue;
        auto item=std::make_unique<QTreeWidgetItem>();
        item->setText(0,QString::fromLatin1(g_szStatFieldName[field]));
        item->setData(0,Qt::UserRole,field);
        ui.treeStatisInfo->addTopLevelItem(item.release());
    }
    return TRUE;
}

BOOL MaintainTool::UpdateGuardStatisInfo(UINT32 area,UINT32 ip)
{
    UINT32 pid=0;
    const auto selected=ui.treeGuardProcesses->selectedItems();
    if (selected.size()==1)
    {
        const auto* item=selected.front();
        if (item->data(enumDataUserRoleAreaID,Qt::UserRole).toUInt()==area &&
            item->data(enumDataUserRoleIP,Qt::UserRole).toUInt()==ip)
            pid=item->data(enumDataUserRolePID,Qt::UserRole).toUInt();
    }
    return RenderStatistics(static_cast<INT32>(area),ip,pid);
}

BOOL MaintainTool::WriteSnapshot(bool summary)
try
{
    struct Selected { INT32 area; UINT32 ip, pid, session; UINT8 type; };
    std::vector<Selected> selected;
    for (const auto* item : ui.treeGuardProcesses->selectedItems())
    {
        Selected entry{item->data(enumDataUserRoleAreaID, Qt::UserRole).toInt(),
            item->data(enumDataUserRoleIP, Qt::UserRole).toUInt(),
            item->data(enumDataUserRolePID, Qt::UserRole).toUInt(),
            item->data(enumDataUserRoleSessionID, Qt::UserRole).toUInt(),
            static_cast<UINT8>(item->data(enumDataUserRoleType, Qt::UserRole).toUInt())};
        if (entry.type < stMS || entry.type >= stClient) throw std::runtime_error("invalid selected process type");
        selected.push_back(entry);
    }
    const auto now = *localtime_ex();
    const auto timestamp = std::format("{:04}_{:02}_{:02}_{:02}_{:02}_{:02}_{:02}",
        now.tm_year, now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec, now.tm_msec);
    std::string output = summary ? timestamp + "\n" : "name";
    if (!summary)
    {
        for (int field = statInvalid + 1; field < statTotal; ++field)
            if (g_szStatFieldName[field] && *g_szStatFieldName[field]) output += std::format("\t{}", g_szStatFieldName[field]);
        output += '\n';
    }
    for (const auto& entry : selected)
    {
        auto row = summary ? SnapShootGuardStatisInfoByIp(entry.area, entry.ip, entry.pid)
                           : SnapShootGuardStatisInfoCurrentByIp(entry.area, entry.ip, entry.pid);
        if (!row) throw std::runtime_error(row.error());
        output += std::format("{}-{}:{}@{}", GetServerShortName(entry.type), entry.session, entry.pid, ip_text(entry.ip));
        if (summary) output += '\n';
        output += *row;
        output += '\n';
    }
    const auto result = maintain::write_atomic(std::filesystem::path{"log"} / ("snapshoot_" + timestamp + ".tsv"), output);
    if (!result) throw std::runtime_error(result.error());
    INF("snapshot exported");
    return TRUE;
}
catch (const std::exception& error) { ERR("cannot export snapshot: %s", error.what()); return FALSE; }

BOOL MaintainTool::SnapShootGuardStatisInfo() { return WriteSnapshot(true); }
BOOL MaintainTool::SnapShootGuardStatisInfoCurrent() { return WriteSnapshot(false); }

BOOL MaintainTool::HibernateGuardStatisInfo()
{
	INT32 nRetCode = 0;
	QTreeWidgetItem* pItem = NULL;
	QString str;


    {
	if (ui.actionHibernate->isChecked())
	{
		QList<QTreeWidgetItem*> lstSelectProcess;
		lstSelectProcess = ui.treeGuardProcesses->selectedItems();

		for (QList<QTreeWidgetItem*>::iterator itProcess = lstSelectProcess.begin();
			itProcess != lstSelectProcess.end(); ++itProcess)
		{
			INT32 nAreaID = (*itProcess)->data(enumDataUserRoleAreaID, Qt::UserRole).toInt();
			UINT32 dwIp = (*itProcess)->data(enumDataUserRoleIP, Qt::UserRole).toInt();
			UINT32 dwPID = (*itProcess)->data(enumDataUserRolePID, Qt::UserRole).toInt();

			UINT32 dwSessionID = (*itProcess)->data(enumDataUserRoleSessionID, Qt::UserRole).toInt();
			UINT8 byType = (*itProcess)->data(enumDataUserRoleType, Qt::UserRole).toInt();
			LOG_PROCESS_ERROR(byType >= stMS && byType < stClient);

			//
			BeginHibernateGuardStatisInfoByIp(nAreaID, dwIp, dwPID, byType, dwSessionID);
		}
	}
	else
	{
		EndHibernateGuardStatisInfoByIp();
	}

    }
Exit0:
	return TRUE;
}

void MaintainTool::Login()
{
    QDialog dialog{this};
    QFormLayout form{&dialog};
    QLineEdit user{&dialog},password{&dialog};
    QLabel feedback{&dialog};feedback.setObjectName("loginError");feedback.setWordWrap(true);
    user.setObjectName("loginUser");password.setObjectName("loginPassword");
    password.setEchoMode(QLineEdit::Password);
    form.addRow(tr("&User:"),&user);form.addRow(tr("&Password:"),&password);form.addRow(&feedback);
    QDialogButtonBox buttons{QDialogButtonBox::Ok|QDialogButtonBox::Cancel,Qt::Horizontal,&dialog};
    const auto valid=[&] {
        const auto name=user.text().toUtf8(),secret=password.text().toUtf8();
        return name.size()<sizeof(m_szUserName) && secret.size()<sizeof(m_szPassword) &&
            !name.contains('\0') && !secret.contains('\0');
    };
    QObject::connect(&buttons,&QDialogButtonBox::accepted,&dialog,[&] {
        if (valid()) dialog.accept();
        else
        {
            feedback.setText(tr("User name or password is too long or contains an invalid character."));
            feedback.setMinimumHeight(feedback.heightForWidth(feedback.width()));
            dialog.adjustSize();
        }
    });
    QObject::connect(&buttons,&QDialogButtonBox::rejected,&dialog,&QDialog::reject);
    form.addRow(&buttons);
    if (dialog.exec()!=QDialog::Accepted || !valid()) return;
    const auto name=user.text().toUtf8(),secret=password.text().toUtf8();
    std::ranges::fill(m_szUserName,'\0');std::ranges::fill(m_szPassword,'\0');
    std::memcpy(m_szUserName,name.constData(),static_cast<std::size_t>(name.size()));
    std::memcpy(m_szPassword,secret.constData(),static_cast<std::size_t>(secret.size()));
    m_bLogin=TRUE;ui.frameMain->show();
}

void MaintainTool::Logout(void)
{
	m_nWaitCmdCount=0;m_currentCommand.clear();m_bCmdResult=TRUE;
	m_bLogin = FALSE;
	CTransmitFileMgr::GetInstance().UnInit();
	CSessionMgr::GetInstance().CloseAllSession();
	ui.frameMain->hide();
	std::ranges::fill(m_szUserName, '\0');
	std::ranges::fill(m_szPassword, '\0');
}

void MaintainTool::SnapShoot(void)
{
	//SnapShootGuardStatisInfo();
	SnapShootGuardStatisInfoCurrent();
}

void MaintainTool::Hibernate(void)
{
	HibernateGuardStatisInfo();
}

void MaintainTool::OnAreaTreeClick(QTreeWidgetItem*, int)
{
	INT32 nRetCode = 0;

	nRetCode = UpdateGuardProcessesInfo();

    {
	LOG_PROCESS_ERROR(nRetCode);

    }
Exit0:
	return;
}

namespace
{
void open_local(const std::filesystem::path& path)
try
{
    if (path.empty() || !std::filesystem::exists(path)) throw std::runtime_error("local path does not exist");
    const auto url=QUrl::fromLocalFile(path_name(std::filesystem::absolute(path)));
    if (!QDesktopServices::openUrl(url)) throw std::runtime_error("cannot open local path");
}
catch (const std::exception& error) { ERR("%s",error.what()); }
}
void MaintainTool::OnAreaItemDoubleClicked(QTreeWidgetItem* item,int)
try
{
    if (!item || !item->parent()) return;
    const auto area=item->data(enumDataUserRoleAreaID,Qt::UserRole).toInt();
    const auto ip=item->data(enumDataUserRoleIP,Qt::UserRole).toUInt();
    if (area<=0 || area>=MAX_AREA_COUNT || !ip) return;
    open_local(path_field(g_MTConfig.szAreaConfigPath)/path_field(g_MTConfig.AreaConfig[area].szAreaFolderName)/ip_text(ip));
}
catch (const std::exception& error) { ERR("cannot open guard folder: %s",error.what()); }
void MaintainTool::OnVesionWidgetDoubleClicked(const QModelIndex&) { open_local(m_versionPath); }
void MaintainTool::OnPatchWidgetDoubleClicked(const QModelIndex&) { open_local(m_patchPath); }
void MaintainTool::OnLogDoubleClicked(QListWidgetItem*)
{
    const auto* file=CLog::GetInstance().GetLogFilePath();
    if (file && *file) open_local(std::filesystem::path{file});
}

BOOL MaintainTool::ExecCmd(const char* text)
try
{
    if (!text || !*text || std::strlen(text)>MAX_MESSAGE_SIZE-sizeof(InternalMessage::N2U_UPDATE_CMD)-1) return FALSE;
    std::vector<std::pair<INT32,UINT32>> peers;
    for (const auto* item:GetSelectedIpItems())
        peers.emplace_back(item->data(enumDataUserRoleAreaID,Qt::UserRole).toInt(),
            item->data(enumDataUserRoleIP,Qt::UserRole).toUInt());
    m_currentCommand=text;
    m_bCmdResult=TRUE;
    bool sent=false;
    for (const auto [area,ip]:peers)
    {
        ++m_nWaitCmdCount;
        if (CMTMessageHandler::GetInstance().DoGDExecCmd(area,ip,text)) sent=true;
        else if (m_nWaitCmdCount) --m_nWaitCmdCount;
    }
    if (!m_nWaitCmdCount) m_currentCommand.clear();
    return sent;
}
catch (const std::exception& error) { ERR("cannot execute command: %s",error.what()); return FALSE; }

BOOL MaintainTool::UpdateNotifyInfo(QString text)
{
    auto item=std::make_unique<QListWidgetItem>(std::move(text));
    item->setForeground(QBrush{Qt::red});
    while (ui.listAlert->count()>=1024) delete ui.listAlert->takeItem(0);
    ui.listAlert->addItem(item.release());
    AlertFilter(ui.editAlertFliter->text());
    return TRUE;
}
BOOL MaintainTool::UpdateNotifyInfo(UINT32 ip,UINT8 type,UINT8 id,UINT8 level,INT32 error,const char* text)
{
    if (!text) return FALSE;
    const auto address=ip_text(ip);
    const auto* name=type>=stMS && type<stClient ? GetServerShortName(type) : "unknown";
    INF("NOTIFY_%d [%s:%d]@%s: %s",level,name,id,address.c_str(),text);
    if (error==errSuccess) return TRUE;
    auto message=QString::fromStdString(address+" ")+QString::fromUtf8(text);
    message.remove(QChar{'\r'});message.remove(QChar{'\n'});message.replace(QChar{'\t'},QStringLiteral("  "));
    return UpdateNotifyInfo(std::move(message));
}
BOOL MaintainTool::UpdateCmdResult(UINT32 ip,INT32 error,const char* text)
{
    if (!text) return FALSE;
    if (m_nWaitCmdCount) --m_nWaitCmdCount;
    if (error!=errSuccess) m_bCmdResult=FALSE;
    INF("ip %s UpdateCmdResult cmd [%s] result: %s",ip_text(ip).c_str(),m_currentCommand.c_str(),text);
    if (!m_nWaitCmdCount) m_currentCommand.clear();
    return TRUE;
}

BOOL MaintainTool::CheckAreaExpanded(INT32 nAreaID)
{
    QTreeWidgetItem *pAreaItem = NULL;

	pAreaItem = _GetAreaWidget(nAreaID);

    {
	PROCESS_ERROR( pAreaItem );

    return pAreaItem->isExpanded() ? TRUE : FALSE;
    }
Exit0:
    return FALSE;
}
//
//BOOL MaintainTool::GetAreaIpList(UINT32 dwAreaID, UINT32& nMaxCount, UINT32* dwIplst)
//{
//	INT32 nRetCode = 0;
//	QTreeWidgetItem *pAreaItem = NULL;
//	QTreeWidgetItem *pGuardItem = NULL;
//	QVariant qValue;
//	INT32 nCountlimit = nMaxCount;
//	nMaxCount = 0;
//
//	INT32 nAreaCount = ui.treeArea->topLevelItemCount();
//	for(INT32 i = 0; i < nAreaCount; ++i)
//	{
//		INT32 nGuardCount = 0;
//		pAreaItem = ui.treeArea->topLevelItem(i);
//		LOG_PROCESS_ERROR(pAreaItem);
//
//		if(pAreaItem->data(enumAreaDataUserRoleAreaID, Qt::UserRole) == dwAreaID)
//		{
//			nGuardCount = pAreaItem->childCount();
//			for(INT32 j = 0; j < nGuardCount; ++j)
//			{
//				pGuardItem = pAreaItem->child(j);
//				LOG_PROCESS_ERROR(pGuardItem);
//
//				qValue = pGuardItem->data(enumAreaDataUserRoleAreaID, Qt::UserRole);
//				dwIplst[nMaxCount++] = qValue.toInt();
//
//				if(nMaxCount >= nCountlimit)
//					break;
//			}
//			break;
//		}
//	}
//
//	return TRUE;
//Exit0:
//	return FALSE;
//}

BOOL MaintainTool::_CheckGuardStateByIp(INT32 nAreaID, UINT32 dwIp)
{
	INT32 nRetCode = 0;

	SRemoteGuardSession* pGuardSession = CSessionMgr::GetInstance().GetRemoteGDSession(nAreaID, dwIp);

    {
	PROCESS_ERROR(pGuardSession);
	PROCESS_ERROR(pGuardSession->pSocketSession);
	PROCESS_ERROR(pGuardSession->bConnected);

	return TRUE;
    }
Exit0:
	return FALSE;
}

void MaintainTool::Start(void)
{
	INT32 nRetCode = 0;
	const char* pcszCmd = "start -apr";

	//bool ok;
	//QString text = QInputDialog::getText(this, tr("confirm"),
	//	tr("Are you sure? please input \'yes\'"), QLineEdit::Normal, tr("no"), &ok);
	//if (ok && !text.isEmpty() && text == "yes" )
	{
		nRetCode = ExecCmd(pcszCmd);

    {
		LOG_PROCESS_ERROR(nRetCode);
	}


    }
Exit0:
	return;
}

void MaintainTool::Open(void)
{
	INT32 nRetCode = 0;
	INT32 nGroupID = -1;
	QString strCmd;

	QInputDialog InputDialog;
	InputDialog.setLabelText("GroupID: ");
	InputDialog.exec();
	nGroupID = InputDialog.textValue().toInt();


    {
	LOG_PROCESS_ERROR(nGroupID > -1);
    strCmd=QStringLiteral(".switch %1 1").arg(nGroupID);

	//QString text;
	//bool ok;
	//text = QInputDialog::getText(this, tr("confirm"),
	//	tr("Are you sure? please input \'yes\'"), QLineEdit::Normal, tr("no"), &ok);
	//if (ok && !text.isEmpty() && text == "yes")
	{
		nRetCode = ExecCmd(strCmd.toUtf8().data());
		LOG_PROCESS_ERROR(nRetCode);
	}

	ui.editCmd->clear();
    }
Exit0:
	return;
}

void MaintainTool::Stop(void)
{
	INT32 nRetCode = 0;
	const char* pcszCmd = "stop -f";
	bool ok;
	QString reply;

	QString strAreaInfo;

	QList<QTreeWidgetItem*> lstSelectItem;
	lstSelectItem = GetSelectedIpItems();    //ui.treeArea->selectedItems();

	strAreaInfo.clear();

    {
	for (QList<QTreeWidgetItem*>::iterator itr = lstSelectItem.begin(); itr != lstSelectItem.end(); ++itr)
	{
		INT32 nChildCount = (*itr)->childCount();
		LOG_PROCESS_ERROR(nChildCount == 0);
		//if(nChildCount == 0)// guard
		{
			INT32 nAreaID = (*itr)->data(enumDataUserRoleAreaID, Qt::UserRole).toInt();
			UINT32 dwIp = (*itr)->data(enumDataUserRoleIP, Qt::UserRole).toInt();

			QString strIp;
			struct in_addr addr;
			addr.s_addr = dwIp;
            strIp=QString::fromStdString(ip_text(dwIp));

			strAreaInfo += "\t" + strIp + " @ " + (*itr)->data(enumDataUserRoleAreaName, Qt::UserRole).toString() + "\n";
		}
	}

	reply = QInputDialog::getText(this, tr("confirm"),
		QString(" Are you sure to stop these? \n%1 Please input \'yes\'").arg(strAreaInfo), QLineEdit::Normal, tr("no"), &ok);
	if (ok && !reply.isEmpty() && reply == "yes")
	{
		nRetCode = ExecCmd(pcszCmd);
		LOG_PROCESS_ERROR(nRetCode);
	}
    }
Exit0:
	return;
}

void MaintainTool::Close(void)
{
	INT32 nRetCode = 0;
	const char* pcszCmd = "close";
	bool ok;
	QString reply;

	QString strAreaInfo;

	QList<QTreeWidgetItem*> lstSelectItem;
	lstSelectItem = GetSelectedIpItems();    //ui.treeArea->selectedItems();

	strAreaInfo.clear();

    {
	for (QList<QTreeWidgetItem*>::iterator itr = lstSelectItem.begin(); itr != lstSelectItem.end(); ++itr)
	{
		INT32 nChildCount = (*itr)->childCount();
		LOG_PROCESS_ERROR(nChildCount == 0);
		//if(nChildCount == 0)// guard
		{
			INT32 nAreaID = (*itr)->data(enumDataUserRoleAreaID, Qt::UserRole).toInt();
			UINT32 dwIp = (*itr)->data(enumDataUserRoleIP, Qt::UserRole).toInt();

			QString strIp;
			struct in_addr addr;
			addr.s_addr = dwIp;
            strIp=QString::fromStdString(ip_text(dwIp));

			strAreaInfo += "\t" + strIp + " @ " + (*itr)->data(enumDataUserRoleAreaName, Qt::UserRole).toString() + "\n";
		}
	}

	reply = QInputDialog::getText(this, tr("confirm"),
		QString(" Are you sure to close these? \n%1 Please input \'yes\'").arg(strAreaInfo), QLineEdit::Normal, tr("no"), &ok);
	if (ok && !reply.isEmpty() && reply == "yes")
	{
		nRetCode = ExecCmd(pcszCmd);
		LOG_PROCESS_ERROR(nRetCode);
	}

    }
Exit0:
	return;
}

void MaintainTool::ExecuteCmd(void)
{
	INT32 nRetCode = 0;
	QString strCmd;
	bool ok;
	QString reply;
	QString strAreaInfo;
	QList<QTreeWidgetItem*> lstSelectItem;

	strCmd = ui.editCmd->text();

    {
	PROCESS_ERROR(strCmd.size() > 0);

	lstSelectItem = GetSelectedIpItems();    //ui.treeArea->selectedItems();

	strAreaInfo.clear();
	for (QList<QTreeWidgetItem*>::iterator itr = lstSelectItem.begin(); itr != lstSelectItem.end(); ++itr)
	{
		INT32 nChildCount = (*itr)->childCount();
		LOG_PROCESS_ERROR(nChildCount == 0);
		//if(nChildCount == 0)// guard
		{
			INT32 nAreaID = (*itr)->data(enumDataUserRoleAreaID, Qt::UserRole).toInt();
			UINT32 dwIp = (*itr)->data(enumDataUserRoleIP, Qt::UserRole).toInt();

			QString strIp;
			struct in_addr addr;
			addr.s_addr = dwIp;
            strIp=QString::fromStdString(ip_text(dwIp));

			strAreaInfo += "\t" + strIp + " @ " + (*itr)->data(enumDataUserRoleAreaName, Qt::UserRole).toString() + "\n";
		}
	}

	reply = QInputDialog::getText(this, tr("confirm"),
		QString(" Are you sure send cmd \'%1\' to these? \n%2 Please input \'yes\'").arg(strCmd.toUtf8().data(), strAreaInfo), QLineEdit::Normal, tr("no"), &ok);
	if (ok && !reply.isEmpty() && reply == "yes")
	{
		nRetCode = ExecCmd(strCmd.toUtf8().data());
		LOG_PROCESS_ERROR(nRetCode);
	}

	ui.editCmd->clear();
    }
Exit0:
	return;
}

void MaintainTool::VersionRefresh(void)
{
    if (m_rendering) return;
	INT32 nRetCode = 0;

	nRetCode = _RefreshAreaInfo(true);

    {
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = CTransmitFileMgr::GetInstance().Reload();
	LOG_PROCESS_ERROR(nRetCode);

	nRetCode = _RefreshServerVersion();
	LOG_CHECK_ERROR(nRetCode);

	nRetCode = _RefreshServerPatch();
	LOG_CHECK_ERROR(nRetCode);
    }
Exit0:
	return;
}

BOOL MaintainTool::GetSelectVersion(char* output)
try
{
    const auto* item = ui.listVersion->currentItem();
    if (!output || !item) return FALSE;
    const auto name = wire_name(item->text());
    std::memcpy(output, name.c_str(), name.size() + 1);
    return TRUE;
}
catch (const std::exception& error) { ERR("%s", error.what()); return FALSE; }

BOOL MaintainTool::GetSelectPatch(char* output)
try
{
    const auto* item = ui.listPatch->currentItem();
    if (!output || !item) return FALSE;
    const auto name = wire_name(item->text());
    std::memcpy(output, name.c_str(), name.size() + 1);
    return TRUE;
}
catch (const std::exception& error) { ERR("%s", error.what()); return FALSE; }

QList<QTreeWidgetItem*> MaintainTool::GetSelectedIpItems(BOOL bRequireSameParent)
{
	BOOL nRetCode = FALSE;
	INT32 nItemChildCount = 0;
    QList<QTreeWidgetItem*> lstResult = ui.treeArea->selectedItems();
    QList<QTreeWidgetItem*> lstParents;


    {
    for(QList<QTreeWidgetItem*>::iterator itr = lstResult.begin(); itr != lstResult.end();)
    {
        QTreeWidgetItem* pItem = *itr;
        LOG_PROCESS_ERROR(pItem);
		nItemChildCount = pItem->childCount();
        if(nItemChildCount > 0)
        {
            bool bChooseWithChild = false;
            for(int i=0; i<nItemChildCount; ++i)
            {
                bChooseWithChild = lstResult.contains( pItem->child(i) );
                if( bChooseWithChild )
                {
                    break;
                }
            }

            if ( !bChooseWithChild )
            {
                lstParents.append( pItem );
            }
            itr = lstResult.erase(itr);
        }
        else
        {
            ++itr;
        }
    }

	if ( bRequireSameParent )
	{
		QList<QTreeWidgetItem*> lstDumpParents(lstParents);
		for (QList<QTreeWidgetItem*>::iterator itr = lstResult.begin(); itr != lstResult.end(); ++itr)
		{
			LOG_PROCESS_ERROR(*itr);
			QTreeWidgetItem* pParentItem = (*itr)->parent();
			LOG_PROCESS_ERROR(pParentItem);
			if ( !lstDumpParents.contains(pParentItem) )
			{
				lstDumpParents.append(pParentItem);
			}
		}
		if (lstDumpParents.count() > 1)
		{
			CRI( "all selected guard items must in the same area" );
			LOG_PROCESS_ERROR(FALSE);
		}
	}

    for(QList<QTreeWidgetItem*>::iterator itr = lstParents.begin(); itr != lstParents.end(); ++itr)
    {
        QTreeWidgetItem* pItem = *itr;
        LOG_PROCESS_ERROR(pItem);

		nItemChildCount = pItem->childCount();
		for (int i = 0; i < nItemChildCount; ++i)
        {
            lstResult.append( pItem->child(i));
        }
    }

	return lstResult;
    }
Exit0:
	return QList<QTreeWidgetItem*>();
}

BOOL MaintainTool::_GenerateGMSecurityFile(const char* source, const char* destination)
{
    if (!source || !destination) return FALSE;
    const auto result = maintain::generate_security_file(source, destination);
    if (!result) ERR("%s", result.error().c_str());
    return result.has_value();
}

namespace
{
std::string wire_name(const QString& name)
{
    const auto bytes = name.toLocal8Bit();
    if (bytes.isEmpty() || bytes.size() >= 128 || bytes.contains('\0') || QString::fromLocal8Bit(bytes) != name)
        throw std::runtime_error("filename cannot be represented in the legacy wire field");
    return std::string{bytes.constData(), static_cast<std::size_t>(bytes.size())};
}
std::string ip_text(UINT32 ip)
{
    if constexpr (std::endian::native == std::endian::little) ip = std::byteswap(ip);
    return std::format("{}.{}.{}.{}", (ip >> 24) & 255, (ip >> 16) & 255, (ip >> 8) & 255, ip & 255);
}
}
void MaintainTool::SendFiles(INT32 type)
try
{
    using Source = CTransmitFileMgr::Source;
    struct Destination { INT32 area; UINT32 ip; };
    std::vector<Destination> destinations;
    for (auto* item : GetSelectedIpItems())
    {
        const auto area = item->data(enumDataUserRoleAreaID, Qt::UserRole).toInt();
        const auto ip = item->data(enumDataUserRoleIP, Qt::UserRole).toUInt();
        if (!_CheckGuardStateByIp(area, ip)) throw std::runtime_error("selected guard is not ready for transfer");
        if (type == InternalMessage::etftConfig &&
            (!item->parent() || item->parent()->data(enumDataUserRoleVersion, Qt::UserRole).toString().isEmpty()))
            throw std::runtime_error("selected area has no current version");
        if (CTransmitFileMgr::GetInstance().IsSending(area, ip)) throw std::runtime_error("selected guard already has a pending transfer");
        destinations.push_back({area, ip});
    }
    if (destinations.empty()) throw std::runtime_error("select at least one guard");
    QListWidgetItem* selected = type == InternalMessage::etftPatch ? ui.listPatch->currentItem() : ui.listVersion->currentItem();
    std::filesystem::path package;
    std::string name;
    if (type != InternalMessage::etftConfig)
    {
        if (!selected) throw std::runtime_error("select a package first");
        name = wire_name(selected->text());
        package = (type == InternalMessage::etftPatch ? m_patchPath : m_versionPath) /
            std::filesystem::path{selected->text().toStdWString()};
    }
    const auto areaRoot = path_field(g_MTConfig.szAreaConfigPath);
    const auto encrypted = areaRoot / "gmsecurityconfig-encrypted.lua";
    if (type != InternalMessage::etftPatch)
    {
        const auto generated = maintain::generate_security_file(areaRoot / "gmsecurityconfig.lua", encrypted);
        if (!generated) throw std::runtime_error(generated.error());
    }
    for (const auto [area, ip] : destinations)
    {
        std::vector<Source> files;
        if (type == InternalMessage::etftPatch) files.push_back({name, package});
        else
        {
            const auto areaPath = areaRoot / path_field(g_MTConfig.AreaConfig[area].szAreaFolderName);
            const auto prefix = type == InternalMessage::etftVersion ? name + "-" : std::string{};
            if (type == InternalMessage::etftVersion) files.push_back({name, package});
            files.push_back({prefix + "guard_area_config.lua", areaPath / ip_text(ip) / "guard_area_config.lua"});
            files.push_back({prefix + "area_config.lua", areaPath / "area_config.lua"});
            files.push_back({prefix + "gmsecurityconfig.lua", encrypted});
        }
        if (!CTransmitFileMgr::GetInstance().StartSending(area, ip, files, type))
            throw std::runtime_error("cannot start selected file transfer");
    }
    INF("sending selected files, wait please");
}
catch (const std::exception& error) { ERR("cannot send files: %s", error.what()); }

void MaintainTool::SendVersion() { SendFiles(InternalMessage::etftVersion); }
void MaintainTool::SendConfig() { SendFiles(InternalMessage::etftConfig); }
void MaintainTool::SendPatch() { SendFiles(InternalMessage::etftPatch); }

void MaintainTool::DeployVersion(void)
{
	INT32 nRetCode = 0;
	QList<QTreeWidgetItem*> lstGuardSelectItem;
	QTreeWidgetItem* pItem = NULL;

    char szFileName[COMMON_NAME_LEN] = {0};
    nRetCode = GetSelectVersion( szFileName );

    {
	if (!nRetCode || szFileName[0] == '\0')
	{
		CRI("please select a version first");
		PROCESS_ERROR(FALSE);
	}

	lstGuardSelectItem = GetSelectedIpItems(); //ui.treeArea->selectedItems();
	if(lstGuardSelectItem.count() == 0)
	{
		CRI("please select area or guard");
		PROCESS_ERROR(FALSE);
	}

	for(QList<QTreeWidgetItem*>::iterator itr = lstGuardSelectItem.begin(); itr != lstGuardSelectItem.end(); ++itr)
	{
		pItem = *itr;
        LOG_PROCESS_ERROR( pItem && pItem->childCount() == 0 );

		UINT32 dwIp = pItem->data(enumDataUserRoleIP, Qt::UserRole).toUInt();
		LOG_PROCESS_ERROR(dwIp > 0);
		INT32 nAreaID = pItem->data(enumDataUserRoleAreaID, Qt::UserRole).toInt();
		LOG_PROCESS_ERROR(nAreaID > 0 && nAreaID < MAX_AREA_COUNT);

        nRetCode = _CheckGuardStateByIp(nAreaID, dwIp);
        LOG_PROCESS_ERROR(nRetCode);

        nRetCode = CMTMessageHandler::GetInstance().DoGDDeployServerVersion(nAreaID, dwIp, szFileName );
        LOG_PROCESS_ERROR(nRetCode);
	}

	INF("deploy version:%s, please wait", szFileName );
	//ui.frameMain->setEnabled(false);

	return;
    }
Exit0:
	INF("deploy version:%s failed", szFileName );

	return;
}

void MaintainTool::HotUpdatePatch(void)
{
	INT32 nRetCode = 0;
	QList<QTreeWidgetItem*> lstGuardSelectItem;
	QTreeWidgetItem* pItem = NULL;

	char szFileName[COMMON_NAME_LEN] = { 0 };
	nRetCode = GetSelectPatch(szFileName);

    {
	LOG_PROCESS_ERROR(nRetCode && szFileName[0]);
	//strncat( szFileName, ".pak", sizeof(szFileName) );

	lstGuardSelectItem = GetSelectedIpItems(); //ui.treeArea->selectedItems();
	if (lstGuardSelectItem.count() == 0)
	{
		CRI("please select area or guard");
		PROCESS_ERROR(FALSE);
	}

	for (QList<QTreeWidgetItem*>::iterator itr = lstGuardSelectItem.begin(); itr != lstGuardSelectItem.end(); ++itr)
	{
		pItem = *itr;
		LOG_PROCESS_ERROR(pItem && pItem->childCount() == 0);

		UINT32 dwIp = pItem->data(enumDataUserRoleIP, Qt::UserRole).toUInt();
		LOG_PROCESS_ERROR(dwIp > 0);
		INT32 nAreaID = pItem->data(enumDataUserRoleAreaID, Qt::UserRole).toInt();
		LOG_PROCESS_ERROR(nAreaID > 0 && nAreaID < MAX_AREA_COUNT);

		nRetCode = _CheckGuardStateByIp(nAreaID, dwIp);
		LOG_PROCESS_ERROR(nRetCode);

		nRetCode = CMTMessageHandler::GetInstance().DoGDHotUpdatePatch(nAreaID, dwIp, szFileName);
		LOG_PROCESS_ERROR(nRetCode);
	}

	INF("hot update patch:%s, please wait", szFileName);
	//ui.frameMain->setEnabled(false);

	return;
    }
Exit0:
	INF("hot update patch:%s failed", szFileName);

	return;
}

BOOL MaintainTool::SetAreaVersion(INT32 nAreaID, const char* pcszVersion)
{
    const GuiMutation mutation{m_rendering};
	QTreeWidgetItem* pAreaItem = NULL;
	QVariant AreaName;
	QString strAreaInfo;


    {
    LOG_PROCESS_ERROR(pcszVersion);

	pAreaItem = _GetAreaWidget(nAreaID);
	LOG_PROCESS_ERROR(pAreaItem);

	AreaName = pAreaItem->data(enumDataUserRoleAreaName, Qt::UserRole);
	strAreaInfo = AreaName.toString() + pcszVersion;

	pAreaItem->setText(enumAreaTextColumnName, strAreaInfo);
	pAreaItem->setTextColor(enumAreaTextColumnName, Qt::black);

	pAreaItem->setData(enumDataUserRoleVersion, Qt::UserRole, pcszVersion);

    return TRUE;
    }
Exit0:
    return FALSE;
}

QTreeWidgetItem* MaintainTool::_GetGuardWidget(INT32 nAreaID, UINT32 dwIp)
{
	INT32 nRetCode = 0;
	INT32 nAreaCount = ui.treeArea->topLevelItemCount();
	QTreeWidgetItem* pResult = NULL;


    {
	LOG_PROCESS_ERROR(nAreaID < MAX_AREA_COUNT);

	// get target
	for (INT32 i = 0; i < nAreaCount; ++i)
	{
		QTreeWidgetItem* pAreaItem = ui.treeArea->topLevelItem(i);
		if (pAreaItem && pAreaItem->data(enumDataUserRoleAreaID, Qt::UserRole) == nAreaID)
		{
			INT32 nGuardCount = pAreaItem->childCount();
			for (INT32 j = 0; j < nGuardCount; ++j)
			{
				QTreeWidgetItem* pGuardItem = pAreaItem->child(j);
				if (pGuardItem->data(enumDataUserRoleIP, Qt::UserRole) == dwIp)
				{
					pResult = pGuardItem;
					break;
				}
			}
			break;
		}
	}
	LOG_PROCESS_ERROR(pResult);
    }
Exit0:
	return pResult;
}

QTreeWidgetItem* MaintainTool::_GetAreaWidget(INT32 nAreaID)
{
	INT32 nRetCode = 0;
	INT32 nAreaCount = ui.treeArea->topLevelItemCount();
	QTreeWidgetItem* pResult = NULL;


    {
	LOG_PROCESS_ERROR(nAreaID < MAX_AREA_COUNT);

	// get target
	for (INT32 i = 0; i < nAreaCount; ++i)
	{
		QTreeWidgetItem* pAreaItem = ui.treeArea->topLevelItem(i);
		if (pAreaItem && pAreaItem->data(enumDataUserRoleAreaID, Qt::UserRole) == nAreaID)
		{
			pResult = pAreaItem;
			break;
		}
	}
	LOG_PROCESS_ERROR(pResult);
    }
Exit0:
	return pResult;
}

BOOL MaintainTool::_HasUpdatedWholeArea(INT32 nAreaID, UINT32 dwIp, const char* pcszVersion)
{
    const GuiMutation mutation{m_rendering};
	BOOL bHasUpdateWholeArea = TRUE;

	//check and update config
	for (INT32 i = 1; i < MAX_AREA_COUNT; ++i)
	{
		if (g_MTConfig.AreaConfig[i].nAreaID == nAreaID)
		{
			for (INT32 j = 1; j < MAX_GUARD_COUNT_PER_AREA; ++j)
			{
				if (g_MTConfig.AreaConfig[i].guardList[j].szAddress[0] != 0)
				{
					INT32 dwCurAddr = inet_addr(g_MTConfig.AreaConfig[i].guardList[j].szAddress);
					if (dwCurAddr == dwIp)
					{
						strxcpy(g_MTConfig.AreaConfig[i].guardList[j].szGuardVersion,
							pcszVersion, sizeof(g_MTConfig.AreaConfig[i].guardList[j].szGuardVersion));
					}
					else if (strnicmp(g_MTConfig.AreaConfig[i].guardList[j].szGuardVersion,
						pcszVersion, sizeof(g_MTConfig.AreaConfig[i].guardList[j].szGuardVersion)) != 0)
					{
						bHasUpdateWholeArea = FALSE;
					}
				}
			}

			if (bHasUpdateWholeArea)
			{
				strxcpy(g_MTConfig.AreaConfig[i].szVersion, pcszVersion, sizeof(g_MTConfig.AreaConfig[i].szVersion));
			}
			break;
		}
	}
	return bHasUpdateWholeArea;
}

BOOL MaintainTool::DeployVersionResultByIp(INT32 nAreaID, UINT32 dwIp, const char* pcszVersion, INT32 nErrorCode)
{
    const GuiMutation mutation{m_rendering};
	INT32 nRetCode = 0;

    QTreeWidgetItem* pTargetAreaItem = NULL;
    QTreeWidgetItem* pTargetGuardItem = NULL;
	BOOL bHasDeployWholeArea = TRUE;

	struct in_addr addr = {};
	addr.s_addr = dwIp;

	pTargetGuardItem = _GetGuardWidget(nAreaID, dwIp);

    {
	LOG_PROCESS_ERROR(pTargetGuardItem);

	PROCESS_ERROR(nErrorCode == errSuccess);

    INF("ip:%s deploy version:%s success", inet_ntoa(addr), pcszVersion);

	bHasDeployWholeArea = _HasUpdatedWholeArea(nAreaID, dwIp, pcszVersion);
	if(bHasDeployWholeArea)
	{
		pTargetAreaItem = _GetAreaWidget(nAreaID);
		LOG_PROCESS_ERROR(pTargetAreaItem);

		QVariant AreaName = pTargetAreaItem->data(enumDataUserRoleAreaName, Qt::UserRole);
		QString strAreaInfo = AreaName.toString() + pcszVersion;

        pTargetAreaItem->setText(0, strAreaInfo);
        pTargetAreaItem->setData(1, Qt::UserRole, pcszVersion);
        pTargetAreaItem->setTextColor(0, Qt::black);

		INF("Area:%d deploy version:%s success", nAreaID, pcszVersion);
	}

    if( pTargetGuardItem )
    {
        pTargetGuardItem->setText(4,QStringLiteral("deploy ok: ")+QString::fromLocal8Bit(pcszVersion));
    }

	return TRUE;

    }
Exit0:
    CRI("Area:%d IP:%s deploy version:%s failed", nAreaID, inet_ntoa(addr), pcszVersion);
	//ui.frameMain->setEnabled(true);

    if ( pTargetGuardItem )
    {
        pTargetGuardItem->setText(4,QStringLiteral("deploy error: %1, details see log").arg(nErrorCode));
    }

	return FALSE;
}

BOOL MaintainTool::HotUpdatePatchResultByIp(INT32 nAreaID, UINT32 dwIp, const char* pcszVersion, INT32 nErrorCode)
{
    const GuiMutation mutation{m_rendering};
	INT32 nRetCode = 0;

	QTreeWidgetItem* pTargetAreaItem = NULL;
	QTreeWidgetItem* pTargetGuardItem = NULL;
	BOOL bHasDeployWholeArea = TRUE;

	struct in_addr addr;
	addr.s_addr = dwIp;

	pTargetGuardItem = _GetGuardWidget(nAreaID, dwIp);

    {
	LOG_PROCESS_ERROR(pTargetGuardItem);

	PROCESS_ERROR(nErrorCode == errSuccess);

	INF("ip:%s hot patch:%s success", inet_ntoa(addr), pcszVersion);

	bHasDeployWholeArea = _HasUpdatedWholeArea(nAreaID, dwIp, pcszVersion);
	if (bHasDeployWholeArea)
	{
		pTargetAreaItem = _GetAreaWidget(nAreaID);
		LOG_PROCESS_ERROR(pTargetAreaItem);

		QVariant AreaName = pTargetAreaItem->data(enumDataUserRoleAreaName, Qt::UserRole);
		QString strAreaInfo = AreaName.toString() + pcszVersion;

		pTargetAreaItem->setText(0, strAreaInfo);
		pTargetAreaItem->setData(1, Qt::UserRole, pcszVersion);
		pTargetAreaItem->setTextColor(0, Qt::black);

		INF("Area:%d hot patch:%s success", nAreaID, pcszVersion);
	}

	if (pTargetGuardItem)
	{
        pTargetGuardItem->setText(4,QStringLiteral("hot patch ok: ")+QString::fromLocal8Bit(pcszVersion));
	}

	return TRUE;

    }
Exit0:
	CRI("Area:%d IP:%s hot patch:%s failed, error code = %d", nAreaID, inet_ntoa(addr), pcszVersion, nErrorCode);
	//ui.frameMain->setEnabled(true);

	if (pTargetGuardItem)
	{
        pTargetGuardItem->setText(4,QStringLiteral("hot patch error: %1").arg(nErrorCode));
	}

	return FALSE;
}

void MaintainTool::CleanLog(void)
{
	ui.listLog->clear();
}

void MaintainTool::CleanAlert(void)
{
	ui.listAlert->clear();
}

void MaintainTool::LogFilter(const QString& text)
{

    {
	for (INT32 nIndex = 0; nIndex < ui.listLog->count(); nIndex++)
	{
		QListWidgetItem* pItem = ui.listLog->item(nIndex);
		LOG_PROCESS_ERROR(pItem);

		if (pItem->text().contains(text, Qt::CaseInsensitive))
			pItem->setHidden(false);
		else
			pItem->setHidden(true);
	}

    }
Exit0:
	return;
}

void MaintainTool::AlertFilter(const QString& text)
{

    {
	for (INT32 nIndex = 0; nIndex < ui.listAlert->count(); nIndex++)
	{
		QListWidgetItem* pItem = ui.listAlert->item(nIndex);
		LOG_PROCESS_ERROR(pItem);

		if (pItem->text().contains(text, Qt::CaseInsensitive))
			pItem->setHidden(false);
		else
			pItem->setHidden(true);
	}

    }
Exit0:
	return;
}

void MaintainTool::OnStatisInfoItemDoubleClicked(QTreeWidgetItem* item,int)
{
    const auto selected=ui.treeGuardProcesses->selectedItems();
    if (!item || selected.size()!=1) return;
    const auto field=item->data(0,Qt::UserRole).toInt();
    if (field<=statInvalid || field>=statTotal) return;
    const auto* process=selected.front();
    const auto area=process->data(enumDataUserRoleAreaID,Qt::UserRole).toInt();
    const auto ip=process->data(enumDataUserRoleIP,Qt::UserRole).toUInt();
    const auto pid=process->data(enumDataUserRolePID,Qt::UserRole).toUInt();
    const auto label=QStringLiteral("Area %1 %2 [%3] %4").arg(area).arg(process->text(1),process->text(0),
        QString::fromLatin1(g_szStatFieldName[field]));
    auto& monitor=g_MonitorList[g_nLastSetMonitor];
    monitor.nArea=area; monitor.dwIp=ip; monitor.dwProcess=pid; monitor.nIndex=field;
    monitor.pLabel->setText(label);
    g_nLastSetMonitor=(g_nLastSetMonitor+1)%MAX_MONITOR_COUNT;
    RenderStatistics(area,ip,pid);
}
