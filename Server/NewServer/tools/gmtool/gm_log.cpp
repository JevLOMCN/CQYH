#include "gm_log.h"
#include "gm_log_data.h"
#include "log.h"
#include <string>

CGMLog CGMLog::m_Instance;
CGMLog& CGMLog::GetInstance() { return m_Instance; }
CGMLog::CGMLog() = default;
CGMLog::~CGMLog() = default;
BOOL CGMLog::Init() { return TRUE; }
BOOL CGMLog::UnInit() { return TRUE; }
BOOL CGMLog::Log(std::string_view text)
{
    if (text.contains('\0')) return FALSE;
    const std::string owned(text);
    if (!gm::output_tool_log(owned)) return FALSE;
    INF("%s",owned.c_str());
    return TRUE;
}
