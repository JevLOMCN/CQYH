"""Actual final-path-point and three global-map-marker update methods."""
import re


def harness(game, source, function):
    def clean(path):
        return re.sub(r"/\*[\s\S]*?\*/|//[^\n]*", "", source(game / path))

    parts = [r'''
#pragma push_macro("SCOPE_CYCLE_COUNTER")
#undef SCOPE_CYCLE_COUNTER
#define SCOPE_CYCLE_COUNTER(value) ((void)0)
namespace NavigationMarkerTests {
using FVectorBase = PositionDefaultTests::FVector;
struct FVector : FVectorBase {
    using FVectorBase::FVectorBase;
    FVector(const FVectorBase& v) : FVectorBase(v) {}
    FLOAT DistanceSquared(const FVector& v) const { auto d = *this - v; return d.X*d.X + d.Y*d.Y + d.Z*d.Z; }
};
struct FVector2D {
    FLOAT X, Y;
    FVector2D() : X(std::numeric_limits<FLOAT>::quiet_NaN()), Y(X) {}
    FVector2D(FLOAT x, FLOAT y) : X(x), Y(y) {}
};
template<class T> using TArray = InventoryLookupTests::TArray<T>;
struct AMMOActor { FVector Location{0, 0, 0}; };
struct AMMOPlayerController { AMMOActor* Pawn = nullptr; };
struct World {
    bool has_info = true;
    World* GetWorldInfo() { return has_info ? this : nullptr; }
};
World world;
World* GWorld = &world;
struct MMOFacade {
    AMMOPlayerController controller;
    bool has_controller = true;
    std::map<UINT, AMMOActor*> actors;
    static MMOFacade& getSingleton() { static MMOFacade value; return value; }
    AMMOPlayerController* GetMainController() { return has_controller ? &controller : nullptr; }
    AMMOActor* GetCharacter(UINT id) { auto it = actors.find(id); return it == actors.end() ? nullptr : it->second; }
};
const FLOAT PATH_POINT_UPDATE_TIME = 0.25f, PATH_POINT_PLAYER_DIST_SQUARED_MAX = 100,
            PATH_EFFECT_DISAPPEAR_TIME = 2;
struct PathPointInfo {
    FVector Pos{0, 0, 0};
    UBOOL NeedDisappear = FALSE;
    FLOAT DisappearTime = 0;
};
struct MMONavigationManager {
    FLOAT _PathPointProgress = 0;
    TArray<PathPointInfo> _PathPointList;
    INT cleared = 0;
    void ClearPathEffect() { ++cleared; }
    void _FiltPathPoint(FVector&) {}
    void _UpdatePathPoint(FLOAT);
};
''', function(clean("Src/MMONavigationManager.cpp"), "void MMONavigationManager::_UpdatePathPoint("), r'''
void NavigationTests() {
    AMMOActor player; MMOFacade::getSingleton().controller.Pawn = &player;
    MMONavigationManager manager;
    manager._UpdatePathPoint(PATH_POINT_UPDATE_TIME); assert(manager.cleared == 1);
    // A one-point path and a final point after skipped disappearing entries used
    // to read an unset lastPathPointDir (the vector mock starts with NaNs).
    manager._PathPointList.values = {{{1, 0, 0}, FALSE, 0}};
    manager._UpdatePathPoint(PATH_POINT_UPDATE_TIME);
    assert(manager._PathPointList(0).NeedDisappear && manager._PathPointList(0).DisappearTime == 2);
    manager._PathPointList.values = {{{0, 0, 0}, TRUE, 1}, {{1, 0, 0}, FALSE, 0}};
    manager._UpdatePathPoint(PATH_POINT_UPDATE_TIME);
    assert(manager._PathPointList(0).DisappearTime == 0.75f && manager._PathPointList(1).NeedDisappear);
    manager._PathPointList.values = {{{100, 0, 0}, FALSE, 0}};
    manager._UpdatePathPoint(PATH_POINT_UPDATE_TIME); assert(!manager._PathPointList(0).NeedDisappear);
    manager._PathPointList.values = {{{0, 0, 0}, FALSE, 0}, {{5, 0, 0}, FALSE, 0}};
    player.Location = FVector(1, 0, 0);
    manager._UpdatePathPoint(PATH_POINT_UPDATE_TIME);
    assert(manager._PathPointList(0).NeedDisappear && !manager._PathPointList(1).NeedDisappear);
    manager._PathPointList.values = {{{1, 0, 0}, FALSE, 0}};
    manager._UpdatePathPoint(0.1f); assert(!manager._PathPointList(0).NeedDisappear);
    GWorld = nullptr; manager._UpdatePathPoint(1); assert(!manager._PathPointList(0).NeedDisappear); GWorld = &world;
    world.has_info = false; manager._UpdatePathPoint(1); assert(!manager._PathPointList(0).NeedDisappear); world.has_info = true;
    MMOFacade::getSingleton().has_controller = false;
    manager._UpdatePathPoint(1); assert(!manager._PathPointList(0).NeedDisappear);
    MMOFacade::getSingleton().has_controller = true; MMOFacade::getSingleton().controller.Pawn = nullptr;
    manager._UpdatePathPoint(1); assert(!manager._PathPointList(0).NeedDisappear);
    puts("Final path-point direction is defined for single/skipped paths; normal direction, distance and unavailable-world paths passed");
}
enum { EACTMUI_GlobalTeammate = 1, EACTMUI_BattleFlag = 2, EACTUIUF_WorldMap = 1,
       EACTUIUF_PathMap = 2, EACTUIUF_MiniMap = 4, E_WORLDMAP_TILES, E_PATHMAP_TILES, E_MINIMAP_TILES, E_est_Line };
struct UGFxObject {
    INT positions = 0;
    FLOAT x = 0, y = 0;
    bool visible = true;
    void SetPosition(FLOAT px, FLOAT py) { assert(std::isfinite(px) && std::isfinite(py)); x = px; y = py; ++positions; }
    void SetVisible(UBOOL v) { visible = !!v; }
    void SetFloat(const char*, FLOAT) {}
    void SetObject(const char*, UGFxObject*) {}
    void SetInt(const char*, INT) {}
    void SetString(const char*, const char*) {}
    void SetElementObject(INT, UGFxObject*) {}
};
struct GfxPlayer {
    UGFxObject object;
    UGFxObject* CreateArray() { return &object; }
    UGFxObject* CreateObject(const char*) { return &object; }
};
struct FActorInMapUI {
    INT _NeedUpdateFlag = 0, _InMapID = 7, _ID = 0, _Type = 0;
    UINT _ActorID = 0, _AidID = UINT(-1);
    UBOOL _IsValid = TRUE;
    const char* _UiIconIdx = "test";
    UGFxObject *_UiObjInWorldMap = nullptr, *_UiObjInPathMap = nullptr, *_UiObjInMiniMap = nullptr;
    FVector _OrigInMapPos{12, 16, 0};
};
struct FBox { UBOOL IsInside(FVector) const { return TRUE; } };
struct FInMapRegion { FBox BoundingBox; };
struct FExportMap { INT ServerID = 7, ServerMapType = E_est_Line; };
struct MMOTeamMember { FVector WorldPos{22, 26, 0}; INT MapID = 7, MapLineID = 1; };
struct TeamManager {
    MMOTeamMember member;
    static TeamManager& getSingleton() { static TeamManager value; return value; }
    MMOTeamMember* GetMemberById(UINT id) { return id == 10 ? &member : nullptr; }
};
struct MMOMapDataManager {
    std::vector<FActorInMapUI> markers;
    FInMapRegion region;
    FExportMap map;
    struct MapDynamicActorIterator {
        MMOMapDataManager* manager;
        size_t index = 0, steps = 0;
        explicit operator bool() { assert(++steps < manager->markers.size()*4 + 20); return index < manager->markers.size(); }
        FActorInMapUI& Value() { return manager->markers.at(index); }
        void operator++() { ++index; }
    };
    static MMOMapDataManager& getSingleton() { static MMOMapDataManager value; return value; }
    MapDynamicActorIterator BeginIterateDymcActorGlobal() { return {this}; }
    const FInMapRegion* GetCurWorldMapRegion() { return &region; }
    const FInMapRegion* GetCurReginInCurMap() { return &region; }
    const FExportMap& GetCurMap() { return map; }
    const FExportMap& GetCurWorldMap() { return map; }
    INT GetCurSceneLineIn() { return 1; }
    void RemoveMapDymcActorGlobal(UINT id) { for (auto& v : markers) if (v._ActorID == id) v._IsValid = FALSE; }
};
struct MMOMapImgTiles {
    static MMOMapImgTiles* GetMapImgTiles(INT) { static MMOMapImgTiles value; return &value; }
    FVector2D ScaledMap2DOffsetFromWorld3DPos(FVector pos, FVector center) { return {pos.X-center.X, pos.Y-center.Y}; }
};
struct PanelBase {
    GfxPlayer* GFXPlayer = nullptr;
    bool fail_create = false;
    UGFxObject created;
    INT flag;
    explicit PanelBase(INT f) : flag(f) {}
    UBOOL CreateMapMarkUIObject(FActorInMapUI* marker) {
        if (fail_create) return FALSE;
        if (flag == EACTUIUF_WorldMap) marker->_UiObjInWorldMap = &created;
        if (flag == EACTUIUF_PathMap) marker->_UiObjInPathMap = &created;
        if (flag == EACTUIUF_MiniMap) marker->_UiObjInMiniMap = &created;
        return TRUE;
    }
    void eventInitMapMark(UGFxObject*, INT, INT, const char*) {}
    void eventSetMapMarkIcon(UGFxObject*) {}
    FVector2D CvtScenePosToCurMinimapUiPosByScaledCenterPos(FVector pos, FVector2D center, FLOAT, FLOAT) { return {pos.X-center.X, pos.Y-center.Y}; }
};
struct UMMOUIWorldMap : PanelBase {
    UMMOUIWorldMap() : PanelBase(EACTUIUF_WorldMap) {}
    void UpdateMarksGlobalPlayer(FVector, float);
    void Update() { UpdateMarksGlobalPlayer(FVector(2, 6, 0), 0); }
};
struct UMMOUIPathMap : PanelBase {
    UMMOUIPathMap() : PanelBase(EACTUIUF_PathMap) {}
    void UpdateMarksGlobalPlayer(FVector, float);
    void Update() { UpdateMarksGlobalPlayer(FVector(2, 6, 0), 0); }
};
struct UMMOUIMinimap : PanelBase {
    UMMOUIMinimap() : PanelBase(EACTUIUF_MiniMap) {}
    void UpdateMarksGlobalPlayer(FVector2D, FLOAT, FLOAT);
    void Update() { UpdateMarksGlobalPlayer(FVector2D(2, 6), 1, 0); }
};
''']
    for file, cls in [("MMOUIWorldMap.cpp", "UMMOUIWorldMap"), ("MMOUIPathMap.cpp", "UMMOUIPathMap"), ("MMOMinimap.cpp", "UMMOUIMinimap")]:
        parts.append(function(clean("Src/" + file), "void " + cls + "::UpdateMarksGlobalPlayer("))
    parts.append(r'''
template<class Panel> void MarkerTests() {
    Panel panel; GfxPlayer player; panel.GFXPlayer = &player;
    auto& manager = MMOMapDataManager::getSingleton();
    UGFxObject invalid, flag, teammate;
    FActorInMapUI marker;
    marker._UiObjInWorldMap = marker._UiObjInPathMap = marker._UiObjInMiniMap = &invalid;
    marker._Type = 999;
    manager.markers = {marker};
    marker._Type = EACTMUI_BattleFlag;
    marker._UiObjInWorldMap = marker._UiObjInPathMap = marker._UiObjInMiniMap = &flag;
    manager.markers.push_back(marker);
    marker._Type = EACTMUI_GlobalTeammate; marker._ActorID = 10;
    marker._UiObjInWorldMap = marker._UiObjInPathMap = marker._UiObjInMiniMap = &teammate;
    manager.markers.push_back(marker);
    panel.Update();
    assert(!invalid.positions && flag.positions == 1 && teammate.positions == 1);
    assert(flag.x == 10 && flag.y == 10 && teammate.x == 20 && teammate.y == 20);
    AMMOActor nearby; nearby.Location = FVector(32, 36, 0);
    MMOFacade::getSingleton().actors[10] = &nearby;
    manager.markers[1]._AidID = 10;
    panel.Update();
    assert(!invalid.positions && flag.x == 30 && flag.y == 30 && teammate.x == 30 && teammate.y == 30);
    MMOFacade::getSingleton().actors.clear();
    manager.markers[2]._ActorID = 11;
    INT previous = teammate.positions;
    panel.Update(); assert(teammate.positions == previous && !manager.markers[2]._IsValid);
    // Missing/failed UI creation must also finish iterating, without positions
    // being computed from an unsupported type.
    marker._Type = 999; marker._NeedUpdateFlag = panel.flag;
    marker._UiObjInWorldMap = marker._UiObjInPathMap = marker._UiObjInMiniMap = nullptr;
    manager.markers = {marker}; panel.fail_create = true; panel.Update();
    assert(!panel.created.positions);
    manager.markers[0]._NeedUpdateFlag = panel.flag; panel.fail_create = false; panel.Update();
    assert(!panel.created.positions);
    manager.markers.clear(); panel.Update();
}
void Run() {
    NavigationTests();
    MarkerTests<UMMOUIWorldMap>(); MarkerTests<UMMOUIPathMap>(); MarkerTests<UMMOUIMinimap>();
    puts("All three map views skip unsupported marker positions, advance iteration and preserve teammate/flag locations");
}
}
#pragma pop_macro("SCOPE_CYCLE_COUNTER")
''')
    return "\n".join(parts)
