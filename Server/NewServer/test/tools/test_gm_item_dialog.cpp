#include "stdafx.h"
#include "gm_item_info.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QTextEdit>
#include <QtGui/QTextDocument>
#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char** argv)
try
{
    QApplication app(argc, argv);
    CGMItemInfo dialog;
    auto* editor = dialog.findChild<QTextEdit*>();
    if (!editor)
        throw std::runtime_error("missing editor");
    auto* document = editor->document();
    const auto child_count = dialog.findChildren<QObject*>().size();
    std::size_t checks = 1;
    auto check = [&](bool condition, const char* message)
    {
        ++checks;
        if (!condition)
            throw std::runtime_error(message);
    };
    for (unsigned iteration = 0; iteration < 1000; ++iteration)
    {
        const auto text = std::string(32768 + iteration, 'x') + "\t4294967295\n";
        dialog.ShowInfo(text.c_str(), FALSE);
        check(editor->toPlainText() == QString::fromLocal8Bit(text.c_str()), "owned display text");
        check(editor->document() == document, "reuse owned document");
        check(dialog.findChildren<QObject*>().size() == child_count, "stable QObject ownership");
    }
    dialog.ShowInfo("物品ID\t18446744073709551615\n", TRUE);
    check(editor->toPlainText() == QString::fromLocal8Bit("物品ID\t18446744073709551615\n"), "legacy local text");
    check(editor->font().bold() && editor->font().pointSize() == 12, "large bold text");
    const auto before = editor->toPlainText();
    dialog.ShowInfo(nullptr, FALSE);
    check(editor->toPlainText() == before, "null input preserves text");
    dialog.show();
    dialog.Exit();
    check(!dialog.isVisible() && dialog.result() == 0, "exit closes dialog");
    std::cout << checks << " GM item-dialog checks passed\n";
    return 0;
}
catch (const std::exception& error)
{
    std::cerr << error.what() << '\n';
    return 1;
}
