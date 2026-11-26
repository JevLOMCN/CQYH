"""Run the three real dialog rectangle readers in ANSI and Unicode modes."""
import re


def fixtures(client, source, function):
    parts = ['#include <sstream>\n#pragma push_macro("_T")']
    runners = []
    for index, file in enumerate(["FilePatch/FilePatchDlg.cpp", "FixClient/FixClientDlg.cpp", "PatchMPQ/PatchMPQDlg.cpp"]):
        cpp = re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", source(client / "client/PatchTools" / file))
        body = function(cpp, "CRect GetControlRect(")
        for wide in [False, True]:
            namespace = "PatchRectangle%d%s" % (index, "Wide" if wide else "Ansi")
            runners.append(namespace + "::Run();")
            parts.append('#undef _T\n#define _T(s) ' + ('L##s' if wide else 's'))
            parts.append("namespace " + namespace + " {\nusing TCHAR = " + ("wchar_t" if wide else "char") + ";")
            parts.append(r'''
using LPCTSTR = const TCHAR*;
using String = std::basic_string<TCHAR>;
struct CString : String {
    using String::basic_string;
    CString(const String& text) : String(text) {}
    operator LPCTSTR() const { return c_str(); }
};
struct App { static CString GetAppPath() { return _T("test/"); } };
const TCHAR* C_IniFile = _T("test.ini");
CString config;
bool present = true;
struct Configer {
    void LoadFile(const CString&) {}
    CString GetStringValue(LPCTSTR, const CString&, LPCTSTR fallback) { return present ? config : CString(fallback); }
};
struct CRect {
    LONG left, top, right, bottom;
    CRect() : left(0x12345678), top(0x12345678), right(0x12345678), bottom(0x12345678) {}
    CRect(LONG l, LONG t, LONG r, LONG b) : left(l), top(t), right(r), bottom(b) {}
};
''')
            parts.append(body)
            parts.append(r'''
void Check(LPCTSTR text, LONG left, LONG top, LONG right, LONG bottom) {
    config = CString(text);
    CRect rectangle = GetControlRect(_T("control"));
    assert(rectangle.left == left && rectangle.top == top && rectangle.right == right && rectangle.bottom == bottom);
}
void Run() {
    present = false; Check(_T("ignored"), 0, 0, 50, 50); present = true;
    for (LPCTSTR malformed : {_T(""), _T("garbage"), _T("1"), _T("1,2"), _T("1,2,3"),
                             _T("1,2,3,garbage"), _T("1;2;3;4"), _T("1,2,,4"),
                             _T("2147483648,0,0,0"), _T("-2147483649,0,0,0"),
                             _T("0,0,2147483648,0"), _T("0,0,0,-2147483649"),
                             _T("2147483647,0,1,1"), _T("-2147483648,0,-1,1"),
                             _T("0,2147483647,1,1"), _T("0,-2147483648,1,-1")})
        Check(malformed, 0, 0, 50, 50);
    String huge(1000, _T('9')); huge += _T(",0,1,1");
    Check(huge.c_str(), 0, 0, 50, 50);
    Check(_T("10,20,30,40"), 10, 20, 40, 60);
    Check(_T("-10,-20,30,40"), -10, -20, 20, 20);
    Check(_T("10,20,-3,-4"), 10, 20, 7, 16);
    Check(_T("0,0,0,0"), 0, 0, 0, 0);
    Check(_T(" 1 , 2 , 3 , 4 "), 1, 2, 4, 6);
    Check(_T("2147483647,2147483647,0,0"), MAXLONG, MAXLONG, MAXLONG, MAXLONG);
    Check(_T("-2147483648,-2147483648,0,0"), -MAXLONG - 1, -MAXLONG - 1, -MAXLONG - 1, -MAXLONG - 1);
}
}
''')
    parts.append('#pragma pop_macro("_T")\nvoid TestPatchRectangles() {\n' + "\n".join(runners) + '\nputs("Three patch dialog rectangle readers reject incomplete/oversized settings and coordinate overflow (ANSI/Unicode)");\n}')
    return "\n".join(parts)
