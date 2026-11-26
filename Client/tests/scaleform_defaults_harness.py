"""Actual Scaleform getter bodies, including the integration-disabled stubs."""
import re


def harness(game, source, function):
    def clean(text):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", text)

    root = game.parent / "GFxUI"
    # Split before removing comments: the integration-mode separator is a comment.
    active, stub = source(root / "Src/ScaleformMovie.cpp").split("#else // WITH_GFx = 0", 1)
    active, stub = clean(active), clean(stub)
    header = clean(source(root / "Inc/GFxUIClasses.h"))
    records = [function(header, "struct " + name + "\n") + ";" for name in ["FASDisplayInfo", "FASColorTransform"]]
    node = function(clean(source(game / "MMOFacade.cpp")), "struct ShowNodeEx\n") + ";"
    parts = [r'''
#include <cmath>
#include <limits>
#pragma push_macro("SCRIPT_ALIGN")
#undef SCRIPT_ALIGN
#define SCRIPT_ALIGN
''']
    signatures = ["struct FASDisplayInfo UGFxObject::GetDisplayInfo()", "FMatrix UGFxObject::GetDisplayMatrix()",
                  "FMatrix UGFxObject::GetDisplayMatrix3D()", "FASColorTransform UGFxObject::GetColorTransform()",
                  "UBOOL UGFxObject::GetPosition ( FLOAT& X, FLOAT& Y )",
                  "struct FASDisplayInfo UGFxObject::GetElementDisplayInfo ( INT Index )",
                  "FMatrix UGFxObject::GetElementDisplayMatrix ( INT Index )"]
    for enabled, text in [(0, stub), (1, active)]:
        parts.append("namespace ScaleformDefault%dTests {" % enabled)
        parts.append(r'''
using BITFIELD = UINT;
enum EEventParm { EC_EventParm };
struct FLinearColor {
    FLOAT R, G, B, A;
    FLinearColor() : R(std::numeric_limits<FLOAT>::quiet_NaN()), G(R), B(R), A(R) {}
    FLinearColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a) : R(r), G(g), B(b), A(a) {}
};
struct FMatrix {
    FLOAT M[4][4];
    FMatrix() { for (auto& row : M) for (auto& v : row) v = std::numeric_limits<FLOAT>::quiet_NaN(); }
    static const FMatrix Identity;
};
FMatrix MakeIdentity() {
    FMatrix result;
    for (INT row = 0; row < 4; ++row) for (INT column = 0; column < 4; ++column) result.M[row][column] = row == column ? 1.0f : 0.0f;
    return result;
}
const FMatrix FMatrix::Identity = MakeIdentity();
template<INT Rows> struct Matrix {
    FLOAT M[Rows][4];
    Matrix() { for (auto& row : M) for (auto& v : row) v = std::numeric_limits<FLOAT>::quiet_NaN(); }
};
using Matrix2F = Matrix<2>; using Matrix3F = Matrix<3>; using Cxform = Matrix<2>;
''')
        parts.extend(records)
        parts.append(r'''
namespace GFx {
struct Value {
    struct DisplayInfo {
        enum { V_alpha=1, V_visible=2, V_x=4, V_y=8, V_xscale=16, V_yscale=32, V_rotation=64,
               V_z=128, V_zscale=256, V_xrotation=512, V_yrotation=1024 };
        FLOAT values[10];
        UINT flags = 0;
        DisplayInfo() { for (auto& v : values) v = std::numeric_limits<FLOAT>::quiet_NaN(); }
        FLOAT GetX() const { return values[0]; } FLOAT GetY() const { return values[1]; }
        FLOAT GetZ() const { return values[2]; } FLOAT GetRotation() const { return values[3]; }
        FLOAT GetXRotation() const { return values[4]; } FLOAT GetYRotation() const { return values[5]; }
        FLOAT GetXScale() const { return values[6]; } FLOAT GetYScale() const { return values[7]; }
        FLOAT GetZScale() const { return values[8]; } FLOAT GetAlpha() const { return values[9]; }
        bool GetVisible() const { return true; } bool IsFlagSet(UINT flag) const { return (flags & flag) != 0; }
    } info;
    Matrix2F matrix2; Matrix3F matrix3; Cxform colors;
    bool display = false, array = false, query_ok = true;
    std::vector<Value> elements;
    Value() {
        for (INT i = 0; i < 10; ++i) info.values[i] = FLOAT(i + 1);
        info.flags = 2047;
        FLOAT value = 1;
        for (auto& row : matrix2.M) for (auto& v : row) v = value++;
        matrix2.M[0][3] = 11; matrix2.M[1][3] = 17;
        value = 1; for (auto& row : matrix3.M) for (auto& v : row) v = value++;
        value = 1; for (auto& row : colors.M) for (auto& v : row) v = value++;
    }
    bool IsDisplayObject() const { return display; } bool IsArray() const { return array; }
    bool GetDisplayInfo(DisplayInfo* result) { if (!query_ok) return false; *result = info; return true; }
    bool GetDisplayMatrix(Matrix2F* result) { if (!query_ok) return false; *result = matrix2; return true; }
    bool GetMatrix3D(Matrix3F* result) { if (!query_ok) return false; *result = matrix3; return true; }
    bool GetColorTransform(Cxform* result) { if (!query_ok) return false; *result = colors; return true; }
    bool GetElement(INT index, Value* result) {
        if (index < 0 || (size_t)index >= elements.size()) return false;
        *result = elements[index]; return true;
    }
};
}
struct UGFxObject {
    GFx::Value* Value;
    explicit UGFxObject(GFx::Value* value) : Value(value) {}
    FASDisplayInfo GetDisplayInfo(); FASDisplayInfo GetElementDisplayInfo(INT);
    FMatrix GetDisplayMatrix(); FMatrix GetDisplayMatrix3D(); FMatrix GetElementDisplayMatrix(INT);
    FASColorTransform GetColorTransform(); UBOOL GetPosition(FLOAT&, FLOAT&);
};
''')
        if enabled:
            for signature in ["static void GFxDisplayInfoToU (", "static void ConvertMatrix2To4 (", "static void ConvertMatrix3To4 ("]:
                parts.append(function(active, signature))
        parts.extend(function(text, signature) for signature in signatures)
        parts.append("struct AMMOActor;\n" + node)
        parts.append(r'''
void Identity(const FMatrix& matrix) {
    for (INT row = 0; row < 4; ++row) for (INT column = 0; column < 4; ++column)
        assert(matrix.M[row][column] == (row == column ? 1.0f : 0.0f));
}
void Empty(const FASDisplayInfo& value) {
    assert(!value.X && !value.Y && !value.Z && !value.Rotation && !value.XRotation && !value.YRotation);
    assert(!value.XScale && !value.YScale && !value.ZScale && !value.Alpha && !value.Visible);
    assert(!value.hasX && !value.hasY && !value.hasZ && !value.hasRotation && !value.hasXRotation && !value.hasYRotation);
    assert(!value.hasXScale && !value.hasYScale && !value.hasZScale && !value.hasAlpha && !value.hasVisible);
}
void ColorIdentity(const FASColorTransform& color) {
    assert(color.Multiply.R == 1 && color.Multiply.G == 1 && color.Multiply.B == 1 && color.Multiply.A == 1);
    assert(!color.Add.R && !color.Add.G && !color.Add.B && !color.Add.A);
}
void Failed(UGFxObject& object) {
    FLOAT x = 123, y = 456;
    assert(!object.GetPosition(x, y) && !x && !y);
    Empty(object.GetDisplayInfo()); Identity(object.GetDisplayMatrix()); Identity(object.GetDisplayMatrix3D());
    ColorIdentity(object.GetColorTransform());
}
void ElementFailed(UGFxObject& object, INT index) {
    Empty(object.GetElementDisplayInfo(index)); Identity(object.GetElementDisplayMatrix(index));
}
void Run() {
    ScreenRecordDefaultsTests::DefaultConstruct<ShowNodeEx>([](auto& v) { assert(!v.Distance && !v.bVisible && !v.bPet && !v.Actor); });
    GFx::Value value; UGFxObject object(&value);
    Failed(object);
    ElementFailed(object, -1); ElementFailed(object, 0);
    value.display = true; value.query_ok = false;
    Failed(object);
    value.array = true; value.elements.resize(1);
    ElementFailed(object, -1); ElementFailed(object, 0); ElementFailed(object, 1); ElementFailed(object, 2147483647);
    value.elements[0].display = true; value.elements[0].query_ok = false;
    ElementFailed(object, 0);
''')
        if enabled:
            parts.append(r'''
    value.query_ok = true;
    FLOAT x = -1, y = -1; assert(object.GetPosition(x, y) && x == 1 && y == 2);
    auto info = object.GetDisplayInfo();
    assert(info.X == 1 && info.Y == 2 && info.Z == 3 && info.Rotation == 4 && info.XRotation == 5 && info.YRotation == 6);
    assert(info.XScale == 7 && info.YScale == 8 && info.ZScale == 9 && info.Alpha == 10 && info.Visible);
    assert(info.hasX && info.hasY && info.hasZ && info.hasRotation && info.hasXRotation && info.hasYRotation);
    assert(info.hasXScale && info.hasYScale && info.hasZScale && info.hasAlpha && info.hasVisible);
    auto matrix = object.GetDisplayMatrix();
    assert(matrix.M[3][0] == 11 && matrix.M[3][1] == 17 && matrix.M[3][2] == 0 && matrix.M[3][3] == 1);
    matrix = object.GetDisplayMatrix3D();
    for (INT row = 0; row < 3; ++row) for (INT column = 0; column < 4; ++column)
        assert(matrix.M[column][row] == value.matrix3.M[row][column]);
    auto color = object.GetColorTransform();
    assert(color.Multiply.R == 1 && color.Multiply.G == 2 && color.Multiply.B == 3 && color.Multiply.A == 4);
    assert(color.Add.R == 5 && color.Add.G == 6 && color.Add.B == 7 && color.Add.A == 8);
    value.elements[0].query_ok = true;
    assert(object.GetElementDisplayInfo(0).X == 1 && object.GetElementDisplayInfo(0).ZScale == 9);
    matrix = object.GetElementDisplayMatrix(0);
    assert(matrix.M[3][0] == 11 && matrix.M[3][1] == 17);
    puts("Scaleform getter failure outputs, successful records/colors/matrices and distinct X/Y translation passed");
''')
        else:
            parts.append(r'''
    value.query_ok = true; value.elements[0].query_ok = true;
    Failed(object); ElementFailed(object, 0);
    puts("Scaleform-disabled getters return defined coordinates, display records and identity transforms");
''')
        parts.append("}\n}")
    parts.append('#pragma pop_macro("SCRIPT_ALIGN")')
    return "\n".join(parts)
