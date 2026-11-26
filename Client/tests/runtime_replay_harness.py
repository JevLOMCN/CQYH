"""Exercise optional in-game replay serialization without enabling the feature."""
import re


def harness(game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    header = clean(source(game / "ReplayManager.h"))
    implementation = clean(source(game / "ReplayManager.cpp"))
    parts = [r'''
#include <list>
namespace RuntimeReplayTests {
#undef TEXT
#define TEXT(s) L##s
using TCHAR = wchar_t;
using DOUBLE = double;
using NAME_INDEX = INT;
using FString = NetworkTests::FString;
enum EName { NAME_None = 0 };
struct FName {
    INT index, number;
    FName(EName name = NAME_None, INT value = 0) : index(name), number(value) {
        assert(index >= 0 && index < GetMaxNames() && GetEntry(index) && number >= 0);
    }
    INT GetIndex() const { return index; }
    INT GetNumber() const { return number; }
    static INT GetMaxNames() { return 16; }
    static void* GetEntry(INT index) { static INT value; return index == 5 ? nullptr : &value; }
};
enum EInputEvent { IE_Pressed, IE_Released, IE_Repeat, IE_DoubleClick, IE_Axis, IE_MAX };
enum { g2c_frame_signal = 1, g2c_focus_hifreq_sync, g2c_self_hifreq_sync,
       g2c_region_hifreq_sync, t2c_ping_ack, g2c_ping_ack, g2c_char_move };
struct FArchive {
    std::vector<char> bytes;
    INT position = 0, failAt = -1;
    bool saving = false, error = false;
    bool IsSaving() const { return saving; }
    bool IsError() const { return error; }
    bool AtEnd() const { return position >= (INT)bytes.size(); }
    INT Tell() const { return position; }
    INT TotalSize() const { return (INT)bytes.size(); }
    void Serialize(void* data, INT size) {
        if (size < 0 || position < 0 || (!saving && size > TotalSize() - position) ||
            (failAt >= 0 && position >= failAt)) { error = true; return; }
        if (saving) {
            const char* begin = static_cast<const char*>(data);
            bytes.insert(bytes.end(), begin, begin + size);
        } else if (size) memcpy(data, bytes.data() + position, size);
        position += size;
    }
    void ByteOrderSerialize(void* data, INT size) { Serialize(data, size); }
    template<class T> FArchive& operator<<(T& value) { Serialize(&value, sizeof(value)); return *this; }
    template<class T> void Append(T value) {
        const char* begin = reinterpret_cast<const char*>(&value);
        bytes.insert(bytes.end(), begin, begin + sizeof(value));
    }
};
''', header[header.index("enum EReplayDataType"):header.index("class ReplayManager")]]
    for name in ["Network", "Input", "ScreenRes"]:
        parts.append(function(implementation, "UBOOL FReplayData" + name + "::Serialize("))
        parts.append("void FReplayData" + name + "::DispatchReplayData() {}\n"
                     "FString FReplayData" + name + "::ToString() const { return FString(); }")
    parts.append(r'''
std::vector<FReplayData*> Load(FArchive& input) {
    FArchive* ar = &input;
    std::list<FReplayData*> _replayDataList;
''')
    parts.append(function(implementation, "while (!ar->AtEnd() && !ar->IsError())"))
    parts.append("return std::vector<FReplayData*>(_replayDataList.begin(), _replayDataList.end());\n}")
    parts.append(r'''
void Base(FArchive& archive) { archive.Append<UINT64>(123); archive.Append<DOUBLE>(2.5); }
template<class T> void Truncations(const FArchive& valid) {
    for (size_t length = 0; length < valid.bytes.size(); ++length) {
        FArchive truncated; truncated.bytes.assign(valid.bytes.begin(), valid.bytes.begin() + length);
        T record;
        assert(!record.Serialize(truncated));
    }
}
void Run() {
    check_constructed<FReplayDataNetwork>([](FReplayDataNetwork& record) {
        assert(!record.Index && !record.GameTime && !record.DataSize && !record.DataBuffer);
        assert(!record.IsHiFrequencyData());
    });
    check_constructed<FReplayDataInput>([](FReplayDataInput& record) {
        assert(!record.Index && !record.GameTime && !record.ControllerId && !record.MousePoint.X && !record.MousePoint.Y);
        assert(record.DataType == EIDT_Key && record.EventType == IE_Pressed && !record.Key.GetIndex());
        assert(!record.AmountDepressed && !record.GamePad && !record.Delta && !record.DeltaTime && !record.Character);
    });
    check_constructed<FReplayDataScreenRes>([](FReplayDataScreenRes& record) {
        assert(!record.Index && !record.GameTime && !record.ResX && !record.ResY && !record.IsFullScreen);
    });
    for (INT length : {-1, 0, 1, 65536, 0x7fffffff}) {
        FArchive input; Base(input); input.Append<INT>(ENDT_Recv); input.Append(length);
        FReplayDataNetwork record;
        assert(!record.Serialize(input) && !record.DataBuffer);
    }
    FArchive network;
    Base(network); network.Append<INT>(ENDT_Recv); network.Append<INT>(3);
    network.Append<char>(1); network.Append<char>(0); network.Append<char>(2);
    FReplayDataNetwork record;
    assert(record.Serialize(network) && record.DataSize == 3 && record.DataBuffer[2] == 2);
    FArchive write; write.saving = true;
    assert(record.Serialize(write) && write.bytes == network.bytes);
    char* previous = record.DataBuffer;
    FArchive failed; failed.bytes = network.bytes; failed.failAt = 24;
    const INT replacementSize = 64, replacementType = ENDT_Send;
    memcpy(failed.bytes.data() + 16, &replacementType, sizeof(replacementType));
    memcpy(failed.bytes.data() + 20, &replacementSize, sizeof(replacementSize));
    failed.bytes.resize(24 + replacementSize, 0);
    assert(!record.Serialize(failed) && record.DataBuffer == previous && record.DataSize == 3 && record.DataType == ENDT_Recv);
    delete[] record.DataBuffer; record.DataBuffer = nullptr;
    Truncations<FReplayDataNetwork>(network);
    for (INT type = EIDT_Key; type <= EIDT_MousePosition; ++type) {
        FArchive input; Base(input); input.Append(type);
        input.Append<INT>(11); input.Append<INT>(22); input.Append<INT>(1);
        if (type == EIDT_Key) { input.Append<INT>(3); input.Append<INT>(4); }
        input.Append<INT>(IE_Pressed); input.Append<FLOAT>(1); input.Append<UBOOL>(FALSE);
        input.Append<FLOAT>(2); input.Append<FLOAT>(3); input.Append<TCHAR>(L'A');
        FReplayDataInput value;
        assert(value.Serialize(input) && value.ControllerId == 1 && value.Character == L'A');
        FArchive saved; saved.saving = true;
        assert(value.Serialize(saved) && saved.bytes == input.bytes);
        Truncations<FReplayDataInput>(input);
        if (type == EIDT_Key) {
            for (INT index : {-1, 5, 16, 0x7fffffff}) {
                FArchive bad; bad.bytes = input.bytes;
                memcpy(bad.bytes.data() + 32, &index, sizeof(index));
                FReplayDataInput invalid;
                assert(!invalid.Serialize(bad));
            }
            for (INT event : {-1, (INT)IE_MAX, 0x7fffffff}) {
                FArchive bad; bad.bytes = input.bytes;
                memcpy(bad.bytes.data() + 40, &event, sizeof(event));
                FReplayDataInput invalid;
                assert(!invalid.Serialize(bad));
            }
            FArchive badNumber; badNumber.bytes = input.bytes;
            const INT negativeNumber = -1;
            memcpy(badNumber.bytes.data() + 36, &negativeNumber, sizeof(negativeNumber));
            FReplayDataInput invalidNumber;
            assert(!invalidNumber.Serialize(badNumber));
        }
    }
    FArchive resolution; resolution.Append<UINT>(1920); resolution.Append<UINT>(1080); resolution.Append<UBOOL>(TRUE);
    FReplayDataScreenRes screen;
    assert(screen.Serialize(resolution) && screen.ResX == 1920 && screen.ResY == 1080);
    Truncations<FReplayDataScreenRes>(resolution);
    FArchive file;
    file.Append<EReplayDataType>(ERDT_Network);
    file.bytes.insert(file.bytes.end(), network.bytes.begin(), network.bytes.end());
    const INT validBytes = (INT)file.bytes.size();
    file.Append<INT>(999); // Unknown record type: stop, preserving the preceding valid record.
    auto records = Load(file);
    assert(records.size() == 1 && file.Tell() == validBytes + sizeof(INT));
    for (auto* value : records) value->Destroy();
    FArchive truncated; truncated.Append<EReplayDataType>(ERDT_Network); truncated.Append<char>(0);
    assert(Load(truncated).empty());
    puts("Runtime replay initialization and serialization regressions passed");
}
#undef TEXT
#define TEXT(s) s
}
''')
    return "\n".join(parts)
