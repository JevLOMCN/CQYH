#include "gm_log_view.h"
#include <QtCore/QPointer>
#include <QtCore/QTextCodec>
#include <QtGui/QTextCursor>
#include <QtWidgets/QTextEdit>
#include <limits>

namespace
{
void plain(QTextEdit& view,const QString& text)
{
    const QPointer<QTextEdit> alive(&view);
    auto cursor=view.textCursor();cursor.movePosition(QTextCursor::End);
    cursor.insertText((view.document()->isEmpty() ? QString() : QStringLiteral("\n"))+text);
    if (!alive) return;
    view.setTextCursor(cursor);
    if (alive) view.ensureCursorVisible();
}
}
std::expected<void,std::string> gm::show_tool_log(QTextEdit& view,std::string_view text,std::chrono::system_clock::time_point when)
{
    const auto line=timed_log(text,when);
    if (!line) return std::unexpected(line.error());
    if (line->size()>std::numeric_limits<int>::max()) return std::unexpected("tool log exceeds Qt text capacity");
    const auto* codec=QTextCodec::codecForName("GB18030");
    plain(view,codec ? codec->toUnicode(line->data(),static_cast<int>(line->size())) :
        QString::fromLocal8Bit(line->data(),static_cast<int>(line->size())));
    return {};
}
std::expected<void,std::string> gm::show_game_log(QTextEdit& view,std::span<const char> bytes,const std::filesystem::path& path)
{
    const auto body=game_log_body(bytes);
    if (!body) return std::unexpected(body.error());
    const auto written=append_game_log(path,*body);
    if (!written) return written;
    plain(view,QString::fromUtf8(body->data(),static_cast<int>(body->size())));
    return {};
}
