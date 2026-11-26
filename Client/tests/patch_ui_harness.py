"""Production UI constructors and bitmap guards with small MFC drawing mocks."""
import re


def fixtures(client, source, function):
    def clean(text):
        text = re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)
        return re.sub(r"(?m)^\s*#(?:include|pragma)[^\n]*", "", text)

    parts = [r'''
#define SAFE
#define UNSAFE
#define afx_msg
#define DECLARE_MESSAGE_MAP()
#define DECLARE_DYNAMIC(value)
#undef LoadCursor
#define LoadCursor MockUiLoadCursor
''']
    for index, folder in enumerate(["FixClient", "PatchMPQ", "FilePatch"]):
        root = client / "client/PatchTools" / folder
        parts.append("namespace PatchUi%d {\n" % index)
        parts.append(r'''
using CString = std::wstring;
struct CWnd { virtual ~CWnd() = default; };
int positions = 0, pixels = 0, blits = 0, regions = 0;
struct CButton : CWnd {
    void SetWindowPos(void*, int, int, int width, int height, UINT) {
        assert(width > 0 && height > 0); ++positions;
    }
};
struct CStatic : CWnd {};
struct CPoint { int x, y; CPoint(int a = 0, int b = 0) : x(a), y(b) {} };
struct CSize { int cx, cy; CSize(int a = 0, int b = 0) : cx(a), cy(b) {} };
struct CRect { int left, top, right, bottom; void SetRectEmpty() { left = top = right = bottom = 0; } };
struct CFont { bool CreatePointFont(int, const wchar_t*) { return true; } };
struct CBitmap {
    bool query_ok = true;
    LONG width = 16, height = 8;
    int GetBitmap(BITMAP* output) {
        if (!query_ok) return 0;
        output->bmWidth = width; output->bmHeight = height; return sizeof(*output);
    }
    void DeleteObject() {}
    operator HGDIOBJ() const { return reinterpret_cast<HGDIOBJ>(1); }
};
struct CRgn {
    void CreateRectRgn(int left, int top, int right, int bottom) {
        assert(left >= 0 && right >= left && top >= 0 && bottom >= top); ++regions;
    }
    void CombineRgn(CRgn*, CRgn*, int) {}
    void DeleteObject() {}
};
struct CDC {
    CBitmap* bitmap = nullptr;
    BOOL CreateCompatibleDC(CDC*) { return TRUE; }
    CBitmap* SelectObject(CBitmap* value) { auto* old = bitmap; bitmap = value; return old; }
    HDC GetSafeHdc() { return reinterpret_cast<HDC>(this); }
    COLORREF GetPixel(int x, int y) {
        assert(bitmap && x >= 0 && x < bitmap->width && y >= 0 && y < bitmap->height);
        ++pixels; return 0;
    }
    BOOL BitBlt(int, int, int width, int height, CDC* src, int x, int y, DWORD) {
        assert(src->bitmap && x >= 0 && y >= 0 && width > 0 && height > 0);
        assert(x + width <= src->bitmap->width && y + height <= src->bitmap->height);
        ++blits; return TRUE;
    }
    BOOL StretchBlt(int, int, int width, int height, CDC* src, int x, int y, int sw, int sh, DWORD) {
        assert(width > 0 && height > 0);
        return BitBlt(0, 0, sw, sh, src, x, y, 0);
    }
};
HCURSOR LoadCursor(HINSTANCE, LPCTSTR) { return nullptr; }
HINSTANCE AfxGetResourceHandle() { return nullptr; }
enum { IDC_HAND_DLL = 1 };
''')
        parts.append(clean(source(root / "Controls.h")))
        parts.append(r'''
LPTRANSPARENTBLT g_pTransparentBlt = nullptr;
BOOL WINAPI TransBlt(HDC, int, int, int width, int height, HDC source,
                    int x, int y, int sw, int sh, UINT) {
    assert(width > 0 && height > 0);
    CDC* src = reinterpret_cast<CDC*>(source);
    return src->BitBlt(0, 0, sw, sh, src, x, y, 0);
}
void CU_TransBlt(HDC dest, int dx, int dy, int dw, int dh, HDC src,
                 int x, int y, int sw, int sh, UINT color) {
    TransBlt(dest, dx, dy, dw, dh, src, x, y, sw, sh, color);
}
''')
        controls = clean(source(root / "Controls.cpp"))
        for name in ["void CU_SetupRegion(", "void CU_TransparentBlt(", "SIZE CU_GetBitmapSize("]:
            parts.append(function(controls, name))
        for name in ["BmpRgnButton", "TransparentCaption", "Caption"]:
            header = clean(source(root / (name + ".h"))).replace("protected:", "public:")
            parts.append(header)
            cpp = clean(source(root / (name + ".cpp")))
            for signature in [name + "::" + name + "()", name + "::~" + name + "()"]:
                parts.append(function(cpp, signature))
            if name == "BmpRgnButton":
                parts.append(function(cpp, "void BmpRgnButton::AdjustSize()"))
                parts.append("void BmpRgnButton::DrawItem(LPDRAWITEMSTRUCT) {}\n")
            if name == "Caption":
                parts.append(function(cpp, "void Caption::SetCaptionFont("))
        parts.append(r'''
template<class T> void CheckRect() {
    for (unsigned char pattern : {0xa5, 0x5a}) {
        alignas(T) unsigned char storage[sizeof(T)];
        memset(storage, pattern, sizeof(storage));
        T* caption = new(storage) T;
        auto rect = caption->_rcOnBgImg;
        assert(!rect.left && !rect.top && !rect.right && !rect.bottom && !caption->_pBgImg);
        caption->~T();
    }
}
void Run() {
    CheckRect<Caption>(); CheckRect<TransparentCaption>();
    for (unsigned char pattern : {0xa5, 0x5a}) {
        alignas(BmpRgnButton) unsigned char storage[sizeof(BmpRgnButton)];
        memset(storage, pattern, sizeof(storage));
        BmpRgnButton* button = new(storage) BmpRgnButton;
        assert(!button->_pBmp && !button->_uBmpItemWidth && !button->_uBmpItemHeight);
        assert(!button->_bMouseHover && !button->_bDrawTrans && !button->IsVerticalCaption());
        assert(button->_refTrans == RGB(255, 0, 255) && button->_refDrawTrans == RGB(255, 0, 255));
        button->~BmpRgnButton();
    }
    BmpRgnButton button; CBitmap bitmap;
    button._pBmp = &bitmap;
    for (int failure = 0; failure < 4; ++failure) {
        bitmap.query_ok = true; bitmap.width = 16; bitmap.height = 8; positions = 0;
        if (failure == 1) bitmap.query_ok = false;
        if (failure == 2) bitmap.width = 3;
        if (failure == 3) bitmap.height = 0;
        button.AdjustSize();
        assert(positions == (failure == 0));
        assert(button._uBmpItemWidth == (failure == 0 ? 4 : 0));
        assert(button._uBmpItemHeight == (failure == 0 ? 8 : 0));
    }
    button._pBmp = nullptr;
    CDC dc;
    for (int failure = 0; failure < 8; ++failure) {
        bitmap.query_ok = true; bitmap.width = 16; bitmap.height = 8;
        UINT tx = 4, ty = 1, ox = 3, oy = 0;
        if (failure == 1) bitmap.query_ok = false;
        if (failure == 2) tx = 0;
        if (failure == 3) ty = 0;
        if (failure == 4) ox = tx;
        if (failure == 5) oy = ty;
        if (failure == 6) bitmap.width = 0;
        if (failure == 7) bitmap.height = -1;
        regions = pixels = 0;
        CRgn region;
        CU_SetupRegion(region, &dc, bitmap, 1, tx, ox, ty, oy);
        assert((pixels > 0) == (failure == 0));
        assert((regions > 0) == (failure == 0));
        for (bool transparent : {false, true}) for (bool pointer : {false, true}) {
            g_pTransparentBlt = pointer ? TransBlt : nullptr;
            blits = 0;
            CU_TransparentBlt(&dc, bitmap, CPoint(), CSize(), transparent, 1, tx, ox, ty, oy, 0);
            assert(blits == (failure == 0));
        }
    }
    bitmap.query_ok = false;
    SIZE size = CU_GetBitmapSize(bitmap);
    assert(!size.cx && !size.cy);
    bitmap.query_ok = true; bitmap.width = 16; bitmap.height = 8;
    size = CU_GetBitmapSize(bitmap);
    assert(size.cx == 16 && size.cy == 8);
    blits = 0;
    CU_TransparentBlt(&dc, bitmap, CPoint(), CSize(-1, 1), TRUE, 1, 4, 0, 1, 0, 0);
    assert(!blits);
}
}
''')
    parts.append("void TestPatchUi() { " + " ".join("PatchUi%d::Run();" % i for i in range(3)) +
                 ' puts("UI member initialization and invalid-bitmap regressions passed (three copies)"); }')
    parts.append("\n#undef LoadCursor\n#define LoadCursor LoadCursorW\n")
    parts.append(r'''
int message_boxes = 0;
int MockDevMessageBox(HWND, LPCTSTR message, LPCTSTR title, UINT) {
    assert(!wcscmp(message, L"message") && !wcscmp(title, L"title"));
    ++message_boxes; return 0;
}
#undef MessageBox
#define MessageBox MockDevMessageBox
namespace DevMessageTests {
''')
    parts.append(function(source(client / "client/DevTools/MsgBox/MsgBox.cpp"), "int _tmain(").replace(
        "int _tmain(", "int RunMessageBox(", 1))
    parts.append(r'''
void Run() {
    wchar_t program[] = L"program", message[] = L"message", title[] = L"title";
    wchar_t* args[] = {program, message, title, nullptr};
    assert(RunMessageBox(0, nullptr) == 1);
    assert(RunMessageBox(1, args) == 1 && RunMessageBox(2, args) == 1);
    assert(!message_boxes);
    assert(!RunMessageBox(3, args) && message_boxes == 1);
}
}
#undef MessageBox
#define MessageBox MessageBoxW
''')
    return "\n".join(parts)
