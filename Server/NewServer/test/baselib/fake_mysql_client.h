#pragma once
#include <atomic>
#include <span>
#include <string_view>
#include <vector>
#include "mysql.h"

// Link-time C API fake for the production SQL-manager lifetime tests.
// It opens no sockets and creates no database or files.
namespace fake_mysql
{
using Cell = std::vector<unsigned char>;
using Row = std::vector<Cell>;
using Rows = std::vector<Row>;
// Optional prepared-query fixtures. Bindings are borrowed only for this call.
// The handler validates the schema before returning one cell per result binding.
using PreparedHandler = Rows (*)(std::string_view, std::span<const MYSQL_BIND>, MYSQL_BIND*);
inline std::atomic<PreparedHandler> preparedHandler{nullptr};
inline std::atomic<int> scriptErrors{0};
inline std::atomic<int> connections{0};
inline std::atomic<int> statements{0};
inline std::atomic<int> threads{0};
inline std::atomic<int> results{0};
inline std::atomic<int> orderErrors{0};
inline std::atomic<int> executions{0};
inline std::atomic<int> failExecutions{0};
inline std::atomic<int> failFreeResults{0};
inline std::atomic<bool> ready{false};
inline std::atomic<bool> failQuery{false};
inline std::atomic<bool> emptyRows{false};
inline std::atomic<bool> failLibrary{false};
inline std::atomic<bool> failInit{false};
inline std::atomic<bool> failConnect{false};
inline std::atomic<bool> failStmtInit{false};
inline std::atomic<bool> failPrepare{false};
inline std::atomic<bool> failBindParam{false};
inline std::atomic<bool> failClose{false};
inline std::atomic<bool> failThreadInit{false};
inline std::atomic<bool> failFetchAfterRow{false};
inline std::atomic<int> parameterCountAdjustment{0};
}
