"""Actual web drag state and query-failure handling from both implementations."""
import re


def fixtures(client, source, function):
    game = client / "client/UE3/Development/Src/MMOGame"
    apis = ["GetWindowRect", "GetClientRect", "GetCursorPos", "ScreenToClient", "MoveWindow", "GetAsyncKeyState", "PtInRect", "RedrawWindow"]
    parts = ["\n".join('#pragma push_macro("' + name + '")\n#undef ' + name + '\n#define ' + name + ' MockWebDrag' + name for name in apis), r'''
namespace WebDragTests {
using INT = int;
HWND GGameWindow = (HWND)uintptr_t(1);
HWND WebWindow = (HWND)uintptr_t(2);
struct Move { INT x, y, width, height; };
struct State {
    INT queries = 0, fail_query = 0, redraws = 0, browser_redraws = 0;
    bool mouse_down = true;
    POINT cursor{300, 400};
    RECT game{100, 200, 1110, 825}, client{0, 0, 1000, 600}, web{200, 300, 500, 500};
    std::vector<Move> moves;
} state;
bool Query() { return ++state.queries != state.fail_query; }
BOOL MockWebDragGetWindowRect(HWND window, RECT* result) {
    if (!Query()) return FALSE;
    assert(window == GGameWindow || window == WebWindow);
    *result = window == GGameWindow ? state.game : state.web; return TRUE;
}
BOOL MockWebDragGetClientRect(HWND window, RECT* result) {
    assert(window == GGameWindow); if (!Query()) return FALSE; *result = state.client; return TRUE;
}
BOOL MockWebDragGetCursorPos(POINT* result) { if (!Query()) return FALSE; *result = state.cursor; return TRUE; }
BOOL MockWebDragScreenToClient(HWND window, POINT* point) {
    assert(window == GGameWindow); if (!Query()) return FALSE;
    point->x -= state.game.left + 5; point->y -= state.game.top + 20; return TRUE;
}
BOOL MockWebDragMoveWindow(HWND window, INT x, INT y, INT width, INT height, BOOL redraw) {
    assert(window == WebWindow && redraw && width > 0 && height > 0);
    state.moves.push_back({x, y, width, height});
    state.web = {state.game.left + 5 + x, state.game.top + 20 + y,
                 state.game.left + 5 + x + width, state.game.top + 20 + y + height};
    return TRUE;
}
SHORT MockWebDragGetAsyncKeyState(INT key) { assert(key == VK_LBUTTON); return state.mouse_down ? SHORT(0x8000) : 0; }
BOOL MockWebDragPtInRect(const RECT* rectangle, POINT point) {
    return point.x >= rectangle->left && point.x < rectangle->right && point.y >= rectangle->top && point.y < rectangle->bottom;
}
}
BOOL MockWebDragRedrawWindow(HWND window, const RECT*, HRGN, UINT) {
    assert(window == WebDragTests::WebWindow); ++WebDragTests::state.redraws; return TRUE;
}
namespace WebDragTests {
''']
    copies = sorted(game.glob("WebHtmlManager*.cpp"))
    if len(copies) != 2:
        raise ValueError("Expected both web implementations")
    for index, path in enumerate(copies):
        cpp = re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", source(path))
        parts.append("namespace Copy%d {" % index)
        parts.append(r'''
struct WebHtmlManager {
    static WebHtmlManager& getSingleton() { static WebHtmlManager value; return value; }
    void RedrawWebWindow() { ++state.browser_redraws; }
};
''')
        parts.append(function(cpp, "struct CursorDragWebWindowCaption") + ";")
        for name in ["SetHwnd", "BeginDrag", "BeginDragSize", "EndDrag", "DragProcess"]:
            parts.append(function(cpp, "void CursorDragWebWindowCaption::" + name + "("))
        parts.append("}")
    parts.append(r'''
template<class T> void Check() {
    for (BYTE pattern : {BYTE(0xa5), BYTE(0x5a)}) {
        alignas(T) BYTE bytes[sizeof(T)]; memset(bytes, pattern, sizeof(bytes));
        T* drag = new(bytes) T;
        assert(!drag->bBenginDrag && !drag->bBeginDragSize && !drag->nRelHwnd && !drag->nFrameSizeState);
        assert(!drag->nOldCursorPoint.x && !drag->nOldCursorPoint.y);
        assert(!drag->nOldWndRect.left && !drag->nOldWndRect.top && !drag->nOldWndRect.right && !drag->nOldWndRect.bottom);
        assert(drag->nLstFramePoint.x == -123 && !drag->nLstFramePoint.y && !drag->nFrameSize0 && !drag->nFrameSize1);
        drag->~T();
    }
    for (INT fail : {1, 2}) {
        state = State(); state.fail_query = fail; T drag;
        drag.nFrameSize0 = drag.nFrameSize1 = 123; drag.bBenginDrag = drag.bBeginDragSize = true;
        drag.SetHwnd(WebWindow);
        assert(!drag.nFrameSize0 && !drag.nFrameSize1 && !drag.bBenginDrag && !drag.bBeginDragSize);
    }
    state = State(); T unattached; unattached.SetHwnd(nullptr);
    unattached.BeginDrag(); unattached.BeginDragSize(8);
    assert(!state.queries && !unattached.bBenginDrag && !unattached.bBeginDragSize);
    for (INT fail : {1, 2, 3}) {
        for (bool resize : {false, true}) {
            state = State(); T drag; drag.SetHwnd(WebWindow);
            state.queries = 0; state.fail_query = fail;
            drag.bBenginDrag = drag.bBeginDragSize = true; drag.nFrameSizeState = 8;
            if (resize) drag.BeginDragSize(8); else drag.BeginDrag();
            assert(!drag.bBenginDrag && !drag.bBeginDragSize && !drag.nFrameSizeState && state.moves.empty());
        }
    }
    for (INT invalid : {(-2147483647 - 1), -1, 0, 9, 2147483647}) {
        state = State(); T drag; drag.SetHwnd(WebWindow); state.queries = 0;
        drag.BeginDragSize(invalid);
        assert(!state.queries && !drag.bBeginDragSize && !drag.bBenginDrag && !drag.nFrameSizeState);
    }
    for (INT fail : {1, 2, 3, 4}) {
        for (bool resize : {false, true}) {
            state = State(); T drag; drag.SetHwnd(WebWindow);
            if (resize) drag.BeginDragSize(8); else drag.BeginDrag();
            state.queries = 0; state.fail_query = fail; state.cursor = {320, 430};
            drag.DragProcess();
            assert(!drag.bBenginDrag && !drag.bBeginDragSize && !drag.nFrameSizeState);
            assert(state.moves.empty() && !state.redraws && !state.browser_redraws);
        }
    }
    state = State(); T drag; drag.SetHwnd(WebWindow);
    assert(drag.nFrameSize0 == 5 && drag.nFrameSize1 == 20);
    drag.BeginDrag();
    assert(drag.bBenginDrag && !drag.bBeginDragSize && drag.nOldCursorPoint.x == 195 && drag.nOldCursorPoint.y == 180);
    state.cursor = {320, 430}; drag.DragProcess();
    assert(state.moves.size() == 1 && state.moves[0].x == 115 && state.moves[0].y == 110);
    assert(state.moves[0].width == 300 && state.moves[0].height == 200 && state.redraws == 1 && state.browser_redraws == 1);
    assert(drag.nLstFramePoint.x == 215 && drag.nLstFramePoint.y == 210);
    drag.DragProcess(); assert(state.moves.size() == 1 && state.redraws == 1);
    state.mouse_down = false; drag.DragProcess(); assert(!drag.bBenginDrag && !drag.bBeginDragSize);
    const Move expected[] = {{115,80,280,200}, {95,80,320,200}, {95,110,300,170}, {115,110,280,170},
                             {95,110,320,170}, {95,80,300,230}, {115,80,280,230}, {95,80,320,230}};
    for (INT edge = 1; edge <= 8; ++edge) {
        state = State(); T resize; resize.SetHwnd(WebWindow); resize.BeginDrag(); resize.BeginDragSize(edge);
        assert(!resize.bBenginDrag && resize.bBeginDragSize);
        state.cursor = {320, 430}; resize.DragProcess();
        assert(state.moves.size() == 1);
        const Move& actual = state.moves[0]; const Move& want = expected[edge - 1];
        assert(actual.x == want.x && actual.y == want.y && actual.width == want.width && actual.height == want.height);
        resize.DragProcess(); assert(state.moves.size() == 1);
        resize.SetHwnd(nullptr); assert(!resize.bBeginDragSize && !resize.bBenginDrag);
    }
    state = State(); T cleared; cleared.bBenginDrag = cleared.bBeginDragSize = true; cleared.nFrameSizeState = 8;
    cleared.DragProcess(); assert(!cleared.bBenginDrag && !cleared.bBeginDragSize && !cleared.nFrameSizeState);
}
void Run() {
    Check<Copy0::CursorDragWebWindowCaption>(); Check<Copy1::CursorDragWebWindowCaption>();
    puts("Web drag defaults, every cursor/rectangle query failure, stationary frames and all eight resize edges passed (two copies)");
}
}
''')
    parts.append("\n".join('#pragma pop_macro("' + name + '")' for name in reversed(apis)))
    return "\n".join(parts)
