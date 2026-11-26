"""Actual message-button records and manager paths, including reentrant callbacks."""
import re


def harness(client, game, source, function):
    def clean(text):
        text = re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)
        return re.sub(r"(?m)^\s*#(?:include|pragma)[^\n]*", "", text)

    parts = [r'''
#include <functional>
namespace MessageButtonTests {
using FString = std::string;
enum EEventParm { EC_EventParm };
enum { NAME_None = 0 };
struct FName {
    int value;
    FName() {}
    FName(int v) : value(v) {}
    bool IsValid() const { return value >= 0; }
    bool operator!=(int v) const { return value != v; }
};
struct UObject {
    int calls = 0;
    std::function<void(void*)> callback;
    void* FindFunctionChecked(FName) { return nullptr; }
    void ProcessEvent(void*, void* params, void*) { ++calls; if (callback) callback(params); }
};
''']
    delegate = source(game.parent / "Core/Inc/UnObjBas.h")
    start = delegate.index("struct FScriptDelegate\n")
    end = delegate.index("\n\tinline UBOOL IsCallable", start)
    parts.append(clean(delegate[start:end]) + "\n};")
    parts.append(r'''
template<class T> struct SingletonMeyers {
    static T& getSingleton() { static T value; return value; }
};
struct UGFxObject {
    UGFxObject* yes = nullptr;
    UGFxObject* no = nullptr;
    std::map<std::string, FString> labels;
    UGFxObject* GetObject(const char* name) { return name[0] == 'y' ? yes : no; }
    void SetString(const char* key, const FString& value) { labels[key] = value; }
};
''')
    event = source(game / "MMOGameEvent.h")
    parts.append(function(clean(event), "struct MMOGameEvent_Any") + ";")
    common = clean(source(game / "CommonGameEvents.h"))
    for name in ["MMOGameEvent_OneInt", "MMOGameEvent_AddMsgBall"]:
        parts.append(function(common, "struct " + name) + ";")
    parts.append(r'''
namespace MMOUI {
int events = 0;
void PostGameEvent(MMOGameEvent*) { ++events; }
FString ParseLocalizedPropertyPath(const FString& text) { return text; }
}
''')
    header = clean(source(game / "Src/MMOMsgButtonManager.h"))
    header = header.replace(", M_UE3_STL_ALLOC(MsgButtonInfo)", "").replace("protected:", "public:")
    parts.append(header)
    boxes = clean(source(game / "Src/MMOMsgBoxManager.h"))
    for kind, name in [("enum", "MsgboxType"), ("struct", "MsgInfoEvent"), ("struct", "MsgInfo")]:
        parts.append(function(boxes, kind + " " + name + "\n") + ";")
    parts.append(r'''
struct MMOMsgBoxManager : SingletonMeyers<MMOMsgBoxManager> {
    int created = 0;
    std::function<void()> on_create;
    FString text;
    int info = 0, type = 0;
    UGFxObject root, yes, no;
    MMOMsgBoxManager() { root.yes = &yes; root.no = &no; }
    UGFxObject* AddMessageBoxEvent(const FString& message, INT id, EGameEvent, eMsgBtnShowType category) {
        ++created; text = message; info = id; type = category;
        if (on_create) on_create();
        return &root;
    }
};
''')
    parts.append(clean(source(game / "Src/MMOMsgButtonManager.cpp")))
    parts.append(r'''
void Run() {
    for (unsigned char pattern : {0xa5, 0x5a}) {
        alignas(MsgButtonInfo) unsigned char bytes[sizeof(MsgButtonInfo)];
        memset(bytes, pattern, sizeof(bytes));
        MsgButtonInfo* button = new(bytes) MsgButtonInfo;
        assert(button->infoId == 0 && button->EventId == GameEvent_Invalid);
        assert(!button->OnEventHandle.Object && button->OnEventHandle.FunctionName.value == NAME_None);
        assert(!button->_Parms.Parm1 && !button->_Parms.Parm2 && !button->_Parms.Parm3);
        assert(button->type == E_MsgBtnShow_Max && button->_eventHandleType == EVENTHANDLE_GAMEEVENT);
        assert(button->leftButtonName == "MMOGame.OKMsg" && button->rightButtonName == "MMOGame.CancelMsg");
        button->~MsgButtonInfo();
        alignas(MsgInfo) unsigned char info_bytes[sizeof(MsgInfo)];
        memset(info_bytes, pattern, sizeof(info_bytes));
        MsgInfo* info = new(info_bytes) MsgInfo;
        assert(!info->MsgId && !info->OnClick.Object && !info->OnClickNo.Object);
        assert(!info->OnClick.FunctionName.value && !info->OnClickNo.FunctionName.value);
        assert(info->type == EMsgboxType_Modeless);
        info->~MsgInfo();
        alignas(MsgInfoEvent) unsigned char event_bytes[sizeof(MsgInfoEvent)];
        memset(event_bytes, pattern, sizeof(event_bytes));
        MsgInfoEvent* event = new(event_bytes) MsgInfoEvent;
        assert(!event->MsgId && event->OnClickEventId == GameEvent_Invalid);
    }
    MMOMsgButtonManager manager;
    auto& boxes = MMOMsgBoxManager::getSingleton();
    FScriptDelegate empty(EC_EventParm);
    MMOMsgButtonManager_Parms params(EC_EventParm);
    for (int type : {-2147483647 - 1, -1, int(E_MsgBtnShow_Max), E_MsgBtnShow_Max + 1, 2147483647}) {
        auto category = static_cast<eMsgBtnShowType>(type);
        int before = MMOUI::events;
        assert(manager.AddNewButton("text", 1, "sender", GameEvent_Invalid, category) == -1);
        assert(manager.AddNewButton(empty, params, category) == -1);
        assert(manager.GetMsgButtonInfo(type).empty());
        manager.ClickButton(type); manager.ClickMsgLine(0, type); manager._EmitDelegate(type);
        manager.UpdateButton(type); manager.UpdateMsgLine(type); manager.ToggleMsgLine(type);
        manager.RemoveOneTypeButtons(type);
        assert(MMOUI::events == before && !boxes.created);
    }
    FString left(80, 'L'), right(80, 'R');
    assert(manager.AddNewButton("text", 123, "sender", GameEvent_Invalid, E_MsgBtnShow_Mail, left, right) == 0);
    assert(!manager.GetMsgButtonInfo(E_MsgBtnShow_Mail)[0].OnEventHandle.Object);
    manager.ClickMsgLine(-1, E_MsgBtnShow_Mail);
    manager.ClickMsgLine(1, E_MsgBtnShow_Mail);
    manager.ClickMsgLine(2147483647, E_MsgBtnShow_Mail);
    assert(!boxes.created && manager.GetMsgButtonInfo(E_MsgBtnShow_Mail).size() == 1);

    // Widget creation can synchronously dispatch events that clear/reallocate the queue.
    boxes.on_create = [&] {
        auto& list = manager._MsgButtonInfoList[E_MsgBtnShow_Mail];
        MsgButtonInfoVec().swap(list);
    };
    manager.ClickButton(E_MsgBtnShow_Mail);
    assert(boxes.created == 1 && boxes.info == 123 && boxes.text == "text");
    assert(boxes.yes.labels["label"] == left && boxes.no.labels["label"] == right);
    assert(manager.GetMsgButtonInfo(E_MsgBtnShow_Mail).empty());
    assert(manager.AddNewButton("row", 456, "sender", GameEvent_Invalid, E_MsgBtnShow_Mail, left, right) == 0);
    manager.ClickMsgLine(0, E_MsgBtnShow_Mail);
    assert(boxes.created == 2 && boxes.info == 456 && boxes.text == "row");
    assert(boxes.yes.labels["label"] == left && boxes.no.labels["label"] == right);
    boxes.on_create = nullptr;

    UObject receiver;
    params.Parm1 = 123; params.Parm2 = 456; params.Parm3 = 789;
    receiver.callback = [&](void* arguments) {
        MsgButtonInfoVec().swap(manager._MsgButtonInfoList[E_MsgBtnShow_Guild]);
        auto* p = static_cast<MMOMsgButtonManager_Parms*>(arguments);
        assert(p->Parm1 == 123 && p->Parm2 == 456 && p->Parm3 == 789);
    };
    FScriptDelegate callback(&receiver, FName(1));
    assert(manager.AddNewButton(callback, params, E_MsgBtnShow_Guild, left, right) == 0);
    const auto& stored = manager.GetMsgButtonInfo(E_MsgBtnShow_Guild)[0];
    assert(stored.type == E_MsgBtnShow_Guild && stored.EventId == GameEvent_Invalid);
    assert(stored.OnEventHandle.Object == &receiver && stored.OnEventHandle.FunctionName.value == 1);
    manager.ClickButton(E_MsgBtnShow_Guild);
    assert(receiver.calls == 1 && manager.GetMsgButtonInfo(E_MsgBtnShow_Guild).empty());
    puts("Message-button initialization, index bounds and reentrant-callback regressions passed");
}
}
''')
    return "\n".join(parts)
