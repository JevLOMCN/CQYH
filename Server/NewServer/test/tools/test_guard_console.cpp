#include "guard_console.h"
#include <algorithm>
#include <cstdio>
#include <string>

int main()
{
    int checks=0,failures=0;
    const auto check=[&](bool value,const char* message) {
        ++checks; if (!value) { ++failures; std::fprintf(stderr,"FAIL: %s\n",message); }
    };
    guard_runtime::ConsoleInput input;
    check(!input.push('\b') && input.text().empty(),"empty backspace");
    for (const auto key:std::string{"start -g"}) check(!input.push(key),"characters wait for enter");
    check(input.text()=="start -g","input retained");
    auto line=input.push('\r');
    check(line && !line->overflow && line->text=="start -g" && input.text().empty(),"enter transfers owned command");
    input.push('a'); input.push('b'); input.push('\b');
    line=input.push('\n'); check(line && line->text=="a","backspace edits the line");
    for (int prefix:{0,224}) {
        input.push('x'); input.push(prefix); input.push(75); input.push('y');
        line=input.push('\r'); check(line && line->text=="xy","extended keys never become printable command text");
    }
    for (int key:{-1,1,9,27,127,200,255}) input.push(key);
    check(input.text().empty(),"nonprintable keys ignored");
    for (std::size_t size=0;size<=guard_runtime::ConsoleInput::capacity+3;++size) {
        input.clear(); for (std::size_t i=0;i<size;++i) input.push('x');
        line=input.push('\r');
        check(line && line->text.size()==std::min(size,guard_runtime::ConsoleInput::capacity) &&
            line->overflow==(size>guard_runtime::ConsoleInput::capacity),"all command length boundaries");
    }
    for (std::size_t i=0;i<guard_runtime::ConsoleInput::capacity+1;++i) input.push('x');
    input.push('\b'); for (char key:std::string{"quit"}) input.push(key);
    line=input.push('\r'); check(line && line->overflow,"backspace cannot make truncated suffix executable");
    for (char key:std::string{"help"}) input.push(key);
    line=input.push('\r'); check(line && !line->overflow && line->text=="help","next line recovers after overflow");
    input.push(224); input.clear(); input.push('a');
    line=input.push('\n'); check(line && line->text=="a","clear resets extended-key state");
    input.push('x'); input.clear(); check(input.text().empty(),"clear drops pending text");
    std::printf("guard console: %d checks, %d failures\n",checks,failures); return failures!=0;
}
