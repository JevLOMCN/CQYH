class ServerEnum extends Object native;

enum ECHAR_ATTR_TYPE
{
// conflicted enum definition catEndExAttr(53)
	E_cat_Invalid                     <DisplayName="E_cat_Invalid">,
	E_cat_Default                     <DisplayName="E_cat_Default">,
	E_cat_PhyDef                      <DisplayName="E_cat_PhyDef">,
	E_cat_MaxPhyDef                   <DisplayName="E_cat_MaxPhyDef">,
	E_cat_MagDef                      <DisplayName="E_cat_MagDef">,
	E_cat_MaxMagDef                   <DisplayName="E_cat_MaxMagDef">,
	E_cat_PhyAtk                      <DisplayName="E_cat_PhyAtk">,
	E_cat_MaxPhyAtk                   <DisplayName="E_cat_MaxPhyAtk">,
	E_cat_MagAtk                      <DisplayName="E_cat_MagAtk">,
	E_cat_MaxMagAtk                   <DisplayName="E_cat_MaxMagAtk">,
	E_cat_TaoAtk                      <DisplayName="E_cat_TaoAtk">,
	E_cat_MaxTaoAtk                   <DisplayName="E_cat_MaxTaoAtk">,
	E_cat_StabAtk                   <DisplayName="E_cat_StabAtk">,
	E_cat_MaxStabAtk                   <DisplayName="E_cat_MaxStabAtk">,
	E_cat_ArcheryAtk                   <DisplayName="E_cat_ArcheryAtk">,
	E_cat_MaxArcheryAtk                   <DisplayName="E_cat_MaxArcheryAtk">,
	E_cat_MaxHp                       <DisplayName="E_cat_MaxHp">,
	E_cat_MaxMp                       <DisplayName="E_cat_MaxMp">,
	E_cat_WalkSpeed                   <DisplayName="E_cat_WalkSpeed">,
	E_cat_RunSpeed                    <DisplayName="E_cat_RunSpeed">,
	E_cat_Accurate                    <DisplayName="E_cat_Accurate">,
	E_cat_Agility                     <DisplayName="E_cat_Agility">,
	E_cat_MagDodge                    <DisplayName="E_cat_MagDodge">,
	E_cat_Crit                        <DisplayName="E_cat_Crit">,
	E_cat_CritDmg                     <DisplayName="E_cat_CritDmg">,
	E_cat_CritResist                  <DisplayName="E_cat_CritResist">,
	E_cat_CritDmgResist               <DisplayName="E_cat_CritDmgResist">,
	E_cat_AntiPhyDef                  <DisplayName="E_cat_AntiPhyDef">,
	E_cat_AntiMagDef                  <DisplayName="E_cat_AntiMagDef">,
	E_cat_DrugHpRecovery              <DisplayName="E_cat_DrugHpRecovery">,
	E_cat_DrugMpRecovery              <DisplayName="E_cat_DrugMpRecovery">,
	E_cat_Luck                        <DisplayName="E_cat_Luck">,
	E_cat_HolyDmg                     <DisplayName="E_cat_HolyDmg">,
	E_cat_HurtEnhanced                <DisplayName="E_cat_HurtEnhanced">,
	E_cat_DmgResist                   <DisplayName="E_cat_DmgResist">,
	E_cat_DmgEnhance                  <DisplayName="E_cat_DmgEnhance">,
	E_cat_AtkSpeed                    <DisplayName="E_cat_AtkSpeed">,
	E_cat_HpRecovery                  <DisplayName="E_cat_HpRecovery">,
	E_cat_MpRecovery                  <DisplayName="E_cat_MpRecovery">,
	E_cat_HpRecoveryTap               <DisplayName="E_cat_HpRecoveryTap">,
	E_cat_MpRecoveryTap               <DisplayName="E_cat_MpRecoveryTap">,
	E_cat_TapIncrease                 <DisplayName="E_cat_TapIncrease">,
	E_cat_AtkSpeedRate                <DisplayName="E_cat_AtkSpeedRate">,
	E_cat_PoisonDef                   <DisplayName="E_cat_PoisonDef">,
	E_cat_PoisonRecovery              <DisplayName="E_cat_PoisonRecovery">,
	E_cat_PoisonResist                <DisplayName="E_cat_PoisonResist">,
	E_cat_StunResist                  <DisplayName="E_cat_StunResist">,
	E_cat_RepelResist                 <DisplayName="E_cat_RepelResist">,
	E_cat_HurtResist                  <DisplayName="E_cat_HurtResist">,
	E_cat_BurnResist                  <DisplayName="E_cat_BurnResist">,
	E_cat_WeaponWeightLmt             <DisplayName="E_cat_WeaponWeightLmt">,
	E_cat_ArmorWeightLmt              <DisplayName="E_cat_ArmorWeightLmt">,
	E_cat_BagWeightLmt                <DisplayName="E_cat_BagWeightLmt">,
	E_cat_DmgAbsorb                   <DisplayName="E_cat_DmgAbsorb">,
	E_cat_ExtraDmg                    <DisplayName="E_cat_ExtraDmg">,
	E_cat_ExtraHardTime               <DisplayName="E_cat_ExtraHardTime">,
	E_cat_PHitCountDown               <DisplayName="E_cat_PHitCountDown">,
	E_cat_PHitNum                     <DisplayName="E_cat_PHitNum">,
	E_cat_SpellPtsIncr                <DisplayName="E_cat_SpellPtsIncr">,
	E_cat_Leech                       <DisplayName="E_cat_Leech">,
	E_cat_MpToHp                      <DisplayName="E_cat_MpToHp">,
	E_cat_MpToHpRemain                <DisplayName="E_cat_MpToHpRemain">,
	E_cat_ExtraThunderDmg             <DisplayName="E_cat_ExtraThunderDmg">,
	E_cat_DrugHpSlowRecovery          <DisplayName="E_cat_DrugHpSlowRecovery">,
	E_cat_ExpRate                     <DisplayName="E_cat_ExpRate">,
	E_cat_Total                       <DisplayName="E_cat_Total">,
	E_cat_MAX
};

enum ESPELL_TARGET_POS_TYPE
{
	E_sptpt_Front                     <DisplayName="E_sptpt_Front">,
	E_sptpt_Back                      <DisplayName="E_sptpt_Back">,
	E_sptpt_MAX
};

enum EERolePkMode
{
	E_rpm_Peace                       <DisplayName="E_rpm_Peace">,
	E_rpm_Guild                       <DisplayName="E_rpm_Guild">,
	E_rpm_Team                        <DisplayName="E_rpm_Team">,
	E_rpm_Slayer                      <DisplayName="E_rpm_Slayer">,
	E_rpm_Justice                     <DisplayName="E_rpm_Justice">,
	E_rpm_Master                      <DisplayName="E_rpm_Master">,
	E_rpm_Total                       <DisplayName="E_rpm_Total">,
	E_rpm_MAX
};

enum EGUILD_STORAGE_TAG
{
	E_GUILD_STORAGE_TAG_1             <DisplayName="E_GUILD_STORAGE_TAG_1">,
	E_GUILD_STORAGE_TAG_2             <DisplayName="E_GUILD_STORAGE_TAG_2">,
	E_GUILD_STORAGE_TAG_3             <DisplayName="E_GUILD_STORAGE_TAG_3">,
	E_GUILD_STORAGE_TAG_4             <DisplayName="E_GUILD_STORAGE_TAG_4">,
	E_GUILD_STORAGE_TAG_5             <DisplayName="E_GUILD_STORAGE_TAG_5">,
	E_GUILD_STORAGE_TAG_6             <DisplayName="E_GUILD_STORAGE_TAG_6">,
	E_GUILD_STORAGE_TAG_MAX
};

enum ECHAR_REVIVE_TYPE
{
	E_crt_Invalid                     <DisplayName="E_crt_Invalid">,
	E_crt_AtCurrentPos                <DisplayName="E_crt_AtCurrentPos">,
	E_crt_Instant                     <DisplayName="E_crt_Instant">,
	E_crt_Free                        <DisplayName="E_crt_Free">,
	E_crt_Total                       <DisplayName="E_crt_Total">,
	E_crt_MAX
};

enum ESCENE_DAYTIME_TYPE
{
	E_sdt_Day                         <DisplayName="E_sdt_Day">,
	E_sdt_Night                       <DisplayName="E_sdt_Night">,
	E_sdt_Total                       <DisplayName="E_sdt_Total">,
	E_sdt_MAX
};

enum EQUEST_RELATION_TYPE
{
	E_qrt_Invalid                     <DisplayName="E_qrt_Invalid">,
	E_qrt_Master                      <DisplayName="E_qrt_Master">,
	E_qrt_Disciple                    <DisplayName="E_qrt_Disciple">,
	E_qrt_Husband                     <DisplayName="E_qrt_Husband">,
	E_qrt_Wife                        <DisplayName="E_qrt_Wife">,
	E_qrt_GuildLeader                 <DisplayName="E_qrt_GuildLeader">,
	E_qrt_GuildMember                 <DisplayName="E_qrt_GuildMember">,
	E_qrt_TeamMaster                  <DisplayName="E_qrt_TeamMaster">,
	E_qrt_TeamHusband                 <DisplayName="E_qrt_TeamHusband">,
	E_qrt_TeamGuild                   <DisplayName="E_qrt_TeamGuild">,
	E_qrt_Total                       <DisplayName="E_qrt_Total">,
	E_qrt_MAX
};

enum EITEM_TYPE
{
	E_ITEMTYPE_INVALID                <DisplayName="E_ITEMTYPE_INVALID">,
	E_ITEMTYPE_USABLE                 <DisplayName="E_ITEMTYPE_USABLE">,
	E_ITEMTYPE_EQUIP                  <DisplayName="E_ITEMTYPE_EQUIP">,
	E_ITEMTYPE_MATERIAL               <DisplayName="E_ITEMTYPE_MATERIAL">,
	E_ITEMTYPE_FORMULA                <DisplayName="E_ITEMTYPE_FORMULA">,
	E_ITEMTYPE_SPELLBOOK              <DisplayName="E_ITEMTYPE_SPELLBOOK">,
	E_ITEMTYPE_RUNE                   <DisplayName="E_ITEMTYPE_RUNE">,
	E_ITEMTYPE_DAN                    <DisplayName="E_ITEMTYPE_DAN">,
	E_ITEMTYPE_SOUL                   <DisplayName="E_ITEMTYPE_SOUL">,
	E_ITEMTYPE_MISC                   <DisplayName="E_ITEMTYPE_MISC">,
	E_ITEMTYPE_QUEST                  <DisplayName="E_ITEMTYPE_QUEST">,
	E_ITEMTYPE_DEPOSIT                <DisplayName="E_ITEMTYPE_DEPOSIT">,
	E_ITEMTYPE_RECOVER                <DisplayName="E_ITEMTYPE_RECOVER">,
	E_ITEMTYPE_RMB                    <DisplayName="E_ITEMTYPE_RMB">,
	E_ITEMTYPE_TOTAL                  <DisplayName="E_ITEMTYPE_TOTAL">,
	E_ITEMTYPE_MAX
};

enum ESPELL_STIFF_BITS
{
	E_spsl_BreakCast                  <DisplayName="E_spsl_BreakCast">,
	E_spsl_BreakMovement              <DisplayName="E_spsl_BreakMovement">,
	E_spsl_KnockBack                  <DisplayName="E_spsl_KnockBack">,
	E_spsl_Total                      <DisplayName="E_spsl_Total">,
	E_spsl_MAX
};

enum EEFcmState
{
	E_fcms_Full                       <DisplayName="E_fcms_Full">,
	E_fcms_Half                       <DisplayName="E_fcms_Half">,
	E_fcms_None                       <DisplayName="E_fcms_None">,
	E_fcms_Total                      <DisplayName="E_fcms_Total">,
	E_fcms_MAX
};

enum ESPELL_COOLDOWN_TRIGGER_TYPE
{
	E_spcdt_OnPrepare                 <DisplayName="E_spcdt_OnPrepare">,
	E_spcdt_OnCast                    <DisplayName="E_spcdt_OnCast">,
	E_spcdt_Total                     <DisplayName="E_spcdt_Total">,
	E_spcdt_MAX
};

enum ECHAR_STANCE_FLAG
{
	E_csf_Stand                       <DisplayName="E_csf_Stand">,
	E_csf_Up                          <DisplayName="E_csf_Up">,
	E_csf_Down                        <DisplayName="E_csf_Down">,
	E_csf_Total                       <DisplayName="E_csf_Total">,
	E_csf_MAX
};

enum ESPELL_ATTR_TYPE
{
// conflicted enum definition satBegin(0)
	E_sat_Class                       <DisplayName="E_sat_Class">,
	E_sat_RequireLevel                <DisplayName="E_sat_RequireLevel">,
	E_sat_NextSpell                   <DisplayName="E_sat_NextSpell">,
	E_sat_SwitchSpell                 <DisplayName="E_sat_SwitchSpell">,
	E_sat_ResetTime                   <DisplayName="E_sat_ResetTime">,
	E_sat_CacheTime                   <DisplayName="E_sat_CacheTime">,
	E_sat_WeaponTypeR                 <DisplayName="E_sat_WeaponTypeR">,
	E_sat_WeaponTypeL                 <DisplayName="E_sat_WeaponTypeL">,
	E_sat_CanEquip                    <DisplayName="E_sat_CanEquip">,
	E_sat_CastType                    <DisplayName="E_sat_CastType">,
	E_sat_JudgeType                   <DisplayName="E_sat_JudgeType">,
	E_sat_AttackType                  <DisplayName="E_sat_AttackType">,
	E_sat_ElementaryType              <DisplayName="E_sat_ElementaryType">,
	E_sat_QteType                     <DisplayName="E_sat_QteType">,
	E_sat_LockType                    <DisplayName="E_sat_LockType">,
	E_sat_MinDistance                 <DisplayName="E_sat_MinDistance">,
	E_sat_MaxDistance                 <DisplayName="E_sat_MaxDistance">,
	E_sat_MinGrids                    <DisplayName="E_sat_MinGrids">,
	E_sat_MaxGrids                    <DisplayName="E_sat_MaxGrids">,
	E_sat_TolerantDistance            <DisplayName="E_sat_TolerantDistance">,
	E_sat_TolerantGrids               <DisplayName="E_sat_TolerantGrids">,
	E_sat_RangeType                   <DisplayName="E_sat_RangeType">,
	E_sat_RangeParam1                 <DisplayName="E_sat_RangeParam1">,
	E_sat_RangeParam2                 <DisplayName="E_sat_RangeParam2">,
	E_sat_RangeParam3                 <DisplayName="E_sat_RangeParam3">,
	E_sat_IgnoreObstacle              <DisplayName="E_sat_IgnoreObstacle">,
	E_sat_UseWeaponRange              <DisplayName="E_sat_UseWeaponRange">,
	E_sat_IgnoreTargetRadius          <DisplayName="E_sat_IgnoreTargetRadius">,
	E_sat_AffectedByAtkSpeed          <DisplayName="E_sat_AffectedByAtkSpeed">,
	E_sat_UseDPH                      <DisplayName="E_sat_UseDPH">,
	E_sat_IgnoreDefense               <DisplayName="E_sat_IgnoreDefense">,
	E_sat_IgnoreClientTarget          <DisplayName="E_sat_IgnoreClientTarget">,
	E_sat_MaxTargetCount              <DisplayName="E_sat_MaxTargetCount">,
	E_sat_OffsetDirection             <DisplayName="E_sat_OffsetDirection">,
	E_sat_OffsetDistance              <DisplayName="E_sat_OffsetDistance">,
	E_sat_HOffsetGrids                <DisplayName="E_sat_HOffsetGrids">,
	E_sat_VOffsetGrids                <DisplayName="E_sat_VOffsetGrids">,
	E_sat_TargetObjType               <DisplayName="E_sat_TargetObjType">,
	E_sat_TargetRelationship          <DisplayName="E_sat_TargetRelationship">,
	E_sat_TargetSpecies               <DisplayName="E_sat_TargetSpecies">,
	E_sat_TargetSex                   <DisplayName="E_sat_TargetSex">,
	E_sat_TargetDirection             <DisplayName="E_sat_TargetDirection">,
	E_sat_TargetStance                <DisplayName="E_sat_TargetStance">,
	E_sat_SelfStance                  <DisplayName="E_sat_SelfStance">,
	E_sat_CostType                    <DisplayName="E_sat_CostType">,
	E_sat_InitialCostValue            <DisplayName="E_sat_InitialCostValue">,
	E_sat_InitialCostValuePct         <DisplayName="E_sat_InitialCostValuePct">,
	E_sat_DurativeCostValue           <DisplayName="E_sat_DurativeCostValue">,
	E_sat_DurativeCostValuePct        <DisplayName="E_sat_DurativeCostValuePct">,
	E_sat_ModifierCostValue           <DisplayName="E_sat_ModifierCostValue">,
	E_sat_ModifierCostValuePct        <DisplayName="E_sat_ModifierCostValuePct">,
	E_sat_ItemCostID                  <DisplayName="E_sat_ItemCostID">,
	E_sat_ItemCostCount               <DisplayName="E_sat_ItemCostCount">,
	E_sat_ConsumeItem                 <DisplayName="E_sat_ConsumeItem">,
	E_sat_ItemCostType                <DisplayName="E_sat_ItemCostType">,
	E_sat_ItemCostSubType             <DisplayName="E_sat_ItemCostSubType">,
	E_sat_DuraCost                    <DisplayName="E_sat_DuraCost">,
	E_sat_CoolDownTriggerType         <DisplayName="E_sat_CoolDownTriggerType">,
	E_sat_SelfCoolDown                <DisplayName="E_sat_SelfCoolDown">,
	E_sat_EventCoolDown               <DisplayName="E_sat_EventCoolDown">,
	E_sat_AdjustDirection             <DisplayName="E_sat_AdjustDirection">,
	E_sat_AffectedByWeaponSpeed       <DisplayName="E_sat_AffectedByWeaponSpeed">,
	E_sat_PrepareType                 <DisplayName="E_sat_PrepareType">,
	E_sat_PrepareTimeMin              <DisplayName="E_sat_PrepareTimeMin">,
	E_sat_PrepareTimeMax              <DisplayName="E_sat_PrepareTimeMax">,
	E_sat_AccumulateTimeMax           <DisplayName="E_sat_AccumulateTimeMax">,
	E_sat_AccumulateSeg               <DisplayName="E_sat_AccumulateSeg">,
	E_sat_ChannelCount                <DisplayName="E_sat_ChannelCount">,
	E_sat_MinEffectCount              <DisplayName="E_sat_MinEffectCount">,
	E_sat_Interval                    <DisplayName="E_sat_Interval">,
	E_sat_HardTime                    <DisplayName="E_sat_HardTime">,
	E_sat_WalkDelayTime               <DisplayName="E_sat_WalkDelayTime">,
	E_sat_RunDelayTime                <DisplayName="E_sat_RunDelayTime">,
	E_sat_HitLevel                    <DisplayName="E_sat_HitLevel">,
	E_sat_MoveBreak                   <DisplayName="E_sat_MoveBreak">,
	E_sat_BreakMove                   <DisplayName="E_sat_BreakMove">,
	E_sat_CanMoveInPrepare            <DisplayName="E_sat_CanMoveInPrepare">,
	E_sat_CanMoveInHardTime           <DisplayName="E_sat_CanMoveInHardTime">,
	E_sat_HitBreak                    <DisplayName="E_sat_HitBreak">,
	E_sat_HardTimeAffectedByCharLevel <DisplayName="E_sat_HardTimeAffectedByCharLevel">,
	E_sat_CancelType                  <DisplayName="E_sat_CancelType">,
	E_sat_CancelLevel                 <DisplayName="E_sat_CancelLevel">,
	E_sat_CancelHardTime              <DisplayName="E_sat_CancelHardTime">,
	E_sat_HitTag                      <DisplayName="E_sat_HitTag">,
	E_sat_Speed                       <DisplayName="E_sat_Speed">,
	E_sat_IsMixEffect                 <DisplayName="E_sat_IsMixEffect">,
	E_sat_MixEffectPhyRate            <DisplayName="E_sat_MixEffectPhyRate">,
	E_sat_MixEffectMagRate            <DisplayName="E_sat_MixEffectMagRate">,
	E_sat_EffectPct                   <DisplayName="E_sat_EffectPct">,
	E_sat_EffectValue                 <DisplayName="E_sat_EffectValue">,
	E_sat_EffectValueMax              <DisplayName="E_sat_EffectValueMax">,
	E_sat_DefaultValue                <DisplayName="E_sat_DefaultValue">,
	E_sat_DefaultValueMax             <DisplayName="E_sat_DefaultValueMax">,
	E_sat_AffectedByNpcParam          <DisplayName="E_sat_AffectedByNpcParam">,
	E_sat_ThreatPct                   <DisplayName="E_sat_ThreatPct">,
	E_sat_ThreatValue                 <DisplayName="E_sat_ThreatValue">,
	E_sat_ThreatMax                   <DisplayName="E_sat_ThreatMax">,
	E_sat_CanCastOnMount              <DisplayName="E_sat_CanCastOnMount">,
	E_sat_CasterDeadLimit             <DisplayName="E_sat_CasterDeadLimit">,
	E_sat_TargetDeadLimit             <DisplayName="E_sat_TargetDeadLimit">,
	E_sat_CasterLimitBits             <DisplayName="E_sat_CasterLimitBits">,
	E_sat_CasterCCLimitBits           <DisplayName="E_sat_CasterCCLimitBits">,
	E_sat_SceneTypeLimit              <DisplayName="E_sat_SceneTypeLimit">,
	E_sat_CasterCheckNode             <DisplayName="E_sat_CasterCheckNode">,
	E_sat_TargetCheckNode             <DisplayName="E_sat_TargetCheckNode">,
	E_sat_OnLearn                     <DisplayName="E_sat_OnLearn">,
	E_sat_OnForget                    <DisplayName="E_sat_OnForget">,
	E_sat_OnEquip                     <DisplayName="E_sat_OnEquip">,
	E_sat_OnUnEquip                   <DisplayName="E_sat_OnUnEquip">,
	E_sat_OnPrepare                   <DisplayName="E_sat_OnPrepare">,
	E_sat_OnCast                      <DisplayName="E_sat_OnCast">,
	E_sat_OnBreak                     <DisplayName="E_sat_OnBreak">,
	E_sat_OnCreateBullet              <DisplayName="E_sat_OnCreateBullet">,
	E_sat_BeforeRoll                  <DisplayName="E_sat_BeforeRoll">,
	E_sat_AfterRoll                   <DisplayName="E_sat_AfterRoll">,
	E_sat_BeforeEffect                <DisplayName="E_sat_BeforeEffect">,
	E_sat_AfterEffect                 <DisplayName="E_sat_AfterEffect">,
	E_sat_NeedCache                   <DisplayName="E_sat_NeedCache">,
	E_sat_CancelStiff                 <DisplayName="E_sat_CancelStiff">,
	E_sat_KnockBackRate               <DisplayName="E_sat_KnockBackRate">,
	E_sat_BaseStiffTime               <DisplayName="E_sat_BaseStiffTime">,
	E_sat_MonsterChaseDelayTime       <DisplayName="E_sat_MonsterChaseDelayTime">,
	E_sat_ChainEffect                 <DisplayName="E_sat_ChainEffect">,
	E_sat_BindSlot                    <DisplayName="E_sat_BindSlot">,
	E_sat_TrigEvent                   <DisplayName="E_sat_TrigEvent">,
	E_sat_Total                       <DisplayName="E_sat_Total">,
	E_sat_MAX
};

enum ESCENE_WEATHER_TYPE
{
	E_swt_Invalid                     <DisplayName="E_swt_Invalid">,
	E_swt_Normal                      <DisplayName="E_swt_Normal">,
	E_swt_Fog                         <DisplayName="E_swt_Fog">,
	E_swt_Thunder                     <DisplayName="E_swt_Thunder">,
	E_swt_Rain                        <DisplayName="E_swt_Rain">,
	E_swt_Snow                        <DisplayName="E_swt_Snow">,
	E_swt_Storm                       <DisplayName="E_swt_Storm">,
	E_swt_Eclipse                     <DisplayName="E_swt_Eclipse">,
	E_swt_Total                       <DisplayName="E_swt_Total">,
	E_swt_MAX
};

enum EMISSILE_LOCK_TYPE
{
	E_mlt_Self                        <DisplayName="E_mlt_Self">,
	E_mlt_Object                      <DisplayName="E_mlt_Object">,
	E_mlt_Pos                         <DisplayName="E_mlt_Pos">,
	E_mlt_Total                       <DisplayName="E_mlt_Total">,
	E_mlt_MAX
};

enum ESPELL_JUDGE_TYPE
{
	E_spjt_Directly                   <DisplayName="E_spjt_Directly">,
	E_spjt_Phy                        <DisplayName="E_spjt_Phy">,
	E_spjt_Mag                        <DisplayName="E_spjt_Mag">,
	E_spjt_Total                      <DisplayName="E_spjt_Total">,
	E_spjt_MAX
};

enum ESPELL_RANGE_TYPE
{
// conflicted enum definition sprtGridEnd(14)
	E_sprt_Single                     <DisplayName="E_sprt_Single">,
	E_sprt_Circle                     <DisplayName="E_sprt_Circle">,
	E_sprt_Sector                     <DisplayName="E_sprt_Sector">,
	E_sprt_Arc                        <DisplayName="E_sprt_Arc">,
	E_sprt_Rectangle                  <DisplayName="E_sprt_Rectangle">,
	E_sprt_RearHalfCircle             <DisplayName="E_sprt_RearHalfCircle">,
	E_sprt_GridLine                   <DisplayName="E_sprt_GridLine">,
	E_sprt_GridRect                   <DisplayName="E_sprt_GridRect">,
	E_sprt_GridCross                  <DisplayName="E_sprt_GridCross">,
	E_sprt_GridX                      <DisplayName="E_sprt_GridX">,
	E_sprt_GridU                      <DisplayName="E_sprt_GridU">,
	E_sprt_GridSingle                 <DisplayName="E_sprt_GridSingle">,
	E_sprt_GridLineTransverse         <DisplayName="E_sprt_GridLineTransverse">,
	E_sprt_GridMultiLine              <DisplayName="E_sprt_GridMultiLine">,
	E_sprt_Total                      <DisplayName="E_sprt_Total">,
	E_sprt_MAX
};

enum ESPELL_SLOTS_DEF
{
// conflicted enum definition qssQteEnd(2)
// conflicted enum definition qssPassiveEnd(5)
// conflicted enum definition qssActiveEnd(17)
// conflicted enum definition qssActiveBegin(5)
// conflicted enum definition qssPassiveBegin(2)
// conflicted enum definition qssSlotBegin(0)
	E_qss_NormalAtk                   <DisplayName="E_qss_NormalAtk">,
	E_qss_BossQTE                     <DisplayName="E_qss_BossQTE">,
	E_qss_Passive1                    <DisplayName="E_qss_Passive1">,
	E_qss_Passive2                    <DisplayName="E_qss_Passive2">,
	E_qss_Passive3                    <DisplayName="E_qss_Passive3">,
	E_qss_Key0                        <DisplayName="E_qss_Key0">,
	E_qss_Key1                        <DisplayName="E_qss_Key1">,
	E_qss_Key2                        <DisplayName="E_qss_Key2">,
	E_qss_Key3                        <DisplayName="E_qss_Key3">,
	E_qss_Key4                        <DisplayName="E_qss_Key4">,
	E_qss_Key5                        <DisplayName="E_qss_Key5">,
	E_qss_Key6                        <DisplayName="E_qss_Key6">,
	E_qss_Key7                        <DisplayName="E_qss_Key7">,
	E_qss_Key8                        <DisplayName="E_qss_Key8">,
	E_qss_Key9                        <DisplayName="E_qss_Key9">,
	E_qss_Key10                       <DisplayName="E_qss_Key10">,
	E_qss_Key11                       <DisplayName="E_qss_Key11">,
	E_qss_SlotCount                   <DisplayName="E_qss_SlotCount">,
	E_qss_MAX
};

enum ESPELL_HIT_TAG
{
	E_sphtag_Common                   <DisplayName="E_sphtag_Common">,
	E_sphtag_Defence                  <DisplayName="E_sphtag_Defence">,
	E_sphtag_SuperArmor               <DisplayName="E_sphtag_SuperArmor">,
	E_sphtag_Invincible               <DisplayName="E_sphtag_Invincible">,
	E_sphtag_Total                    <DisplayName="E_sphtag_Total">,
	E_sphtag_MAX
};

enum EACHIEVEMENT_RESET_TYPE
{
	E_acrt_Invalid                    <DisplayName="E_acrt_Invalid">,
	E_acrt_Week                       <DisplayName="E_acrt_Week">,
	E_acrt_Month                      <DisplayName="E_acrt_Month">,
	E_acrt_Total                      <DisplayName="E_acrt_Total">,
	E_acrt_MAX
};

enum EMISSILE_ATTR_TYPE
{
	E_mat_Default                     <DisplayName="E_mat_Default">,
	E_mat_LockType                    <DisplayName="E_mat_LockType">,
	E_mat_MoveType                    <DisplayName="E_mat_MoveType">,
	E_mat_MoveSpeed                   <DisplayName="E_mat_MoveSpeed">,
	E_mat_Height                      <DisplayName="E_mat_Height">,
	E_mat_LifeTime                    <DisplayName="E_mat_LifeTime">,
	E_mat_MaxCollisionCount           <DisplayName="E_mat_MaxCollisionCount">,
	E_mat_MaxTargetCountPerCollision  <DisplayName="E_mat_MaxTargetCountPerCollision">,
	E_mat_RangeType                   <DisplayName="E_mat_RangeType">,
	E_mat_RangeParam1                 <DisplayName="E_mat_RangeParam1">,
	E_mat_RangeParam2                 <DisplayName="E_mat_RangeParam2">,
	E_mat_RangeParam3                 <DisplayName="E_mat_RangeParam3">,
	E_mat_BlockShape                  <DisplayName="E_mat_BlockShape">,
	E_mat_BlockParam                  <DisplayName="E_mat_BlockParam">,
	E_mat_OffsetDirection             <DisplayName="E_mat_OffsetDirection">,
	E_mat_OffsetDistance              <DisplayName="E_mat_OffsetDistance">,
	E_mat_HOffsetGrids                <DisplayName="E_mat_HOffsetGrids">,
	E_mat_VOffsetGrids                <DisplayName="E_mat_VOffsetGrids">,
	E_mat_Delay                       <DisplayName="E_mat_Delay">,
	E_mat_Interval                    <DisplayName="E_mat_Interval">,
	E_mat_NotMultiEffect              <DisplayName="E_mat_NotMultiEffect">,
	E_mat_NotHitLastTarget            <DisplayName="E_mat_NotHitLastTarget">,
	E_mat_BlockType                   <DisplayName="E_mat_BlockType">,
	E_mat_OnInit                      <DisplayName="E_mat_OnInit">,
	E_mat_OnCreate                    <DisplayName="E_mat_OnCreate">,
	E_mat_OnScan                      <DisplayName="E_mat_OnScan">,
	E_mat_OnTimeup                    <DisplayName="E_mat_OnTimeup">,
	E_mat_OnRemove                    <DisplayName="E_mat_OnRemove">,
	E_mat_OnCharEnter                 <DisplayName="E_mat_OnCharEnter">,
	E_mat_OnCharLeave                 <DisplayName="E_mat_OnCharLeave">,
	E_mat_Total                       <DisplayName="E_mat_Total">,
	E_mat_MAX
};

enum EAI_PATROL_STYLE
{
	E_aps_Invalid                     <DisplayName="E_aps_Invalid">,
	E_aps_Line                        <DisplayName="E_aps_Line">,
	E_aps_LineBack                    <DisplayName="E_aps_LineBack">,
	E_aps_Circle                      <DisplayName="E_aps_Circle">,
	E_aps_Total                       <DisplayName="E_aps_Total">,
	E_aps_MAX
};

enum EQUEST_ACCEPT_TYPE
{
	E_qat_Invalid                     <DisplayName="E_qat_Invalid">,
	E_qat_Npc                         <DisplayName="E_qat_Npc">,
	E_qat_Share                       <DisplayName="E_qat_Share">,
	E_qat_Publish                     <DisplayName="E_qat_Publish">,
	E_qat_Emegency                    <DisplayName="E_qat_Emegency">,
	E_qat_Item                        <DisplayName="E_qat_Item">,
	E_qat_Total                       <DisplayName="E_qat_Total">,
	E_qat_MAX
};

enum EIDENTIFY_TYPE
{
	E_IDENTIFY_TYPE_NO                <DisplayName="E_IDENTIFY_TYPE_NO">,
	E_IDENTIFY_TYPE_YES               <DisplayName="E_IDENTIFY_TYPE_YES">,
	E_IDENTIFY_TYPE_LEVEL_UP          <DisplayName="E_IDENTIFY_TYPE_LEVEL_UP">,
	E_IDENTIFY_TYPE_MAX
};

enum ECHAR_MODIFIER_INDEX
{
// max enum definition is over 254
	E_cmi_Invalid                     <DisplayName="E_cmi_Invalid">,
	E_cmi_A                           <DisplayName="E_cmi_A">,
	E_cmi_B                           <DisplayName="E_cmi_B">,
	E_cmi_003                         <DisplayName="E_cmi_003">,
	E_cmi_004                         <DisplayName="E_cmi_004">,
	E_cmi_005                         <DisplayName="E_cmi_005">,
	E_cmi_006                         <DisplayName="E_cmi_006">,
	E_cmi_007                         <DisplayName="E_cmi_007">,
	E_cmi_008                         <DisplayName="E_cmi_008">,
	E_cmi_009                         <DisplayName="E_cmi_009">,
	E_cmi_010                         <DisplayName="E_cmi_010">,
	E_cmi_011                         <DisplayName="E_cmi_011">,
	E_cmi_012                         <DisplayName="E_cmi_012">,
	E_cmi_013                         <DisplayName="E_cmi_013">,
	E_cmi_014                         <DisplayName="E_cmi_014">,
	E_cmi_015                         <DisplayName="E_cmi_015">,
	E_cmi_016                         <DisplayName="E_cmi_016">,
	E_cmi_017                         <DisplayName="E_cmi_017">,
	E_cmi_018                         <DisplayName="E_cmi_018">,
	E_cmi_019                         <DisplayName="E_cmi_019">,
	E_cmi_020                         <DisplayName="E_cmi_020">,
	E_cmi_021                         <DisplayName="E_cmi_021">,
	E_cmi_022                         <DisplayName="E_cmi_022">,
	E_cmi_023                         <DisplayName="E_cmi_023">,
	E_cmi_024                         <DisplayName="E_cmi_024">,
	E_cmi_025                         <DisplayName="E_cmi_025">,
	E_cmi_026                         <DisplayName="E_cmi_026">,
	E_cmi_027                         <DisplayName="E_cmi_027">,
	E_cmi_028                         <DisplayName="E_cmi_028">,
	E_cmi_029                         <DisplayName="E_cmi_029">,
	E_cmi_030                         <DisplayName="E_cmi_030">,
	E_cmi_031                         <DisplayName="E_cmi_031">,
	E_cmi_032                         <DisplayName="E_cmi_032">,
	E_cmi_033                         <DisplayName="E_cmi_033">,
	E_cmi_034                         <DisplayName="E_cmi_034">,
	E_cmi_035                         <DisplayName="E_cmi_035">,
	E_cmi_036                         <DisplayName="E_cmi_036">,
	E_cmi_037                         <DisplayName="E_cmi_037">,
	E_cmi_038                         <DisplayName="E_cmi_038">,
	E_cmi_039                         <DisplayName="E_cmi_039">,
	E_cmi_040                         <DisplayName="E_cmi_040">,
	E_cmi_041                         <DisplayName="E_cmi_041">,
	E_cmi_042                         <DisplayName="E_cmi_042">,
	E_cmi_043                         <DisplayName="E_cmi_043">,
	E_cmi_044                         <DisplayName="E_cmi_044">,
	E_cmi_045                         <DisplayName="E_cmi_045">,
	E_cmi_046                         <DisplayName="E_cmi_046">,
	E_cmi_047                         <DisplayName="E_cmi_047">,
	E_cmi_048                         <DisplayName="E_cmi_048">,
	E_cmi_049                         <DisplayName="E_cmi_049">,
	E_cmi_050                         <DisplayName="E_cmi_050">,
	E_cmi_051                         <DisplayName="E_cmi_051">,
	E_cmi_052                         <DisplayName="E_cmi_052">,
	E_cmi_053                         <DisplayName="E_cmi_053">,
	E_cmi_054                         <DisplayName="E_cmi_054">,
	E_cmi_055                         <DisplayName="E_cmi_055">,
	E_cmi_056                         <DisplayName="E_cmi_056">,
	E_cmi_057                         <DisplayName="E_cmi_057">,
	E_cmi_058                         <DisplayName="E_cmi_058">,
	E_cmi_059                         <DisplayName="E_cmi_059">,
	E_cmi_060                         <DisplayName="E_cmi_060">,
	E_cmi_061                         <DisplayName="E_cmi_061">,
	E_cmi_062                         <DisplayName="E_cmi_062">,
	E_cmi_063                         <DisplayName="E_cmi_063">,
	E_cmi_064                         <DisplayName="E_cmi_064">,
	E_cmi_065                         <DisplayName="E_cmi_065">,
	E_cmi_066                         <DisplayName="E_cmi_066">,
	E_cmi_067                         <DisplayName="E_cmi_067">,
	E_cmi_068                         <DisplayName="E_cmi_068">,
	E_cmi_069                         <DisplayName="E_cmi_069">,
	E_cmi_070                         <DisplayName="E_cmi_070">,
	E_cmi_071                         <DisplayName="E_cmi_071">,
	E_cmi_072                         <DisplayName="E_cmi_072">,
	E_cmi_073                         <DisplayName="E_cmi_073">,
	E_cmi_074                         <DisplayName="E_cmi_074">,
	E_cmi_075                         <DisplayName="E_cmi_075">,
	E_cmi_076                         <DisplayName="E_cmi_076">,
	E_cmi_077                         <DisplayName="E_cmi_077">,
	E_cmi_078                         <DisplayName="E_cmi_078">,
	E_cmi_079                         <DisplayName="E_cmi_079">,
	E_cmi_080                         <DisplayName="E_cmi_080">,
	E_cmi_081                         <DisplayName="E_cmi_081">,
	E_cmi_082                         <DisplayName="E_cmi_082">,
	E_cmi_083                         <DisplayName="E_cmi_083">,
	E_cmi_084                         <DisplayName="E_cmi_084">,
	E_cmi_085                         <DisplayName="E_cmi_085">,
	E_cmi_086                         <DisplayName="E_cmi_086">,
	E_cmi_087                         <DisplayName="E_cmi_087">,
	E_cmi_088                         <DisplayName="E_cmi_088">,
	E_cmi_089                         <DisplayName="E_cmi_089">,
	E_cmi_090                         <DisplayName="E_cmi_090">,
	E_cmi_091                         <DisplayName="E_cmi_091">,
	E_cmi_092                         <DisplayName="E_cmi_092">,
	E_cmi_093                         <DisplayName="E_cmi_093">,
	E_cmi_094                         <DisplayName="E_cmi_094">,
	E_cmi_095                         <DisplayName="E_cmi_095">,
	E_cmi_096                         <DisplayName="E_cmi_096">,
	E_cmi_097                         <DisplayName="E_cmi_097">,
	E_cmi_098                         <DisplayName="E_cmi_098">,
	E_cmi_099                         <DisplayName="E_cmi_099">,
	E_cmi_100                         <DisplayName="E_cmi_100">,
	E_cmi_101                         <DisplayName="E_cmi_101">,
	E_cmi_102                         <DisplayName="E_cmi_102">,
	E_cmi_103                         <DisplayName="E_cmi_103">,
	E_cmi_104                         <DisplayName="E_cmi_104">,
	E_cmi_105                         <DisplayName="E_cmi_105">,
	E_cmi_106                         <DisplayName="E_cmi_106">,
	E_cmi_107                         <DisplayName="E_cmi_107">,
	E_cmi_108                         <DisplayName="E_cmi_108">,
	E_cmi_109                         <DisplayName="E_cmi_109">,
	E_cmi_110                         <DisplayName="E_cmi_110">,
	E_cmi_111                         <DisplayName="E_cmi_111">,
	E_cmi_112                         <DisplayName="E_cmi_112">,
	E_cmi_113                         <DisplayName="E_cmi_113">,
	E_cmi_114                         <DisplayName="E_cmi_114">,
	E_cmi_115                         <DisplayName="E_cmi_115">,
	E_cmi_116                         <DisplayName="E_cmi_116">,
	E_cmi_117                         <DisplayName="E_cmi_117">,
	E_cmi_118                         <DisplayName="E_cmi_118">,
	E_cmi_119                         <DisplayName="E_cmi_119">,
	E_cmi_120                         <DisplayName="E_cmi_120">,
	E_cmi_121                         <DisplayName="E_cmi_121">,
	E_cmi_122                         <DisplayName="E_cmi_122">,
	E_cmi_123                         <DisplayName="E_cmi_123">,
	E_cmi_124                         <DisplayName="E_cmi_124">,
	E_cmi_125                         <DisplayName="E_cmi_125">,
	E_cmi_126                         <DisplayName="E_cmi_126">,
	E_cmi_127                         <DisplayName="E_cmi_127">,
	E_cmi_128                         <DisplayName="E_cmi_128">,
	E_cmi_129                         <DisplayName="E_cmi_129">,
	E_cmi_130                         <DisplayName="E_cmi_130">,
	E_cmi_131                         <DisplayName="E_cmi_131">,
	E_cmi_132                         <DisplayName="E_cmi_132">,
	E_cmi_133                         <DisplayName="E_cmi_133">,
	E_cmi_134                         <DisplayName="E_cmi_134">,
	E_cmi_135                         <DisplayName="E_cmi_135">,
	E_cmi_136                         <DisplayName="E_cmi_136">,
	E_cmi_AllNpcSpecies               <DisplayName="E_cmi_AllNpcSpecies">,
	E_cmi_138                         <DisplayName="E_cmi_138">,
	E_cmi_139                         <DisplayName="E_cmi_139">,
	E_cmi_140                         <DisplayName="E_cmi_140">,
	E_cmi_141                         <DisplayName="E_cmi_141">,
	E_cmi_142                         <DisplayName="E_cmi_142">,
	E_cmi_143                         <DisplayName="E_cmi_143">,
	E_cmi_144                         <DisplayName="E_cmi_144">,
	E_cmi_145                         <DisplayName="E_cmi_145">,
	E_cmi_146                         <DisplayName="E_cmi_146">,
	E_cmi_147                         <DisplayName="E_cmi_147">,
	E_cmi_148                         <DisplayName="E_cmi_148">,
	E_cmi_149                         <DisplayName="E_cmi_149">,
	E_cmi_150                         <DisplayName="E_cmi_150">,
	E_cmi_151                         <DisplayName="E_cmi_151">,
	E_cmi_152                         <DisplayName="E_cmi_152">,
	E_cmi_153                         <DisplayName="E_cmi_153">,
	E_cmi_154                         <DisplayName="E_cmi_154">,
	E_cmi_155                         <DisplayName="E_cmi_155">,
	E_cmi_156                         <DisplayName="E_cmi_156">,
	E_cmi_157                         <DisplayName="E_cmi_157">,
	E_cmi_158                         <DisplayName="E_cmi_158">,
	E_cmi_159                         <DisplayName="E_cmi_159">,
	E_cmi_160                         <DisplayName="E_cmi_160">,
	E_cmi_161                         <DisplayName="E_cmi_161">,
	E_cmi_162                         <DisplayName="E_cmi_162">,
	E_cmi_163                         <DisplayName="E_cmi_163">,
	E_cmi_164                         <DisplayName="E_cmi_164">,
	E_cmi_165                         <DisplayName="E_cmi_165">,
	E_cmi_166                         <DisplayName="E_cmi_166">,
	E_cmi_167                         <DisplayName="E_cmi_167">,
	E_cmi_168                         <DisplayName="E_cmi_168">,
	E_cmi_169                         <DisplayName="E_cmi_169">,
	E_cmi_170                         <DisplayName="E_cmi_170">,
	E_cmi_171                         <DisplayName="E_cmi_171">,
	E_cmi_172                         <DisplayName="E_cmi_172">,
	E_cmi_173                         <DisplayName="E_cmi_173">,
	E_cmi_174                         <DisplayName="E_cmi_174">,
	E_cmi_175                         <DisplayName="E_cmi_175">,
	E_cmi_176                         <DisplayName="E_cmi_176">,
	E_cmi_177                         <DisplayName="E_cmi_177">,
	E_cmi_178                         <DisplayName="E_cmi_178">,
	E_cmi_179                         <DisplayName="E_cmi_179">,
	E_cmi_180                         <DisplayName="E_cmi_180">,
	E_cmi_181                         <DisplayName="E_cmi_181">,
	E_cmi_182                         <DisplayName="E_cmi_182">,
	E_cmi_183                         <DisplayName="E_cmi_183">,
	E_cmi_184                         <DisplayName="E_cmi_184">,
	E_cmi_185                         <DisplayName="E_cmi_185">,
	E_cmi_186                         <DisplayName="E_cmi_186">,
	E_cmi_187                         <DisplayName="E_cmi_187">,
	E_cmi_188                         <DisplayName="E_cmi_188">,
	E_cmi_189                         <DisplayName="E_cmi_189">,
	E_cmi_190                         <DisplayName="E_cmi_190">,
	E_cmi_191                         <DisplayName="E_cmi_191">,
	E_cmi_192                         <DisplayName="E_cmi_192">,
	E_cmi_193                         <DisplayName="E_cmi_193">,
	E_cmi_194                         <DisplayName="E_cmi_194">,
	E_cmi_195                         <DisplayName="E_cmi_195">,
	E_cmi_196                         <DisplayName="E_cmi_196">,
	E_cmi_197                         <DisplayName="E_cmi_197">,
	E_cmi_198                         <DisplayName="E_cmi_198">,
	E_cmi_199                         <DisplayName="E_cmi_199">,
	E_cmi_200                         <DisplayName="E_cmi_200">,
	E_cmi_201                         <DisplayName="E_cmi_201">,
	E_cmi_202                         <DisplayName="E_cmi_202">,
	E_cmi_203                         <DisplayName="E_cmi_203">,
	E_cmi_204                         <DisplayName="E_cmi_204">,
	E_cmi_205                         <DisplayName="E_cmi_205">,
	E_cmi_206                         <DisplayName="E_cmi_206">,
	E_cmi_207                         <DisplayName="E_cmi_207">,
	E_cmi_208                         <DisplayName="E_cmi_208">,
	E_cmi_209                         <DisplayName="E_cmi_209">,
	E_cmi_210                         <DisplayName="E_cmi_210">,
	E_cmi_211                         <DisplayName="E_cmi_211">,
	E_cmi_212                         <DisplayName="E_cmi_212">,
	E_cmi_213                         <DisplayName="E_cmi_213">,
	E_cmi_214                         <DisplayName="E_cmi_214">,
	E_cmi_215                         <DisplayName="E_cmi_215">,
	E_cmi_216                         <DisplayName="E_cmi_216">,
	E_cmi_217                         <DisplayName="E_cmi_217">,
	E_cmi_218                         <DisplayName="E_cmi_218">,
	E_cmi_219                         <DisplayName="E_cmi_219">,
	E_cmi_220                         <DisplayName="E_cmi_220">,
	E_cmi_221                         <DisplayName="E_cmi_221">,
	E_cmi_222                         <DisplayName="E_cmi_222">,
	E_cmi_223                         <DisplayName="E_cmi_223">,
	E_cmi_224                         <DisplayName="E_cmi_224">,
	E_cmi_225                         <DisplayName="E_cmi_225">,
	E_cmi_226                         <DisplayName="E_cmi_226">,
	E_cmi_227                         <DisplayName="E_cmi_227">,
	E_cmi_228                         <DisplayName="E_cmi_228">,
	E_cmi_229                         <DisplayName="E_cmi_229">,
	E_cmi_230                         <DisplayName="E_cmi_230">,
	E_cmi_231                         <DisplayName="E_cmi_231">,
	E_cmi_232                         <DisplayName="E_cmi_232">,
	E_cmi_233                         <DisplayName="E_cmi_233">,
	E_cmi_234                         <DisplayName="E_cmi_234">,
	E_cmi_235                         <DisplayName="E_cmi_235">,
	E_cmi_236                         <DisplayName="E_cmi_236">,
	E_cmi_237                         <DisplayName="E_cmi_237">,
	E_cmi_238                         <DisplayName="E_cmi_238">,
	E_cmi_239                         <DisplayName="E_cmi_239">,
	E_cmi_240                         <DisplayName="E_cmi_240">,
	E_cmi_241                         <DisplayName="E_cmi_241">,
	E_cmi_242                         <DisplayName="E_cmi_242">,
	E_cmi_243                         <DisplayName="E_cmi_243">,
	E_cmi_244                         <DisplayName="E_cmi_244">,
	E_cmi_245                         <DisplayName="E_cmi_245">,
	E_cmi_246                         <DisplayName="E_cmi_246">,
	E_cmi_247                         <DisplayName="E_cmi_247">,
	E_cmi_248                         <DisplayName="E_cmi_248">,
	E_cmi_249                         <DisplayName="E_cmi_249">,
	E_cmi_250                         <DisplayName="E_cmi_250">,
	E_cmi_251                         <DisplayName="E_cmi_251">,
	E_cmi_252                         <DisplayName="E_cmi_252">,
	E_cmi_253                         <DisplayName="E_cmi_253">,
	E_cmi_254                         <DisplayName="E_cmi_254">,
	E_cmi_MAX
};

enum EMODIFIER_RET_CODE
{
	E_mrc_False                       <DisplayName="E_mrc_False">,
	E_mrc_True                        <DisplayName="E_mrc_True">,
	E_mrc_TrueAndBreak                <DisplayName="E_mrc_TrueAndBreak">,
	E_mrc_MAX
};

enum EMODIFIER_TARGET_TYPE
{
	E_mtt_Character                   <DisplayName="E_mtt_Character">,
	E_mtt_Bullet                      <DisplayName="E_mtt_Bullet">,
	E_mtt_Missile                     <DisplayName="E_mtt_Missile">,
	E_mtt_Total                       <DisplayName="E_mtt_Total">,
	E_mtt_MAX
};

enum ELINE_SCENE_STATE
{
	E_lss_Invalid                     <DisplayName="E_lss_Invalid">,
	E_lss_Normal                      <DisplayName="E_lss_Normal">,
	E_lss_Busy                        <DisplayName="E_lss_Busy">,
	E_lss_Full                        <DisplayName="E_lss_Full">,
	E_lss_Total                       <DisplayName="E_lss_Total">,
	E_lss_MAX
};

enum EMODIFIER_X_TYPE
{
	E_mxt_UseNone                     <DisplayName="E_mxt_UseNone">,
	E_mxt_UseX1                       <DisplayName="E_mxt_UseX1">,
	E_mxt_UseX2                       <DisplayName="E_mxt_UseX2">,
	E_mxt_UseX3                       <DisplayName="E_mxt_UseX3">,
	E_mxt_UseX4                       <DisplayName="E_mxt_UseX4">,
	E_mxt_Total                       <DisplayName="E_mxt_Total">,
	E_mxt_MAX
};

enum EGUILD_ACHIEVEMENT_AWARD_TYPE
{
	E_GA_AWARD_NONE                   <DisplayName="E_GA_AWARD_NONE">,
	E_GA_AWARD_POINT                  <DisplayName="E_GA_AWARD_POINT">,
	E_GA_AWARD_EXP                    <DisplayName="E_GA_AWARD_EXP">,
	E_GA_AWARD_ITEM                   <DisplayName="E_GA_AWARD_ITEM">,
	E_GA_AWARD_TITLE                  <DisplayName="E_GA_AWARD_TITLE">,
	E_GA_AWARD_NUM                    <DisplayName="E_GA_AWARD_NUM">,
	E_GA_AWARD_MAX
};

enum ESPELL_PRACTICE_TYPE
{
	E_pt_None                         <DisplayName="E_pt_None">,
	E_pt_Normal                       <DisplayName="E_pt_Normal">,
	E_pt_Fast                         <DisplayName="E_pt_Fast">,
	E_pt_Total                        <DisplayName="E_pt_Total">,
	E_pt_MAX
};

enum ETEAM_AUCTION_ACTION
{
	E_TAA_ADDITEM                     <DisplayName="E_TAA_ADDITEM">,
	E_TAA_START_AUCTION               <DisplayName="E_TAA_START_AUCTION">,
	E_TAA_BID                         <DisplayName="E_TAA_BID">,
	E_TAA_ABANDON_SELF_REQUEST        <DisplayName="E_TAA_ABANDON_SELF_REQUEST">,
	E_TAA_ABANDON_LEAVE_SCENE         <DisplayName="E_TAA_ABANDON_LEAVE_SCENE">,
	E_TAA_REWARD_MONEY                <DisplayName="E_TAA_REWARD_MONEY">,
	E_TAA_WIN_ITEM                    <DisplayName="E_TAA_WIN_ITEM">,
	E_TAA_LEADER_GOT_ITEM             <DisplayName="E_TAA_LEADER_GOT_ITEM">,
	E_TAA_RETURN_DEPOSIT              <DisplayName="E_TAA_RETURN_DEPOSIT">,
	E_TAA_FINISH_AUCTION              <DisplayName="E_TAA_FINISH_AUCTION">,
	E_TAA_TOTAL                       <DisplayName="E_TAA_TOTAL">,
	E_TAA_MAX
};

enum EGUILD_CAST_SPELL_TYPE
{
	E_GUILD_CS_Reserved_0             <DisplayName="E_GUILD_CS_Reserved_0">,
	E_GUILD_CS_UPGRADING              <DisplayName="E_GUILD_CS_UPGRADING">,
	E_GUILD_CS_UPDATE                 <DisplayName="E_GUILD_CS_UPDATE">,
	E_GUILD_CS_DEL                    <DisplayName="E_GUILD_CS_DEL">,
	E_GUILD_CS_MAX
};

enum EGUILD_BUILDING_STATE
{
	E_GUILD_BUILDING_ENABLE           <DisplayName="E_GUILD_BUILDING_ENABLE">,
	E_GUILD_BUILDING_DISABLE          <DisplayName="E_GUILD_BUILDING_DISABLE">,
	E_GUILD_BUILDING_MAX
};

enum ESPELL_TARGET_SEX_TYPE
{
	E_sptst_Male                      <DisplayName="E_sptst_Male">,
	E_sptst_Female                    <DisplayName="E_sptst_Female">,
	E_sptst_None                      <DisplayName="E_sptst_None">,
	E_sptst_MAX
};

enum ENORMAL_EQUIP_SLOT
{
	E_NES_MAIN_HAND                   <DisplayName="E_NES_MAIN_HAND">,
	E_NES_BODY                        <DisplayName="E_NES_BODY">,
	E_NES_BACK                        <DisplayName="E_NES_BACK">,
	E_NES_HEAD                        <DisplayName="E_NES_HEAD">,
	E_NES_SHOULDER                    <DisplayName="E_NES_SHOULDER">,
	E_NES_ARM                         <DisplayName="E_NES_ARM">,
	E_NES_WAIST                       <DisplayName="E_NES_WAIST">,
	E_NES_FOOT                        <DisplayName="E_NES_FOOT">,
	E_NES_NECK                        <DisplayName="E_NES_NECK">,
	E_NES_FINGER_L                    <DisplayName="E_NES_FINGER_L">,
	E_NES_FINGER_R                    <DisplayName="E_NES_FINGER_R">,
	E_NES_WRIST_L                     <DisplayName="E_NES_WRIST_L">,
	E_NES_WRIST_R                     <DisplayName="E_NES_WRIST_R">,
	E_NES_TOTAL                       <DisplayName="E_NES_TOTAL">,
	E_NES_MAX
};

enum ETEAM_AUCTION_MONEY_REASON
{
	E_TAMR_NO_CHANGE                  <DisplayName="E_TAMR_NO_CHANGE">,
	E_TAMR_OFFER_PRICE                <DisplayName="E_TAMR_OFFER_PRICE">,
	E_TAMR_ABANDON                    <DisplayName="E_TAMR_ABANDON">,
	E_TAMR_REWARD                     <DisplayName="E_TAMR_REWARD">,
	E_TAMR_RETURN_DEPOSITE            <DisplayName="E_TAMR_RETURN_DEPOSITE">,
	E_TAMR_MAX
};

enum EUPGRADE_BUILDING_TYPE
{
	E_G_B_UPGRADE_BY_Reserved_0       <DisplayName="E_G_B_UPGRADE_BY_Reserved_0">,
	E_G_B_UPGRADE_BY_NPC              <DisplayName="E_G_B_UPGRADE_BY_NPC">,
	E_G_B_UPGRADE_BY_PANEL            <DisplayName="E_G_B_UPGRADE_BY_PANEL">,
	E_G_B_UPGRADE_BY_MAX
};

enum EGUILD_WAR_DEFENDER_RESULT
{
	E_GUILD_WAR_DEFENDER_NONE         <DisplayName="E_GUILD_WAR_DEFENDER_NONE">,
	E_GUILD_WAR_DEFENDER_WIN          <DisplayName="E_GUILD_WAR_DEFENDER_WIN">,
	E_GUILD_WAR_DEFENDER_LOSE         <DisplayName="E_GUILD_WAR_DEFENDER_LOSE">,
	E_GUILD_WAR_DEFENDER_MAX
};

enum EGUILD_WAR_CHARACTER
{
	E_GUILD_WAR_NONE                  <DisplayName="E_GUILD_WAR_NONE">,
	E_GUILD_WAR_INTRUDER              <DisplayName="E_GUILD_WAR_INTRUDER">,
	E_GUILD_WAR_DEFENDER              <DisplayName="E_GUILD_WAR_DEFENDER">,
	E_GUILD_WAR_MAX
};

enum EACHIENEMENT_DEF
{
	E_ad_Invalid                      <DisplayName="E_ad_Invalid">,
	E_ad_Level                        <DisplayName="E_ad_Level">,
	E_ad_Experience                   <DisplayName="E_ad_Experience">,
	E_ad_PkValue                      <DisplayName="E_ad_PkValue">,
	E_ad_ForceReputation              <DisplayName="E_ad_ForceReputation">,
	E_ad_VariousForceCount            <DisplayName="E_ad_VariousForceCount">,
	E_ad_GetItem                      <DisplayName="E_ad_GetItem">,
	E_ad_ItemLuck                     <DisplayName="E_ad_ItemLuck">,
	E_ad_ItemRefine                   <DisplayName="E_ad_ItemRefine">,
	E_ad_RefineLevel                  <DisplayName="E_ad_RefineLevel">,
	E_ad_Equip                        <DisplayName="E_ad_Equip">,
	E_ad_EquipColor                   <DisplayName="E_ad_EquipColor">,
	E_ad_EquipBodyColor               <DisplayName="E_ad_EquipBodyColor">,
	E_ad_Set                          <DisplayName="E_ad_Set">,
	E_ad_SetCount                     <DisplayName="E_ad_SetCount">,
	E_ad_BankSlotCount                <DisplayName="E_ad_BankSlotCount">,
	E_ad_BagSlotCount                 <DisplayName="E_ad_BagSlotCount">,
	E_ad_ItemBagSlotCount             <DisplayName="E_ad_ItemBagSlotCount">,
	E_ad_EquipSlotCount               <DisplayName="E_ad_EquipSlotCount">,
	E_ad_MaterialBagSlotCount         <DisplayName="E_ad_MaterialBagSlotCount">,
	E_ad_TotalMoneyType               <DisplayName="E_ad_TotalMoneyType">,
	E_ad_KillPlayerTotalCount         <DisplayName="E_ad_KillPlayerTotalCount">,
	E_ad_EnterScene                   <DisplayName="E_ad_EnterScene">,
	E_ad_CompleteQuest                <DisplayName="E_ad_CompleteQuest">,
	E_ad_CompleteQuestCount           <DisplayName="E_ad_CompleteQuestCount">,
	E_ad_SpellLevelSpecified          <DisplayName="E_ad_SpellLevelSpecified">,
	E_ad_SpellLevel                   <DisplayName="E_ad_SpellLevel">,
	E_ad_LearnSpell                   <DisplayName="E_ad_LearnSpell">,
	E_ad_SpellCount                   <DisplayName="E_ad_SpellCount">,
	E_ad_ActiveSpellCount             <DisplayName="E_ad_ActiveSpellCount">,
	E_ad_RuneCount                    <DisplayName="E_ad_RuneCount">,
	E_ad_EquipPassiveSpellCount       <DisplayName="E_ad_EquipPassiveSpellCount">,
	E_ad_EquipPassiveSpellIDCount     <DisplayName="E_ad_EquipPassiveSpellIDCount">,
	E_ad_Achievement                  <DisplayName="E_ad_Achievement">,
	E_ad_AchievementCount             <DisplayName="E_ad_AchievementCount">,
	E_ad_Title                        <DisplayName="E_ad_Title">,
	E_ad_TitleCount                   <DisplayName="E_ad_TitleCount">,
	E_ad_FriendCount                  <DisplayName="E_ad_FriendCount">,
	E_ad_LeaveStudentCount            <DisplayName="E_ad_LeaveStudentCount">,
	E_ad_CreateTime                   <DisplayName="E_ad_CreateTime">,
	E_ad_Variable                     <DisplayName="E_ad_Variable">,
	E_ad_RoleVariable                 <DisplayName="E_ad_RoleVariable">,
	E_ad_KilledByNpcIDCount           <DisplayName="E_ad_KilledByNpcIDCount">,
	E_ad_KillNpcIDCount               <DisplayName="E_ad_KillNpcIDCount">,
	E_ad_KillNpcGroupIDCount          <DisplayName="E_ad_KillNpcGroupIDCount">,
	E_ad_UseItemIDCount               <DisplayName="E_ad_UseItemIDCount">,
	E_ad_Hero                         <DisplayName="E_ad_Hero">,
	E_ad_SpiritLevel                  <DisplayName="E_ad_SpiritLevel">,
	E_ad_SpiritAirColorCount          <DisplayName="E_ad_SpiritAirColorCount">,
	E_ad_SpiritAirColorLevel          <DisplayName="E_ad_SpiritAirColorLevel">,
	E_ad_SpiritBuyAirEquipSlotCount   <DisplayName="E_ad_SpiritBuyAirEquipSlotCount">,
	E_ad_SpiritBuyAirBagSlotCount     <DisplayName="E_ad_SpiritBuyAirBagSlotCount">,
	E_ad_SpiritBuyAirStallSlotCount   <DisplayName="E_ad_SpiritBuyAirStallSlotCount">,
	E_ad_SpiritBuyBloodSlotCount      <DisplayName="E_ad_SpiritBuyBloodSlotCount">,
	E_ad_SpiritBloodPageCount         <DisplayName="E_ad_SpiritBloodPageCount">,
	E_ad_SpiritBloodPageCompleteCount <DisplayName="E_ad_SpiritBloodPageCompleteCount">,
	E_ad_SpiritBloodPageLevel         <DisplayName="E_ad_SpiritBloodPageLevel">,
	E_ad_EquipDecompose               <DisplayName="E_ad_EquipDecompose">,
	E_ad_EquipColorDecompose          <DisplayName="E_ad_EquipColorDecompose">,
	E_ad_EquipCompose                 <DisplayName="E_ad_EquipCompose">,
	E_ad_EquipColorCompose            <DisplayName="E_ad_EquipColorCompose">,
	E_ad_Vip                          <DisplayName="E_ad_Vip">,
	E_ad_SubscribeVipTotalMonth       <DisplayName="E_ad_SubscribeVipTotalMonth">,
	E_ad_JoinTeamCount                <DisplayName="E_ad_JoinTeamCount">,
	E_ad_AuctionBuyItemCount          <DisplayName="E_ad_AuctionBuyItemCount">,
	E_ad_AuctionSellItemCount         <DisplayName="E_ad_AuctionSellItemCount">,
	E_ad_Ranklist                     <DisplayName="E_ad_Ranklist">,
	E_ad_ColorItemCompose             <DisplayName="E_ad_ColorItemCompose">,
	E_ad_ColorItemDeCompose           <DisplayName="E_ad_ColorItemDeCompose">,
	E_ad_RuneWeaponCount              <DisplayName="E_ad_RuneWeaponCount">,
	E_ad_Explore                      <DisplayName="E_ad_Explore">,
	E_ad_Handbook                     <DisplayName="E_ad_Handbook">,
	E_ad_Total                        <DisplayName="E_ad_Total">,
	E_ad_MAX
};

enum EGUILD_CHANGE_WEATHER_TIME_TYPE
{
	E_GUILD_CW_TIME_NONE              <DisplayName="E_GUILD_CW_TIME_NONE">,
	E_GUILD_CW_TIME_5M                <DisplayName="E_GUILD_CW_TIME_5M">,
	E_GUILD_CW_TIME_10M               <DisplayName="E_GUILD_CW_TIME_10M">,
	E_GUILD_CW_TIME_30M               <DisplayName="E_GUILD_CW_TIME_30M">,
	E_GUILD_CW_TIME_END               <DisplayName="E_GUILD_CW_TIME_END">,
	E_GUILD_CW_TIME_MAX
};

enum EGUILD_REPLY
{
	E_GUILD_REPLY_REFUSE              <DisplayName="E_GUILD_REPLY_REFUSE">,
	E_GUILD_REPLY_AGREE               <DisplayName="E_GUILD_REPLY_AGREE">,
	E_GUILD_REPLY_MAX
};

enum EGUILD_HISTORY_EVENT_TYPE
{
// conflicted enum definition GUILD_CHEST_BEGIN(50)
// failed to found enum prefix
	E__GUILD_HE_TYPE_BEGIN            <DisplayName="E__GUILD_HE_TYPE_BEGIN">,
	E__GUILD_HE_TYPE_CREATE           <DisplayName="E__GUILD_HE_TYPE_CREATE">,
	E__GUILD_HE_TYPE_JOIN             <DisplayName="E__GUILD_HE_TYPE_JOIN">,
	E__GUILD_HE_TYPE_LEAVE            <DisplayName="E__GUILD_HE_TYPE_LEAVE">,
	E__GUILD_HE_TYPE_BANISH           <DisplayName="E__GUILD_HE_TYPE_BANISH">,
	E__GUILD_HE_TYPE_POSITION_CHANGE  <DisplayName="E__GUILD_HE_TYPE_POSITION_CHANGE">,
	E__GUILD_HE_TYPE_BUILDING_LVLUP   <DisplayName="E__GUILD_HE_TYPE_BUILDING_LVLUP">,
	E__GUILD_HE_TYPE_SKILL_LVLUP      <DisplayName="E__GUILD_HE_TYPE_SKILL_LVLUP">,
	E__GUILD_HE_TYPE_DEMISE           <DisplayName="E__GUILD_HE_TYPE_DEMISE">,
	E__GUILD_HE_TYPE_UNION            <DisplayName="E__GUILD_HE_TYPE_UNION">,
	E__GUILD_HE_TYPE_ENEMY            <DisplayName="E__GUILD_HE_TYPE_ENEMY">,
	E__GUILD_HE_TYPE_WAR              <DisplayName="E__GUILD_HE_TYPE_WAR">,
	E__GUILD_HE_TYPE_BUILDING_CREATE  <DisplayName="E__GUILD_HE_TYPE_BUILDING_CREATE">,
	E__GUILD_HE_TYPE_BUILDING_DESTORY <DisplayName="E__GUILD_HE_TYPE_BUILDING_DESTORY">,
	E__GUILD_HE_TYPE_BUILDING_DEL     <DisplayName="E__GUILD_HE_TYPE_BUILDING_DEL">,
	E__GUILD_HE_TYPE_BUILDING_REVIVE  <DisplayName="E__GUILD_HE_TYPE_BUILDING_REVIVE">,
	E__GUILD_HE_TYPE_ATTACK_SUCC      <DisplayName="E__GUILD_HE_TYPE_ATTACK_SUCC">,
	E__GUILD_HE_TYPE_ATTACK_FAILED    <DisplayName="E__GUILD_HE_TYPE_ATTACK_FAILED">,
	E__GUILD_HE_TYPE_DEFEND_SUCC      <DisplayName="E__GUILD_HE_TYPE_DEFEND_SUCC">,
	E__GUILD_HE_TYPE_DEFEND_FAILED    <DisplayName="E__GUILD_HE_TYPE_DEFEND_FAILED">,
	E__GUILD_HE_TYPE_OPEN_ACTIVITY    <DisplayName="E__GUILD_HE_TYPE_OPEN_ACTIVITY">,
	E__GUILD_HE_CANCEL_ALLIANCE       <DisplayName="E__GUILD_HE_CANCEL_ALLIANCE">,
	E__Reserved_22                    <DisplayName="E__Reserved_22">,
	E__Reserved_23                    <DisplayName="E__Reserved_23">,
	E__Reserved_24                    <DisplayName="E__Reserved_24">,
	E__Reserved_25                    <DisplayName="E__Reserved_25">,
	E__Reserved_26                    <DisplayName="E__Reserved_26">,
	E__Reserved_27                    <DisplayName="E__Reserved_27">,
	E__Reserved_28                    <DisplayName="E__Reserved_28">,
	E__Reserved_29                    <DisplayName="E__Reserved_29">,
	E__GUILD_HE_TYPE_END              <DisplayName="E__GUILD_HE_TYPE_END">,
	E__Reserved_31                    <DisplayName="E__Reserved_31">,
	E__Reserved_32                    <DisplayName="E__Reserved_32">,
	E__Reserved_33                    <DisplayName="E__Reserved_33">,
	E__Reserved_34                    <DisplayName="E__Reserved_34">,
	E__Reserved_35                    <DisplayName="E__Reserved_35">,
	E__Reserved_36                    <DisplayName="E__Reserved_36">,
	E__Reserved_37                    <DisplayName="E__Reserved_37">,
	E__Reserved_38                    <DisplayName="E__Reserved_38">,
	E__Reserved_39                    <DisplayName="E__Reserved_39">,
	E__Reserved_40                    <DisplayName="E__Reserved_40">,
	E__Reserved_41                    <DisplayName="E__Reserved_41">,
	E__Reserved_42                    <DisplayName="E__Reserved_42">,
	E__Reserved_43                    <DisplayName="E__Reserved_43">,
	E__Reserved_44                    <DisplayName="E__Reserved_44">,
	E__Reserved_45                    <DisplayName="E__Reserved_45">,
	E__Reserved_46                    <DisplayName="E__Reserved_46">,
	E__Reserved_47                    <DisplayName="E__Reserved_47">,
	E__Reserved_48                    <DisplayName="E__Reserved_48">,
	E__Reserved_49                    <DisplayName="E__Reserved_49">,
	E__GUILD_CHEST_CREATE_BUILDING    <DisplayName="E__GUILD_CHEST_CREATE_BUILDING">,
	E__GUILD_CHEST_ACTIVITY           <DisplayName="E__GUILD_CHEST_ACTIVITY">,
	E__GUILD_CHEST_CONTRIBUTE         <DisplayName="E__GUILD_CHEST_CONTRIBUTE">,
	E__GUILD_CHEST_B_UPGRADE          <DisplayName="E__GUILD_CHEST_B_UPGRADE">,
	E__GUILD_CHEST_W_UPGRADE          <DisplayName="E__GUILD_CHEST_W_UPGRADE">,
	E__GUILD_CHEST_ALLIANCE           <DisplayName="E__GUILD_CHEST_ALLIANCE">,
	E__GUILD_CHEST_ARMY               <DisplayName="E__GUILD_CHEST_ARMY">,
	E__GUILD_CHEST_WAR_SACK_A_1       <DisplayName="E__GUILD_CHEST_WAR_SACK_A_1">,
	E__GUILD_CHEST_WAR_SACK_A_2       <DisplayName="E__GUILD_CHEST_WAR_SACK_A_2">,
	E__GUILD_CHEST_WAR_SACK_D_1       <DisplayName="E__GUILD_CHEST_WAR_SACK_D_1">,
	E__GUILD_CHEST_WAR_SACK_D_2       <DisplayName="E__GUILD_CHEST_WAR_SACK_D_2">,
	E__GUILD_CHEST_WAR_WIN            <DisplayName="E__GUILD_CHEST_WAR_WIN">,
	E__GUILD_CHEST_WAR_LOSE           <DisplayName="E__GUILD_CHEST_WAR_LOSE">,
	E__GUILD_CHEST_CANCEL_BUILDING_UPGRADE <DisplayName="E__GUILD_CHEST_CANCEL_BUILDING_UPGRADE">,
	E__GUILD_CHEST_CANCEL_WELFARE_UPGRADE <DisplayName="E__GUILD_CHEST_CANCEL_WELFARE_UPGRADE">,
	E__GUILD_CHEST_DEL_BUILDING       <DisplayName="E__GUILD_CHEST_DEL_BUILDING">,
	E__GUILD_CHEST_OPEN_DUNGEON       <DisplayName="E__GUILD_CHEST_OPEN_DUNGEON">,
	E__GUILD_CHEST_COMPLETE_DUNGEON   <DisplayName="E__GUILD_CHEST_COMPLETE_DUNGEON">,
	E__GUILD_CHEST_WAR_PLUNDER        <DisplayName="E__GUILD_CHEST_WAR_PLUNDER">,
	E__Reserved_69                    <DisplayName="E__Reserved_69">,
	E__GUILD_CHEST_WAR_PLUNDER_EXTRA  <DisplayName="E__GUILD_CHEST_WAR_PLUNDER_EXTRA">,
	E__GUILD_CHEST_ROUTINE_CONSUME    <DisplayName="E__GUILD_CHEST_ROUTINE_CONSUME">,
	E__GUILD_CHEST_DECLARE_WAR        <DisplayName="E__GUILD_CHEST_DECLARE_WAR">,
	E__GUILD_CHEST_MATCH_WAR          <DisplayName="E__GUILD_CHEST_MATCH_WAR">,
	E__GUILD_CHEST_BUILDING_DAMAGE    <DisplayName="E__GUILD_CHEST_BUILDING_DAMAGE">,
	E__GUILD_CHEST_GM                 <DisplayName="E__GUILD_CHEST_GM">,
	E__Reserved_76                    <DisplayName="E__Reserved_76">,
	E__Reserved_77                    <DisplayName="E__Reserved_77">,
	E__Reserved_78                    <DisplayName="E__Reserved_78">,
	E__Reserved_79                    <DisplayName="E__Reserved_79">,
	E__GUILD_CHEST_END                <DisplayName="E__GUILD_CHEST_END">,
	E__Reserved_81                    <DisplayName="E__Reserved_81">,
	E__Reserved_82                    <DisplayName="E__Reserved_82">,
	E__Reserved_83                    <DisplayName="E__Reserved_83">,
	E__Reserved_84                    <DisplayName="E__Reserved_84">,
	E__Reserved_85                    <DisplayName="E__Reserved_85">,
	E__Reserved_86                    <DisplayName="E__Reserved_86">,
	E__Reserved_87                    <DisplayName="E__Reserved_87">,
	E__Reserved_88                    <DisplayName="E__Reserved_88">,
	E__Reserved_89                    <DisplayName="E__Reserved_89">,
	E__STORAGE_HE_TYPE_BEGIN          <DisplayName="E__STORAGE_HE_TYPE_BEGIN">,
	E__STORAGE_HE_TYPE_PUSH           <DisplayName="E__STORAGE_HE_TYPE_PUSH">,
	E__STORAGE_HE_TYPE_POP            <DisplayName="E__STORAGE_HE_TYPE_POP">,
	E__MAX
};

enum EEActivityPtsReason
{
	E_apr_Invalid                     <DisplayName="E_apr_Invalid">,
	E_apr_WuDouChang                  <DisplayName="E_apr_WuDouChang">,
	E_apr_Baoku                       <DisplayName="E_apr_Baoku">,
	E_apr_YanWuChang                  <DisplayName="E_apr_YanWuChang">,
	E_apr_Pray                        <DisplayName="E_apr_Pray">,
	E_apr_Escort                      <DisplayName="E_apr_Escort">,
	E_apr_GuildBoss                   <DisplayName="E_apr_GuildBoss">,
	E_apr_Spring                      <DisplayName="E_apr_Spring">,
	E_apr_Quest                       <DisplayName="E_apr_Quest">,
	E_apr_YaoTa                       <DisplayName="E_apr_YaoTa">,
	E_apr_YouMingSea                  <DisplayName="E_apr_YouMingSea">,
	E_apr_Total                       <DisplayName="E_apr_Total">,
	E_apr_MAX
};

enum EGUILD_DIPLOMAT_TIME_TYPE
{
	E_GUILD_DIPLOMAT_TIME_BEGIN       <DisplayName="E_GUILD_DIPLOMAT_TIME_BEGIN">,
	E_GUILD_DIPLOMAT_TIME_ONE         <DisplayName="E_GUILD_DIPLOMAT_TIME_ONE">,
	E_GUILD_DIPLOMAT_TIME_THREE       <DisplayName="E_GUILD_DIPLOMAT_TIME_THREE">,
	E_GUILD_DIPLOMAT_TIME_SEVEN       <DisplayName="E_GUILD_DIPLOMAT_TIME_SEVEN">,
	E_GUILD_DIPLOMAT_TIME_THIRTY      <DisplayName="E_GUILD_DIPLOMAT_TIME_THIRTY">,
	E_GUILD_DIPLOMAT_TIME_END         <DisplayName="E_GUILD_DIPLOMAT_TIME_END">,
	E_GUILD_DIPLOMAT_TIME_MAX
};

enum EGUILD_DIPLOMAT_OP_TYPE
{
	E_GUILD_DIPLOMAT_NEUTRALLY        <DisplayName="E_GUILD_DIPLOMAT_NEUTRALLY">,
	E_GUILD_DIPLOMAT_ALLIANCE         <DisplayName="E_GUILD_DIPLOMAT_ALLIANCE">,
	E_GUILD_DIPLOMAT_ENEMY            <DisplayName="E_GUILD_DIPLOMAT_ENEMY">,
	E_GUILD_DIPLOMAT_MAX
};

enum EGUILD_STATE
{
	E_GUILD_STATE_NONE                <DisplayName="E_GUILD_STATE_NONE">,
	E_GUILD_STATE_STOP_WELFARE        <DisplayName="E_GUILD_STATE_STOP_WELFARE">,
	E_GUILD_STATE_MAX
};

enum EGUILD_JOIN_AGREE_TYPE
{
	E_GUILD_AGREE_BY_Reserved_0       <DisplayName="E_GUILD_AGREE_BY_Reserved_0">,
	E_GUILD_AGREE_BY_APPLY            <DisplayName="E_GUILD_AGREE_BY_APPLY">,
	E_GUILD_AGREE_BY_INVITE           <DisplayName="E_GUILD_AGREE_BY_INVITE">,
	E_GUILD_AGREE_BY_MAX
};

enum EEItemActionType
{
	E_ITEMACTION_ADD                  <DisplayName="E_ITEMACTION_ADD">,
	E_ITEMACTION_DEL                  <DisplayName="E_ITEMACTION_DEL">,
	E_ITEMACTION_INC                  <DisplayName="E_ITEMACTION_INC">,
	E_ITEMACTION_DEC                  <DisplayName="E_ITEMACTION_DEC">,
	E_ITEMACTION_DROP                 <DisplayName="E_ITEMACTION_DROP">,
	E_ITEMACTION_TOTAL                <DisplayName="E_ITEMACTION_TOTAL">,
	E_ITEMACTION_MAX
};

enum ECHAR_SEX_TYPE
{
	E_cst_Invalid                     <DisplayName="E_cst_Invalid">,
	E_cst_Male                        <DisplayName="E_cst_Male">,
	E_cst_Female                      <DisplayName="E_cst_Female">,
	E_cst_Asexual                     <DisplayName="E_cst_Asexual">,
	E_cst_Total                       <DisplayName="E_cst_Total">,
	E_cst_MAX
};

enum EDOODAD_SUB_TYPE
{
	E_dst_Invalid                     <DisplayName="E_dst_Invalid">,
	E_dst_Ore                         <DisplayName="E_dst_Ore">,
	E_dst_Herb                        <DisplayName="E_dst_Herb">,
	E_dst_FishingPool                 <DisplayName="E_dst_FishingPool">,
	E_dst_Item                        <DisplayName="E_dst_Item">,
	E_dst_Book                        <DisplayName="E_dst_Book">,
	E_dst_Total                       <DisplayName="E_dst_Total">,
	E_dst_MAX
};

enum EGUILD_RESOURCE_CHANGE_TYPE
{
	E_GUILD_RES_Reserved_0            <DisplayName="E_GUILD_RES_Reserved_0">,
	E_GUILD_RES_ADD_COUNT             <DisplayName="E_GUILD_RES_ADD_COUNT">,
	E_GUILD_RES_DEC_COUNT             <DisplayName="E_GUILD_RES_DEC_COUNT">,
	E_GUILD_RES_ADD_PERCENT           <DisplayName="E_GUILD_RES_ADD_PERCENT">,
	E_GUILD_RES_DEC_PERCENT           <DisplayName="E_GUILD_RES_DEC_PERCENT">,
	E_GUILD_RES_MAX
};

enum EHANDBOOK_TYPE
{
// conflicted enum definition htNpcBegin(1)
// conflicted enum definition htEquip_Begin(11)
	E_ht_Invalid                      <DisplayName="E_ht_Invalid">,
	E_ht_Npc_Normal                   <DisplayName="E_ht_Npc_Normal">,
	E_ht_Npc_Elite                    <DisplayName="E_ht_Npc_Elite">,
	E_ht_NPc_Boss                     <DisplayName="E_ht_NPc_Boss">,
	E_ht_Npc_Function                 <DisplayName="E_ht_Npc_Function">,
	E_ht_Npc_Story                    <DisplayName="E_ht_Npc_Story">,
	E_ht_Map_Wild                     <DisplayName="E_ht_Map_Wild">,
	E_ht_Map_Dungeon                  <DisplayName="E_ht_Map_Dungeon">,
	E_ht_Map_Activity                 <DisplayName="E_ht_Map_Activity">,
	E_ht_Item_Compose                 <DisplayName="E_ht_Item_Compose">,
	E_ht_Item_Stone                   <DisplayName="E_ht_Item_Stone">,
	E_ht_Item_Story                   <DisplayName="E_ht_Item_Story">,
	E_ht_Equip_Set                    <DisplayName="E_ht_Equip_Set">,
	E_ht_Equip_Weapon                 <DisplayName="E_ht_Equip_Weapon">,
	E_ht_Equip_Jewelry                <DisplayName="E_ht_Equip_Jewelry">,
	E_ht_Equip_Armor                  <DisplayName="E_ht_Equip_Armor">,
	E_ht_Rune_Warrior                 <DisplayName="E_ht_Rune_Warrior">,
	E_ht_Rune_Mage                    <DisplayName="E_ht_Rune_Mage">,
	E_ht_Rune_Rogue                   <DisplayName="E_ht_Rune_Rogue">,
	E_ht_Rune_Archer                  <DisplayName="E_ht_Rune_Archer">,
	E_ht_Rune_Taoist                  <DisplayName="E_ht_Rune_Taoist">,
	E_ht_Air_Green                    <DisplayName="E_ht_Air_Green">,
	E_ht_Air_Blue                     <DisplayName="E_ht_Air_Blue">,
	E_ht_Air_Purple                   <DisplayName="E_ht_Air_Purple">,
	E_ht_Total                        <DisplayName="E_ht_Total">,
	E_ht_MAX
};

enum ERUNE_FRAG_TYPE
{
	E_rft_FragA                       <DisplayName="E_rft_FragA">,
	E_rft_FragB                       <DisplayName="E_rft_FragB">,
	E_rft_FragC                       <DisplayName="E_rft_FragC">,
	E_rft_FragD                       <DisplayName="E_rft_FragD">,
	E_rft_FragE                       <DisplayName="E_rft_FragE">,
	E_rft_Count                       <DisplayName="E_rft_Count">,
	E_rft_MAX
};

enum EGUILD_FORBID_CHAT_ENUM
{
	E_GUILD_CHAT_Reserved_0           <DisplayName="E_GUILD_CHAT_Reserved_0">,
	E_GUILD_CHAT_FORBID               <DisplayName="E_GUILD_CHAT_FORBID">,
	E_GUILD_CHAT_UNFORBID             <DisplayName="E_GUILD_CHAT_UNFORBID">,
	E_GUILD_CHAT_MAX
};

enum EGUILD_REVERT_APPLY_ENUM
{
	E_GUILD_REVERT_Reserved_0         <DisplayName="E_GUILD_REVERT_Reserved_0">,
	E_GUILD_REVERT_REFUSE             <DisplayName="E_GUILD_REVERT_REFUSE">,
	E_GUILD_REVERT_AGREE              <DisplayName="E_GUILD_REVERT_AGREE">,
	E_GUILD_REVERT_MAX
};

enum EGUILD_DIPLOMATIC_TIME_TYPE
{
	E_G_DIPLOMATIC_TIME_Reserved_0    <DisplayName="E_G_DIPLOMATIC_TIME_Reserved_0">,
	E_G_DIPLOMATIC_TIME_1             <DisplayName="E_G_DIPLOMATIC_TIME_1">,
	E_G_DIPLOMATIC_TIME_2             <DisplayName="E_G_DIPLOMATIC_TIME_2">,
	E_G_DIPLOMATIC_TIME_3             <DisplayName="E_G_DIPLOMATIC_TIME_3">,
	E_G_DIPLOMATIC_TIME_4             <DisplayName="E_G_DIPLOMATIC_TIME_4">,
	E_G_DIPLOMATIC_TIME_MAX
};

enum EITEM_SUBTYPE
{
	E_ITEMSUBTYPE_INVALID             <DisplayName="E_ITEMSUBTYPE_INVALID">,
	E_ITEMSUBTYPE_USABLE_FOOD         <DisplayName="E_ITEMSUBTYPE_USABLE_FOOD">,
	E_ITEMSUBTYPE_EQUIP_HAT           <DisplayName="E_ITEMSUBTYPE_EQUIP_HAT">,
	E_ITEMSUBTYPE_MATERIAL_WEAPON_LEVELUP <DisplayName="E_ITEMSUBTYPE_MATERIAL_WEAPON_LEVELUP">,
	E_ITEMSUBTYPE_EQUIP_CHEST_ARMOR   <DisplayName="E_ITEMSUBTYPE_EQUIP_CHEST_ARMOR">,
	E_ITEMSUBTYPE_USABLE_SCROLL       <DisplayName="E_ITEMSUBTYPE_USABLE_SCROLL">,
	E_ITEMSUBTYPE_EQUIP_BELT          <DisplayName="E_ITEMSUBTYPE_EQUIP_BELT">,
	E_ITEMSUBTYPE_USABLE_GREY_POWDER  <DisplayName="E_ITEMSUBTYPE_USABLE_GREY_POWDER">,
	E_ITEMSUBTYPE_USABLE_YELLOW_POWDER <DisplayName="E_ITEMSUBTYPE_USABLE_YELLOW_POWDER">,
	E_ITEMSUBTYPE_EQUIP_NECKLACE      <DisplayName="E_ITEMSUBTYPE_EQUIP_NECKLACE">,
	E_ITEMSUBTYPE_EQUIP_RING          <DisplayName="E_ITEMSUBTYPE_EQUIP_RING">,
	E_ITEMSUBTYPE_EQUIP_BRACELET      <DisplayName="E_ITEMSUBTYPE_EQUIP_BRACELET">,
	E_ITEMSUBTYPE_MAX
};

enum EAURA_RESIST_TYPE
{
	E_aurt_None                       <DisplayName="E_aurt_None">,
	E_aurt_Poison                     <DisplayName="E_aurt_Poison">,
	E_aurt_Dazzy                      <DisplayName="E_aurt_Dazzy">,
	E_aurt_Freeze                     <DisplayName="E_aurt_Freeze">,
	E_aurt_Burn                       <DisplayName="E_aurt_Burn">,
	E_aurt_Repel                      <DisplayName="E_aurt_Repel">,
	E_aurt_Total                      <DisplayName="E_aurt_Total">,
	E_aurt_MAX
};

enum EGRID_PK_TYPE
{
	E_gpt_Normal                      <DisplayName="E_gpt_Normal">,
	E_gpt_Free                        <DisplayName="E_gpt_Free">,
	E_gpt_Safe                        <DisplayName="E_gpt_Safe">,
	E_gpt_Total                       <DisplayName="E_gpt_Total">,
	E_gpt_MAX
};

enum EGUILD_STORAGE_RIGHT_FLAG
{
// max enum definition is over 254
	E_G_STORAGE_Reserved_0            <DisplayName="E_G_STORAGE_Reserved_0">,
	E_G_STORAGE_1_POP                 <DisplayName="E_G_STORAGE_1_POP">,
	E_G_STORAGE_2_POP                 <DisplayName="E_G_STORAGE_2_POP">,
	E_G_STORAGE_Reserved_3            <DisplayName="E_G_STORAGE_Reserved_3">,
	E_G_STORAGE_3_POP                 <DisplayName="E_G_STORAGE_3_POP">,
	E_G_STORAGE_Reserved_5            <DisplayName="E_G_STORAGE_Reserved_5">,
	E_G_STORAGE_Reserved_6            <DisplayName="E_G_STORAGE_Reserved_6">,
	E_G_STORAGE_Reserved_7            <DisplayName="E_G_STORAGE_Reserved_7">,
	E_G_STORAGE_4_POP                 <DisplayName="E_G_STORAGE_4_POP">,
	E_G_STORAGE_Reserved_9            <DisplayName="E_G_STORAGE_Reserved_9">,
	E_G_STORAGE_Reserved_10           <DisplayName="E_G_STORAGE_Reserved_10">,
	E_G_STORAGE_Reserved_11           <DisplayName="E_G_STORAGE_Reserved_11">,
	E_G_STORAGE_Reserved_12           <DisplayName="E_G_STORAGE_Reserved_12">,
	E_G_STORAGE_Reserved_13           <DisplayName="E_G_STORAGE_Reserved_13">,
	E_G_STORAGE_Reserved_14           <DisplayName="E_G_STORAGE_Reserved_14">,
	E_G_STORAGE_Reserved_15           <DisplayName="E_G_STORAGE_Reserved_15">,
	E_G_STORAGE_5_POP                 <DisplayName="E_G_STORAGE_5_POP">,
	E_G_STORAGE_Reserved_17           <DisplayName="E_G_STORAGE_Reserved_17">,
	E_G_STORAGE_Reserved_18           <DisplayName="E_G_STORAGE_Reserved_18">,
	E_G_STORAGE_Reserved_19           <DisplayName="E_G_STORAGE_Reserved_19">,
	E_G_STORAGE_Reserved_20           <DisplayName="E_G_STORAGE_Reserved_20">,
	E_G_STORAGE_Reserved_21           <DisplayName="E_G_STORAGE_Reserved_21">,
	E_G_STORAGE_Reserved_22           <DisplayName="E_G_STORAGE_Reserved_22">,
	E_G_STORAGE_Reserved_23           <DisplayName="E_G_STORAGE_Reserved_23">,
	E_G_STORAGE_Reserved_24           <DisplayName="E_G_STORAGE_Reserved_24">,
	E_G_STORAGE_Reserved_25           <DisplayName="E_G_STORAGE_Reserved_25">,
	E_G_STORAGE_Reserved_26           <DisplayName="E_G_STORAGE_Reserved_26">,
	E_G_STORAGE_Reserved_27           <DisplayName="E_G_STORAGE_Reserved_27">,
	E_G_STORAGE_Reserved_28           <DisplayName="E_G_STORAGE_Reserved_28">,
	E_G_STORAGE_Reserved_29           <DisplayName="E_G_STORAGE_Reserved_29">,
	E_G_STORAGE_Reserved_30           <DisplayName="E_G_STORAGE_Reserved_30">,
	E_G_STORAGE_Reserved_31           <DisplayName="E_G_STORAGE_Reserved_31">,
	E_G_STORAGE_6_POP                 <DisplayName="E_G_STORAGE_6_POP">,
	E_G_STORAGE_Reserved_33           <DisplayName="E_G_STORAGE_Reserved_33">,
	E_G_STORAGE_Reserved_34           <DisplayName="E_G_STORAGE_Reserved_34">,
	E_G_STORAGE_Reserved_35           <DisplayName="E_G_STORAGE_Reserved_35">,
	E_G_STORAGE_Reserved_36           <DisplayName="E_G_STORAGE_Reserved_36">,
	E_G_STORAGE_Reserved_37           <DisplayName="E_G_STORAGE_Reserved_37">,
	E_G_STORAGE_Reserved_38           <DisplayName="E_G_STORAGE_Reserved_38">,
	E_G_STORAGE_Reserved_39           <DisplayName="E_G_STORAGE_Reserved_39">,
	E_G_STORAGE_Reserved_40           <DisplayName="E_G_STORAGE_Reserved_40">,
	E_G_STORAGE_Reserved_41           <DisplayName="E_G_STORAGE_Reserved_41">,
	E_G_STORAGE_Reserved_42           <DisplayName="E_G_STORAGE_Reserved_42">,
	E_G_STORAGE_Reserved_43           <DisplayName="E_G_STORAGE_Reserved_43">,
	E_G_STORAGE_Reserved_44           <DisplayName="E_G_STORAGE_Reserved_44">,
	E_G_STORAGE_Reserved_45           <DisplayName="E_G_STORAGE_Reserved_45">,
	E_G_STORAGE_Reserved_46           <DisplayName="E_G_STORAGE_Reserved_46">,
	E_G_STORAGE_Reserved_47           <DisplayName="E_G_STORAGE_Reserved_47">,
	E_G_STORAGE_Reserved_48           <DisplayName="E_G_STORAGE_Reserved_48">,
	E_G_STORAGE_Reserved_49           <DisplayName="E_G_STORAGE_Reserved_49">,
	E_G_STORAGE_Reserved_50           <DisplayName="E_G_STORAGE_Reserved_50">,
	E_G_STORAGE_Reserved_51           <DisplayName="E_G_STORAGE_Reserved_51">,
	E_G_STORAGE_Reserved_52           <DisplayName="E_G_STORAGE_Reserved_52">,
	E_G_STORAGE_Reserved_53           <DisplayName="E_G_STORAGE_Reserved_53">,
	E_G_STORAGE_Reserved_54           <DisplayName="E_G_STORAGE_Reserved_54">,
	E_G_STORAGE_Reserved_55           <DisplayName="E_G_STORAGE_Reserved_55">,
	E_G_STORAGE_Reserved_56           <DisplayName="E_G_STORAGE_Reserved_56">,
	E_G_STORAGE_Reserved_57           <DisplayName="E_G_STORAGE_Reserved_57">,
	E_G_STORAGE_Reserved_58           <DisplayName="E_G_STORAGE_Reserved_58">,
	E_G_STORAGE_Reserved_59           <DisplayName="E_G_STORAGE_Reserved_59">,
	E_G_STORAGE_Reserved_60           <DisplayName="E_G_STORAGE_Reserved_60">,
	E_G_STORAGE_Reserved_61           <DisplayName="E_G_STORAGE_Reserved_61">,
	E_G_STORAGE_Reserved_62           <DisplayName="E_G_STORAGE_Reserved_62">,
	E_G_STORAGE_Reserved_63           <DisplayName="E_G_STORAGE_Reserved_63">,
	E_G_STORAGE_1_PUSH                <DisplayName="E_G_STORAGE_1_PUSH">,
	E_G_STORAGE_Reserved_65           <DisplayName="E_G_STORAGE_Reserved_65">,
	E_G_STORAGE_Reserved_66           <DisplayName="E_G_STORAGE_Reserved_66">,
	E_G_STORAGE_Reserved_67           <DisplayName="E_G_STORAGE_Reserved_67">,
	E_G_STORAGE_Reserved_68           <DisplayName="E_G_STORAGE_Reserved_68">,
	E_G_STORAGE_Reserved_69           <DisplayName="E_G_STORAGE_Reserved_69">,
	E_G_STORAGE_Reserved_70           <DisplayName="E_G_STORAGE_Reserved_70">,
	E_G_STORAGE_Reserved_71           <DisplayName="E_G_STORAGE_Reserved_71">,
	E_G_STORAGE_Reserved_72           <DisplayName="E_G_STORAGE_Reserved_72">,
	E_G_STORAGE_Reserved_73           <DisplayName="E_G_STORAGE_Reserved_73">,
	E_G_STORAGE_Reserved_74           <DisplayName="E_G_STORAGE_Reserved_74">,
	E_G_STORAGE_Reserved_75           <DisplayName="E_G_STORAGE_Reserved_75">,
	E_G_STORAGE_Reserved_76           <DisplayName="E_G_STORAGE_Reserved_76">,
	E_G_STORAGE_Reserved_77           <DisplayName="E_G_STORAGE_Reserved_77">,
	E_G_STORAGE_Reserved_78           <DisplayName="E_G_STORAGE_Reserved_78">,
	E_G_STORAGE_Reserved_79           <DisplayName="E_G_STORAGE_Reserved_79">,
	E_G_STORAGE_Reserved_80           <DisplayName="E_G_STORAGE_Reserved_80">,
	E_G_STORAGE_Reserved_81           <DisplayName="E_G_STORAGE_Reserved_81">,
	E_G_STORAGE_Reserved_82           <DisplayName="E_G_STORAGE_Reserved_82">,
	E_G_STORAGE_Reserved_83           <DisplayName="E_G_STORAGE_Reserved_83">,
	E_G_STORAGE_Reserved_84           <DisplayName="E_G_STORAGE_Reserved_84">,
	E_G_STORAGE_Reserved_85           <DisplayName="E_G_STORAGE_Reserved_85">,
	E_G_STORAGE_Reserved_86           <DisplayName="E_G_STORAGE_Reserved_86">,
	E_G_STORAGE_Reserved_87           <DisplayName="E_G_STORAGE_Reserved_87">,
	E_G_STORAGE_Reserved_88           <DisplayName="E_G_STORAGE_Reserved_88">,
	E_G_STORAGE_Reserved_89           <DisplayName="E_G_STORAGE_Reserved_89">,
	E_G_STORAGE_Reserved_90           <DisplayName="E_G_STORAGE_Reserved_90">,
	E_G_STORAGE_Reserved_91           <DisplayName="E_G_STORAGE_Reserved_91">,
	E_G_STORAGE_Reserved_92           <DisplayName="E_G_STORAGE_Reserved_92">,
	E_G_STORAGE_Reserved_93           <DisplayName="E_G_STORAGE_Reserved_93">,
	E_G_STORAGE_Reserved_94           <DisplayName="E_G_STORAGE_Reserved_94">,
	E_G_STORAGE_Reserved_95           <DisplayName="E_G_STORAGE_Reserved_95">,
	E_G_STORAGE_Reserved_96           <DisplayName="E_G_STORAGE_Reserved_96">,
	E_G_STORAGE_Reserved_97           <DisplayName="E_G_STORAGE_Reserved_97">,
	E_G_STORAGE_Reserved_98           <DisplayName="E_G_STORAGE_Reserved_98">,
	E_G_STORAGE_Reserved_99           <DisplayName="E_G_STORAGE_Reserved_99">,
	E_G_STORAGE_Reserved_100          <DisplayName="E_G_STORAGE_Reserved_100">,
	E_G_STORAGE_Reserved_101          <DisplayName="E_G_STORAGE_Reserved_101">,
	E_G_STORAGE_Reserved_102          <DisplayName="E_G_STORAGE_Reserved_102">,
	E_G_STORAGE_Reserved_103          <DisplayName="E_G_STORAGE_Reserved_103">,
	E_G_STORAGE_Reserved_104          <DisplayName="E_G_STORAGE_Reserved_104">,
	E_G_STORAGE_Reserved_105          <DisplayName="E_G_STORAGE_Reserved_105">,
	E_G_STORAGE_Reserved_106          <DisplayName="E_G_STORAGE_Reserved_106">,
	E_G_STORAGE_Reserved_107          <DisplayName="E_G_STORAGE_Reserved_107">,
	E_G_STORAGE_Reserved_108          <DisplayName="E_G_STORAGE_Reserved_108">,
	E_G_STORAGE_Reserved_109          <DisplayName="E_G_STORAGE_Reserved_109">,
	E_G_STORAGE_Reserved_110          <DisplayName="E_G_STORAGE_Reserved_110">,
	E_G_STORAGE_Reserved_111          <DisplayName="E_G_STORAGE_Reserved_111">,
	E_G_STORAGE_Reserved_112          <DisplayName="E_G_STORAGE_Reserved_112">,
	E_G_STORAGE_Reserved_113          <DisplayName="E_G_STORAGE_Reserved_113">,
	E_G_STORAGE_Reserved_114          <DisplayName="E_G_STORAGE_Reserved_114">,
	E_G_STORAGE_Reserved_115          <DisplayName="E_G_STORAGE_Reserved_115">,
	E_G_STORAGE_Reserved_116          <DisplayName="E_G_STORAGE_Reserved_116">,
	E_G_STORAGE_Reserved_117          <DisplayName="E_G_STORAGE_Reserved_117">,
	E_G_STORAGE_Reserved_118          <DisplayName="E_G_STORAGE_Reserved_118">,
	E_G_STORAGE_Reserved_119          <DisplayName="E_G_STORAGE_Reserved_119">,
	E_G_STORAGE_Reserved_120          <DisplayName="E_G_STORAGE_Reserved_120">,
	E_G_STORAGE_Reserved_121          <DisplayName="E_G_STORAGE_Reserved_121">,
	E_G_STORAGE_Reserved_122          <DisplayName="E_G_STORAGE_Reserved_122">,
	E_G_STORAGE_Reserved_123          <DisplayName="E_G_STORAGE_Reserved_123">,
	E_G_STORAGE_Reserved_124          <DisplayName="E_G_STORAGE_Reserved_124">,
	E_G_STORAGE_Reserved_125          <DisplayName="E_G_STORAGE_Reserved_125">,
	E_G_STORAGE_Reserved_126          <DisplayName="E_G_STORAGE_Reserved_126">,
	E_G_STORAGE_Reserved_127          <DisplayName="E_G_STORAGE_Reserved_127">,
	E_G_STORAGE_2_PUSH                <DisplayName="E_G_STORAGE_2_PUSH">,
	E_G_STORAGE_Reserved_129          <DisplayName="E_G_STORAGE_Reserved_129">,
	E_G_STORAGE_Reserved_130          <DisplayName="E_G_STORAGE_Reserved_130">,
	E_G_STORAGE_Reserved_131          <DisplayName="E_G_STORAGE_Reserved_131">,
	E_G_STORAGE_Reserved_132          <DisplayName="E_G_STORAGE_Reserved_132">,
	E_G_STORAGE_Reserved_133          <DisplayName="E_G_STORAGE_Reserved_133">,
	E_G_STORAGE_Reserved_134          <DisplayName="E_G_STORAGE_Reserved_134">,
	E_G_STORAGE_Reserved_135          <DisplayName="E_G_STORAGE_Reserved_135">,
	E_G_STORAGE_Reserved_136          <DisplayName="E_G_STORAGE_Reserved_136">,
	E_G_STORAGE_Reserved_137          <DisplayName="E_G_STORAGE_Reserved_137">,
	E_G_STORAGE_Reserved_138          <DisplayName="E_G_STORAGE_Reserved_138">,
	E_G_STORAGE_Reserved_139          <DisplayName="E_G_STORAGE_Reserved_139">,
	E_G_STORAGE_Reserved_140          <DisplayName="E_G_STORAGE_Reserved_140">,
	E_G_STORAGE_Reserved_141          <DisplayName="E_G_STORAGE_Reserved_141">,
	E_G_STORAGE_Reserved_142          <DisplayName="E_G_STORAGE_Reserved_142">,
	E_G_STORAGE_Reserved_143          <DisplayName="E_G_STORAGE_Reserved_143">,
	E_G_STORAGE_Reserved_144          <DisplayName="E_G_STORAGE_Reserved_144">,
	E_G_STORAGE_Reserved_145          <DisplayName="E_G_STORAGE_Reserved_145">,
	E_G_STORAGE_Reserved_146          <DisplayName="E_G_STORAGE_Reserved_146">,
	E_G_STORAGE_Reserved_147          <DisplayName="E_G_STORAGE_Reserved_147">,
	E_G_STORAGE_Reserved_148          <DisplayName="E_G_STORAGE_Reserved_148">,
	E_G_STORAGE_Reserved_149          <DisplayName="E_G_STORAGE_Reserved_149">,
	E_G_STORAGE_Reserved_150          <DisplayName="E_G_STORAGE_Reserved_150">,
	E_G_STORAGE_Reserved_151          <DisplayName="E_G_STORAGE_Reserved_151">,
	E_G_STORAGE_Reserved_152          <DisplayName="E_G_STORAGE_Reserved_152">,
	E_G_STORAGE_Reserved_153          <DisplayName="E_G_STORAGE_Reserved_153">,
	E_G_STORAGE_Reserved_154          <DisplayName="E_G_STORAGE_Reserved_154">,
	E_G_STORAGE_Reserved_155          <DisplayName="E_G_STORAGE_Reserved_155">,
	E_G_STORAGE_Reserved_156          <DisplayName="E_G_STORAGE_Reserved_156">,
	E_G_STORAGE_Reserved_157          <DisplayName="E_G_STORAGE_Reserved_157">,
	E_G_STORAGE_Reserved_158          <DisplayName="E_G_STORAGE_Reserved_158">,
	E_G_STORAGE_Reserved_159          <DisplayName="E_G_STORAGE_Reserved_159">,
	E_G_STORAGE_Reserved_160          <DisplayName="E_G_STORAGE_Reserved_160">,
	E_G_STORAGE_Reserved_161          <DisplayName="E_G_STORAGE_Reserved_161">,
	E_G_STORAGE_Reserved_162          <DisplayName="E_G_STORAGE_Reserved_162">,
	E_G_STORAGE_Reserved_163          <DisplayName="E_G_STORAGE_Reserved_163">,
	E_G_STORAGE_Reserved_164          <DisplayName="E_G_STORAGE_Reserved_164">,
	E_G_STORAGE_Reserved_165          <DisplayName="E_G_STORAGE_Reserved_165">,
	E_G_STORAGE_Reserved_166          <DisplayName="E_G_STORAGE_Reserved_166">,
	E_G_STORAGE_Reserved_167          <DisplayName="E_G_STORAGE_Reserved_167">,
	E_G_STORAGE_Reserved_168          <DisplayName="E_G_STORAGE_Reserved_168">,
	E_G_STORAGE_Reserved_169          <DisplayName="E_G_STORAGE_Reserved_169">,
	E_G_STORAGE_Reserved_170          <DisplayName="E_G_STORAGE_Reserved_170">,
	E_G_STORAGE_Reserved_171          <DisplayName="E_G_STORAGE_Reserved_171">,
	E_G_STORAGE_Reserved_172          <DisplayName="E_G_STORAGE_Reserved_172">,
	E_G_STORAGE_Reserved_173          <DisplayName="E_G_STORAGE_Reserved_173">,
	E_G_STORAGE_Reserved_174          <DisplayName="E_G_STORAGE_Reserved_174">,
	E_G_STORAGE_Reserved_175          <DisplayName="E_G_STORAGE_Reserved_175">,
	E_G_STORAGE_Reserved_176          <DisplayName="E_G_STORAGE_Reserved_176">,
	E_G_STORAGE_Reserved_177          <DisplayName="E_G_STORAGE_Reserved_177">,
	E_G_STORAGE_Reserved_178          <DisplayName="E_G_STORAGE_Reserved_178">,
	E_G_STORAGE_Reserved_179          <DisplayName="E_G_STORAGE_Reserved_179">,
	E_G_STORAGE_Reserved_180          <DisplayName="E_G_STORAGE_Reserved_180">,
	E_G_STORAGE_Reserved_181          <DisplayName="E_G_STORAGE_Reserved_181">,
	E_G_STORAGE_Reserved_182          <DisplayName="E_G_STORAGE_Reserved_182">,
	E_G_STORAGE_Reserved_183          <DisplayName="E_G_STORAGE_Reserved_183">,
	E_G_STORAGE_Reserved_184          <DisplayName="E_G_STORAGE_Reserved_184">,
	E_G_STORAGE_Reserved_185          <DisplayName="E_G_STORAGE_Reserved_185">,
	E_G_STORAGE_Reserved_186          <DisplayName="E_G_STORAGE_Reserved_186">,
	E_G_STORAGE_Reserved_187          <DisplayName="E_G_STORAGE_Reserved_187">,
	E_G_STORAGE_Reserved_188          <DisplayName="E_G_STORAGE_Reserved_188">,
	E_G_STORAGE_Reserved_189          <DisplayName="E_G_STORAGE_Reserved_189">,
	E_G_STORAGE_Reserved_190          <DisplayName="E_G_STORAGE_Reserved_190">,
	E_G_STORAGE_Reserved_191          <DisplayName="E_G_STORAGE_Reserved_191">,
	E_G_STORAGE_Reserved_192          <DisplayName="E_G_STORAGE_Reserved_192">,
	E_G_STORAGE_Reserved_193          <DisplayName="E_G_STORAGE_Reserved_193">,
	E_G_STORAGE_Reserved_194          <DisplayName="E_G_STORAGE_Reserved_194">,
	E_G_STORAGE_Reserved_195          <DisplayName="E_G_STORAGE_Reserved_195">,
	E_G_STORAGE_Reserved_196          <DisplayName="E_G_STORAGE_Reserved_196">,
	E_G_STORAGE_Reserved_197          <DisplayName="E_G_STORAGE_Reserved_197">,
	E_G_STORAGE_Reserved_198          <DisplayName="E_G_STORAGE_Reserved_198">,
	E_G_STORAGE_Reserved_199          <DisplayName="E_G_STORAGE_Reserved_199">,
	E_G_STORAGE_Reserved_200          <DisplayName="E_G_STORAGE_Reserved_200">,
	E_G_STORAGE_Reserved_201          <DisplayName="E_G_STORAGE_Reserved_201">,
	E_G_STORAGE_Reserved_202          <DisplayName="E_G_STORAGE_Reserved_202">,
	E_G_STORAGE_Reserved_203          <DisplayName="E_G_STORAGE_Reserved_203">,
	E_G_STORAGE_Reserved_204          <DisplayName="E_G_STORAGE_Reserved_204">,
	E_G_STORAGE_Reserved_205          <DisplayName="E_G_STORAGE_Reserved_205">,
	E_G_STORAGE_Reserved_206          <DisplayName="E_G_STORAGE_Reserved_206">,
	E_G_STORAGE_Reserved_207          <DisplayName="E_G_STORAGE_Reserved_207">,
	E_G_STORAGE_Reserved_208          <DisplayName="E_G_STORAGE_Reserved_208">,
	E_G_STORAGE_Reserved_209          <DisplayName="E_G_STORAGE_Reserved_209">,
	E_G_STORAGE_Reserved_210          <DisplayName="E_G_STORAGE_Reserved_210">,
	E_G_STORAGE_Reserved_211          <DisplayName="E_G_STORAGE_Reserved_211">,
	E_G_STORAGE_Reserved_212          <DisplayName="E_G_STORAGE_Reserved_212">,
	E_G_STORAGE_Reserved_213          <DisplayName="E_G_STORAGE_Reserved_213">,
	E_G_STORAGE_Reserved_214          <DisplayName="E_G_STORAGE_Reserved_214">,
	E_G_STORAGE_Reserved_215          <DisplayName="E_G_STORAGE_Reserved_215">,
	E_G_STORAGE_Reserved_216          <DisplayName="E_G_STORAGE_Reserved_216">,
	E_G_STORAGE_Reserved_217          <DisplayName="E_G_STORAGE_Reserved_217">,
	E_G_STORAGE_Reserved_218          <DisplayName="E_G_STORAGE_Reserved_218">,
	E_G_STORAGE_Reserved_219          <DisplayName="E_G_STORAGE_Reserved_219">,
	E_G_STORAGE_Reserved_220          <DisplayName="E_G_STORAGE_Reserved_220">,
	E_G_STORAGE_Reserved_221          <DisplayName="E_G_STORAGE_Reserved_221">,
	E_G_STORAGE_Reserved_222          <DisplayName="E_G_STORAGE_Reserved_222">,
	E_G_STORAGE_Reserved_223          <DisplayName="E_G_STORAGE_Reserved_223">,
	E_G_STORAGE_Reserved_224          <DisplayName="E_G_STORAGE_Reserved_224">,
	E_G_STORAGE_Reserved_225          <DisplayName="E_G_STORAGE_Reserved_225">,
	E_G_STORAGE_Reserved_226          <DisplayName="E_G_STORAGE_Reserved_226">,
	E_G_STORAGE_Reserved_227          <DisplayName="E_G_STORAGE_Reserved_227">,
	E_G_STORAGE_Reserved_228          <DisplayName="E_G_STORAGE_Reserved_228">,
	E_G_STORAGE_Reserved_229          <DisplayName="E_G_STORAGE_Reserved_229">,
	E_G_STORAGE_Reserved_230          <DisplayName="E_G_STORAGE_Reserved_230">,
	E_G_STORAGE_Reserved_231          <DisplayName="E_G_STORAGE_Reserved_231">,
	E_G_STORAGE_Reserved_232          <DisplayName="E_G_STORAGE_Reserved_232">,
	E_G_STORAGE_Reserved_233          <DisplayName="E_G_STORAGE_Reserved_233">,
	E_G_STORAGE_Reserved_234          <DisplayName="E_G_STORAGE_Reserved_234">,
	E_G_STORAGE_Reserved_235          <DisplayName="E_G_STORAGE_Reserved_235">,
	E_G_STORAGE_Reserved_236          <DisplayName="E_G_STORAGE_Reserved_236">,
	E_G_STORAGE_Reserved_237          <DisplayName="E_G_STORAGE_Reserved_237">,
	E_G_STORAGE_Reserved_238          <DisplayName="E_G_STORAGE_Reserved_238">,
	E_G_STORAGE_Reserved_239          <DisplayName="E_G_STORAGE_Reserved_239">,
	E_G_STORAGE_Reserved_240          <DisplayName="E_G_STORAGE_Reserved_240">,
	E_G_STORAGE_Reserved_241          <DisplayName="E_G_STORAGE_Reserved_241">,
	E_G_STORAGE_Reserved_242          <DisplayName="E_G_STORAGE_Reserved_242">,
	E_G_STORAGE_Reserved_243          <DisplayName="E_G_STORAGE_Reserved_243">,
	E_G_STORAGE_Reserved_244          <DisplayName="E_G_STORAGE_Reserved_244">,
	E_G_STORAGE_Reserved_245          <DisplayName="E_G_STORAGE_Reserved_245">,
	E_G_STORAGE_Reserved_246          <DisplayName="E_G_STORAGE_Reserved_246">,
	E_G_STORAGE_Reserved_247          <DisplayName="E_G_STORAGE_Reserved_247">,
	E_G_STORAGE_Reserved_248          <DisplayName="E_G_STORAGE_Reserved_248">,
	E_G_STORAGE_Reserved_249          <DisplayName="E_G_STORAGE_Reserved_249">,
	E_G_STORAGE_Reserved_250          <DisplayName="E_G_STORAGE_Reserved_250">,
	E_G_STORAGE_Reserved_251          <DisplayName="E_G_STORAGE_Reserved_251">,
	E_G_STORAGE_Reserved_252          <DisplayName="E_G_STORAGE_Reserved_252">,
	E_G_STORAGE_Reserved_253          <DisplayName="E_G_STORAGE_Reserved_253">,
	E_G_STORAGE_Reserved_254          <DisplayName="E_G_STORAGE_Reserved_254">,
	E_G_STORAGE_MAX
};

enum EPET_TYPE
{
	E_pet_Invalid                     <DisplayName="E_pet_Invalid">,
	E_pet_Total                       <DisplayName="E_pet_Total">,
	E_pet_MAX
};

enum EGUILD_ACHIEVEMENT_TYPE
{
	E_GUILD_ACHI_NONE                 <DisplayName="E_GUILD_ACHI_NONE">,
	E_GUILD_ACHI_MEMBER               <DisplayName="E_GUILD_ACHI_MEMBER">,
	E_GUILD_ACHI_ENEMY                <DisplayName="E_GUILD_ACHI_ENEMY">,
	E_GUILD_ACHI_ALLIANCE             <DisplayName="E_GUILD_ACHI_ALLIANCE">,
	E_GUILD_ACHI_TAKE_POS_1           <DisplayName="E_GUILD_ACHI_TAKE_POS_1">,
	E_GUILD_ACHI_TAKE_POS_2           <DisplayName="E_GUILD_ACHI_TAKE_POS_2">,
	E_GUILD_ACHI_TYPE_NUM             <DisplayName="E_GUILD_ACHI_TYPE_NUM">,
	E_GUILD_ACHI_MAX
};

enum EBATTLE_TEMPLATE_TYPE
{
	E_btt_Fixed                       <DisplayName="E_btt_Fixed">,
	E_btt_5V5                         <DisplayName="E_btt_5V5">,
	E_btt_10V10                       <DisplayName="E_btt_10V10">,
	E_btt_15V15                       <DisplayName="E_btt_15V15">,
	E_btt_10V10V10                    <DisplayName="E_btt_10V10V10">,
	E_btt_20V20                       <DisplayName="E_btt_20V20">,
	E_btt_Total                       <DisplayName="E_btt_Total">,
	E_btt_MAX
};

enum ETEAM_ASSIGN
{
	E_eta_Invalid                     <DisplayName="E_eta_Invalid">,
	E_eta_FreeForAll                  <DisplayName="E_eta_FreeForAll">,
	E_eta_TeamAuction                 <DisplayName="E_eta_TeamAuction">,
	E_eta_LeaderPick                  <DisplayName="E_eta_LeaderPick">,
	E_eta_Total                       <DisplayName="E_eta_Total">,
	E_eta_MAX
};

enum EAURA_ATTR_TYPE
{
	E_aat_Category                    <DisplayName="E_aat_Category">,
	E_aat_Type                        <DisplayName="E_aat_Type">,
	E_aat_Group                       <DisplayName="E_aat_Group">,
	E_aat_Duration                    <DisplayName="E_aat_Duration">,
	E_aat_Delay                       <DisplayName="E_aat_Delay">,
	E_aat_Interval                    <DisplayName="E_aat_Interval">,
	E_aat_SameIdStackType             <DisplayName="E_aat_SameIdStackType">,
	E_aat_DiffIdStackType             <DisplayName="E_aat_DiffIdStackType">,
	E_aat_StackTypeSameIdSameCaster   <DisplayName="E_aat_StackTypeSameIdSameCaster">,
	E_aat_StackSameTypeSameCaster     <DisplayName="E_aat_StackSameTypeSameCaster">,
	E_aat_StackSameGroupSameCaster    <DisplayName="E_aat_StackSameGroupSameCaster">,
	E_aat_StackTypeSameIdDiffCaster   <DisplayName="E_aat_StackTypeSameIdDiffCaster">,
	E_aat_StackSameTypeDiffCaster     <DisplayName="E_aat_StackSameTypeDiffCaster">,
	E_aat_StackSameGroupDiffCaster    <DisplayName="E_aat_StackSameGroupDiffCaster">,
	E_aat_TimeCover                   <DisplayName="E_aat_TimeCover">,
	E_aat_StackResetTime              <DisplayName="E_aat_StackResetTime">,
	E_aat_InitStackCount              <DisplayName="E_aat_InitStackCount">,
	E_aat_MaxStackCount               <DisplayName="E_aat_MaxStackCount">,
	E_aat_StrengthLevel               <DisplayName="E_aat_StrengthLevel">,
	E_aat_CanDispel                   <DisplayName="E_aat_CanDispel">,
	E_aat_CanCancel                   <DisplayName="E_aat_CanCancel">,
	E_aat_DispelCount                 <DisplayName="E_aat_DispelCount">,
	E_aat_TrigOnRemoveWhenDispel      <DisplayName="E_aat_TrigOnRemoveWhenDispel">,
	E_aat_TrigOnRemoveWhenTimeup      <DisplayName="E_aat_TrigOnRemoveWhenTimeup">,
	E_aat_AttackType                  <DisplayName="E_aat_AttackType">,
	E_aat_JudgeType                   <DisplayName="E_aat_JudgeType">,
	E_aat_ElementaryType              <DisplayName="E_aat_ElementaryType">,
	E_aat_EffectPctX                  <DisplayName="E_aat_EffectPctX">,
	E_aat_EffectValueX                <DisplayName="E_aat_EffectValueX">,
	E_aat_EffectPct                   <DisplayName="E_aat_EffectPct">,
	E_aat_EffectValue                 <DisplayName="E_aat_EffectValue">,
	E_aat_ThreatPct                   <DisplayName="E_aat_ThreatPct">,
	E_aat_ThreatValue                 <DisplayName="E_aat_ThreatValue">,
	E_aat_ThreatMax                   <DisplayName="E_aat_ThreatMax">,
	E_aat_RemoveWhenDead              <DisplayName="E_aat_RemoveWhenDead">,
	E_aat_SaveToDB                    <DisplayName="E_aat_SaveToDB">,
	E_aat_OfflineTimeValid            <DisplayName="E_aat_OfflineTimeValid">,
	E_aat_HiddenToClient              <DisplayName="E_aat_HiddenToClient">,
	E_aat_HiddenToSelf                <DisplayName="E_aat_HiddenToSelf">,
	E_aat_ControlDecay                <DisplayName="E_aat_ControlDecay">,
	E_aat_EffectTimeMin               <DisplayName="E_aat_EffectTimeMin">,
	E_aat_Eternal                     <DisplayName="E_aat_Eternal">,
	E_aat_IgnoreDefense               <DisplayName="E_aat_IgnoreDefense">,
	E_aat_IgnoreResistence            <DisplayName="E_aat_IgnoreResistence">,
	E_aat_AffectedCharAttr            <DisplayName="E_aat_AffectedCharAttr">,
	E_aat_DisplayGroup                <DisplayName="E_aat_DisplayGroup">,
	E_aat_DisplayValue                <DisplayName="E_aat_DisplayValue">,
	E_aat_ResistType                  <DisplayName="E_aat_ResistType">,
	E_aat_EffectDecay                 <DisplayName="E_aat_EffectDecay">,
	E_aat_OnAppend                    <DisplayName="E_aat_OnAppend">,
	E_aat_BeforeStack                 <DisplayName="E_aat_BeforeStack">,
	E_aat_AfterStack                  <DisplayName="E_aat_AfterStack">,
	E_aat_OnTimeup                    <DisplayName="E_aat_OnTimeup">,
	E_aat_OnDispel                    <DisplayName="E_aat_OnDispel">,
	E_aat_OnRemove                    <DisplayName="E_aat_OnRemove">,
	E_aat_Total                       <DisplayName="E_aat_Total">,
	E_aat_MAX
};

enum ESPELL_TARGET_RELATIONSHIP
{
	E_sptr_Self                       <DisplayName="E_sptr_Self">,
	E_sptr_Friendly                   <DisplayName="E_sptr_Friendly">,
	E_sptr_Enemy                      <DisplayName="E_sptr_Enemy">,
	E_sptr_Total                      <DisplayName="E_sptr_Total">,
	E_sptr_MAX
};

enum EGUILD_BUILDING_ENUM
{
	E_GUILD_B_NONE                    <DisplayName="E_GUILD_B_NONE">,
	E_GUILD_B_MAIN                    <DisplayName="E_GUILD_B_MAIN">,
	E_GUILD_B_BUSINESS                <DisplayName="E_GUILD_B_BUSINESS">,
	E_GUILD_B_DEITY                   <DisplayName="E_GUILD_B_DEITY">,
	E_GUILD_B_HOUSE                   <DisplayName="E_GUILD_B_HOUSE">,
	E_GUILD_B_STORAGE                 <DisplayName="E_GUILD_B_STORAGE">,
	E_GUILD_B_STAFF                   <DisplayName="E_GUILD_B_STAFF">,
	E_GUILD_B_WONDER                  <DisplayName="E_GUILD_B_WONDER">,
	E_GUILD_B_HERB                    <DisplayName="E_GUILD_B_HERB">,
	E_GUILD_B_WOOD                    <DisplayName="E_GUILD_B_WOOD">,
	E_GUILD_B_CRYSTAL                 <DisplayName="E_GUILD_B_CRYSTAL">,
	E_GUILD_B_SILVER                  <DisplayName="E_GUILD_B_SILVER">,
	E_GUILD_B_HERB_STORAGE            <DisplayName="E_GUILD_B_HERB_STORAGE">,
	E_GUILD_B_WOOD_STORAGE            <DisplayName="E_GUILD_B_WOOD_STORAGE">,
	E_GUILD_B_CRYSTAL_STORAGE         <DisplayName="E_GUILD_B_CRYSTAL_STORAGE">,
	E_GUILD_B_SILVER_STORAGE          <DisplayName="E_GUILD_B_SILVER_STORAGE">,
	E_GUILD_B_FUNATIONAL_NUM          <DisplayName="E_GUILD_B_FUNATIONAL_NUM">,
	E_GUILD_B_WALL                    <DisplayName="E_GUILD_B_WALL">,
	E_GUILD_B_ARMSHOP                 <DisplayName="E_GUILD_B_ARMSHOP">,
	E_GUILD_B_ARROW                   <DisplayName="E_GUILD_B_ARROW">,
	E_GUILD_B_TURRET                  <DisplayName="E_GUILD_B_TURRET">,
	E_GUILD_B_GROUP_ATTACK            <DisplayName="E_GUILD_B_GROUP_ATTACK">,
	E_GUILD_B_MAGIC                   <DisplayName="E_GUILD_B_MAGIC">,
	E_GUILD_B_CURE                    <DisplayName="E_GUILD_B_CURE">,
	E_GUILD_B_BLOCK                   <DisplayName="E_GUILD_B_BLOCK">,
	E_GUILD_B_PROTECTOR               <DisplayName="E_GUILD_B_PROTECTOR">,
	E_GUILD_B_MAX
};

enum ECHAR_SPECIES_TYPE
{
	E_spc_None                        <DisplayName="E_spc_None">,
	E_spc_Player                      <DisplayName="E_spc_Player">,
	E_spc_Human                       <DisplayName="E_spc_Human">,
	E_spc_Humanlike                   <DisplayName="E_spc_Humanlike">,
	E_spc_Beast                       <DisplayName="E_spc_Beast">,
	E_spc_Insect                      <DisplayName="E_spc_Insect">,
	E_spc_Plant                       <DisplayName="E_spc_Plant">,
	E_spc_Undead                      <DisplayName="E_spc_Undead">,
	E_spc_Machine                     <DisplayName="E_spc_Machine">,
	E_spc_Dragon                      <DisplayName="E_spc_Dragon">,
	E_spc_Demon                       <DisplayName="E_spc_Demon">,
	E_spc_Elements                    <DisplayName="E_spc_Elements">,
	E_spc_Building                    <DisplayName="E_spc_Building">,
	E_spc_Total                       <DisplayName="E_spc_Total">,
	E_spc_MAX
};

enum EARMOR_TYPE
{
	E_ARMORTYPE_NO_ARMOR              <DisplayName="E_ARMORTYPE_NO_ARMOR">,
	E_ARMORTYPE_CLOTH                 <DisplayName="E_ARMORTYPE_CLOTH">,
	E_ARMORTYPE_LEATHER               <DisplayName="E_ARMORTYPE_LEATHER">,
	E_ARMORTYPE_CHAIN                 <DisplayName="E_ARMORTYPE_CHAIN">,
	E_ARMORTYPE_PLATE                 <DisplayName="E_ARMORTYPE_PLATE">,
	E_ARMORTYPE_BACK                  <DisplayName="E_ARMORTYPE_BACK">,
	E_ARMORTYPE_JEWELRY               <DisplayName="E_ARMORTYPE_JEWELRY">,
	E_ARMORTYPE_AVATAR                <DisplayName="E_ARMORTYPE_AVATAR">,
	E_ARMORTYPE_MAX
};

enum EGUILD_NOTIFY_TYPE
{
	E_GUILD_NOTIFY_Reserved_0         <DisplayName="E_GUILD_NOTIFY_Reserved_0">,
	E_GUILD_NOTIFY_ROLE_APP           <DisplayName="E_GUILD_NOTIFY_ROLE_APP">,
	E_GUILD_NOTIFY_DIPLOMAT_APP       <DisplayName="E_GUILD_NOTIFY_DIPLOMAT_APP">,
	E_GUILD_NOTIFY_MATCH_WAR          <DisplayName="E_GUILD_NOTIFY_MATCH_WAR">,
	E_GUILD_NOTIFY_MAX
};

enum ESCENE_OBJECT_APPEAR_TYPE
{
	E_soat_Invalid                    <DisplayName="E_soat_Invalid">,
	E_soat_Login                      <DisplayName="E_soat_Login">,
	E_soat_InRange                    <DisplayName="E_soat_InRange">,
	E_soat_Spawn                      <DisplayName="E_soat_Spawn">,
	E_soat_Revive                     <DisplayName="E_soat_Revive">,
	E_soat_NoPerforSpawn              <DisplayName="E_soat_NoPerforSpawn">,
	E_soat_Total                      <DisplayName="E_soat_Total">,
	E_soat_MAX
};

enum ENPC_QUALITY
{
	E_nq_Invalid                      <DisplayName="E_nq_Invalid">,
	E_nq_Normal1                      <DisplayName="E_nq_Normal1">,
	E_nq_Normal2                      <DisplayName="E_nq_Normal2">,
	E_nq_Normal3                      <DisplayName="E_nq_Normal3">,
	E_nq_Normal4                      <DisplayName="E_nq_Normal4">,
	E_nq_Normal5                      <DisplayName="E_nq_Normal5">,
	E_nq_Elite                        <DisplayName="E_nq_Elite">,
	E_nq_Rare                         <DisplayName="E_nq_Rare">,
	E_nq_Boss                         <DisplayName="E_nq_Boss">,
	E_nq_Total                        <DisplayName="E_nq_Total">,
	E_nq_MAX
};

enum EVARIABLE_TYPE
{
// max enum definition is over 254
	E_v_tInvalid                      <DisplayName="E_v_tInvalid">,
	E_v_tRole                         <DisplayName="E_v_tRole">,
	E_v_tScene                        <DisplayName="E_v_tScene">,
	E_v_tGuild                        <DisplayName="E_v_tGuild">,
	E_v_tQuest                        <DisplayName="E_v_tQuest">,
	E_v_tItemRecycle                  <DisplayName="E_v_tItemRecycle">,
	E_v_tKillNpcCount                 <DisplayName="E_v_tKillNpcCount">,
	E_v_tClient                       <DisplayName="E_v_tClient">,
	E_v_tAI                           <DisplayName="E_v_tAI">,
	E_v_tAISpell                      <DisplayName="E_v_tAISpell">,
	E_v_tServer                       <DisplayName="E_v_tServer">,
	E_v_tKillDoodad                   <DisplayName="E_v_tKillDoodad">,
	E_v_tAchievement                  <DisplayName="E_v_tAchievement">,
	E_v_tKillPlayerCount              <DisplayName="E_v_tKillPlayerCount">,
	E_v_tKillItemNpcCount             <DisplayName="E_v_tKillItemNpcCount">,
	E_v_tTotal                        <DisplayName="E_v_tTotal">,
	E_v_Reserved_16                   <DisplayName="E_v_Reserved_16">,
	E_v_Reserved_17                   <DisplayName="E_v_Reserved_17">,
	E_v_Reserved_18                   <DisplayName="E_v_Reserved_18">,
	E_v_Reserved_19                   <DisplayName="E_v_Reserved_19">,
	E_v_Reserved_20                   <DisplayName="E_v_Reserved_20">,
	E_v_Reserved_21                   <DisplayName="E_v_Reserved_21">,
	E_v_Reserved_22                   <DisplayName="E_v_Reserved_22">,
	E_v_Reserved_23                   <DisplayName="E_v_Reserved_23">,
	E_v_Reserved_24                   <DisplayName="E_v_Reserved_24">,
	E_v_Reserved_25                   <DisplayName="E_v_Reserved_25">,
	E_v_Reserved_26                   <DisplayName="E_v_Reserved_26">,
	E_v_Reserved_27                   <DisplayName="E_v_Reserved_27">,
	E_v_Reserved_28                   <DisplayName="E_v_Reserved_28">,
	E_v_Reserved_29                   <DisplayName="E_v_Reserved_29">,
	E_v_Reserved_30                   <DisplayName="E_v_Reserved_30">,
	E_v_Reserved_31                   <DisplayName="E_v_Reserved_31">,
	E_v_Reserved_32                   <DisplayName="E_v_Reserved_32">,
	E_v_Reserved_33                   <DisplayName="E_v_Reserved_33">,
	E_v_Reserved_34                   <DisplayName="E_v_Reserved_34">,
	E_v_Reserved_35                   <DisplayName="E_v_Reserved_35">,
	E_v_Reserved_36                   <DisplayName="E_v_Reserved_36">,
	E_v_Reserved_37                   <DisplayName="E_v_Reserved_37">,
	E_v_Reserved_38                   <DisplayName="E_v_Reserved_38">,
	E_v_Reserved_39                   <DisplayName="E_v_Reserved_39">,
	E_v_Reserved_40                   <DisplayName="E_v_Reserved_40">,
	E_v_Reserved_41                   <DisplayName="E_v_Reserved_41">,
	E_v_Reserved_42                   <DisplayName="E_v_Reserved_42">,
	E_v_Reserved_43                   <DisplayName="E_v_Reserved_43">,
	E_v_Reserved_44                   <DisplayName="E_v_Reserved_44">,
	E_v_Reserved_45                   <DisplayName="E_v_Reserved_45">,
	E_v_Reserved_46                   <DisplayName="E_v_Reserved_46">,
	E_v_Reserved_47                   <DisplayName="E_v_Reserved_47">,
	E_v_Reserved_48                   <DisplayName="E_v_Reserved_48">,
	E_v_Reserved_49                   <DisplayName="E_v_Reserved_49">,
	E_v_Reserved_50                   <DisplayName="E_v_Reserved_50">,
	E_v_Reserved_51                   <DisplayName="E_v_Reserved_51">,
	E_v_Reserved_52                   <DisplayName="E_v_Reserved_52">,
	E_v_Reserved_53                   <DisplayName="E_v_Reserved_53">,
	E_v_Reserved_54                   <DisplayName="E_v_Reserved_54">,
	E_v_Reserved_55                   <DisplayName="E_v_Reserved_55">,
	E_v_Reserved_56                   <DisplayName="E_v_Reserved_56">,
	E_v_Reserved_57                   <DisplayName="E_v_Reserved_57">,
	E_v_Reserved_58                   <DisplayName="E_v_Reserved_58">,
	E_v_Reserved_59                   <DisplayName="E_v_Reserved_59">,
	E_v_Reserved_60                   <DisplayName="E_v_Reserved_60">,
	E_v_Reserved_61                   <DisplayName="E_v_Reserved_61">,
	E_v_Reserved_62                   <DisplayName="E_v_Reserved_62">,
	E_v_Reserved_63                   <DisplayName="E_v_Reserved_63">,
	E_v_Reserved_64                   <DisplayName="E_v_Reserved_64">,
	E_v_Reserved_65                   <DisplayName="E_v_Reserved_65">,
	E_v_Reserved_66                   <DisplayName="E_v_Reserved_66">,
	E_v_Reserved_67                   <DisplayName="E_v_Reserved_67">,
	E_v_Reserved_68                   <DisplayName="E_v_Reserved_68">,
	E_v_Reserved_69                   <DisplayName="E_v_Reserved_69">,
	E_v_Reserved_70                   <DisplayName="E_v_Reserved_70">,
	E_v_Reserved_71                   <DisplayName="E_v_Reserved_71">,
	E_v_Reserved_72                   <DisplayName="E_v_Reserved_72">,
	E_v_Reserved_73                   <DisplayName="E_v_Reserved_73">,
	E_v_Reserved_74                   <DisplayName="E_v_Reserved_74">,
	E_v_Reserved_75                   <DisplayName="E_v_Reserved_75">,
	E_v_Reserved_76                   <DisplayName="E_v_Reserved_76">,
	E_v_Reserved_77                   <DisplayName="E_v_Reserved_77">,
	E_v_Reserved_78                   <DisplayName="E_v_Reserved_78">,
	E_v_Reserved_79                   <DisplayName="E_v_Reserved_79">,
	E_v_Reserved_80                   <DisplayName="E_v_Reserved_80">,
	E_v_Reserved_81                   <DisplayName="E_v_Reserved_81">,
	E_v_Reserved_82                   <DisplayName="E_v_Reserved_82">,
	E_v_Reserved_83                   <DisplayName="E_v_Reserved_83">,
	E_v_Reserved_84                   <DisplayName="E_v_Reserved_84">,
	E_v_Reserved_85                   <DisplayName="E_v_Reserved_85">,
	E_v_Reserved_86                   <DisplayName="E_v_Reserved_86">,
	E_v_Reserved_87                   <DisplayName="E_v_Reserved_87">,
	E_v_Reserved_88                   <DisplayName="E_v_Reserved_88">,
	E_v_Reserved_89                   <DisplayName="E_v_Reserved_89">,
	E_v_Reserved_90                   <DisplayName="E_v_Reserved_90">,
	E_v_Reserved_91                   <DisplayName="E_v_Reserved_91">,
	E_v_Reserved_92                   <DisplayName="E_v_Reserved_92">,
	E_v_Reserved_93                   <DisplayName="E_v_Reserved_93">,
	E_v_Reserved_94                   <DisplayName="E_v_Reserved_94">,
	E_v_Reserved_95                   <DisplayName="E_v_Reserved_95">,
	E_v_Reserved_96                   <DisplayName="E_v_Reserved_96">,
	E_v_Reserved_97                   <DisplayName="E_v_Reserved_97">,
	E_v_Reserved_98                   <DisplayName="E_v_Reserved_98">,
	E_v_Reserved_99                   <DisplayName="E_v_Reserved_99">,
	E_v_Reserved_100                  <DisplayName="E_v_Reserved_100">,
	E_v_Reserved_101                  <DisplayName="E_v_Reserved_101">,
	E_v_Reserved_102                  <DisplayName="E_v_Reserved_102">,
	E_v_Reserved_103                  <DisplayName="E_v_Reserved_103">,
	E_v_Reserved_104                  <DisplayName="E_v_Reserved_104">,
	E_v_Reserved_105                  <DisplayName="E_v_Reserved_105">,
	E_v_Reserved_106                  <DisplayName="E_v_Reserved_106">,
	E_v_Reserved_107                  <DisplayName="E_v_Reserved_107">,
	E_v_Reserved_108                  <DisplayName="E_v_Reserved_108">,
	E_v_Reserved_109                  <DisplayName="E_v_Reserved_109">,
	E_v_Reserved_110                  <DisplayName="E_v_Reserved_110">,
	E_v_Reserved_111                  <DisplayName="E_v_Reserved_111">,
	E_v_Reserved_112                  <DisplayName="E_v_Reserved_112">,
	E_v_Reserved_113                  <DisplayName="E_v_Reserved_113">,
	E_v_Reserved_114                  <DisplayName="E_v_Reserved_114">,
	E_v_Reserved_115                  <DisplayName="E_v_Reserved_115">,
	E_v_Reserved_116                  <DisplayName="E_v_Reserved_116">,
	E_v_Reserved_117                  <DisplayName="E_v_Reserved_117">,
	E_v_Reserved_118                  <DisplayName="E_v_Reserved_118">,
	E_v_Reserved_119                  <DisplayName="E_v_Reserved_119">,
	E_v_Reserved_120                  <DisplayName="E_v_Reserved_120">,
	E_v_Reserved_121                  <DisplayName="E_v_Reserved_121">,
	E_v_Reserved_122                  <DisplayName="E_v_Reserved_122">,
	E_v_Reserved_123                  <DisplayName="E_v_Reserved_123">,
	E_v_Reserved_124                  <DisplayName="E_v_Reserved_124">,
	E_v_Reserved_125                  <DisplayName="E_v_Reserved_125">,
	E_v_Reserved_126                  <DisplayName="E_v_Reserved_126">,
	E_v_Reserved_127                  <DisplayName="E_v_Reserved_127">,
	E_v_Reserved_128                  <DisplayName="E_v_Reserved_128">,
	E_v_Reserved_129                  <DisplayName="E_v_Reserved_129">,
	E_v_Reserved_130                  <DisplayName="E_v_Reserved_130">,
	E_v_Reserved_131                  <DisplayName="E_v_Reserved_131">,
	E_v_Reserved_132                  <DisplayName="E_v_Reserved_132">,
	E_v_Reserved_133                  <DisplayName="E_v_Reserved_133">,
	E_v_Reserved_134                  <DisplayName="E_v_Reserved_134">,
	E_v_Reserved_135                  <DisplayName="E_v_Reserved_135">,
	E_v_Reserved_136                  <DisplayName="E_v_Reserved_136">,
	E_v_Reserved_137                  <DisplayName="E_v_Reserved_137">,
	E_v_Reserved_138                  <DisplayName="E_v_Reserved_138">,
	E_v_Reserved_139                  <DisplayName="E_v_Reserved_139">,
	E_v_Reserved_140                  <DisplayName="E_v_Reserved_140">,
	E_v_Reserved_141                  <DisplayName="E_v_Reserved_141">,
	E_v_Reserved_142                  <DisplayName="E_v_Reserved_142">,
	E_v_Reserved_143                  <DisplayName="E_v_Reserved_143">,
	E_v_Reserved_144                  <DisplayName="E_v_Reserved_144">,
	E_v_Reserved_145                  <DisplayName="E_v_Reserved_145">,
	E_v_Reserved_146                  <DisplayName="E_v_Reserved_146">,
	E_v_Reserved_147                  <DisplayName="E_v_Reserved_147">,
	E_v_Reserved_148                  <DisplayName="E_v_Reserved_148">,
	E_v_Reserved_149                  <DisplayName="E_v_Reserved_149">,
	E_v_Reserved_150                  <DisplayName="E_v_Reserved_150">,
	E_v_Reserved_151                  <DisplayName="E_v_Reserved_151">,
	E_v_Reserved_152                  <DisplayName="E_v_Reserved_152">,
	E_v_Reserved_153                  <DisplayName="E_v_Reserved_153">,
	E_v_Reserved_154                  <DisplayName="E_v_Reserved_154">,
	E_v_Reserved_155                  <DisplayName="E_v_Reserved_155">,
	E_v_Reserved_156                  <DisplayName="E_v_Reserved_156">,
	E_v_Reserved_157                  <DisplayName="E_v_Reserved_157">,
	E_v_Reserved_158                  <DisplayName="E_v_Reserved_158">,
	E_v_Reserved_159                  <DisplayName="E_v_Reserved_159">,
	E_v_Reserved_160                  <DisplayName="E_v_Reserved_160">,
	E_v_Reserved_161                  <DisplayName="E_v_Reserved_161">,
	E_v_Reserved_162                  <DisplayName="E_v_Reserved_162">,
	E_v_Reserved_163                  <DisplayName="E_v_Reserved_163">,
	E_v_Reserved_164                  <DisplayName="E_v_Reserved_164">,
	E_v_Reserved_165                  <DisplayName="E_v_Reserved_165">,
	E_v_Reserved_166                  <DisplayName="E_v_Reserved_166">,
	E_v_Reserved_167                  <DisplayName="E_v_Reserved_167">,
	E_v_Reserved_168                  <DisplayName="E_v_Reserved_168">,
	E_v_Reserved_169                  <DisplayName="E_v_Reserved_169">,
	E_v_Reserved_170                  <DisplayName="E_v_Reserved_170">,
	E_v_Reserved_171                  <DisplayName="E_v_Reserved_171">,
	E_v_Reserved_172                  <DisplayName="E_v_Reserved_172">,
	E_v_Reserved_173                  <DisplayName="E_v_Reserved_173">,
	E_v_Reserved_174                  <DisplayName="E_v_Reserved_174">,
	E_v_Reserved_175                  <DisplayName="E_v_Reserved_175">,
	E_v_Reserved_176                  <DisplayName="E_v_Reserved_176">,
	E_v_Reserved_177                  <DisplayName="E_v_Reserved_177">,
	E_v_Reserved_178                  <DisplayName="E_v_Reserved_178">,
	E_v_Reserved_179                  <DisplayName="E_v_Reserved_179">,
	E_v_Reserved_180                  <DisplayName="E_v_Reserved_180">,
	E_v_Reserved_181                  <DisplayName="E_v_Reserved_181">,
	E_v_Reserved_182                  <DisplayName="E_v_Reserved_182">,
	E_v_Reserved_183                  <DisplayName="E_v_Reserved_183">,
	E_v_Reserved_184                  <DisplayName="E_v_Reserved_184">,
	E_v_Reserved_185                  <DisplayName="E_v_Reserved_185">,
	E_v_Reserved_186                  <DisplayName="E_v_Reserved_186">,
	E_v_Reserved_187                  <DisplayName="E_v_Reserved_187">,
	E_v_Reserved_188                  <DisplayName="E_v_Reserved_188">,
	E_v_Reserved_189                  <DisplayName="E_v_Reserved_189">,
	E_v_Reserved_190                  <DisplayName="E_v_Reserved_190">,
	E_v_Reserved_191                  <DisplayName="E_v_Reserved_191">,
	E_v_Reserved_192                  <DisplayName="E_v_Reserved_192">,
	E_v_Reserved_193                  <DisplayName="E_v_Reserved_193">,
	E_v_Reserved_194                  <DisplayName="E_v_Reserved_194">,
	E_v_Reserved_195                  <DisplayName="E_v_Reserved_195">,
	E_v_Reserved_196                  <DisplayName="E_v_Reserved_196">,
	E_v_Reserved_197                  <DisplayName="E_v_Reserved_197">,
	E_v_Reserved_198                  <DisplayName="E_v_Reserved_198">,
	E_v_Reserved_199                  <DisplayName="E_v_Reserved_199">,
	E_v_Reserved_200                  <DisplayName="E_v_Reserved_200">,
	E_v_Reserved_201                  <DisplayName="E_v_Reserved_201">,
	E_v_Reserved_202                  <DisplayName="E_v_Reserved_202">,
	E_v_Reserved_203                  <DisplayName="E_v_Reserved_203">,
	E_v_Reserved_204                  <DisplayName="E_v_Reserved_204">,
	E_v_Reserved_205                  <DisplayName="E_v_Reserved_205">,
	E_v_Reserved_206                  <DisplayName="E_v_Reserved_206">,
	E_v_Reserved_207                  <DisplayName="E_v_Reserved_207">,
	E_v_Reserved_208                  <DisplayName="E_v_Reserved_208">,
	E_v_Reserved_209                  <DisplayName="E_v_Reserved_209">,
	E_v_Reserved_210                  <DisplayName="E_v_Reserved_210">,
	E_v_Reserved_211                  <DisplayName="E_v_Reserved_211">,
	E_v_Reserved_212                  <DisplayName="E_v_Reserved_212">,
	E_v_Reserved_213                  <DisplayName="E_v_Reserved_213">,
	E_v_Reserved_214                  <DisplayName="E_v_Reserved_214">,
	E_v_Reserved_215                  <DisplayName="E_v_Reserved_215">,
	E_v_Reserved_216                  <DisplayName="E_v_Reserved_216">,
	E_v_Reserved_217                  <DisplayName="E_v_Reserved_217">,
	E_v_Reserved_218                  <DisplayName="E_v_Reserved_218">,
	E_v_Reserved_219                  <DisplayName="E_v_Reserved_219">,
	E_v_Reserved_220                  <DisplayName="E_v_Reserved_220">,
	E_v_Reserved_221                  <DisplayName="E_v_Reserved_221">,
	E_v_Reserved_222                  <DisplayName="E_v_Reserved_222">,
	E_v_Reserved_223                  <DisplayName="E_v_Reserved_223">,
	E_v_Reserved_224                  <DisplayName="E_v_Reserved_224">,
	E_v_Reserved_225                  <DisplayName="E_v_Reserved_225">,
	E_v_Reserved_226                  <DisplayName="E_v_Reserved_226">,
	E_v_Reserved_227                  <DisplayName="E_v_Reserved_227">,
	E_v_Reserved_228                  <DisplayName="E_v_Reserved_228">,
	E_v_Reserved_229                  <DisplayName="E_v_Reserved_229">,
	E_v_Reserved_230                  <DisplayName="E_v_Reserved_230">,
	E_v_Reserved_231                  <DisplayName="E_v_Reserved_231">,
	E_v_Reserved_232                  <DisplayName="E_v_Reserved_232">,
	E_v_Reserved_233                  <DisplayName="E_v_Reserved_233">,
	E_v_Reserved_234                  <DisplayName="E_v_Reserved_234">,
	E_v_Reserved_235                  <DisplayName="E_v_Reserved_235">,
	E_v_Reserved_236                  <DisplayName="E_v_Reserved_236">,
	E_v_Reserved_237                  <DisplayName="E_v_Reserved_237">,
	E_v_Reserved_238                  <DisplayName="E_v_Reserved_238">,
	E_v_Reserved_239                  <DisplayName="E_v_Reserved_239">,
	E_v_Reserved_240                  <DisplayName="E_v_Reserved_240">,
	E_v_Reserved_241                  <DisplayName="E_v_Reserved_241">,
	E_v_Reserved_242                  <DisplayName="E_v_Reserved_242">,
	E_v_Reserved_243                  <DisplayName="E_v_Reserved_243">,
	E_v_Reserved_244                  <DisplayName="E_v_Reserved_244">,
	E_v_Reserved_245                  <DisplayName="E_v_Reserved_245">,
	E_v_Reserved_246                  <DisplayName="E_v_Reserved_246">,
	E_v_Reserved_247                  <DisplayName="E_v_Reserved_247">,
	E_v_Reserved_248                  <DisplayName="E_v_Reserved_248">,
	E_v_Reserved_249                  <DisplayName="E_v_Reserved_249">,
	E_v_Reserved_250                  <DisplayName="E_v_Reserved_250">,
	E_v_Reserved_251                  <DisplayName="E_v_Reserved_251">,
	E_v_Reserved_252                  <DisplayName="E_v_Reserved_252">,
	E_v_Reserved_253                  <DisplayName="E_v_Reserved_253">,
	E_v_Reserved_254                  <DisplayName="E_v_Reserved_254">,
	E_v_MAX
};

enum EERoleLogoutType
{
	E_rlt_Invalid                     <DisplayName="E_rlt_Invalid">,
	E_rlt_ReturnLogin                 <DisplayName="E_rlt_ReturnLogin">,
	E_rlt_ReturnSelectRole            <DisplayName="E_rlt_ReturnSelectRole">,
	E_rlt_Disconnect                  <DisplayName="E_rlt_Disconnect">,
	E_rlt_Kickout                     <DisplayName="E_rlt_Kickout">,
	E_rlt_Total                       <DisplayName="E_rlt_Total">,
	E_rlt_MAX
};

enum EDUR_TYPE
{
	E_DURTYPE_NONE                    <DisplayName="E_DURTYPE_NONE">,
	E_DURTYPE_EQUIP                   <DisplayName="E_DURTYPE_EQUIP">,
	E_DURTYPE_STACK                   <DisplayName="E_DURTYPE_STACK">,
	E_DURTYPE_USE                     <DisplayName="E_DURTYPE_USE">,
	E_DURTYPE_DEPOSIT                 <DisplayName="E_DURTYPE_DEPOSIT">,
	E_DURTYPE_RECOVER                 <DisplayName="E_DURTYPE_RECOVER">,
	E_DURTYPE_PURITY                  <DisplayName="E_DURTYPE_PURITY">,
	E_DURTYPE_MAX
};

enum ESPELL_TARGET_RACE_TYPE
{
	E_sptot_None                      <DisplayName="E_sptot_None">,
	E_sptot_Player                    <DisplayName="E_sptot_Player">,
	E_sptot_Human                     <DisplayName="E_sptot_Human">,
	E_sptot_Humanlike                 <DisplayName="E_sptot_Humanlike">,
	E_sptot_Beast                     <DisplayName="E_sptot_Beast">,
	E_sptot_Insect                    <DisplayName="E_sptot_Insect">,
	E_sptot_Plant                     <DisplayName="E_sptot_Plant">,
	E_sptot_Undead                    <DisplayName="E_sptot_Undead">,
	E_sptot_Machine                   <DisplayName="E_sptot_Machine">,
	E_sptot_Dragon                    <DisplayName="E_sptot_Dragon">,
	E_sptot_Demon                     <DisplayName="E_sptot_Demon">,
	E_sptot_Elements                  <DisplayName="E_sptot_Elements">,
	E_sptot_Building                  <DisplayName="E_sptot_Building">,
	E_sptot_Total                     <DisplayName="E_sptot_Total">,
	E_sptot_MAX
};

enum EROLE_FACE_TYPE
{
	E_face_Invalid                    <DisplayName="E_face_Invalid">,
	E_face_Total                      <DisplayName="E_face_Total">,
	E_face_MAX
};

enum EBATTLE_FACTION
{
	E_bf_First                        <DisplayName="E_bf_First">,
	E_bf_Second                       <DisplayName="E_bf_Second">,
	E_bf_Thrid                        <DisplayName="E_bf_Thrid">,
	E_bf_Fourth                       <DisplayName="E_bf_Fourth">,
	E_bf_Total                        <DisplayName="E_bf_Total">,
	E_bf_MAX
};

enum ENPC_TRANSFORM_STATE
{
	E_npct_Invalid                    <DisplayName="E_npct_Invalid">,
	E_npct_Npc                        <DisplayName="E_npct_Npc">,
	E_npct_Pet                        <DisplayName="E_npct_Pet">,
	E_npct_Total                      <DisplayName="E_npct_Total">,
	E_npct_MAX
};

enum EGUILD_VIEW_TYPE
{
	E_GUILD_VIEW_Reserved_0           <DisplayName="E_GUILD_VIEW_Reserved_0">,
	E_GUILD_VIEW_UP                   <DisplayName="E_GUILD_VIEW_UP">,
	E_GUILD_VIEW_DOWN                 <DisplayName="E_GUILD_VIEW_DOWN">,
	E_GUILD_VIEW_MAX
};

enum EENUM_SCENE_VA
{
	E_est_Invalid                     <DisplayName="E_est_Invalid">,
	E_est_Static                      <DisplayName="E_est_Static">,
	E_est_Dungeon                     <DisplayName="E_est_Dungeon">,
	E_est_BattleField                 <DisplayName="E_est_BattleField">,
	E_est_Arena                       <DisplayName="E_est_Arena">,
	E_est_Line                        <DisplayName="E_est_Line">,
	E_est_Birth                       <DisplayName="E_est_Birth">,
	E_est_Guild                       <DisplayName="E_est_Guild">,
	E_est_GuildDungeon                <DisplayName="E_est_GuildDungeon">,
	E_est_Unique                      <DisplayName="E_est_Unique">,
	E_est_SoloDungeon                 <DisplayName="E_est_SoloDungeon">,
	E_est_Total                       <DisplayName="E_est_Total">,
	E_est_MAX
};

enum EGUILD_POSITION_RIGHT_FIELD
{
// max enum definition is over 254
	E_G_POS_RIGHT_NONE                <DisplayName="E_G_POS_RIGHT_NONE">,
	E_G_POS_RIGHT_INVITE              <DisplayName="E_G_POS_RIGHT_INVITE">,
	E_G_POS_RIGHT_RECRUIT             <DisplayName="E_G_POS_RIGHT_RECRUIT">,
	E_G_POS_RIGHT_Reserved_3          <DisplayName="E_G_POS_RIGHT_Reserved_3">,
	E_G_POS_RIGHT_FORBID_CHAT         <DisplayName="E_G_POS_RIGHT_FORBID_CHAT">,
	E_G_POS_RIGHT_Reserved_5          <DisplayName="E_G_POS_RIGHT_Reserved_5">,
	E_G_POS_RIGHT_Reserved_6          <DisplayName="E_G_POS_RIGHT_Reserved_6">,
	E_G_POS_RIGHT_Reserved_7          <DisplayName="E_G_POS_RIGHT_Reserved_7">,
	E_G_POS_RIGHT_NOTICE              <DisplayName="E_G_POS_RIGHT_NOTICE">,
	E_G_POS_RIGHT_Reserved_9          <DisplayName="E_G_POS_RIGHT_Reserved_9">,
	E_G_POS_RIGHT_Reserved_10         <DisplayName="E_G_POS_RIGHT_Reserved_10">,
	E_G_POS_RIGHT_Reserved_11         <DisplayName="E_G_POS_RIGHT_Reserved_11">,
	E_G_POS_RIGHT_Reserved_12         <DisplayName="E_G_POS_RIGHT_Reserved_12">,
	E_G_POS_RIGHT_Reserved_13         <DisplayName="E_G_POS_RIGHT_Reserved_13">,
	E_G_POS_RIGHT_Reserved_14         <DisplayName="E_G_POS_RIGHT_Reserved_14">,
	E_G_POS_RIGHT_Reserved_15         <DisplayName="E_G_POS_RIGHT_Reserved_15">,
	E_G_POS_RIGHT_SET_STORAGE_RIGHT   <DisplayName="E_G_POS_RIGHT_SET_STORAGE_RIGHT">,
	E_G_POS_RIGHT_Reserved_17         <DisplayName="E_G_POS_RIGHT_Reserved_17">,
	E_G_POS_RIGHT_Reserved_18         <DisplayName="E_G_POS_RIGHT_Reserved_18">,
	E_G_POS_RIGHT_Reserved_19         <DisplayName="E_G_POS_RIGHT_Reserved_19">,
	E_G_POS_RIGHT_Reserved_20         <DisplayName="E_G_POS_RIGHT_Reserved_20">,
	E_G_POS_RIGHT_Reserved_21         <DisplayName="E_G_POS_RIGHT_Reserved_21">,
	E_G_POS_RIGHT_Reserved_22         <DisplayName="E_G_POS_RIGHT_Reserved_22">,
	E_G_POS_RIGHT_Reserved_23         <DisplayName="E_G_POS_RIGHT_Reserved_23">,
	E_G_POS_RIGHT_Reserved_24         <DisplayName="E_G_POS_RIGHT_Reserved_24">,
	E_G_POS_RIGHT_Reserved_25         <DisplayName="E_G_POS_RIGHT_Reserved_25">,
	E_G_POS_RIGHT_Reserved_26         <DisplayName="E_G_POS_RIGHT_Reserved_26">,
	E_G_POS_RIGHT_Reserved_27         <DisplayName="E_G_POS_RIGHT_Reserved_27">,
	E_G_POS_RIGHT_Reserved_28         <DisplayName="E_G_POS_RIGHT_Reserved_28">,
	E_G_POS_RIGHT_Reserved_29         <DisplayName="E_G_POS_RIGHT_Reserved_29">,
	E_G_POS_RIGHT_Reserved_30         <DisplayName="E_G_POS_RIGHT_Reserved_30">,
	E_G_POS_RIGHT_Reserved_31         <DisplayName="E_G_POS_RIGHT_Reserved_31">,
	E_G_POS_RIGHT_OPEN_DUNGEON        <DisplayName="E_G_POS_RIGHT_OPEN_DUNGEON">,
	E_G_POS_RIGHT_Reserved_33         <DisplayName="E_G_POS_RIGHT_Reserved_33">,
	E_G_POS_RIGHT_Reserved_34         <DisplayName="E_G_POS_RIGHT_Reserved_34">,
	E_G_POS_RIGHT_Reserved_35         <DisplayName="E_G_POS_RIGHT_Reserved_35">,
	E_G_POS_RIGHT_Reserved_36         <DisplayName="E_G_POS_RIGHT_Reserved_36">,
	E_G_POS_RIGHT_Reserved_37         <DisplayName="E_G_POS_RIGHT_Reserved_37">,
	E_G_POS_RIGHT_Reserved_38         <DisplayName="E_G_POS_RIGHT_Reserved_38">,
	E_G_POS_RIGHT_Reserved_39         <DisplayName="E_G_POS_RIGHT_Reserved_39">,
	E_G_POS_RIGHT_Reserved_40         <DisplayName="E_G_POS_RIGHT_Reserved_40">,
	E_G_POS_RIGHT_Reserved_41         <DisplayName="E_G_POS_RIGHT_Reserved_41">,
	E_G_POS_RIGHT_Reserved_42         <DisplayName="E_G_POS_RIGHT_Reserved_42">,
	E_G_POS_RIGHT_Reserved_43         <DisplayName="E_G_POS_RIGHT_Reserved_43">,
	E_G_POS_RIGHT_Reserved_44         <DisplayName="E_G_POS_RIGHT_Reserved_44">,
	E_G_POS_RIGHT_Reserved_45         <DisplayName="E_G_POS_RIGHT_Reserved_45">,
	E_G_POS_RIGHT_Reserved_46         <DisplayName="E_G_POS_RIGHT_Reserved_46">,
	E_G_POS_RIGHT_Reserved_47         <DisplayName="E_G_POS_RIGHT_Reserved_47">,
	E_G_POS_RIGHT_Reserved_48         <DisplayName="E_G_POS_RIGHT_Reserved_48">,
	E_G_POS_RIGHT_Reserved_49         <DisplayName="E_G_POS_RIGHT_Reserved_49">,
	E_G_POS_RIGHT_Reserved_50         <DisplayName="E_G_POS_RIGHT_Reserved_50">,
	E_G_POS_RIGHT_Reserved_51         <DisplayName="E_G_POS_RIGHT_Reserved_51">,
	E_G_POS_RIGHT_Reserved_52         <DisplayName="E_G_POS_RIGHT_Reserved_52">,
	E_G_POS_RIGHT_Reserved_53         <DisplayName="E_G_POS_RIGHT_Reserved_53">,
	E_G_POS_RIGHT_Reserved_54         <DisplayName="E_G_POS_RIGHT_Reserved_54">,
	E_G_POS_RIGHT_Reserved_55         <DisplayName="E_G_POS_RIGHT_Reserved_55">,
	E_G_POS_RIGHT_Reserved_56         <DisplayName="E_G_POS_RIGHT_Reserved_56">,
	E_G_POS_RIGHT_Reserved_57         <DisplayName="E_G_POS_RIGHT_Reserved_57">,
	E_G_POS_RIGHT_Reserved_58         <DisplayName="E_G_POS_RIGHT_Reserved_58">,
	E_G_POS_RIGHT_Reserved_59         <DisplayName="E_G_POS_RIGHT_Reserved_59">,
	E_G_POS_RIGHT_Reserved_60         <DisplayName="E_G_POS_RIGHT_Reserved_60">,
	E_G_POS_RIGHT_Reserved_61         <DisplayName="E_G_POS_RIGHT_Reserved_61">,
	E_G_POS_RIGHT_Reserved_62         <DisplayName="E_G_POS_RIGHT_Reserved_62">,
	E_G_POS_RIGHT_Reserved_63         <DisplayName="E_G_POS_RIGHT_Reserved_63">,
	E_G_POS_RIGHT_UPGRADE_BUILDING    <DisplayName="E_G_POS_RIGHT_UPGRADE_BUILDING">,
	E_G_POS_RIGHT_Reserved_65         <DisplayName="E_G_POS_RIGHT_Reserved_65">,
	E_G_POS_RIGHT_Reserved_66         <DisplayName="E_G_POS_RIGHT_Reserved_66">,
	E_G_POS_RIGHT_Reserved_67         <DisplayName="E_G_POS_RIGHT_Reserved_67">,
	E_G_POS_RIGHT_Reserved_68         <DisplayName="E_G_POS_RIGHT_Reserved_68">,
	E_G_POS_RIGHT_Reserved_69         <DisplayName="E_G_POS_RIGHT_Reserved_69">,
	E_G_POS_RIGHT_Reserved_70         <DisplayName="E_G_POS_RIGHT_Reserved_70">,
	E_G_POS_RIGHT_Reserved_71         <DisplayName="E_G_POS_RIGHT_Reserved_71">,
	E_G_POS_RIGHT_Reserved_72         <DisplayName="E_G_POS_RIGHT_Reserved_72">,
	E_G_POS_RIGHT_Reserved_73         <DisplayName="E_G_POS_RIGHT_Reserved_73">,
	E_G_POS_RIGHT_Reserved_74         <DisplayName="E_G_POS_RIGHT_Reserved_74">,
	E_G_POS_RIGHT_Reserved_75         <DisplayName="E_G_POS_RIGHT_Reserved_75">,
	E_G_POS_RIGHT_Reserved_76         <DisplayName="E_G_POS_RIGHT_Reserved_76">,
	E_G_POS_RIGHT_Reserved_77         <DisplayName="E_G_POS_RIGHT_Reserved_77">,
	E_G_POS_RIGHT_Reserved_78         <DisplayName="E_G_POS_RIGHT_Reserved_78">,
	E_G_POS_RIGHT_Reserved_79         <DisplayName="E_G_POS_RIGHT_Reserved_79">,
	E_G_POS_RIGHT_Reserved_80         <DisplayName="E_G_POS_RIGHT_Reserved_80">,
	E_G_POS_RIGHT_Reserved_81         <DisplayName="E_G_POS_RIGHT_Reserved_81">,
	E_G_POS_RIGHT_Reserved_82         <DisplayName="E_G_POS_RIGHT_Reserved_82">,
	E_G_POS_RIGHT_Reserved_83         <DisplayName="E_G_POS_RIGHT_Reserved_83">,
	E_G_POS_RIGHT_Reserved_84         <DisplayName="E_G_POS_RIGHT_Reserved_84">,
	E_G_POS_RIGHT_Reserved_85         <DisplayName="E_G_POS_RIGHT_Reserved_85">,
	E_G_POS_RIGHT_Reserved_86         <DisplayName="E_G_POS_RIGHT_Reserved_86">,
	E_G_POS_RIGHT_Reserved_87         <DisplayName="E_G_POS_RIGHT_Reserved_87">,
	E_G_POS_RIGHT_Reserved_88         <DisplayName="E_G_POS_RIGHT_Reserved_88">,
	E_G_POS_RIGHT_Reserved_89         <DisplayName="E_G_POS_RIGHT_Reserved_89">,
	E_G_POS_RIGHT_Reserved_90         <DisplayName="E_G_POS_RIGHT_Reserved_90">,
	E_G_POS_RIGHT_Reserved_91         <DisplayName="E_G_POS_RIGHT_Reserved_91">,
	E_G_POS_RIGHT_Reserved_92         <DisplayName="E_G_POS_RIGHT_Reserved_92">,
	E_G_POS_RIGHT_Reserved_93         <DisplayName="E_G_POS_RIGHT_Reserved_93">,
	E_G_POS_RIGHT_Reserved_94         <DisplayName="E_G_POS_RIGHT_Reserved_94">,
	E_G_POS_RIGHT_Reserved_95         <DisplayName="E_G_POS_RIGHT_Reserved_95">,
	E_G_POS_RIGHT_Reserved_96         <DisplayName="E_G_POS_RIGHT_Reserved_96">,
	E_G_POS_RIGHT_Reserved_97         <DisplayName="E_G_POS_RIGHT_Reserved_97">,
	E_G_POS_RIGHT_Reserved_98         <DisplayName="E_G_POS_RIGHT_Reserved_98">,
	E_G_POS_RIGHT_Reserved_99         <DisplayName="E_G_POS_RIGHT_Reserved_99">,
	E_G_POS_RIGHT_Reserved_100        <DisplayName="E_G_POS_RIGHT_Reserved_100">,
	E_G_POS_RIGHT_Reserved_101        <DisplayName="E_G_POS_RIGHT_Reserved_101">,
	E_G_POS_RIGHT_Reserved_102        <DisplayName="E_G_POS_RIGHT_Reserved_102">,
	E_G_POS_RIGHT_Reserved_103        <DisplayName="E_G_POS_RIGHT_Reserved_103">,
	E_G_POS_RIGHT_Reserved_104        <DisplayName="E_G_POS_RIGHT_Reserved_104">,
	E_G_POS_RIGHT_Reserved_105        <DisplayName="E_G_POS_RIGHT_Reserved_105">,
	E_G_POS_RIGHT_Reserved_106        <DisplayName="E_G_POS_RIGHT_Reserved_106">,
	E_G_POS_RIGHT_Reserved_107        <DisplayName="E_G_POS_RIGHT_Reserved_107">,
	E_G_POS_RIGHT_Reserved_108        <DisplayName="E_G_POS_RIGHT_Reserved_108">,
	E_G_POS_RIGHT_Reserved_109        <DisplayName="E_G_POS_RIGHT_Reserved_109">,
	E_G_POS_RIGHT_Reserved_110        <DisplayName="E_G_POS_RIGHT_Reserved_110">,
	E_G_POS_RIGHT_Reserved_111        <DisplayName="E_G_POS_RIGHT_Reserved_111">,
	E_G_POS_RIGHT_Reserved_112        <DisplayName="E_G_POS_RIGHT_Reserved_112">,
	E_G_POS_RIGHT_Reserved_113        <DisplayName="E_G_POS_RIGHT_Reserved_113">,
	E_G_POS_RIGHT_Reserved_114        <DisplayName="E_G_POS_RIGHT_Reserved_114">,
	E_G_POS_RIGHT_Reserved_115        <DisplayName="E_G_POS_RIGHT_Reserved_115">,
	E_G_POS_RIGHT_Reserved_116        <DisplayName="E_G_POS_RIGHT_Reserved_116">,
	E_G_POS_RIGHT_Reserved_117        <DisplayName="E_G_POS_RIGHT_Reserved_117">,
	E_G_POS_RIGHT_Reserved_118        <DisplayName="E_G_POS_RIGHT_Reserved_118">,
	E_G_POS_RIGHT_Reserved_119        <DisplayName="E_G_POS_RIGHT_Reserved_119">,
	E_G_POS_RIGHT_Reserved_120        <DisplayName="E_G_POS_RIGHT_Reserved_120">,
	E_G_POS_RIGHT_Reserved_121        <DisplayName="E_G_POS_RIGHT_Reserved_121">,
	E_G_POS_RIGHT_Reserved_122        <DisplayName="E_G_POS_RIGHT_Reserved_122">,
	E_G_POS_RIGHT_Reserved_123        <DisplayName="E_G_POS_RIGHT_Reserved_123">,
	E_G_POS_RIGHT_Reserved_124        <DisplayName="E_G_POS_RIGHT_Reserved_124">,
	E_G_POS_RIGHT_Reserved_125        <DisplayName="E_G_POS_RIGHT_Reserved_125">,
	E_G_POS_RIGHT_Reserved_126        <DisplayName="E_G_POS_RIGHT_Reserved_126">,
	E_G_POS_RIGHT_Reserved_127        <DisplayName="E_G_POS_RIGHT_Reserved_127">,
	E_G_POS_RIGHT_UPGRADE_WELFARE     <DisplayName="E_G_POS_RIGHT_UPGRADE_WELFARE">,
	E_G_POS_RIGHT_Reserved_129        <DisplayName="E_G_POS_RIGHT_Reserved_129">,
	E_G_POS_RIGHT_Reserved_130        <DisplayName="E_G_POS_RIGHT_Reserved_130">,
	E_G_POS_RIGHT_Reserved_131        <DisplayName="E_G_POS_RIGHT_Reserved_131">,
	E_G_POS_RIGHT_Reserved_132        <DisplayName="E_G_POS_RIGHT_Reserved_132">,
	E_G_POS_RIGHT_Reserved_133        <DisplayName="E_G_POS_RIGHT_Reserved_133">,
	E_G_POS_RIGHT_Reserved_134        <DisplayName="E_G_POS_RIGHT_Reserved_134">,
	E_G_POS_RIGHT_Reserved_135        <DisplayName="E_G_POS_RIGHT_Reserved_135">,
	E_G_POS_RIGHT_Reserved_136        <DisplayName="E_G_POS_RIGHT_Reserved_136">,
	E_G_POS_RIGHT_Reserved_137        <DisplayName="E_G_POS_RIGHT_Reserved_137">,
	E_G_POS_RIGHT_Reserved_138        <DisplayName="E_G_POS_RIGHT_Reserved_138">,
	E_G_POS_RIGHT_Reserved_139        <DisplayName="E_G_POS_RIGHT_Reserved_139">,
	E_G_POS_RIGHT_Reserved_140        <DisplayName="E_G_POS_RIGHT_Reserved_140">,
	E_G_POS_RIGHT_Reserved_141        <DisplayName="E_G_POS_RIGHT_Reserved_141">,
	E_G_POS_RIGHT_Reserved_142        <DisplayName="E_G_POS_RIGHT_Reserved_142">,
	E_G_POS_RIGHT_Reserved_143        <DisplayName="E_G_POS_RIGHT_Reserved_143">,
	E_G_POS_RIGHT_Reserved_144        <DisplayName="E_G_POS_RIGHT_Reserved_144">,
	E_G_POS_RIGHT_Reserved_145        <DisplayName="E_G_POS_RIGHT_Reserved_145">,
	E_G_POS_RIGHT_Reserved_146        <DisplayName="E_G_POS_RIGHT_Reserved_146">,
	E_G_POS_RIGHT_Reserved_147        <DisplayName="E_G_POS_RIGHT_Reserved_147">,
	E_G_POS_RIGHT_Reserved_148        <DisplayName="E_G_POS_RIGHT_Reserved_148">,
	E_G_POS_RIGHT_Reserved_149        <DisplayName="E_G_POS_RIGHT_Reserved_149">,
	E_G_POS_RIGHT_Reserved_150        <DisplayName="E_G_POS_RIGHT_Reserved_150">,
	E_G_POS_RIGHT_Reserved_151        <DisplayName="E_G_POS_RIGHT_Reserved_151">,
	E_G_POS_RIGHT_Reserved_152        <DisplayName="E_G_POS_RIGHT_Reserved_152">,
	E_G_POS_RIGHT_Reserved_153        <DisplayName="E_G_POS_RIGHT_Reserved_153">,
	E_G_POS_RIGHT_Reserved_154        <DisplayName="E_G_POS_RIGHT_Reserved_154">,
	E_G_POS_RIGHT_Reserved_155        <DisplayName="E_G_POS_RIGHT_Reserved_155">,
	E_G_POS_RIGHT_Reserved_156        <DisplayName="E_G_POS_RIGHT_Reserved_156">,
	E_G_POS_RIGHT_Reserved_157        <DisplayName="E_G_POS_RIGHT_Reserved_157">,
	E_G_POS_RIGHT_Reserved_158        <DisplayName="E_G_POS_RIGHT_Reserved_158">,
	E_G_POS_RIGHT_Reserved_159        <DisplayName="E_G_POS_RIGHT_Reserved_159">,
	E_G_POS_RIGHT_Reserved_160        <DisplayName="E_G_POS_RIGHT_Reserved_160">,
	E_G_POS_RIGHT_Reserved_161        <DisplayName="E_G_POS_RIGHT_Reserved_161">,
	E_G_POS_RIGHT_Reserved_162        <DisplayName="E_G_POS_RIGHT_Reserved_162">,
	E_G_POS_RIGHT_Reserved_163        <DisplayName="E_G_POS_RIGHT_Reserved_163">,
	E_G_POS_RIGHT_Reserved_164        <DisplayName="E_G_POS_RIGHT_Reserved_164">,
	E_G_POS_RIGHT_Reserved_165        <DisplayName="E_G_POS_RIGHT_Reserved_165">,
	E_G_POS_RIGHT_Reserved_166        <DisplayName="E_G_POS_RIGHT_Reserved_166">,
	E_G_POS_RIGHT_Reserved_167        <DisplayName="E_G_POS_RIGHT_Reserved_167">,
	E_G_POS_RIGHT_Reserved_168        <DisplayName="E_G_POS_RIGHT_Reserved_168">,
	E_G_POS_RIGHT_Reserved_169        <DisplayName="E_G_POS_RIGHT_Reserved_169">,
	E_G_POS_RIGHT_Reserved_170        <DisplayName="E_G_POS_RIGHT_Reserved_170">,
	E_G_POS_RIGHT_Reserved_171        <DisplayName="E_G_POS_RIGHT_Reserved_171">,
	E_G_POS_RIGHT_Reserved_172        <DisplayName="E_G_POS_RIGHT_Reserved_172">,
	E_G_POS_RIGHT_Reserved_173        <DisplayName="E_G_POS_RIGHT_Reserved_173">,
	E_G_POS_RIGHT_Reserved_174        <DisplayName="E_G_POS_RIGHT_Reserved_174">,
	E_G_POS_RIGHT_Reserved_175        <DisplayName="E_G_POS_RIGHT_Reserved_175">,
	E_G_POS_RIGHT_Reserved_176        <DisplayName="E_G_POS_RIGHT_Reserved_176">,
	E_G_POS_RIGHT_Reserved_177        <DisplayName="E_G_POS_RIGHT_Reserved_177">,
	E_G_POS_RIGHT_Reserved_178        <DisplayName="E_G_POS_RIGHT_Reserved_178">,
	E_G_POS_RIGHT_Reserved_179        <DisplayName="E_G_POS_RIGHT_Reserved_179">,
	E_G_POS_RIGHT_Reserved_180        <DisplayName="E_G_POS_RIGHT_Reserved_180">,
	E_G_POS_RIGHT_Reserved_181        <DisplayName="E_G_POS_RIGHT_Reserved_181">,
	E_G_POS_RIGHT_Reserved_182        <DisplayName="E_G_POS_RIGHT_Reserved_182">,
	E_G_POS_RIGHT_Reserved_183        <DisplayName="E_G_POS_RIGHT_Reserved_183">,
	E_G_POS_RIGHT_Reserved_184        <DisplayName="E_G_POS_RIGHT_Reserved_184">,
	E_G_POS_RIGHT_Reserved_185        <DisplayName="E_G_POS_RIGHT_Reserved_185">,
	E_G_POS_RIGHT_Reserved_186        <DisplayName="E_G_POS_RIGHT_Reserved_186">,
	E_G_POS_RIGHT_Reserved_187        <DisplayName="E_G_POS_RIGHT_Reserved_187">,
	E_G_POS_RIGHT_Reserved_188        <DisplayName="E_G_POS_RIGHT_Reserved_188">,
	E_G_POS_RIGHT_Reserved_189        <DisplayName="E_G_POS_RIGHT_Reserved_189">,
	E_G_POS_RIGHT_Reserved_190        <DisplayName="E_G_POS_RIGHT_Reserved_190">,
	E_G_POS_RIGHT_Reserved_191        <DisplayName="E_G_POS_RIGHT_Reserved_191">,
	E_G_POS_RIGHT_Reserved_192        <DisplayName="E_G_POS_RIGHT_Reserved_192">,
	E_G_POS_RIGHT_Reserved_193        <DisplayName="E_G_POS_RIGHT_Reserved_193">,
	E_G_POS_RIGHT_Reserved_194        <DisplayName="E_G_POS_RIGHT_Reserved_194">,
	E_G_POS_RIGHT_Reserved_195        <DisplayName="E_G_POS_RIGHT_Reserved_195">,
	E_G_POS_RIGHT_Reserved_196        <DisplayName="E_G_POS_RIGHT_Reserved_196">,
	E_G_POS_RIGHT_Reserved_197        <DisplayName="E_G_POS_RIGHT_Reserved_197">,
	E_G_POS_RIGHT_Reserved_198        <DisplayName="E_G_POS_RIGHT_Reserved_198">,
	E_G_POS_RIGHT_Reserved_199        <DisplayName="E_G_POS_RIGHT_Reserved_199">,
	E_G_POS_RIGHT_Reserved_200        <DisplayName="E_G_POS_RIGHT_Reserved_200">,
	E_G_POS_RIGHT_Reserved_201        <DisplayName="E_G_POS_RIGHT_Reserved_201">,
	E_G_POS_RIGHT_Reserved_202        <DisplayName="E_G_POS_RIGHT_Reserved_202">,
	E_G_POS_RIGHT_Reserved_203        <DisplayName="E_G_POS_RIGHT_Reserved_203">,
	E_G_POS_RIGHT_Reserved_204        <DisplayName="E_G_POS_RIGHT_Reserved_204">,
	E_G_POS_RIGHT_Reserved_205        <DisplayName="E_G_POS_RIGHT_Reserved_205">,
	E_G_POS_RIGHT_Reserved_206        <DisplayName="E_G_POS_RIGHT_Reserved_206">,
	E_G_POS_RIGHT_Reserved_207        <DisplayName="E_G_POS_RIGHT_Reserved_207">,
	E_G_POS_RIGHT_Reserved_208        <DisplayName="E_G_POS_RIGHT_Reserved_208">,
	E_G_POS_RIGHT_Reserved_209        <DisplayName="E_G_POS_RIGHT_Reserved_209">,
	E_G_POS_RIGHT_Reserved_210        <DisplayName="E_G_POS_RIGHT_Reserved_210">,
	E_G_POS_RIGHT_Reserved_211        <DisplayName="E_G_POS_RIGHT_Reserved_211">,
	E_G_POS_RIGHT_Reserved_212        <DisplayName="E_G_POS_RIGHT_Reserved_212">,
	E_G_POS_RIGHT_Reserved_213        <DisplayName="E_G_POS_RIGHT_Reserved_213">,
	E_G_POS_RIGHT_Reserved_214        <DisplayName="E_G_POS_RIGHT_Reserved_214">,
	E_G_POS_RIGHT_Reserved_215        <DisplayName="E_G_POS_RIGHT_Reserved_215">,
	E_G_POS_RIGHT_Reserved_216        <DisplayName="E_G_POS_RIGHT_Reserved_216">,
	E_G_POS_RIGHT_Reserved_217        <DisplayName="E_G_POS_RIGHT_Reserved_217">,
	E_G_POS_RIGHT_Reserved_218        <DisplayName="E_G_POS_RIGHT_Reserved_218">,
	E_G_POS_RIGHT_Reserved_219        <DisplayName="E_G_POS_RIGHT_Reserved_219">,
	E_G_POS_RIGHT_Reserved_220        <DisplayName="E_G_POS_RIGHT_Reserved_220">,
	E_G_POS_RIGHT_Reserved_221        <DisplayName="E_G_POS_RIGHT_Reserved_221">,
	E_G_POS_RIGHT_Reserved_222        <DisplayName="E_G_POS_RIGHT_Reserved_222">,
	E_G_POS_RIGHT_Reserved_223        <DisplayName="E_G_POS_RIGHT_Reserved_223">,
	E_G_POS_RIGHT_Reserved_224        <DisplayName="E_G_POS_RIGHT_Reserved_224">,
	E_G_POS_RIGHT_Reserved_225        <DisplayName="E_G_POS_RIGHT_Reserved_225">,
	E_G_POS_RIGHT_Reserved_226        <DisplayName="E_G_POS_RIGHT_Reserved_226">,
	E_G_POS_RIGHT_Reserved_227        <DisplayName="E_G_POS_RIGHT_Reserved_227">,
	E_G_POS_RIGHT_Reserved_228        <DisplayName="E_G_POS_RIGHT_Reserved_228">,
	E_G_POS_RIGHT_Reserved_229        <DisplayName="E_G_POS_RIGHT_Reserved_229">,
	E_G_POS_RIGHT_Reserved_230        <DisplayName="E_G_POS_RIGHT_Reserved_230">,
	E_G_POS_RIGHT_Reserved_231        <DisplayName="E_G_POS_RIGHT_Reserved_231">,
	E_G_POS_RIGHT_Reserved_232        <DisplayName="E_G_POS_RIGHT_Reserved_232">,
	E_G_POS_RIGHT_Reserved_233        <DisplayName="E_G_POS_RIGHT_Reserved_233">,
	E_G_POS_RIGHT_Reserved_234        <DisplayName="E_G_POS_RIGHT_Reserved_234">,
	E_G_POS_RIGHT_Reserved_235        <DisplayName="E_G_POS_RIGHT_Reserved_235">,
	E_G_POS_RIGHT_Reserved_236        <DisplayName="E_G_POS_RIGHT_Reserved_236">,
	E_G_POS_RIGHT_Reserved_237        <DisplayName="E_G_POS_RIGHT_Reserved_237">,
	E_G_POS_RIGHT_Reserved_238        <DisplayName="E_G_POS_RIGHT_Reserved_238">,
	E_G_POS_RIGHT_Reserved_239        <DisplayName="E_G_POS_RIGHT_Reserved_239">,
	E_G_POS_RIGHT_Reserved_240        <DisplayName="E_G_POS_RIGHT_Reserved_240">,
	E_G_POS_RIGHT_Reserved_241        <DisplayName="E_G_POS_RIGHT_Reserved_241">,
	E_G_POS_RIGHT_Reserved_242        <DisplayName="E_G_POS_RIGHT_Reserved_242">,
	E_G_POS_RIGHT_Reserved_243        <DisplayName="E_G_POS_RIGHT_Reserved_243">,
	E_G_POS_RIGHT_Reserved_244        <DisplayName="E_G_POS_RIGHT_Reserved_244">,
	E_G_POS_RIGHT_Reserved_245        <DisplayName="E_G_POS_RIGHT_Reserved_245">,
	E_G_POS_RIGHT_Reserved_246        <DisplayName="E_G_POS_RIGHT_Reserved_246">,
	E_G_POS_RIGHT_Reserved_247        <DisplayName="E_G_POS_RIGHT_Reserved_247">,
	E_G_POS_RIGHT_Reserved_248        <DisplayName="E_G_POS_RIGHT_Reserved_248">,
	E_G_POS_RIGHT_Reserved_249        <DisplayName="E_G_POS_RIGHT_Reserved_249">,
	E_G_POS_RIGHT_Reserved_250        <DisplayName="E_G_POS_RIGHT_Reserved_250">,
	E_G_POS_RIGHT_Reserved_251        <DisplayName="E_G_POS_RIGHT_Reserved_251">,
	E_G_POS_RIGHT_Reserved_252        <DisplayName="E_G_POS_RIGHT_Reserved_252">,
	E_G_POS_RIGHT_Reserved_253        <DisplayName="E_G_POS_RIGHT_Reserved_253">,
	E_G_POS_RIGHT_Reserved_254        <DisplayName="E_G_POS_RIGHT_Reserved_254">,
	E_G_POS_RIGHT_MAX
};

enum EACHIEVEMENT_REWARD_TYPE
{
	E_adt_Invalid                     <DisplayName="E_adt_Invalid">,
	E_adt_Title                       <DisplayName="E_adt_Title">,
	E_adt_Item                        <DisplayName="E_adt_Item">,
	E_adt_Exp                         <DisplayName="E_adt_Exp">,
	E_adt_Total                       <DisplayName="E_adt_Total">,
	E_adt_MAX
};

enum EDEPOSIT_ITEM_SLOT
{
	E_DEPOSIT_SOUL_SLOT               <DisplayName="E_DEPOSIT_SOUL_SLOT">,
	E_DEPOSIT_EXP_SLOT                <DisplayName="E_DEPOSIT_EXP_SLOT">,
	E_DEPOSIT_ITEM_SLOT_TOTAL         <DisplayName="E_DEPOSIT_ITEM_SLOT_TOTAL">,
	E_DEPOSIT_MAX
};

enum ESPELL_QTE_TYPE
{
	E_spqte_None                      <DisplayName="E_spqte_None">,
	E_spqte_Counter                   <DisplayName="E_spqte_Counter">,
	E_spqte_Chase                     <DisplayName="E_spqte_Chase">,
	E_spqte_Terminate                 <DisplayName="E_spqte_Terminate">,
	E_spqte_Total                     <DisplayName="E_spqte_Total">,
	E_spqte_MAX
};

enum EQUEUE_STATE
{
	E_qs_NotInQueue                   <DisplayName="E_qs_NotInQueue">,
	E_qs_InQueue                      <DisplayName="E_qs_InQueue">,
	E_qs_QueueConfirm                 <DisplayName="E_qs_QueueConfirm">,
	E_qs_Total                        <DisplayName="E_qs_Total">,
	E_qs_MAX
};

enum ESPELL_PREPARE_TYPE
{
	E_sppt_Channeled                  <DisplayName="E_sppt_Channeled">,
	E_sppt_Prepared                   <DisplayName="E_sppt_Prepared">,
	E_sppt_Accumulated                <DisplayName="E_sppt_Accumulated">,
	E_sppt_Total                      <DisplayName="E_sppt_Total">,
	E_sppt_MAX
};

enum EMST_SYNC_GS_DATA_TYPE
{
	E_MST_SYNC_Reserved_0             <DisplayName="E_MST_SYNC_Reserved_0">,
	E_MST_SYNC_GS_CD                  <DisplayName="E_MST_SYNC_GS_CD">,
	E_MST_SYNC_GS_TWITTER_CD          <DisplayName="E_MST_SYNC_GS_TWITTER_CD">,
	E_MST_SYNC_PUNISH_EXP             <DisplayName="E_MST_SYNC_PUNISH_EXP">,
	E_MST_SYNC_MASTER_EXP_DEC         <DisplayName="E_MST_SYNC_MASTER_EXP_DEC">,
	E_MST_SYNC_MAX
};

enum EMST_SUCCESS_RSP_TYPE
{
	E_MST_Reserved_0                  <DisplayName="E_MST_Reserved_0">,
	E_MST_APPLY_RECRUIT_SUCC          <DisplayName="E_MST_APPLY_RECRUIT_SUCC">,
	E_MST_CANCEL_RECRUIT_SUCC         <DisplayName="E_MST_CANCEL_RECRUIT_SUCC">,
	E_MST_MAX
};

enum EMASTER_STATE
{
	E_MASTER_NONE                     <DisplayName="E_MASTER_NONE">,
	E_MASTER_STUDENT                  <DisplayName="E_MASTER_STUDENT">,
	E_MASTER_TEACHER                  <DisplayName="E_MASTER_TEACHER">,
	E_MASTER_BECOME_TEACHER           <DisplayName="E_MASTER_BECOME_TEACHER">,
	E_MASTER_MAX
};

enum EOBJECT_TYPE
{
	E_ot_Role                         <DisplayName="E_ot_Role">,
	E_ot_Npc                          <DisplayName="E_ot_Npc">,
	E_ot_Hero                         <DisplayName="E_ot_Hero">,
	E_ot_Doodad                       <DisplayName="E_ot_Doodad">,
	E_ot_Missile                      <DisplayName="E_ot_Missile">,
	E_ot_Drop                         <DisplayName="E_ot_Drop">,
	E_ot_Guild                        <DisplayName="E_ot_Guild">,
	E_ot_Team                         <DisplayName="E_ot_Team">,
	E_ot_BattleTeam                   <DisplayName="E_ot_BattleTeam">,
	E_ot_Channel                      <DisplayName="E_ot_Channel">,
	E_ot_Trumpet                      <DisplayName="E_ot_Trumpet">,
	E_ot_Total                        <DisplayName="E_ot_Total">,
	E_ot_MAX
};

enum EEGambleType
{
	E_egt_EquipWeapon                 <DisplayName="E_egt_EquipWeapon">,
	E_egt_EquipHat                    <DisplayName="E_egt_EquipHat">,
	E_egt_EquipShoulderArmor          <DisplayName="E_egt_EquipShoulderArmor">,
	E_egt_EquipChestArmor             <DisplayName="E_egt_EquipChestArmor">,
	E_egt_EquipWristArmor             <DisplayName="E_egt_EquipWristArmor">,
	E_egt_EquipBelt                   <DisplayName="E_egt_EquipBelt">,
	E_egt_EquipShoes                  <DisplayName="E_egt_EquipShoes">,
	E_egt_EquipCloak                  <DisplayName="E_egt_EquipCloak">,
	E_egt_EquipNecklace               <DisplayName="E_egt_EquipNecklace">,
	E_egt_EquipRing                   <DisplayName="E_egt_EquipRing">,
	E_egt_EquipBracelet               <DisplayName="E_egt_EquipBracelet">,
	E_egt_SpellBook                   <DisplayName="E_egt_SpellBook">,
	E_egt_RuneWarrior                 <DisplayName="E_egt_RuneWarrior">,
	E_egt_RuneMage                    <DisplayName="E_egt_RuneMage">,
	E_egt_RuneRogue                   <DisplayName="E_egt_RuneRogue">,
	E_egt_RuneArcher                  <DisplayName="E_egt_RuneArcher">,
	E_egt_RuneTaoist                  <DisplayName="E_egt_RuneTaoist">,
	E_egt_Reserved01                  <DisplayName="E_egt_Reserved01">,
	E_egt_Potion                      <DisplayName="E_egt_Potion">,
	E_egt_Reserved03                  <DisplayName="E_egt_Reserved03">,
	E_egt_Reserved04                  <DisplayName="E_egt_Reserved04">,
	E_egt_Reserved05                  <DisplayName="E_egt_Reserved05">,
	E_egt_Reserved06                  <DisplayName="E_egt_Reserved06">,
	E_egt_Reserved07                  <DisplayName="E_egt_Reserved07">,
	E_egt_Reserved08                  <DisplayName="E_egt_Reserved08">,
	E_egt_Reserved09                  <DisplayName="E_egt_Reserved09">,
	E_egt_Reserved10                  <DisplayName="E_egt_Reserved10">,
	E_egt_Reserved11                  <DisplayName="E_egt_Reserved11">,
	E_egt_Reserved12                  <DisplayName="E_egt_Reserved12">,
	E_egt_Reserved13                  <DisplayName="E_egt_Reserved13">,
	E_egt_Reserved14                  <DisplayName="E_egt_Reserved14">,
	E_egt_Reserved15                  <DisplayName="E_egt_Reserved15">,
	E_egt_Total                       <DisplayName="E_egt_Total">,
	E_egt_MAX
};

enum ENS_ERROR_CODE_DEF
{
	E_errNs_UnknownError              <DisplayName="E_errNs_UnknownError">,
	E_errNs_Success                   <DisplayName="E_errNs_Success">,
	E_errNs_NameDuplicated            <DisplayName="E_errNs_NameDuplicated">,
	E_errNs_NameDBSysErr              <DisplayName="E_errNs_NameDBSysErr">,
	E_errNs_NameUnknown               <DisplayName="E_errNs_NameUnknown">,
	E_errNs_TooManyUser               <DisplayName="E_errNs_TooManyUser">,
	E_errNs_MAX
};

enum EROLE_PK_RESULT
{
	E_rpr_Invalid                     <DisplayName="E_rpr_Invalid">,
	E_rpr_Normal                      <DisplayName="E_rpr_Normal">,
	E_rpr_RedName                     <DisplayName="E_rpr_RedName">,
	E_rpr_CounterStrike               <DisplayName="E_rpr_CounterStrike">,
	E_rpr_OpponentGuildMember         <DisplayName="E_rpr_OpponentGuildMember">,
	E_rpr_LuckyKill                   <DisplayName="E_rpr_LuckyKill">,
	E_rpr_NoPunishment                <DisplayName="E_rpr_NoPunishment">,
	E_rpr_Total                       <DisplayName="E_rpr_Total">,
	E_rpr_MAX
};

enum EACHIEVEMENT_VARIABLE_DEF
{
// conflicted enum definition avdForSceneBegin(1)
	E_avd_Invalid                     <DisplayName="E_avd_Invalid">,
	E_avd_SceneTempID                 <DisplayName="E_avd_SceneTempID">,
	E_avd_SceneID                     <DisplayName="E_avd_SceneID">,
	E_avd_Reward_1                    <DisplayName="E_avd_Reward_1">,
	E_avd_Reward_2                    <DisplayName="E_avd_Reward_2">,
	E_avd_Reward_3                    <DisplayName="E_avd_Reward_3">,
	E_avd_DestoryItemCount            <DisplayName="E_avd_DestoryItemCount">,
	E_avd_PickMoneyTotal              <DisplayName="E_avd_PickMoneyTotal">,
	E_avd_QuestMoneyTotal             <DisplayName="E_avd_QuestMoneyTotal">,
	E_avd_SaleMoneyTotal              <DisplayName="E_avd_SaleMoneyTotal">,
	E_avd_RelieveMoneyTotal           <DisplayName="E_avd_RelieveMoneyTotal">,
	E_avd_ReliveRMBTotal              <DisplayName="E_avd_ReliveRMBTotal">,
	E_avd_RepairMoneyTotal            <DisplayName="E_avd_RepairMoneyTotal">,
	E_avd_RepairMoneyMax              <DisplayName="E_avd_RepairMoneyMax">,
	E_avd_KillNpcTotalCount           <DisplayName="E_avd_KillNpcTotalCount">,
	E_avd_SelfDieCount                <DisplayName="E_avd_SelfDieCount">,
	E_avd_PVPKilledByPlayer           <DisplayName="E_avd_PVPKilledByPlayer">,
	E_avd_PVPKilledByNpc              <DisplayName="E_avd_PVPKilledByNpc">,
	E_avd_KillWarriorCount            <DisplayName="E_avd_KillWarriorCount">,
	E_avd_KillMageCount               <DisplayName="E_avd_KillMageCount">,
	E_avd_KillRogueCount              <DisplayName="E_avd_KillRogueCount">,
	E_avd_KillArcherCount             <DisplayName="E_avd_KillArcherCount">,
	E_avd_KillPkValuePlayerCount      <DisplayName="E_avd_KillPkValuePlayerCount">,
	E_avd_KillWithNoGuilty            <DisplayName="E_avd_KillWithNoGuilty">,
	E_avd_EnterDungeonSceneCount      <DisplayName="E_avd_EnterDungeonSceneCount">,
	E_avd_CompleteStoryQuestCount     <DisplayName="E_avd_CompleteStoryQuestCount">,
	E_avd_CompleteNormalQuestCount    <DisplayName="E_avd_CompleteNormalQuestCount">,
	E_avd_CompleteCircleQuestCount    <DisplayName="E_avd_CompleteCircleQuestCount">,
	E_avd_CompleteRewardQuestCount    <DisplayName="E_avd_CompleteRewardQuestCount">,
	E_avd_CompleteEmegencyQuestCount  <DisplayName="E_avd_CompleteEmegencyQuestCount">,
	E_avd_BrushRewardQuestCount       <DisplayName="E_avd_BrushRewardQuestCount">,
	E_avd_BuyRewardQuestCompleteCount <DisplayName="E_avd_BuyRewardQuestCompleteCount">,
	E_avd_AbandonQuestCount           <DisplayName="E_avd_AbandonQuestCount">,
	E_avd_MaxHPHit                    <DisplayName="E_avd_MaxHPHit">,
	E_avd_MaxHPBeHit                  <DisplayName="E_avd_MaxHPBeHit">,
	E_avd_UseItemCount_Useable        <DisplayName="E_avd_UseItemCount_Useable">,
	E_avd_UseItemCount_SpellBoke      <DisplayName="E_avd_UseItemCount_SpellBoke">,
	E_avd_OnlineTime                  <DisplayName="E_avd_OnlineTime">,
	E_avd_LeaveMaster                 <DisplayName="E_avd_LeaveMaster">,
	E_avd_TotalRefineTime             <DisplayName="E_avd_TotalRefineTime">,
	E_avd_RefineFailTime              <DisplayName="E_avd_RefineFailTime">,
	E_avd_FirstBloodCount             <DisplayName="E_avd_FirstBloodCount">,
	E_avd_IdentifyItemCount           <DisplayName="E_avd_IdentifyItemCount">,
	E_avd_DropItemBeKilled            <DisplayName="E_avd_DropItemBeKilled">,
	E_avd_StatisticEnd                <DisplayName="E_avd_StatisticEnd">,
	E_avd_QuestFlytoZone              <DisplayName="E_avd_QuestFlytoZone">,
	E_avd_LastOfflineTime             <DisplayName="E_avd_LastOfflineTime">,
	E_avd_RewardQuestCompleteCount    <DisplayName="E_avd_RewardQuestCompleteCount">,
	E_avd_KillPlayerLevelDuarion1     <DisplayName="E_avd_KillPlayerLevelDuarion1">,
	E_avd_KillPlayerLevelDuarion2     <DisplayName="E_avd_KillPlayerLevelDuarion2">,
	E_avd_KillPlayerLevelDuarion3     <DisplayName="E_avd_KillPlayerLevelDuarion3">,
	E_avd_EquipDecompose              <DisplayName="E_avd_EquipDecompose">,
	E_avd_EquipCompose                <DisplayName="E_avd_EquipCompose">,
	E_avd_TeamAuctionCount            <DisplayName="E_avd_TeamAuctionCount">,
	E_avd_Vip                         <DisplayName="E_avd_Vip">,
	E_avd_SubscribeVipMonth           <DisplayName="E_avd_SubscribeVipMonth">,
	E_avd_JoinTeamCount               <DisplayName="E_avd_JoinTeamCount">,
	E_avd_ComposeOrangeItem           <DisplayName="E_avd_ComposeOrangeItem">,
	E_avd_ComposePurpleItem           <DisplayName="E_avd_ComposePurpleItem">,
	E_avd_DecomposeOrangeItem         <DisplayName="E_avd_DecomposeOrangeItem">,
	E_avd_DecomposePurpleItem         <DisplayName="E_avd_DecomposePurpleItem">,
	E_avd_ComposeItemCount            <DisplayName="E_avd_ComposeItemCount">,
	E_avd_DeComposeItemCount          <DisplayName="E_avd_DeComposeItemCount">,
	E_avd_AuctionSellCount            <DisplayName="E_avd_AuctionSellCount">,
	E_avd_AuctionBid                  <DisplayName="E_avd_AuctionBid">,
	E_avd_MaxConsecutiveKillNpcCount  <DisplayName="E_avd_MaxConsecutiveKillNpcCount">,
	E_avd_MaxMomentaryKillNpcCount    <DisplayName="E_avd_MaxMomentaryKillNpcCount">,
	E_avd_MaxConsecutiveKillPlayerCount <DisplayName="E_avd_MaxConsecutiveKillPlayerCount">,
	E_avd_RealMoneyToItemCount        <DisplayName="E_avd_RealMoneyToItemCount">,
	E_avd_RealMoneyToMoney02Count     <DisplayName="E_avd_RealMoneyToMoney02Count">,
	E_avd_SpiritLevelUP               <DisplayName="E_avd_SpiritLevelUP">,
	E_avd_KillNpcNormal               <DisplayName="E_avd_KillNpcNormal">,
	E_avd_KillNpcElit                 <DisplayName="E_avd_KillNpcElit">,
	E_avd_KillNpcBoss                 <DisplayName="E_avd_KillNpcBoss">,
	E_avd_CompleteGuildQuest          <DisplayName="E_avd_CompleteGuildQuest">,
	E_avd_CompleteAdventureQuest      <DisplayName="E_avd_CompleteAdventureQuest">,
	E_avd_ItemRepairCount             <DisplayName="E_avd_ItemRepairCount">,
	E_avd_RuneAddCount                <DisplayName="E_avd_RuneAddCount">,
	E_avd_CompleteQuestTotalCount     <DisplayName="E_avd_CompleteQuestTotalCount">,
	E_avd_KillNpcRare                 <DisplayName="E_avd_KillNpcRare">,
	E_avd_KillNpcCorpse               <DisplayName="E_avd_KillNpcCorpse">,
	E_avd_LevelOnlineTime             <DisplayName="E_avd_LevelOnlineTime">,
	E_avd_RankPow                     <DisplayName="E_avd_RankPow">,
	E_avd_RankLevel                   <DisplayName="E_avd_RankLevel">,
	E_avd_RankMoney                   <DisplayName="E_avd_RankMoney">,
	E_avd_RankPkValue                 <DisplayName="E_avd_RankPkValue">,
	E_avd_RankMasterExp               <DisplayName="E_avd_RankMasterExp">,
	E_avd_RankAchievementPts          <DisplayName="E_avd_RankAchievementPts">,
	E_avd_ExeciseTime                 <DisplayName="E_avd_ExeciseTime">,
	E_avd_Total                       <DisplayName="E_avd_Total">,
	E_avd_MAX
};

enum EPET_STATE
{
	E_ps_Invalid                      <DisplayName="E_ps_Invalid">,
	E_ps_Idle                         <DisplayName="E_ps_Idle">,
	E_ps_Fight                        <DisplayName="E_ps_Fight">,
	E_ps_Total                        <DisplayName="E_ps_Total">,
	E_ps_MAX
};

enum EGUILD_ENTER_MAP_TYPE
{
	E_GUILD_ENTER_Reserved_0          <DisplayName="E_GUILD_ENTER_Reserved_0">,
	E_GUILD_ENTER_OWNER               <DisplayName="E_GUILD_ENTER_OWNER">,
	E_GUILD_ENTER_ALLIANCE            <DisplayName="E_GUILD_ENTER_ALLIANCE">,
	E_GUILD_ENTER_MAX
};

enum EGUILD_POSITION_TYPE
{
	E_GUILD_POSITION_NONE             <DisplayName="E_GUILD_POSITION_NONE">,
	E_GUILD_POSITION_LEADER           <DisplayName="E_GUILD_POSITION_LEADER">,
	E_GUILD_POSITION_SUBLEADER        <DisplayName="E_GUILD_POSITION_SUBLEADER">,
	E_GUILD_POSITION_ELDER            <DisplayName="E_GUILD_POSITION_ELDER">,
	E_GUILD_POSITION_BISHOP           <DisplayName="E_GUILD_POSITION_BISHOP">,
	E_GUILD_POSITION_DEACON           <DisplayName="E_GUILD_POSITION_DEACON">,
	E_GUILD_POSITION_SUBDEACON        <DisplayName="E_GUILD_POSITION_SUBDEACON">,
	E_GUILD_POSITION_CIVILIAN         <DisplayName="E_GUILD_POSITION_CIVILIAN">,
	E_GUILD_POSITION_TOTAL            <DisplayName="E_GUILD_POSITION_TOTAL">,
	E_GUILD_POSITION_MAX
};

enum EQUEST_TYPE
{
	E_qt_Trunk                        <DisplayName="E_qt_Trunk">,
	E_qt_Branch                       <DisplayName="E_qt_Branch">,
	E_qt_Daily                        <DisplayName="E_qt_Daily">,
	E_qt_Reward                       <DisplayName="E_qt_Reward">,
	E_qt_Circle                       <DisplayName="E_qt_Circle">,
	E_qt_Activity                     <DisplayName="E_qt_Activity">,
	E_qt_Guild                        <DisplayName="E_qt_Guild">,
	E_qt_Master                       <DisplayName="E_qt_Master">,
	E_qt_Wedding                      <DisplayName="E_qt_Wedding">,
	E_qt_Adventure                    <DisplayName="E_qt_Adventure">,
	E_qt_Guide                        <DisplayName="E_qt_Guide">,
	E_qt_Emegency                     <DisplayName="E_qt_Emegency">,
	E_qt_Total                        <DisplayName="E_qt_Total">,
	E_qt_MAX
};

enum EENUM_COMBAT_STAT_ENTRY
{
	E_escs_DeathCount                 <DisplayName="E_escs_DeathCount">,
	E_escs_RoleKillCount              <DisplayName="E_escs_RoleKillCount">,
	E_escs_NpcKillCount               <DisplayName="E_escs_NpcKillCount">,
	E_escs_EliteNpcKillCount          <DisplayName="E_escs_EliteNpcKillCount">,
	E_escs_BossKillCount              <DisplayName="E_escs_BossKillCount">,
	E_escs_DoodadKillCount            <DisplayName="E_escs_DoodadKillCount">,
	E_escs_TotalHeal                  <DisplayName="E_escs_TotalHeal">,
	E_escs_TotalDamage                <DisplayName="E_escs_TotalDamage">,
	E_escs_PhyAtkCritCount            <DisplayName="E_escs_PhyAtkCritCount">,
	E_escs_MagAtkCritCount            <DisplayName="E_escs_MagAtkCritCount">,
	E_escs_Score1                     <DisplayName="E_escs_Score1">,
	E_escs_Score2                     <DisplayName="E_escs_Score2">,
	E_escs_TotalScore                 <DisplayName="E_escs_TotalScore">,
	E_escs_Total                      <DisplayName="E_escs_Total">,
	E_escs_MAX
};

enum EEStallState
{
	E_ess_NotOpen                     <DisplayName="E_ess_NotOpen">,
	E_ess_Preparing                   <DisplayName="E_ess_Preparing">,
	E_ess_Open                        <DisplayName="E_ess_Open">,
	E_ess_MAX
};

enum ESABAC_STATE
{
	E_sab_NotEnroll                   <DisplayName="E_sab_NotEnroll">,
	E_sab_Enrolled                    <DisplayName="E_sab_Enrolled">,
	E_sab_Occupied                    <DisplayName="E_sab_Occupied">,
	E_sab_MAX
};

enum ETEAM_BID_STATE
{
	E_etbs_Invalid                    <DisplayName="E_etbs_Invalid">,
	E_etbs_Bidding                    <DisplayName="E_etbs_Bidding">,
	E_etbs_Abandon                    <DisplayName="E_etbs_Abandon">,
	E_etbs_Total                      <DisplayName="E_etbs_Total">,
	E_etbs_MAX
};

enum EAURA_DISPLAY_GROUP
{
// conflicted enum definition adgBegin(0)
	E_adg_GroupA                      <DisplayName="E_adg_GroupA">,
	E_adg_GroupB                      <DisplayName="E_adg_GroupB">,
	E_adg_GroupC                      <DisplayName="E_adg_GroupC">,
	E_adg_GroupD                      <DisplayName="E_adg_GroupD">,
	E_adg_GroupE                      <DisplayName="E_adg_GroupE">,
	E_adg_Total                       <DisplayName="E_adg_Total">,
	E_adg_MAX
};

enum ETEAM_ACTIVITY_ACTION_TYPE
{
	E_etaat_Invalid                   <DisplayName="E_etaat_Invalid">,
	E_etaat_Activity                  <DisplayName="E_etaat_Activity">,
	E_etaat_Dungeon                   <DisplayName="E_etaat_Dungeon">,
	E_etaat_Total                     <DisplayName="E_etaat_Total">,
	E_etaat_MAX
};

enum ETEAM_MEMBER_STATE
{
	E_etsO_nline                      <DisplayName="E_etsO_nline">,
	E_etsO_ffline                     <DisplayName="E_etsO_ffline">,
	E_etsO_MAX
};

enum ETEAM_ANSWER_TYPE
{
	E_etat_Invite                     <DisplayName="E_etat_Invite">,
	E_etat_Enter                      <DisplayName="E_etat_Enter">,
	E_etat_MAX
};

enum EROLE_FACTION_RELATION
{
	E_rfr_Default                     <DisplayName="E_rfr_Default">,
	E_rfr_Friend                      <DisplayName="E_rfr_Friend">,
	E_rfr_Enemy                       <DisplayName="E_rfr_Enemy">,
	E_rfr_Total                       <DisplayName="E_rfr_Total">,
	E_rfr_MAX
};

enum EGUILD_WAR_STAGE
{
	E_GUILD_WAR_STAGE_NONE            <DisplayName="E_GUILD_WAR_STAGE_NONE">,
	E_GUILD_WAR_STAGE_DECLARE         <DisplayName="E_GUILD_WAR_STAGE_DECLARE">,
	E_GUILD_WAR_STAGE_BEGIN           <DisplayName="E_GUILD_WAR_STAGE_BEGIN">,
	E_GUILD_WAR_STAGE_END             <DisplayName="E_GUILD_WAR_STAGE_END">,
	E_GUILD_WAR_STAGE_MAX
};

enum ESERVER_VARIABLE_TYPE
{
	E_esvt_Invalid                    <DisplayName="E_esvt_Invalid">,
	E_esvt_ItemDropLimitType0         <DisplayName="E_esvt_ItemDropLimitType0">,
	E_esvt_ItemDropLimitType0RefreshTime <DisplayName="E_esvt_ItemDropLimitType0RefreshTime">,
	E_esvt_Rune                       <DisplayName="E_esvt_Rune">,
	E_esvt_RuneRefreshTime            <DisplayName="E_esvt_RuneRefreshTime">,
	E_esvt_ItemDropLimitType1         <DisplayName="E_esvt_ItemDropLimitType1">,
	E_esvt_ItemDropLimitType1RefreshTime <DisplayName="E_esvt_ItemDropLimitType1RefreshTime">,
	E_esvt_ItemDropLimitType2         <DisplayName="E_esvt_ItemDropLimitType2">,
	E_esvt_ItemDropLimitType2RefreshTime <DisplayName="E_esvt_ItemDropLimitType2RefreshTime">,
	E_esvt_WorldGambleCount           <DisplayName="E_esvt_WorldGambleCount">,
	E_esvt_PreviousWorldGambleCount   <DisplayName="E_esvt_PreviousWorldGambleCount">,
	E_esvt_Arena                      <DisplayName="E_esvt_Arena">,
	E_esvt_Guild                      <DisplayName="E_esvt_Guild">,
	E_esvt_Total                      <DisplayName="E_esvt_Total">,
	E_esvt_MAX
};

enum EEBulletEffectType
{
// conflicted enum definition betBegin(0)
	E_bet_Damage                      <DisplayName="E_bet_Damage">,
	E_bet_Heal                        <DisplayName="E_bet_Heal">,
	E_bet_None                        <DisplayName="E_bet_None">,
	E_bet_Total                       <DisplayName="E_bet_Total">,
	E_bet_MAX
};

enum ETEAM_ANSWER_RESULT
{
	E_etar_Agree                      <DisplayName="E_etar_Agree">,
	E_etar_DisAgree                   <DisplayName="E_etar_DisAgree">,
	E_etar_MAX
};

enum ESPELL_CONTROL_TAG
{
	E_spctag_NoMove                   <DisplayName="E_spctag_NoMove">,
	E_spctag_NoCast                   <DisplayName="E_spctag_NoCast">,
	E_spctag_Total                    <DisplayName="E_spctag_Total">,
	E_spctag_MAX
};

enum EMASTER_CONG_TYPE
{
	E_MASTER_CONG_BEGIN               <DisplayName="E_MASTER_CONG_BEGIN">,
	E_MASTER_CONG_BY_FREE             <DisplayName="E_MASTER_CONG_BY_FREE">,
	E_MASTER_CONG_BY_MONEY            <DisplayName="E_MASTER_CONG_BY_MONEY">,
	E_MASTER_CONG_BY_GOLD             <DisplayName="E_MASTER_CONG_BY_GOLD">,
	E_MASTER_CONG_END                 <DisplayName="E_MASTER_CONG_END">,
	E_MASTER_CONG_MAX
};

enum EETradeActionType
{
	E_TRADEACTION_ASK                 <DisplayName="E_TRADEACTION_ASK">,
	E_TRADEACTION_ACCEPT              <DisplayName="E_TRADEACTION_ACCEPT">,
	E_TRADEACTION_PUT_ITEM            <DisplayName="E_TRADEACTION_PUT_ITEM">,
	E_TRADEACTION_PUT_MONEY           <DisplayName="E_TRADEACTION_PUT_MONEY">,
	E_TRADEACTION_LOCK                <DisplayName="E_TRADEACTION_LOCK">,
	E_TRADEACTION_CONFIRM             <DisplayName="E_TRADEACTION_CONFIRM">,
	E_TRADEACTION_UNLOCK              <DisplayName="E_TRADEACTION_UNLOCK">,
	E_TRADEACTION_CANCEL              <DisplayName="E_TRADEACTION_CANCEL">,
	E_TRADEACTION_COMMIT_FAIL_ROLE_MONEY <DisplayName="E_TRADEACTION_COMMIT_FAIL_ROLE_MONEY">,
	E_TRADEACTION_COMMIT_FAIL_TARGET_MONEY <DisplayName="E_TRADEACTION_COMMIT_FAIL_TARGET_MONEY">,
	E_TRADEACTION_COMMIT_FAIL_ROLE_BAG <DisplayName="E_TRADEACTION_COMMIT_FAIL_ROLE_BAG">,
	E_TRADEACTION_COMMIT_FAIL_TARGET_BAG <DisplayName="E_TRADEACTION_COMMIT_FAIL_TARGET_BAG">,
	E_TRADEACTION_COMMIT_SUCCESS      <DisplayName="E_TRADEACTION_COMMIT_SUCCESS">,
	E_TRADEACTION_COMMIT_FAIL_ROLE_OVERWEIGHT <DisplayName="E_TRADEACTION_COMMIT_FAIL_ROLE_OVERWEIGHT">,
	E_TRADEACTION_COMMIT_FAIL_TARGET_OVERWEIGHT <DisplayName="E_TRADEACTION_COMMIT_FAIL_TARGET_OVERWEIGHT">,
	E_TRADEACTION_TOTAL               <DisplayName="E_TRADEACTION_TOTAL">,
	E_TRADEACTION_MAX
};

enum EGUILD_LEAVE_TYPE
{
	E_GUILD_LEAVE_Reserved_0          <DisplayName="E_GUILD_LEAVE_Reserved_0">,
	E_GUILD_LEAVE_ACTIVE              <DisplayName="E_GUILD_LEAVE_ACTIVE">,
	E_GUILD_LEAVE_PASSIVE             <DisplayName="E_GUILD_LEAVE_PASSIVE">,
	E_GUILD_LEAVE_MAX
};

enum ENPC_STATE
{
	E_npc_Invalid                     <DisplayName="E_npc_Invalid">,
	E_npc_Spawn                       <DisplayName="E_npc_Spawn">,
	E_npc_Alive                       <DisplayName="E_npc_Alive">,
	E_npc_Corpse                      <DisplayName="E_npc_Corpse">,
	E_npc_Respawn                     <DisplayName="E_npc_Respawn">,
	E_npc_Revive                      <DisplayName="E_npc_Revive">,
	E_npc_Total                       <DisplayName="E_npc_Total">,
	E_npc_MAX
};

enum EPET_BORN_TYPE
{
	E_petBorn_Invalid                 <DisplayName="E_petBorn_Invalid">,
	E_petBorn_Holy                    <DisplayName="E_petBorn_Holy">,
	E_petBorn_Skeleton                <DisplayName="E_petBorn_Skeleton">,
	E_petBorn_ByMage                  <DisplayName="E_petBorn_ByMage">,
	E_petBorn_Total                   <DisplayName="E_petBorn_Total">,
	E_petBorn_MAX
};

enum EITEM_COMBAT_TYPE
{
	E_ITEMCOMBAT_BOTH                 <DisplayName="E_ITEMCOMBAT_BOTH">,
	E_ITEMCOMBAT_IN                   <DisplayName="E_ITEMCOMBAT_IN">,
	E_ITEMCOMBAT_OUT                  <DisplayName="E_ITEMCOMBAT_OUT">,
	E_ITEMCOMBAT_MAX
};

enum ESCENE_OBJECT_DISAPPEAR_TYPE
{
	E_sodt_Invalid                    <DisplayName="E_sodt_Invalid">,
	E_sodt_LeaveScene                 <DisplayName="E_sodt_LeaveScene">,
	E_sodt_OutOfRange                 <DisplayName="E_sodt_OutOfRange">,
	E_sodt_Total                      <DisplayName="E_sodt_Total">,
	E_sodt_MAX
};

enum ECHAR_STIFF_BITS
{
	E_cstf_Freeze                     <DisplayName="E_cstf_Freeze">,
	E_cstf_KnockBack                  <DisplayName="E_cstf_KnockBack">,
	E_cstf_Silent                     <DisplayName="E_cstf_Silent">,
	E_cstf_Total                      <DisplayName="E_cstf_Total">,
	E_cstf_MAX
};

enum ECHAR_CC_FLAG
{
	E_ccl_Stun                        <DisplayName="E_ccl_Stun">,
	E_ccl_Fear                        <DisplayName="E_ccl_Fear">,
	E_ccl_Fury                        <DisplayName="E_ccl_Fury">,
	E_ccl_Dazzle                      <DisplayName="E_ccl_Dazzle">,
	E_ccl_Slowed                      <DisplayName="E_ccl_Slowed">,
	E_ccl_KnockBack                   <DisplayName="E_ccl_KnockBack">,
	E_ccl_Freeze                      <DisplayName="E_ccl_Freeze">,
	E_ccl_Seduced                     <DisplayName="E_ccl_Seduced">,
	E_ccl_Invincible                  <DisplayName="E_ccl_Invincible">,
	E_ccl_Leech                       <DisplayName="E_ccl_Leech">,
	E_ccl_TransportationRing          <DisplayName="E_ccl_TransportationRing">,
	E_ccl_ProbeNecklace               <DisplayName="E_ccl_ProbeNecklace">,
	E_ccl_MemoSuit                    <DisplayName="E_ccl_MemoSuit">,
	E_ccl_Invisible                   <DisplayName="E_ccl_Invisible">,
	E_ccl_Total                       <DisplayName="E_ccl_Total">,
	E_ccl_MAX
};

enum ESERVER_POINT_TYPE
{
	E_spt_Invalid                     <DisplayName="E_spt_Invalid">,
	E_spt_Custom                      <DisplayName="E_spt_Custom">,
	E_spt_Revive                      <DisplayName="E_spt_Revive">,
	E_spt_Replay                      <DisplayName="E_spt_Replay">,
	E_spt_Total                       <DisplayName="E_spt_Total">,
	E_spt_MAX
};

enum EMONEY_TYPE
{
	E_MONEYTYPE_01                    <DisplayName="E_MONEYTYPE_01">,
	E_MONEYTYPE_02                    <DisplayName="E_MONEYTYPE_02">,
	E_MONEYTYPE_03                    <DisplayName="E_MONEYTYPE_03">,
	E_MONEYTYPE_04                    <DisplayName="E_MONEYTYPE_04">,
	E_MONEYTYPE_FATIGUE               <DisplayName="E_MONEYTYPE_FATIGUE">,
	E_MONEYTYPE_SOUL_PTS              <DisplayName="E_MONEYTYPE_SOUL_PTS">,
	E_MONEYTYPE_MASTER_EXP            <DisplayName="E_MONEYTYPE_MASTER_EXP">,
	E_MONEYTYPE_AIR                   <DisplayName="E_MONEYTYPE_AIR">,
	E_MONEYTYPE_BF                    <DisplayName="E_MONEYTYPE_BF">,
	E_MONEYTYPE_GUILD                 <DisplayName="E_MONEYTYPE_GUILD">,
	E_MONEYTYPE_SPELL_PTS             <DisplayName="E_MONEYTYPE_SPELL_PTS">,
	E_MONEYTYPE_MORAL                 <DisplayName="E_MONEYTYPE_MORAL">,
	E_MONEYTYPE_ARENA                 <DisplayName="E_MONEYTYPE_ARENA">,
	E_MONEYTYPE_ACHIEVEMENTPTS        <DisplayName="E_MONEYTYPE_ACHIEVEMENTPTS">,
	E_MONEYTYPE_COUNT                 <DisplayName="E_MONEYTYPE_COUNT">,
	E_MONEYTYPE_MAX
};

enum EEBlackMarketLimitType
{
	E_ebmlt_None                      <DisplayName="E_ebmlt_None">,
	E_ebmlt_Role                      <DisplayName="E_ebmlt_Role">,
	E_ebmlt_Server                    <DisplayName="E_ebmlt_Server">,
	E_ebmlt_MAX
};

enum EAURA_STACK_TYPE
{
	E_aust_None                       <DisplayName="E_aust_None">,
	E_aust_Substitute                 <DisplayName="E_aust_Substitute">,
	E_aust_Stack                      <DisplayName="E_aust_Stack">,
	E_aust_Coexist                    <DisplayName="E_aust_Coexist">,
	E_aust_Total                      <DisplayName="E_aust_Total">,
	E_aust_MAX
};

enum ESOCKET_TYPE
{
	E_SOCKET_NONE                     <DisplayName="E_SOCKET_NONE">,
	E_SOCKET_RED                      <DisplayName="E_SOCKET_RED">,
	E_SOCKET_YELLOW                   <DisplayName="E_SOCKET_YELLOW">,
	E_SOCKET_BLUE                     <DisplayName="E_SOCKET_BLUE">,
	E_SOCKET_GREEN                    <DisplayName="E_SOCKET_GREEN">,
	E_SOCKET_PURPLE                   <DisplayName="E_SOCKET_PURPLE">,
	E_SOCKET_COLORFUL                 <DisplayName="E_SOCKET_COLORFUL">,
	E_SOCKET_TYPE_COUNT               <DisplayName="E_SOCKET_TYPE_COUNT">,
	E_SOCKET_MAX
};

enum ECONTAINER_SLOT
{
	E_CONSLOT_EQUIPPED                <DisplayName="E_CONSLOT_EQUIPPED">,
	E_CONSLOT_DEFAULT                 <DisplayName="E_CONSLOT_DEFAULT">,
	E_CONSLOT_BANK                    <DisplayName="E_CONSLOT_BANK">,
	E_CONSLOT_HERO_EQUIPPED           <DisplayName="E_CONSLOT_HERO_EQUIPPED">,
	E_CONSLOT_HERO_EQUIPMENT          <DisplayName="E_CONSLOT_HERO_EQUIPMENT">,
	E_CONSLOT_DEPOSIT                 <DisplayName="E_CONSLOT_DEPOSIT">,
	E_CONSLOT_RECOVER                 <DisplayName="E_CONSLOT_RECOVER">,
	E_CONSLOT_RESOURCE                <DisplayName="E_CONSLOT_RESOURCE">,
	E_CONSLOT_COUNT                   <DisplayName="E_CONSLOT_COUNT">,
	E_CONSLOT_P2P_MY                  <DisplayName="E_CONSLOT_P2P_MY">,
	E_CONSLOT_P2P_TG                  <DisplayName="E_CONSLOT_P2P_TG">,
	E_CONSLOT_STALL_SETTING           <DisplayName="E_CONSLOT_STALL_SETTING">,
	E_CONSLOT_STALL_BUY               <DisplayName="E_CONSLOT_STALL_BUY">,
	E_CONSLOT_PROFILE                 <DisplayName="E_CONSLOT_PROFILE">,
	E_CONSLOT_GUILD_BANK              <DisplayName="E_CONSLOT_GUILD_BANK">,
	E_CONSLOT_Reserved_15             <DisplayName="E_CONSLOT_Reserved_15">,
	E_CONSLOT_Reserved_16             <DisplayName="E_CONSLOT_Reserved_16">,
	E_CONSLOT_Reserved_17             <DisplayName="E_CONSLOT_Reserved_17">,
	E_CONSLOT_Reserved_18             <DisplayName="E_CONSLOT_Reserved_18">,
	E_CONSLOT_Reserved_19             <DisplayName="E_CONSLOT_Reserved_19">,
	E_CONSLOT_Reserved_20             <DisplayName="E_CONSLOT_Reserved_20">,
	E_CONSLOT_Reserved_21             <DisplayName="E_CONSLOT_Reserved_21">,
	E_CONSLOT_Reserved_22             <DisplayName="E_CONSLOT_Reserved_22">,
	E_CONSLOT_Reserved_23             <DisplayName="E_CONSLOT_Reserved_23">,
	E_CONSLOT_Reserved_24             <DisplayName="E_CONSLOT_Reserved_24">,
	E_CONSLOT_Reserved_25             <DisplayName="E_CONSLOT_Reserved_25">,
	E_CONSLOT_Reserved_26             <DisplayName="E_CONSLOT_Reserved_26">,
	E_CONSLOT_Reserved_27             <DisplayName="E_CONSLOT_Reserved_27">,
	E_CONSLOT_Reserved_28             <DisplayName="E_CONSLOT_Reserved_28">,
	E_CONSLOT_Reserved_29             <DisplayName="E_CONSLOT_Reserved_29">,
	E_CONSLOT_Reserved_30             <DisplayName="E_CONSLOT_Reserved_30">,
	E_CONSLOT_Reserved_31             <DisplayName="E_CONSLOT_Reserved_31">,
	E_CONSLOT_Reserved_32             <DisplayName="E_CONSLOT_Reserved_32">,
	E_CONSLOT_Reserved_33             <DisplayName="E_CONSLOT_Reserved_33">,
	E_CONSLOT_Reserved_34             <DisplayName="E_CONSLOT_Reserved_34">,
	E_CONSLOT_Reserved_35             <DisplayName="E_CONSLOT_Reserved_35">,
	E_CONSLOT_Reserved_36             <DisplayName="E_CONSLOT_Reserved_36">,
	E_CONSLOT_Reserved_37             <DisplayName="E_CONSLOT_Reserved_37">,
	E_CONSLOT_Reserved_38             <DisplayName="E_CONSLOT_Reserved_38">,
	E_CONSLOT_Reserved_39             <DisplayName="E_CONSLOT_Reserved_39">,
	E_CONSLOT_Reserved_40             <DisplayName="E_CONSLOT_Reserved_40">,
	E_CONSLOT_Reserved_41             <DisplayName="E_CONSLOT_Reserved_41">,
	E_CONSLOT_Reserved_42             <DisplayName="E_CONSLOT_Reserved_42">,
	E_CONSLOT_Reserved_43             <DisplayName="E_CONSLOT_Reserved_43">,
	E_CONSLOT_Reserved_44             <DisplayName="E_CONSLOT_Reserved_44">,
	E_CONSLOT_Reserved_45             <DisplayName="E_CONSLOT_Reserved_45">,
	E_CONSLOT_Reserved_46             <DisplayName="E_CONSLOT_Reserved_46">,
	E_CONSLOT_Reserved_47             <DisplayName="E_CONSLOT_Reserved_47">,
	E_CONSLOT_Reserved_48             <DisplayName="E_CONSLOT_Reserved_48">,
	E_CONSLOT_Reserved_49             <DisplayName="E_CONSLOT_Reserved_49">,
	E_CONSLOT_Reserved_50             <DisplayName="E_CONSLOT_Reserved_50">,
	E_CONSLOT_Reserved_51             <DisplayName="E_CONSLOT_Reserved_51">,
	E_CONSLOT_Reserved_52             <DisplayName="E_CONSLOT_Reserved_52">,
	E_CONSLOT_Reserved_53             <DisplayName="E_CONSLOT_Reserved_53">,
	E_CONSLOT_Reserved_54             <DisplayName="E_CONSLOT_Reserved_54">,
	E_CONSLOT_Reserved_55             <DisplayName="E_CONSLOT_Reserved_55">,
	E_CONSLOT_Reserved_56             <DisplayName="E_CONSLOT_Reserved_56">,
	E_CONSLOT_Reserved_57             <DisplayName="E_CONSLOT_Reserved_57">,
	E_CONSLOT_Reserved_58             <DisplayName="E_CONSLOT_Reserved_58">,
	E_CONSLOT_Reserved_59             <DisplayName="E_CONSLOT_Reserved_59">,
	E_CONSLOT_Reserved_60             <DisplayName="E_CONSLOT_Reserved_60">,
	E_CONSLOT_Reserved_61             <DisplayName="E_CONSLOT_Reserved_61">,
	E_CONSLOT_Reserved_62             <DisplayName="E_CONSLOT_Reserved_62">,
	E_CONSLOT_Reserved_63             <DisplayName="E_CONSLOT_Reserved_63">,
	E_CONSLOT_Reserved_64             <DisplayName="E_CONSLOT_Reserved_64">,
	E_CONSLOT_Reserved_65             <DisplayName="E_CONSLOT_Reserved_65">,
	E_CONSLOT_Reserved_66             <DisplayName="E_CONSLOT_Reserved_66">,
	E_CONSLOT_Reserved_67             <DisplayName="E_CONSLOT_Reserved_67">,
	E_CONSLOT_Reserved_68             <DisplayName="E_CONSLOT_Reserved_68">,
	E_CONSLOT_Reserved_69             <DisplayName="E_CONSLOT_Reserved_69">,
	E_CONSLOT_Reserved_70             <DisplayName="E_CONSLOT_Reserved_70">,
	E_CONSLOT_Reserved_71             <DisplayName="E_CONSLOT_Reserved_71">,
	E_CONSLOT_Reserved_72             <DisplayName="E_CONSLOT_Reserved_72">,
	E_CONSLOT_Reserved_73             <DisplayName="E_CONSLOT_Reserved_73">,
	E_CONSLOT_Reserved_74             <DisplayName="E_CONSLOT_Reserved_74">,
	E_CONSLOT_Reserved_75             <DisplayName="E_CONSLOT_Reserved_75">,
	E_CONSLOT_Reserved_76             <DisplayName="E_CONSLOT_Reserved_76">,
	E_CONSLOT_Reserved_77             <DisplayName="E_CONSLOT_Reserved_77">,
	E_CONSLOT_Reserved_78             <DisplayName="E_CONSLOT_Reserved_78">,
	E_CONSLOT_Reserved_79             <DisplayName="E_CONSLOT_Reserved_79">,
	E_CONSLOT_Reserved_80             <DisplayName="E_CONSLOT_Reserved_80">,
	E_CONSLOT_Reserved_81             <DisplayName="E_CONSLOT_Reserved_81">,
	E_CONSLOT_Reserved_82             <DisplayName="E_CONSLOT_Reserved_82">,
	E_CONSLOT_Reserved_83             <DisplayName="E_CONSLOT_Reserved_83">,
	E_CONSLOT_Reserved_84             <DisplayName="E_CONSLOT_Reserved_84">,
	E_CONSLOT_Reserved_85             <DisplayName="E_CONSLOT_Reserved_85">,
	E_CONSLOT_Reserved_86             <DisplayName="E_CONSLOT_Reserved_86">,
	E_CONSLOT_Reserved_87             <DisplayName="E_CONSLOT_Reserved_87">,
	E_CONSLOT_Reserved_88             <DisplayName="E_CONSLOT_Reserved_88">,
	E_CONSLOT_Reserved_89             <DisplayName="E_CONSLOT_Reserved_89">,
	E_CONSLOT_Reserved_90             <DisplayName="E_CONSLOT_Reserved_90">,
	E_CONSLOT_Reserved_91             <DisplayName="E_CONSLOT_Reserved_91">,
	E_CONSLOT_Reserved_92             <DisplayName="E_CONSLOT_Reserved_92">,
	E_CONSLOT_Reserved_93             <DisplayName="E_CONSLOT_Reserved_93">,
	E_CONSLOT_Reserved_94             <DisplayName="E_CONSLOT_Reserved_94">,
	E_CONSLOT_Reserved_95             <DisplayName="E_CONSLOT_Reserved_95">,
	E_CONSLOT_Reserved_96             <DisplayName="E_CONSLOT_Reserved_96">,
	E_CONSLOT_Reserved_97             <DisplayName="E_CONSLOT_Reserved_97">,
	E_CONSLOT_Reserved_98             <DisplayName="E_CONSLOT_Reserved_98">,
	E_CONSLOT_Reserved_99             <DisplayName="E_CONSLOT_Reserved_99">,
	E_CONSLOT_WEAPON_LEVEL_UP         <DisplayName="E_CONSLOT_WEAPON_LEVEL_UP">,
	E_CONSLOT_MAX
};

enum EPK_DROP_TYPE
{
	E_PKDROP_BY_CHANCE                <DisplayName="E_PKDROP_BY_CHANCE">,
	E_PKDROP_NOT_DROP                 <DisplayName="E_PKDROP_NOT_DROP">,
	E_PKDROP_MUST_DROP                <DisplayName="E_PKDROP_MUST_DROP">,
	E_PKDROP_MAX
};

enum ESPELL_CAST_TYPE
{
	E_spct_Passive                    <DisplayName="E_spct_Passive">,
	E_spct_Active                     <DisplayName="E_spct_Active">,
	E_spct_SubSpell                   <DisplayName="E_spct_SubSpell">,
	E_spct_Directly                   <DisplayName="E_spct_Directly">,
	E_spct_Switch                     <DisplayName="E_spct_Switch">,
	E_spct_Total                      <DisplayName="E_spct_Total">,
	E_spct_MAX
};

enum ECHAR_SPELL_STATE
{
	E_csps_Idle                       <DisplayName="E_csps_Idle">,
	E_csps_Channeling                 <DisplayName="E_csps_Channeling">,
	E_csps_Preparing                  <DisplayName="E_csps_Preparing">,
	E_csps_PreAccumulating            <DisplayName="E_csps_PreAccumulating">,
	E_csps_Accumulating               <DisplayName="E_csps_Accumulating">,
	E_csps_Breaking                   <DisplayName="E_csps_Breaking">,
	E_csps_Opening                    <DisplayName="E_csps_Opening">,
	E_csps_Hard                       <DisplayName="E_csps_Hard">,
	E_csps_Total                      <DisplayName="E_csps_Total">,
	E_csps_MAX
};

enum EBIND_TYPE
{
	E_BINDTYPE_NONE                   <DisplayName="E_BINDTYPE_NONE">,
	E_BINDTYPE_BOU                    <DisplayName="E_BINDTYPE_BOU">,
	E_BINDTYPE_BOE                    <DisplayName="E_BINDTYPE_BOE">,
	E_BINDTYPE_BOP                    <DisplayName="E_BINDTYPE_BOP">,
	E_BINDTYPE_BOT                    <DisplayName="E_BINDTYPE_BOT">,
	E_BINDTYPE_Reserved_5             <DisplayName="E_BINDTYPE_Reserved_5">,
	E_BINDTYPE_Reserved_6             <DisplayName="E_BINDTYPE_Reserved_6">,
	E_BINDTYPE_Reserved_7             <DisplayName="E_BINDTYPE_Reserved_7">,
	E_BINDTYPE_Reserved_8             <DisplayName="E_BINDTYPE_Reserved_8">,
	E_BINDTYPE_Reserved_9             <DisplayName="E_BINDTYPE_Reserved_9">,
	E_BINDTYPE_BINDED                 <DisplayName="E_BINDTYPE_BINDED">,
	E_BINDTYPE_MAX
};

enum EAURA_CATEGORY
{
	E_ac_None                         <DisplayName="E_ac_None">,
	E_ac_Positive                     <DisplayName="E_ac_Positive">,
	E_ac_Negative                     <DisplayName="E_ac_Negative">,
	E_ac_Total                        <DisplayName="E_ac_Total">,
	E_ac_MAX
};

enum ECHAR_FLAG
{
	E_cf_NotInCombat                  <DisplayName="E_cf_NotInCombat">,
	E_cf_InCombat                     <DisplayName="E_cf_InCombat">,
	E_cf_Counter                      <DisplayName="E_cf_Counter">,
	E_cf_OnMount                      <DisplayName="E_cf_OnMount">,
	E_cf_CannotBeThrown               <DisplayName="E_cf_CannotBeThrown">,
	E_cf_TempUndead                   <DisplayName="E_cf_TempUndead">,
	E_cf_ImmunePhyAtk                 <DisplayName="E_cf_ImmunePhyAtk">,
	E_cf_ImmuleMagAtk                 <DisplayName="E_cf_ImmuleMagAtk">,
	E_cf_CanNotBeSelectedAsTarget     <DisplayName="E_cf_CanNotBeSelectedAsTarget">,
	E_cf_BTDenyDie                    <DisplayName="E_cf_BTDenyDie">,
	E_cf_BTHasFlee                    <DisplayName="E_cf_BTHasFlee">,
	E_cf_AuraInvalid                  <DisplayName="E_cf_AuraInvalid">,
	E_cf_CombatPoseOn                 <DisplayName="E_cf_CombatPoseOn">,
	E_cf_ImmunePassiveMove            <DisplayName="E_cf_ImmunePassiveMove">,
	E_cf_Fury                         <DisplayName="E_cf_Fury">,
	E_cf_DamageStatistics             <DisplayName="E_cf_DamageStatistics">,
	E_cf_NeverBeHit                   <DisplayName="E_cf_NeverBeHit">,
	E_cf_Total                        <DisplayName="E_cf_Total">,
	E_cf_MAX
};

enum ECHAT_CHANNEL_TYPE
{
	E_cct_MAX
};

enum EGUILD_FLAG_FIELD
{
	E_GUILD_FLAG_IS_CHAT_FORBIDDEN_Reserved_0 <DisplayName="E_GUILD_FLAG_IS_CHAT_FORBIDDEN_Reserved_0">,
	E_GUILD_FLAG_IS_CHAT_FORBIDDEN_   <DisplayName="E_GUILD_FLAG_IS_CHAT_FORBIDDEN_">,
	E_GUILD_FLAG_IS_CHAT_FORBIDDEN_MAX
};

enum EQUEST_TIME_TYPE
{
	E_qtt_NoReset                     <DisplayName="E_qtt_NoReset">,
	E_qtt_Day                         <DisplayName="E_qtt_Day">,
	E_qtt_Week                        <DisplayName="E_qtt_Week">,
	E_qtt_Total                       <DisplayName="E_qtt_Total">,
	E_qtt_MAX
};

enum EGUILD_ATTR_ENUM
{
// max enum definition is over 254
	E_GUILD_ATTR_Reserved_0           <DisplayName="E_GUILD_ATTR_Reserved_0">,
	E_GUILD_ATTR_Reserved_1           <DisplayName="E_GUILD_ATTR_Reserved_1">,
	E_GUILD_ATTR_Reserved_2           <DisplayName="E_GUILD_ATTR_Reserved_2">,
	E_GUILD_ATTR_Reserved_3           <DisplayName="E_GUILD_ATTR_Reserved_3">,
	E_GUILD_ATTR_Reserved_4           <DisplayName="E_GUILD_ATTR_Reserved_4">,
	E_GUILD_ATTR_Reserved_5           <DisplayName="E_GUILD_ATTR_Reserved_5">,
	E_GUILD_ATTR_Reserved_6           <DisplayName="E_GUILD_ATTR_Reserved_6">,
	E_GUILD_ATTR_Reserved_7           <DisplayName="E_GUILD_ATTR_Reserved_7">,
	E_GUILD_ATTR_Reserved_8           <DisplayName="E_GUILD_ATTR_Reserved_8">,
	E_GUILD_ATTR_Reserved_9           <DisplayName="E_GUILD_ATTR_Reserved_9">,
	E_GUILD_ATTR_Reserved_10          <DisplayName="E_GUILD_ATTR_Reserved_10">,
	E_GUILD_ATTR_Reserved_11          <DisplayName="E_GUILD_ATTR_Reserved_11">,
	E_GUILD_ATTR_Reserved_12          <DisplayName="E_GUILD_ATTR_Reserved_12">,
	E_GUILD_ATTR_Reserved_13          <DisplayName="E_GUILD_ATTR_Reserved_13">,
	E_GUILD_ATTR_Reserved_14          <DisplayName="E_GUILD_ATTR_Reserved_14">,
	E_GUILD_ATTR_Reserved_15          <DisplayName="E_GUILD_ATTR_Reserved_15">,
	E_GUILD_ATTR_Reserved_16          <DisplayName="E_GUILD_ATTR_Reserved_16">,
	E_GUILD_ATTR_Reserved_17          <DisplayName="E_GUILD_ATTR_Reserved_17">,
	E_GUILD_ATTR_Reserved_18          <DisplayName="E_GUILD_ATTR_Reserved_18">,
	E_GUILD_ATTR_Reserved_19          <DisplayName="E_GUILD_ATTR_Reserved_19">,
	E_GUILD_ATTR_Reserved_20          <DisplayName="E_GUILD_ATTR_Reserved_20">,
	E_GUILD_ATTR_Reserved_21          <DisplayName="E_GUILD_ATTR_Reserved_21">,
	E_GUILD_ATTR_Reserved_22          <DisplayName="E_GUILD_ATTR_Reserved_22">,
	E_GUILD_ATTR_Reserved_23          <DisplayName="E_GUILD_ATTR_Reserved_23">,
	E_GUILD_ATTR_Reserved_24          <DisplayName="E_GUILD_ATTR_Reserved_24">,
	E_GUILD_ATTR_Reserved_25          <DisplayName="E_GUILD_ATTR_Reserved_25">,
	E_GUILD_ATTR_Reserved_26          <DisplayName="E_GUILD_ATTR_Reserved_26">,
	E_GUILD_ATTR_Reserved_27          <DisplayName="E_GUILD_ATTR_Reserved_27">,
	E_GUILD_ATTR_Reserved_28          <DisplayName="E_GUILD_ATTR_Reserved_28">,
	E_GUILD_ATTR_Reserved_29          <DisplayName="E_GUILD_ATTR_Reserved_29">,
	E_GUILD_ATTR_Reserved_30          <DisplayName="E_GUILD_ATTR_Reserved_30">,
	E_GUILD_ATTR_Reserved_31          <DisplayName="E_GUILD_ATTR_Reserved_31">,
	E_GUILD_ATTR_Reserved_32          <DisplayName="E_GUILD_ATTR_Reserved_32">,
	E_GUILD_ATTR_Reserved_33          <DisplayName="E_GUILD_ATTR_Reserved_33">,
	E_GUILD_ATTR_Reserved_34          <DisplayName="E_GUILD_ATTR_Reserved_34">,
	E_GUILD_ATTR_Reserved_35          <DisplayName="E_GUILD_ATTR_Reserved_35">,
	E_GUILD_ATTR_Reserved_36          <DisplayName="E_GUILD_ATTR_Reserved_36">,
	E_GUILD_ATTR_Reserved_37          <DisplayName="E_GUILD_ATTR_Reserved_37">,
	E_GUILD_ATTR_Reserved_38          <DisplayName="E_GUILD_ATTR_Reserved_38">,
	E_GUILD_ATTR_Reserved_39          <DisplayName="E_GUILD_ATTR_Reserved_39">,
	E_GUILD_ATTR_Reserved_40          <DisplayName="E_GUILD_ATTR_Reserved_40">,
	E_GUILD_ATTR_Reserved_41          <DisplayName="E_GUILD_ATTR_Reserved_41">,
	E_GUILD_ATTR_Reserved_42          <DisplayName="E_GUILD_ATTR_Reserved_42">,
	E_GUILD_ATTR_Reserved_43          <DisplayName="E_GUILD_ATTR_Reserved_43">,
	E_GUILD_ATTR_Reserved_44          <DisplayName="E_GUILD_ATTR_Reserved_44">,
	E_GUILD_ATTR_Reserved_45          <DisplayName="E_GUILD_ATTR_Reserved_45">,
	E_GUILD_ATTR_Reserved_46          <DisplayName="E_GUILD_ATTR_Reserved_46">,
	E_GUILD_ATTR_Reserved_47          <DisplayName="E_GUILD_ATTR_Reserved_47">,
	E_GUILD_ATTR_Reserved_48          <DisplayName="E_GUILD_ATTR_Reserved_48">,
	E_GUILD_ATTR_Reserved_49          <DisplayName="E_GUILD_ATTR_Reserved_49">,
	E_GUILD_ATTR_Reserved_50          <DisplayName="E_GUILD_ATTR_Reserved_50">,
	E_GUILD_ATTR_Reserved_51          <DisplayName="E_GUILD_ATTR_Reserved_51">,
	E_GUILD_ATTR_Reserved_52          <DisplayName="E_GUILD_ATTR_Reserved_52">,
	E_GUILD_ATTR_Reserved_53          <DisplayName="E_GUILD_ATTR_Reserved_53">,
	E_GUILD_ATTR_Reserved_54          <DisplayName="E_GUILD_ATTR_Reserved_54">,
	E_GUILD_ATTR_Reserved_55          <DisplayName="E_GUILD_ATTR_Reserved_55">,
	E_GUILD_ATTR_Reserved_56          <DisplayName="E_GUILD_ATTR_Reserved_56">,
	E_GUILD_ATTR_Reserved_57          <DisplayName="E_GUILD_ATTR_Reserved_57">,
	E_GUILD_ATTR_Reserved_58          <DisplayName="E_GUILD_ATTR_Reserved_58">,
	E_GUILD_ATTR_Reserved_59          <DisplayName="E_GUILD_ATTR_Reserved_59">,
	E_GUILD_ATTR_Reserved_60          <DisplayName="E_GUILD_ATTR_Reserved_60">,
	E_GUILD_ATTR_Reserved_61          <DisplayName="E_GUILD_ATTR_Reserved_61">,
	E_GUILD_ATTR_Reserved_62          <DisplayName="E_GUILD_ATTR_Reserved_62">,
	E_GUILD_ATTR_Reserved_63          <DisplayName="E_GUILD_ATTR_Reserved_63">,
	E_GUILD_ATTR_Reserved_64          <DisplayName="E_GUILD_ATTR_Reserved_64">,
	E_GUILD_ATTR_Reserved_65          <DisplayName="E_GUILD_ATTR_Reserved_65">,
	E_GUILD_ATTR_Reserved_66          <DisplayName="E_GUILD_ATTR_Reserved_66">,
	E_GUILD_ATTR_Reserved_67          <DisplayName="E_GUILD_ATTR_Reserved_67">,
	E_GUILD_ATTR_Reserved_68          <DisplayName="E_GUILD_ATTR_Reserved_68">,
	E_GUILD_ATTR_Reserved_69          <DisplayName="E_GUILD_ATTR_Reserved_69">,
	E_GUILD_ATTR_Reserved_70          <DisplayName="E_GUILD_ATTR_Reserved_70">,
	E_GUILD_ATTR_Reserved_71          <DisplayName="E_GUILD_ATTR_Reserved_71">,
	E_GUILD_ATTR_Reserved_72          <DisplayName="E_GUILD_ATTR_Reserved_72">,
	E_GUILD_ATTR_Reserved_73          <DisplayName="E_GUILD_ATTR_Reserved_73">,
	E_GUILD_ATTR_Reserved_74          <DisplayName="E_GUILD_ATTR_Reserved_74">,
	E_GUILD_ATTR_Reserved_75          <DisplayName="E_GUILD_ATTR_Reserved_75">,
	E_GUILD_ATTR_Reserved_76          <DisplayName="E_GUILD_ATTR_Reserved_76">,
	E_GUILD_ATTR_Reserved_77          <DisplayName="E_GUILD_ATTR_Reserved_77">,
	E_GUILD_ATTR_Reserved_78          <DisplayName="E_GUILD_ATTR_Reserved_78">,
	E_GUILD_ATTR_Reserved_79          <DisplayName="E_GUILD_ATTR_Reserved_79">,
	E_GUILD_ATTR_Reserved_80          <DisplayName="E_GUILD_ATTR_Reserved_80">,
	E_GUILD_ATTR_Reserved_81          <DisplayName="E_GUILD_ATTR_Reserved_81">,
	E_GUILD_ATTR_Reserved_82          <DisplayName="E_GUILD_ATTR_Reserved_82">,
	E_GUILD_ATTR_Reserved_83          <DisplayName="E_GUILD_ATTR_Reserved_83">,
	E_GUILD_ATTR_Reserved_84          <DisplayName="E_GUILD_ATTR_Reserved_84">,
	E_GUILD_ATTR_Reserved_85          <DisplayName="E_GUILD_ATTR_Reserved_85">,
	E_GUILD_ATTR_Reserved_86          <DisplayName="E_GUILD_ATTR_Reserved_86">,
	E_GUILD_ATTR_Reserved_87          <DisplayName="E_GUILD_ATTR_Reserved_87">,
	E_GUILD_ATTR_Reserved_88          <DisplayName="E_GUILD_ATTR_Reserved_88">,
	E_GUILD_ATTR_Reserved_89          <DisplayName="E_GUILD_ATTR_Reserved_89">,
	E_GUILD_ATTR_Reserved_90          <DisplayName="E_GUILD_ATTR_Reserved_90">,
	E_GUILD_ATTR_Reserved_91          <DisplayName="E_GUILD_ATTR_Reserved_91">,
	E_GUILD_ATTR_Reserved_92          <DisplayName="E_GUILD_ATTR_Reserved_92">,
	E_GUILD_ATTR_Reserved_93          <DisplayName="E_GUILD_ATTR_Reserved_93">,
	E_GUILD_ATTR_Reserved_94          <DisplayName="E_GUILD_ATTR_Reserved_94">,
	E_GUILD_ATTR_Reserved_95          <DisplayName="E_GUILD_ATTR_Reserved_95">,
	E_GUILD_ATTR_Reserved_96          <DisplayName="E_GUILD_ATTR_Reserved_96">,
	E_GUILD_ATTR_Reserved_97          <DisplayName="E_GUILD_ATTR_Reserved_97">,
	E_GUILD_ATTR_Reserved_98          <DisplayName="E_GUILD_ATTR_Reserved_98">,
	E_GUILD_ATTR_Reserved_99          <DisplayName="E_GUILD_ATTR_Reserved_99">,
	E_GUILD_ATTR_Reserved_100         <DisplayName="E_GUILD_ATTR_Reserved_100">,
	E_GUILD_ATTR_Reserved_101         <DisplayName="E_GUILD_ATTR_Reserved_101">,
	E_GUILD_ATTR_Reserved_102         <DisplayName="E_GUILD_ATTR_Reserved_102">,
	E_GUILD_ATTR_Reserved_103         <DisplayName="E_GUILD_ATTR_Reserved_103">,
	E_GUILD_ATTR_Reserved_104         <DisplayName="E_GUILD_ATTR_Reserved_104">,
	E_GUILD_ATTR_Reserved_105         <DisplayName="E_GUILD_ATTR_Reserved_105">,
	E_GUILD_ATTR_Reserved_106         <DisplayName="E_GUILD_ATTR_Reserved_106">,
	E_GUILD_ATTR_Reserved_107         <DisplayName="E_GUILD_ATTR_Reserved_107">,
	E_GUILD_ATTR_Reserved_108         <DisplayName="E_GUILD_ATTR_Reserved_108">,
	E_GUILD_ATTR_Reserved_109         <DisplayName="E_GUILD_ATTR_Reserved_109">,
	E_GUILD_ATTR_Reserved_110         <DisplayName="E_GUILD_ATTR_Reserved_110">,
	E_GUILD_ATTR_Reserved_111         <DisplayName="E_GUILD_ATTR_Reserved_111">,
	E_GUILD_ATTR_Reserved_112         <DisplayName="E_GUILD_ATTR_Reserved_112">,
	E_GUILD_ATTR_Reserved_113         <DisplayName="E_GUILD_ATTR_Reserved_113">,
	E_GUILD_ATTR_Reserved_114         <DisplayName="E_GUILD_ATTR_Reserved_114">,
	E_GUILD_ATTR_Reserved_115         <DisplayName="E_GUILD_ATTR_Reserved_115">,
	E_GUILD_ATTR_Reserved_116         <DisplayName="E_GUILD_ATTR_Reserved_116">,
	E_GUILD_ATTR_Reserved_117         <DisplayName="E_GUILD_ATTR_Reserved_117">,
	E_GUILD_ATTR_Reserved_118         <DisplayName="E_GUILD_ATTR_Reserved_118">,
	E_GUILD_ATTR_Reserved_119         <DisplayName="E_GUILD_ATTR_Reserved_119">,
	E_GUILD_ATTR_Reserved_120         <DisplayName="E_GUILD_ATTR_Reserved_120">,
	E_GUILD_ATTR_Reserved_121         <DisplayName="E_GUILD_ATTR_Reserved_121">,
	E_GUILD_ATTR_Reserved_122         <DisplayName="E_GUILD_ATTR_Reserved_122">,
	E_GUILD_ATTR_Reserved_123         <DisplayName="E_GUILD_ATTR_Reserved_123">,
	E_GUILD_ATTR_Reserved_124         <DisplayName="E_GUILD_ATTR_Reserved_124">,
	E_GUILD_ATTR_Reserved_125         <DisplayName="E_GUILD_ATTR_Reserved_125">,
	E_GUILD_ATTR_Reserved_126         <DisplayName="E_GUILD_ATTR_Reserved_126">,
	E_GUILD_ATTR_Reserved_127         <DisplayName="E_GUILD_ATTR_Reserved_127">,
	E_GUILD_ATTR_Reserved_128         <DisplayName="E_GUILD_ATTR_Reserved_128">,
	E_GUILD_ATTR_Reserved_129         <DisplayName="E_GUILD_ATTR_Reserved_129">,
	E_GUILD_ATTR_Reserved_130         <DisplayName="E_GUILD_ATTR_Reserved_130">,
	E_GUILD_ATTR_Reserved_131         <DisplayName="E_GUILD_ATTR_Reserved_131">,
	E_GUILD_ATTR_Reserved_132         <DisplayName="E_GUILD_ATTR_Reserved_132">,
	E_GUILD_ATTR_Reserved_133         <DisplayName="E_GUILD_ATTR_Reserved_133">,
	E_GUILD_ATTR_Reserved_134         <DisplayName="E_GUILD_ATTR_Reserved_134">,
	E_GUILD_ATTR_Reserved_135         <DisplayName="E_GUILD_ATTR_Reserved_135">,
	E_GUILD_ATTR_Reserved_136         <DisplayName="E_GUILD_ATTR_Reserved_136">,
	E_GUILD_ATTR_Reserved_137         <DisplayName="E_GUILD_ATTR_Reserved_137">,
	E_GUILD_ATTR_Reserved_138         <DisplayName="E_GUILD_ATTR_Reserved_138">,
	E_GUILD_ATTR_Reserved_139         <DisplayName="E_GUILD_ATTR_Reserved_139">,
	E_GUILD_ATTR_Reserved_140         <DisplayName="E_GUILD_ATTR_Reserved_140">,
	E_GUILD_ATTR_Reserved_141         <DisplayName="E_GUILD_ATTR_Reserved_141">,
	E_GUILD_ATTR_Reserved_142         <DisplayName="E_GUILD_ATTR_Reserved_142">,
	E_GUILD_ATTR_Reserved_143         <DisplayName="E_GUILD_ATTR_Reserved_143">,
	E_GUILD_ATTR_Reserved_144         <DisplayName="E_GUILD_ATTR_Reserved_144">,
	E_GUILD_ATTR_Reserved_145         <DisplayName="E_GUILD_ATTR_Reserved_145">,
	E_GUILD_ATTR_Reserved_146         <DisplayName="E_GUILD_ATTR_Reserved_146">,
	E_GUILD_ATTR_Reserved_147         <DisplayName="E_GUILD_ATTR_Reserved_147">,
	E_GUILD_ATTR_Reserved_148         <DisplayName="E_GUILD_ATTR_Reserved_148">,
	E_GUILD_ATTR_Reserved_149         <DisplayName="E_GUILD_ATTR_Reserved_149">,
	E_GUILD_ATTR_Reserved_150         <DisplayName="E_GUILD_ATTR_Reserved_150">,
	E_GUILD_ATTR_Reserved_151         <DisplayName="E_GUILD_ATTR_Reserved_151">,
	E_GUILD_ATTR_Reserved_152         <DisplayName="E_GUILD_ATTR_Reserved_152">,
	E_GUILD_ATTR_Reserved_153         <DisplayName="E_GUILD_ATTR_Reserved_153">,
	E_GUILD_ATTR_Reserved_154         <DisplayName="E_GUILD_ATTR_Reserved_154">,
	E_GUILD_ATTR_Reserved_155         <DisplayName="E_GUILD_ATTR_Reserved_155">,
	E_GUILD_ATTR_Reserved_156         <DisplayName="E_GUILD_ATTR_Reserved_156">,
	E_GUILD_ATTR_Reserved_157         <DisplayName="E_GUILD_ATTR_Reserved_157">,
	E_GUILD_ATTR_Reserved_158         <DisplayName="E_GUILD_ATTR_Reserved_158">,
	E_GUILD_ATTR_Reserved_159         <DisplayName="E_GUILD_ATTR_Reserved_159">,
	E_GUILD_ATTR_Reserved_160         <DisplayName="E_GUILD_ATTR_Reserved_160">,
	E_GUILD_ATTR_Reserved_161         <DisplayName="E_GUILD_ATTR_Reserved_161">,
	E_GUILD_ATTR_Reserved_162         <DisplayName="E_GUILD_ATTR_Reserved_162">,
	E_GUILD_ATTR_Reserved_163         <DisplayName="E_GUILD_ATTR_Reserved_163">,
	E_GUILD_ATTR_Reserved_164         <DisplayName="E_GUILD_ATTR_Reserved_164">,
	E_GUILD_ATTR_Reserved_165         <DisplayName="E_GUILD_ATTR_Reserved_165">,
	E_GUILD_ATTR_Reserved_166         <DisplayName="E_GUILD_ATTR_Reserved_166">,
	E_GUILD_ATTR_Reserved_167         <DisplayName="E_GUILD_ATTR_Reserved_167">,
	E_GUILD_ATTR_Reserved_168         <DisplayName="E_GUILD_ATTR_Reserved_168">,
	E_GUILD_ATTR_Reserved_169         <DisplayName="E_GUILD_ATTR_Reserved_169">,
	E_GUILD_ATTR_Reserved_170         <DisplayName="E_GUILD_ATTR_Reserved_170">,
	E_GUILD_ATTR_Reserved_171         <DisplayName="E_GUILD_ATTR_Reserved_171">,
	E_GUILD_ATTR_Reserved_172         <DisplayName="E_GUILD_ATTR_Reserved_172">,
	E_GUILD_ATTR_Reserved_173         <DisplayName="E_GUILD_ATTR_Reserved_173">,
	E_GUILD_ATTR_Reserved_174         <DisplayName="E_GUILD_ATTR_Reserved_174">,
	E_GUILD_ATTR_Reserved_175         <DisplayName="E_GUILD_ATTR_Reserved_175">,
	E_GUILD_ATTR_Reserved_176         <DisplayName="E_GUILD_ATTR_Reserved_176">,
	E_GUILD_ATTR_Reserved_177         <DisplayName="E_GUILD_ATTR_Reserved_177">,
	E_GUILD_ATTR_Reserved_178         <DisplayName="E_GUILD_ATTR_Reserved_178">,
	E_GUILD_ATTR_Reserved_179         <DisplayName="E_GUILD_ATTR_Reserved_179">,
	E_GUILD_ATTR_Reserved_180         <DisplayName="E_GUILD_ATTR_Reserved_180">,
	E_GUILD_ATTR_Reserved_181         <DisplayName="E_GUILD_ATTR_Reserved_181">,
	E_GUILD_ATTR_Reserved_182         <DisplayName="E_GUILD_ATTR_Reserved_182">,
	E_GUILD_ATTR_Reserved_183         <DisplayName="E_GUILD_ATTR_Reserved_183">,
	E_GUILD_ATTR_Reserved_184         <DisplayName="E_GUILD_ATTR_Reserved_184">,
	E_GUILD_ATTR_Reserved_185         <DisplayName="E_GUILD_ATTR_Reserved_185">,
	E_GUILD_ATTR_Reserved_186         <DisplayName="E_GUILD_ATTR_Reserved_186">,
	E_GUILD_ATTR_Reserved_187         <DisplayName="E_GUILD_ATTR_Reserved_187">,
	E_GUILD_ATTR_Reserved_188         <DisplayName="E_GUILD_ATTR_Reserved_188">,
	E_GUILD_ATTR_Reserved_189         <DisplayName="E_GUILD_ATTR_Reserved_189">,
	E_GUILD_ATTR_Reserved_190         <DisplayName="E_GUILD_ATTR_Reserved_190">,
	E_GUILD_ATTR_Reserved_191         <DisplayName="E_GUILD_ATTR_Reserved_191">,
	E_GUILD_ATTR_Reserved_192         <DisplayName="E_GUILD_ATTR_Reserved_192">,
	E_GUILD_ATTR_Reserved_193         <DisplayName="E_GUILD_ATTR_Reserved_193">,
	E_GUILD_ATTR_Reserved_194         <DisplayName="E_GUILD_ATTR_Reserved_194">,
	E_GUILD_ATTR_Reserved_195         <DisplayName="E_GUILD_ATTR_Reserved_195">,
	E_GUILD_ATTR_Reserved_196         <DisplayName="E_GUILD_ATTR_Reserved_196">,
	E_GUILD_ATTR_Reserved_197         <DisplayName="E_GUILD_ATTR_Reserved_197">,
	E_GUILD_ATTR_Reserved_198         <DisplayName="E_GUILD_ATTR_Reserved_198">,
	E_GUILD_ATTR_Reserved_199         <DisplayName="E_GUILD_ATTR_Reserved_199">,
	E_GUILD_ATTR_Reserved_200         <DisplayName="E_GUILD_ATTR_Reserved_200">,
	E_GUILD_ATTR_Reserved_201         <DisplayName="E_GUILD_ATTR_Reserved_201">,
	E_GUILD_ATTR_Reserved_202         <DisplayName="E_GUILD_ATTR_Reserved_202">,
	E_GUILD_ATTR_Reserved_203         <DisplayName="E_GUILD_ATTR_Reserved_203">,
	E_GUILD_ATTR_Reserved_204         <DisplayName="E_GUILD_ATTR_Reserved_204">,
	E_GUILD_ATTR_Reserved_205         <DisplayName="E_GUILD_ATTR_Reserved_205">,
	E_GUILD_ATTR_Reserved_206         <DisplayName="E_GUILD_ATTR_Reserved_206">,
	E_GUILD_ATTR_Reserved_207         <DisplayName="E_GUILD_ATTR_Reserved_207">,
	E_GUILD_ATTR_Reserved_208         <DisplayName="E_GUILD_ATTR_Reserved_208">,
	E_GUILD_ATTR_Reserved_209         <DisplayName="E_GUILD_ATTR_Reserved_209">,
	E_GUILD_ATTR_Reserved_210         <DisplayName="E_GUILD_ATTR_Reserved_210">,
	E_GUILD_ATTR_Reserved_211         <DisplayName="E_GUILD_ATTR_Reserved_211">,
	E_GUILD_ATTR_Reserved_212         <DisplayName="E_GUILD_ATTR_Reserved_212">,
	E_GUILD_ATTR_Reserved_213         <DisplayName="E_GUILD_ATTR_Reserved_213">,
	E_GUILD_ATTR_Reserved_214         <DisplayName="E_GUILD_ATTR_Reserved_214">,
	E_GUILD_ATTR_Reserved_215         <DisplayName="E_GUILD_ATTR_Reserved_215">,
	E_GUILD_ATTR_Reserved_216         <DisplayName="E_GUILD_ATTR_Reserved_216">,
	E_GUILD_ATTR_Reserved_217         <DisplayName="E_GUILD_ATTR_Reserved_217">,
	E_GUILD_ATTR_Reserved_218         <DisplayName="E_GUILD_ATTR_Reserved_218">,
	E_GUILD_ATTR_Reserved_219         <DisplayName="E_GUILD_ATTR_Reserved_219">,
	E_GUILD_ATTR_Reserved_220         <DisplayName="E_GUILD_ATTR_Reserved_220">,
	E_GUILD_ATTR_Reserved_221         <DisplayName="E_GUILD_ATTR_Reserved_221">,
	E_GUILD_ATTR_Reserved_222         <DisplayName="E_GUILD_ATTR_Reserved_222">,
	E_GUILD_ATTR_Reserved_223         <DisplayName="E_GUILD_ATTR_Reserved_223">,
	E_GUILD_ATTR_Reserved_224         <DisplayName="E_GUILD_ATTR_Reserved_224">,
	E_GUILD_ATTR_Reserved_225         <DisplayName="E_GUILD_ATTR_Reserved_225">,
	E_GUILD_ATTR_Reserved_226         <DisplayName="E_GUILD_ATTR_Reserved_226">,
	E_GUILD_ATTR_Reserved_227         <DisplayName="E_GUILD_ATTR_Reserved_227">,
	E_GUILD_ATTR_Reserved_228         <DisplayName="E_GUILD_ATTR_Reserved_228">,
	E_GUILD_ATTR_Reserved_229         <DisplayName="E_GUILD_ATTR_Reserved_229">,
	E_GUILD_ATTR_Reserved_230         <DisplayName="E_GUILD_ATTR_Reserved_230">,
	E_GUILD_ATTR_Reserved_231         <DisplayName="E_GUILD_ATTR_Reserved_231">,
	E_GUILD_ATTR_Reserved_232         <DisplayName="E_GUILD_ATTR_Reserved_232">,
	E_GUILD_ATTR_Reserved_233         <DisplayName="E_GUILD_ATTR_Reserved_233">,
	E_GUILD_ATTR_Reserved_234         <DisplayName="E_GUILD_ATTR_Reserved_234">,
	E_GUILD_ATTR_Reserved_235         <DisplayName="E_GUILD_ATTR_Reserved_235">,
	E_GUILD_ATTR_Reserved_236         <DisplayName="E_GUILD_ATTR_Reserved_236">,
	E_GUILD_ATTR_Reserved_237         <DisplayName="E_GUILD_ATTR_Reserved_237">,
	E_GUILD_ATTR_Reserved_238         <DisplayName="E_GUILD_ATTR_Reserved_238">,
	E_GUILD_ATTR_Reserved_239         <DisplayName="E_GUILD_ATTR_Reserved_239">,
	E_GUILD_ATTR_Reserved_240         <DisplayName="E_GUILD_ATTR_Reserved_240">,
	E_GUILD_ATTR_Reserved_241         <DisplayName="E_GUILD_ATTR_Reserved_241">,
	E_GUILD_ATTR_Reserved_242         <DisplayName="E_GUILD_ATTR_Reserved_242">,
	E_GUILD_ATTR_Reserved_243         <DisplayName="E_GUILD_ATTR_Reserved_243">,
	E_GUILD_ATTR_Reserved_244         <DisplayName="E_GUILD_ATTR_Reserved_244">,
	E_GUILD_ATTR_Reserved_245         <DisplayName="E_GUILD_ATTR_Reserved_245">,
	E_GUILD_ATTR_Reserved_246         <DisplayName="E_GUILD_ATTR_Reserved_246">,
	E_GUILD_ATTR_Reserved_247         <DisplayName="E_GUILD_ATTR_Reserved_247">,
	E_GUILD_ATTR_Reserved_248         <DisplayName="E_GUILD_ATTR_Reserved_248">,
	E_GUILD_ATTR_Reserved_249         <DisplayName="E_GUILD_ATTR_Reserved_249">,
	E_GUILD_ATTR_Reserved_250         <DisplayName="E_GUILD_ATTR_Reserved_250">,
	E_GUILD_ATTR_Reserved_251         <DisplayName="E_GUILD_ATTR_Reserved_251">,
	E_GUILD_ATTR_Reserved_252         <DisplayName="E_GUILD_ATTR_Reserved_252">,
	E_GUILD_ATTR_Reserved_253         <DisplayName="E_GUILD_ATTR_Reserved_253">,
	E_GUILD_ATTR_Reserved_254         <DisplayName="E_GUILD_ATTR_Reserved_254">,
	E_GUILD_ATTR_MAX
};

enum EEMoneyLog
{
// max enum definition is over 254
	E_MO_NEYLOG_BUY_ITEM              <DisplayName="E_MO_NEYLOG_BUY_ITEM">,
	E_MO_NEYLOG_REPAIR                <DisplayName="E_MO_NEYLOG_REPAIR">,
	E_MO_NEYLOG_PLAYER_STALL          <DisplayName="E_MO_NEYLOG_PLAYER_STALL">,
	E_MO_NEYLOG_PLAYER_STALL_TAX      <DisplayName="E_MO_NEYLOG_PLAYER_STALL_TAX">,
	E_MO_NEYLOG_PLAYER_TRADE          <DisplayName="E_MO_NEYLOG_PLAYER_TRADE">,
	E_MO_NEYLOG_PLAYER_TRADE_TAX      <DisplayName="E_MO_NEYLOG_PLAYER_TRADE_TAX">,
	E_MO_NEYLOG_BUY_BACK              <DisplayName="E_MO_NEYLOG_BUY_BACK">,
	E_MO_NEYLOG_REFINE_ITEM           <DisplayName="E_MO_NEYLOG_REFINE_ITEM">,
	E_MO_NEYLOG_BUY_SLOTS             <DisplayName="E_MO_NEYLOG_BUY_SLOTS">,
	E_MO_NEYLOG_REMOVE_SOUL           <DisplayName="E_MO_NEYLOG_REMOVE_SOUL">,
	E_MO_NEYLOG_CREATE_GUILD          <DisplayName="E_MO_NEYLOG_CREATE_GUILD">,
	E_MO_NEYLOG_GUILD_CONTRIBUTE      <DisplayName="E_MO_NEYLOG_GUILD_CONTRIBUTE">,
	E_MO_NEYLOG_GUILD_POP_ITEM        <DisplayName="E_MO_NEYLOG_GUILD_POP_ITEM">,
	E_MO_MEYLOG_GUILD_ENTER_DUNGEON   <DisplayName="E_MO_MEYLOG_GUILD_ENTER_DUNGEON">,
	E_MO_NEYLOG_TELEPORT_PLATFORM     <DisplayName="E_MO_NEYLOG_TELEPORT_PLATFORM">,
	E_MO_NEYLOG_DUNGEON_SELECT_REWARD <DisplayName="E_MO_NEYLOG_DUNGEON_SELECT_REWARD">,
	E_MO_NEYLOG_CAST_SPELL            <DisplayName="E_MO_NEYLOG_CAST_SPELL">,
	E_MO_NEYLOG_MAIL_COST             <DisplayName="E_MO_NEYLOG_MAIL_COST">,
	E_MO_NEYLOG_LOOT                  <DisplayName="E_MO_NEYLOG_LOOT">,
	E_MO_NEYLOG_SELL_ITEM             <DisplayName="E_MO_NEYLOG_SELL_ITEM">,
	E_MO_NEYLOG_QUEST_PREPAY          <DisplayName="E_MO_NEYLOG_QUEST_PREPAY">,
	E_MO_NEYLOG_QUEST_REWARD          <DisplayName="E_MO_NEYLOG_QUEST_REWARD">,
	E_MO_NEYLOG_QUEST_COST            <DisplayName="E_MO_NEYLOG_QUEST_COST">,
	E_MO_NEYLOG_GUILD_SALARY          <DisplayName="E_MO_NEYLOG_GUILD_SALARY">,
	E_MO_NEYLOG_DUNGEON_REWARD        <DisplayName="E_MO_NEYLOG_DUNGEON_REWARD">,
	E_MO_NEYLOG_GM_COMMAND            <DisplayName="E_MO_NEYLOG_GM_COMMAND">,
	E_MO_NEYLOG_SCRIPT                <DisplayName="E_MO_NEYLOG_SCRIPT">,
	E_MO_NEYLOG_IDENTIFY_ITEM         <DisplayName="E_MO_NEYLOG_IDENTIFY_ITEM">,
	E_MO_NEYLOG_RANDOMIZE_ITEM        <DisplayName="E_MO_NEYLOG_RANDOMIZE_ITEM">,
	E_MO_NEYLOG_QUEST_FLY_COST        <DisplayName="E_MO_NEYLOG_QUEST_FLY_COST">,
	E_MO_NEYLOG_SIGN_ITEM_COST        <DisplayName="E_MO_NEYLOG_SIGN_ITEM_COST">,
	E_MO_NEYLOG_AUCTION_SELL_COST     <DisplayName="E_MO_NEYLOG_AUCTION_SELL_COST">,
	E_MO_NEYLOG_AUCTION_DEPOSIT_COST  <DisplayName="E_MO_NEYLOG_AUCTION_DEPOSIT_COST">,
	E_MO_NEYLOG_AUCTION_LEFT_MONEY    <DisplayName="E_MO_NEYLOG_AUCTION_LEFT_MONEY">,
	E_MO_NEYLOG_CHANGE_WEATHER        <DisplayName="E_MO_NEYLOG_CHANGE_WEATHER">,
	E_MO_NEYLOG_BUY_VIP               <DisplayName="E_MO_NEYLOG_BUY_VIP">,
	E_MO_NEYLOG_DAILY_QUEST_BUY_COMPLETE_TIME <DisplayName="E_MO_NEYLOG_DAILY_QUEST_BUY_COMPLETE_TIME">,
	E_MO_NEYLOG_DAILY_QUEST_BUY_ONESHOT <DisplayName="E_MO_NEYLOG_DAILY_QUEST_BUY_ONESHOT">,
	E_MO_NEYLOG_OPEN_GUILD_DUNGEON    <DisplayName="E_MO_NEYLOG_OPEN_GUILD_DUNGEON">,
	E_MO_NEYLOG_GUILD_DECLARE_WAR     <DisplayName="E_MO_NEYLOG_GUILD_DECLARE_WAR">,
	E_MO_NEYLOG_DECOMPOSE             <DisplayName="E_MO_NEYLOG_DECOMPOSE">,
	E_MO_NEYLOG_COMPOSE               <DisplayName="E_MO_NEYLOG_COMPOSE">,
	E_MO_NEYLOG_SPIRIT_EXP2PTS        <DisplayName="E_MO_NEYLOG_SPIRIT_EXP2PTS">,
	E_MO_NEYLOG_SPIRIT_ADDAIR         <DisplayName="E_MO_NEYLOG_SPIRIT_ADDAIR">,
	E_MO_NEYLOG_SPIRIT_BUYSLOT        <DisplayName="E_MO_NEYLOG_SPIRIT_BUYSLOT">,
	E_MO_NEYLOG_SPIRIT_BUYEXCHANGEAIR <DisplayName="E_MO_NEYLOG_SPIRIT_BUYEXCHANGEAIR">,
	E_MO_NEYLOG_SPIRIT_BLOOD          <DisplayName="E_MO_NEYLOG_SPIRIT_BLOOD">,
	E_MO_NEYLOG_REVIVE                <DisplayName="E_MO_NEYLOG_REVIVE">,
	E_MO_NEYLOG_CREATE_ARENA_TEAM     <DisplayName="E_MO_NEYLOG_CREATE_ARENA_TEAM">,
	E_MO_NEYLOG_GUILD_BUY_AURA        <DisplayName="E_MO_NEYLOG_GUILD_BUY_AURA">,
	E_MO_NEYLOG_GET_MAIL_ATTACH       <DisplayName="E_MO_NEYLOG_GET_MAIL_ATTACH">,
	E_MO_NEYLOG_GM                    <DisplayName="E_MO_NEYLOG_GM">,
	E_MO_NEYLOG_REAL_MONEY_SHOP       <DisplayName="E_MO_NEYLOG_REAL_MONEY_SHOP">,
	E_MO_NEYLOG_TEAM_AUCTION_BONUS    <DisplayName="E_MO_NEYLOG_TEAM_AUCTION_BONUS">,
	E_MO_NEYLOG_TEAM_AUCTION_RETURN   <DisplayName="E_MO_NEYLOG_TEAM_AUCTION_RETURN">,
	E_MO_NEYLOG_TEAM_AUCTION_DEPOSITE <DisplayName="E_MO_NEYLOG_TEAM_AUCTION_DEPOSITE">,
	E_MO_NEYLOG_EXCHANGE              <DisplayName="E_MO_NEYLOG_EXCHANGE">,
	E_MO_NEYLOG_EXCHANGE_FEE          <DisplayName="E_MO_NEYLOG_EXCHANGE_FEE">,
	E_MO_NEYLOG_MASTER_CZ             <DisplayName="E_MO_NEYLOG_MASTER_CZ">,
	E_MO_NEYLOG_ACHIEVEMENT_REWARD    <DisplayName="E_MO_NEYLOG_ACHIEVEMENT_REWARD">,
	E_MO_NEYLOG_RUNE                  <DisplayName="E_MO_NEYLOG_RUNE">,
	E_MO_NEYLOG_ARENA_JOIN_SEASON     <DisplayName="E_MO_NEYLOG_ARENA_JOIN_SEASON">,
	E_MO_NEYLOG_ARENA_RANK_REWARD     <DisplayName="E_MO_NEYLOG_ARENA_RANK_REWARD">,
	E_MO_NEYLOG_ENROLL_SABAC          <DisplayName="E_MO_NEYLOG_ENROLL_SABAC">,
	E_MO_NEYLOG_EQUIP_LEVELUP         <DisplayName="E_MO_NEYLOG_EQUIP_LEVELUP">,
	E_MO_NEYLOG_REPUTATION_AWARD      <DisplayName="E_MO_NEYLOG_REPUTATION_AWARD">,
	E_MO_NEYLOG_UPGRADE_STALL         <DisplayName="E_MO_NEYLOG_UPGRADE_STALL">,
	E_MO_NEYLOG_MINE                  <DisplayName="E_MO_NEYLOG_MINE">,
	E_MO_NEYLOG_AWARD                 <DisplayName="E_MO_NEYLOG_AWARD">,
	E_MO_NEYLOG_ERECT_STATUS          <DisplayName="E_MO_NEYLOG_ERECT_STATUS">,
	E_MO_NEYLOG_FCM_LOSS              <DisplayName="E_MO_NEYLOG_FCM_LOSS">,
	E_MO_NEYLOG_DAILY_LIMIT_LOSS      <DisplayName="E_MO_NEYLOG_DAILY_LIMIT_LOSS">,
	E_MO_NEYLOG_WEEKLY_LIMIT_LOSS     <DisplayName="E_MO_NEYLOG_WEEKLY_LIMIT_LOSS">,
	E_MO_NEYLOG_TOTAL_LIMIT_LOSS      <DisplayName="E_MO_NEYLOG_TOTAL_LIMIT_LOSS">,
	E_MO_NEYLOG_CHAT_COST             <DisplayName="E_MO_NEYLOG_CHAT_COST">,
	E_MO_NEYLOG_ESCORT                <DisplayName="E_MO_NEYLOG_ESCORT">,
	E_MO_NEYLOG_BLACK_MARKET          <DisplayName="E_MO_NEYLOG_BLACK_MARKET">,
	E_MO_NEYLOG_MATERIAL              <DisplayName="E_MO_NEYLOG_MATERIAL">,
	E_MO_NEYLOG_BIRTH                 <DisplayName="E_MO_NEYLOG_BIRTH">,
	E_MO_NEYLOG_EXP_MORAL             <DisplayName="E_MO_NEYLOG_EXP_MORAL">,
	E_MO_NEYLOG_TEACHER_REWARD        <DisplayName="E_MO_NEYLOG_TEACHER_REWARD">,
	E_MO_NEYLOG_STUDENT_REWARD        <DisplayName="E_MO_NEYLOG_STUDENT_REWARD">,
	E_MO_NEYLOG_PACKAGE               <DisplayName="E_MO_NEYLOG_PACKAGE">,
	E_MO_NEYLOG_EQUIP_LEVELUP_IMMEDIATELY <DisplayName="E_MO_NEYLOG_EQUIP_LEVELUP_IMMEDIATELY">,
	E_MO_NEYLOG_ARENA_REFUND          <DisplayName="E_MO_NEYLOG_ARENA_REFUND">,
	E_MO_NEYLOG_ARENA_REWARD          <DisplayName="E_MO_NEYLOG_ARENA_REWARD">,
	E_MO_NEYLOG_SELL_RESOURCE_ITEM    <DisplayName="E_MO_NEYLOG_SELL_RESOURCE_ITEM">,
	E_MO_Reserved_87                  <DisplayName="E_MO_Reserved_87">,
	E_MO_Reserved_88                  <DisplayName="E_MO_Reserved_88">,
	E_MO_Reserved_89                  <DisplayName="E_MO_Reserved_89">,
	E_MO_Reserved_90                  <DisplayName="E_MO_Reserved_90">,
	E_MO_Reserved_91                  <DisplayName="E_MO_Reserved_91">,
	E_MO_Reserved_92                  <DisplayName="E_MO_Reserved_92">,
	E_MO_Reserved_93                  <DisplayName="E_MO_Reserved_93">,
	E_MO_Reserved_94                  <DisplayName="E_MO_Reserved_94">,
	E_MO_Reserved_95                  <DisplayName="E_MO_Reserved_95">,
	E_MO_Reserved_96                  <DisplayName="E_MO_Reserved_96">,
	E_MO_Reserved_97                  <DisplayName="E_MO_Reserved_97">,
	E_MO_Reserved_98                  <DisplayName="E_MO_Reserved_98">,
	E_MO_Reserved_99                  <DisplayName="E_MO_Reserved_99">,
	E_MO_Reserved_100                 <DisplayName="E_MO_Reserved_100">,
	E_MO_Reserved_101                 <DisplayName="E_MO_Reserved_101">,
	E_MO_Reserved_102                 <DisplayName="E_MO_Reserved_102">,
	E_MO_Reserved_103                 <DisplayName="E_MO_Reserved_103">,
	E_MO_Reserved_104                 <DisplayName="E_MO_Reserved_104">,
	E_MO_Reserved_105                 <DisplayName="E_MO_Reserved_105">,
	E_MO_Reserved_106                 <DisplayName="E_MO_Reserved_106">,
	E_MO_Reserved_107                 <DisplayName="E_MO_Reserved_107">,
	E_MO_Reserved_108                 <DisplayName="E_MO_Reserved_108">,
	E_MO_Reserved_109                 <DisplayName="E_MO_Reserved_109">,
	E_MO_Reserved_110                 <DisplayName="E_MO_Reserved_110">,
	E_MO_Reserved_111                 <DisplayName="E_MO_Reserved_111">,
	E_MO_Reserved_112                 <DisplayName="E_MO_Reserved_112">,
	E_MO_Reserved_113                 <DisplayName="E_MO_Reserved_113">,
	E_MO_Reserved_114                 <DisplayName="E_MO_Reserved_114">,
	E_MO_Reserved_115                 <DisplayName="E_MO_Reserved_115">,
	E_MO_Reserved_116                 <DisplayName="E_MO_Reserved_116">,
	E_MO_Reserved_117                 <DisplayName="E_MO_Reserved_117">,
	E_MO_Reserved_118                 <DisplayName="E_MO_Reserved_118">,
	E_MO_Reserved_119                 <DisplayName="E_MO_Reserved_119">,
	E_MO_Reserved_120                 <DisplayName="E_MO_Reserved_120">,
	E_MO_Reserved_121                 <DisplayName="E_MO_Reserved_121">,
	E_MO_Reserved_122                 <DisplayName="E_MO_Reserved_122">,
	E_MO_Reserved_123                 <DisplayName="E_MO_Reserved_123">,
	E_MO_Reserved_124                 <DisplayName="E_MO_Reserved_124">,
	E_MO_Reserved_125                 <DisplayName="E_MO_Reserved_125">,
	E_MO_Reserved_126                 <DisplayName="E_MO_Reserved_126">,
	E_MO_Reserved_127                 <DisplayName="E_MO_Reserved_127">,
	E_MO_Reserved_128                 <DisplayName="E_MO_Reserved_128">,
	E_MO_Reserved_129                 <DisplayName="E_MO_Reserved_129">,
	E_MO_Reserved_130                 <DisplayName="E_MO_Reserved_130">,
	E_MO_Reserved_131                 <DisplayName="E_MO_Reserved_131">,
	E_MO_Reserved_132                 <DisplayName="E_MO_Reserved_132">,
	E_MO_Reserved_133                 <DisplayName="E_MO_Reserved_133">,
	E_MO_Reserved_134                 <DisplayName="E_MO_Reserved_134">,
	E_MO_Reserved_135                 <DisplayName="E_MO_Reserved_135">,
	E_MO_Reserved_136                 <DisplayName="E_MO_Reserved_136">,
	E_MO_Reserved_137                 <DisplayName="E_MO_Reserved_137">,
	E_MO_Reserved_138                 <DisplayName="E_MO_Reserved_138">,
	E_MO_Reserved_139                 <DisplayName="E_MO_Reserved_139">,
	E_MO_Reserved_140                 <DisplayName="E_MO_Reserved_140">,
	E_MO_Reserved_141                 <DisplayName="E_MO_Reserved_141">,
	E_MO_Reserved_142                 <DisplayName="E_MO_Reserved_142">,
	E_MO_Reserved_143                 <DisplayName="E_MO_Reserved_143">,
	E_MO_Reserved_144                 <DisplayName="E_MO_Reserved_144">,
	E_MO_Reserved_145                 <DisplayName="E_MO_Reserved_145">,
	E_MO_Reserved_146                 <DisplayName="E_MO_Reserved_146">,
	E_MO_Reserved_147                 <DisplayName="E_MO_Reserved_147">,
	E_MO_Reserved_148                 <DisplayName="E_MO_Reserved_148">,
	E_MO_Reserved_149                 <DisplayName="E_MO_Reserved_149">,
	E_MO_Reserved_150                 <DisplayName="E_MO_Reserved_150">,
	E_MO_Reserved_151                 <DisplayName="E_MO_Reserved_151">,
	E_MO_Reserved_152                 <DisplayName="E_MO_Reserved_152">,
	E_MO_Reserved_153                 <DisplayName="E_MO_Reserved_153">,
	E_MO_Reserved_154                 <DisplayName="E_MO_Reserved_154">,
	E_MO_Reserved_155                 <DisplayName="E_MO_Reserved_155">,
	E_MO_Reserved_156                 <DisplayName="E_MO_Reserved_156">,
	E_MO_Reserved_157                 <DisplayName="E_MO_Reserved_157">,
	E_MO_Reserved_158                 <DisplayName="E_MO_Reserved_158">,
	E_MO_Reserved_159                 <DisplayName="E_MO_Reserved_159">,
	E_MO_Reserved_160                 <DisplayName="E_MO_Reserved_160">,
	E_MO_Reserved_161                 <DisplayName="E_MO_Reserved_161">,
	E_MO_Reserved_162                 <DisplayName="E_MO_Reserved_162">,
	E_MO_Reserved_163                 <DisplayName="E_MO_Reserved_163">,
	E_MO_Reserved_164                 <DisplayName="E_MO_Reserved_164">,
	E_MO_Reserved_165                 <DisplayName="E_MO_Reserved_165">,
	E_MO_Reserved_166                 <DisplayName="E_MO_Reserved_166">,
	E_MO_Reserved_167                 <DisplayName="E_MO_Reserved_167">,
	E_MO_Reserved_168                 <DisplayName="E_MO_Reserved_168">,
	E_MO_Reserved_169                 <DisplayName="E_MO_Reserved_169">,
	E_MO_Reserved_170                 <DisplayName="E_MO_Reserved_170">,
	E_MO_Reserved_171                 <DisplayName="E_MO_Reserved_171">,
	E_MO_Reserved_172                 <DisplayName="E_MO_Reserved_172">,
	E_MO_Reserved_173                 <DisplayName="E_MO_Reserved_173">,
	E_MO_Reserved_174                 <DisplayName="E_MO_Reserved_174">,
	E_MO_Reserved_175                 <DisplayName="E_MO_Reserved_175">,
	E_MO_Reserved_176                 <DisplayName="E_MO_Reserved_176">,
	E_MO_Reserved_177                 <DisplayName="E_MO_Reserved_177">,
	E_MO_Reserved_178                 <DisplayName="E_MO_Reserved_178">,
	E_MO_Reserved_179                 <DisplayName="E_MO_Reserved_179">,
	E_MO_Reserved_180                 <DisplayName="E_MO_Reserved_180">,
	E_MO_Reserved_181                 <DisplayName="E_MO_Reserved_181">,
	E_MO_Reserved_182                 <DisplayName="E_MO_Reserved_182">,
	E_MO_Reserved_183                 <DisplayName="E_MO_Reserved_183">,
	E_MO_Reserved_184                 <DisplayName="E_MO_Reserved_184">,
	E_MO_Reserved_185                 <DisplayName="E_MO_Reserved_185">,
	E_MO_Reserved_186                 <DisplayName="E_MO_Reserved_186">,
	E_MO_Reserved_187                 <DisplayName="E_MO_Reserved_187">,
	E_MO_Reserved_188                 <DisplayName="E_MO_Reserved_188">,
	E_MO_Reserved_189                 <DisplayName="E_MO_Reserved_189">,
	E_MO_Reserved_190                 <DisplayName="E_MO_Reserved_190">,
	E_MO_Reserved_191                 <DisplayName="E_MO_Reserved_191">,
	E_MO_Reserved_192                 <DisplayName="E_MO_Reserved_192">,
	E_MO_Reserved_193                 <DisplayName="E_MO_Reserved_193">,
	E_MO_Reserved_194                 <DisplayName="E_MO_Reserved_194">,
	E_MO_Reserved_195                 <DisplayName="E_MO_Reserved_195">,
	E_MO_Reserved_196                 <DisplayName="E_MO_Reserved_196">,
	E_MO_Reserved_197                 <DisplayName="E_MO_Reserved_197">,
	E_MO_Reserved_198                 <DisplayName="E_MO_Reserved_198">,
	E_MO_Reserved_199                 <DisplayName="E_MO_Reserved_199">,
	E_MO_Reserved_200                 <DisplayName="E_MO_Reserved_200">,
	E_MO_Reserved_201                 <DisplayName="E_MO_Reserved_201">,
	E_MO_Reserved_202                 <DisplayName="E_MO_Reserved_202">,
	E_MO_Reserved_203                 <DisplayName="E_MO_Reserved_203">,
	E_MO_Reserved_204                 <DisplayName="E_MO_Reserved_204">,
	E_MO_Reserved_205                 <DisplayName="E_MO_Reserved_205">,
	E_MO_Reserved_206                 <DisplayName="E_MO_Reserved_206">,
	E_MO_Reserved_207                 <DisplayName="E_MO_Reserved_207">,
	E_MO_Reserved_208                 <DisplayName="E_MO_Reserved_208">,
	E_MO_Reserved_209                 <DisplayName="E_MO_Reserved_209">,
	E_MO_Reserved_210                 <DisplayName="E_MO_Reserved_210">,
	E_MO_Reserved_211                 <DisplayName="E_MO_Reserved_211">,
	E_MO_Reserved_212                 <DisplayName="E_MO_Reserved_212">,
	E_MO_Reserved_213                 <DisplayName="E_MO_Reserved_213">,
	E_MO_Reserved_214                 <DisplayName="E_MO_Reserved_214">,
	E_MO_Reserved_215                 <DisplayName="E_MO_Reserved_215">,
	E_MO_Reserved_216                 <DisplayName="E_MO_Reserved_216">,
	E_MO_Reserved_217                 <DisplayName="E_MO_Reserved_217">,
	E_MO_Reserved_218                 <DisplayName="E_MO_Reserved_218">,
	E_MO_Reserved_219                 <DisplayName="E_MO_Reserved_219">,
	E_MO_Reserved_220                 <DisplayName="E_MO_Reserved_220">,
	E_MO_Reserved_221                 <DisplayName="E_MO_Reserved_221">,
	E_MO_Reserved_222                 <DisplayName="E_MO_Reserved_222">,
	E_MO_Reserved_223                 <DisplayName="E_MO_Reserved_223">,
	E_MO_Reserved_224                 <DisplayName="E_MO_Reserved_224">,
	E_MO_Reserved_225                 <DisplayName="E_MO_Reserved_225">,
	E_MO_Reserved_226                 <DisplayName="E_MO_Reserved_226">,
	E_MO_Reserved_227                 <DisplayName="E_MO_Reserved_227">,
	E_MO_Reserved_228                 <DisplayName="E_MO_Reserved_228">,
	E_MO_Reserved_229                 <DisplayName="E_MO_Reserved_229">,
	E_MO_Reserved_230                 <DisplayName="E_MO_Reserved_230">,
	E_MO_Reserved_231                 <DisplayName="E_MO_Reserved_231">,
	E_MO_Reserved_232                 <DisplayName="E_MO_Reserved_232">,
	E_MO_Reserved_233                 <DisplayName="E_MO_Reserved_233">,
	E_MO_Reserved_234                 <DisplayName="E_MO_Reserved_234">,
	E_MO_Reserved_235                 <DisplayName="E_MO_Reserved_235">,
	E_MO_Reserved_236                 <DisplayName="E_MO_Reserved_236">,
	E_MO_Reserved_237                 <DisplayName="E_MO_Reserved_237">,
	E_MO_Reserved_238                 <DisplayName="E_MO_Reserved_238">,
	E_MO_Reserved_239                 <DisplayName="E_MO_Reserved_239">,
	E_MO_Reserved_240                 <DisplayName="E_MO_Reserved_240">,
	E_MO_Reserved_241                 <DisplayName="E_MO_Reserved_241">,
	E_MO_Reserved_242                 <DisplayName="E_MO_Reserved_242">,
	E_MO_Reserved_243                 <DisplayName="E_MO_Reserved_243">,
	E_MO_Reserved_244                 <DisplayName="E_MO_Reserved_244">,
	E_MO_Reserved_245                 <DisplayName="E_MO_Reserved_245">,
	E_MO_Reserved_246                 <DisplayName="E_MO_Reserved_246">,
	E_MO_Reserved_247                 <DisplayName="E_MO_Reserved_247">,
	E_MO_Reserved_248                 <DisplayName="E_MO_Reserved_248">,
	E_MO_Reserved_249                 <DisplayName="E_MO_Reserved_249">,
	E_MO_Reserved_250                 <DisplayName="E_MO_Reserved_250">,
	E_MO_Reserved_251                 <DisplayName="E_MO_Reserved_251">,
	E_MO_Reserved_252                 <DisplayName="E_MO_Reserved_252">,
	E_MO_Reserved_253                 <DisplayName="E_MO_Reserved_253">,
	E_MO_Reserved_254                 <DisplayName="E_MO_Reserved_254">,
	E_MO_MAX
};

enum EEItemLog
{
	E_ITEMLOG_SHOP                    <DisplayName="E_ITEMLOG_SHOP">,
	E_ITEMLOG_LOOT                    <DisplayName="E_ITEMLOG_LOOT">,
	E_ITEMLOG_GM_COMMAND              <DisplayName="E_ITEMLOG_GM_COMMAND">,
	E_ITEMLOG_QUEST_PREPAY            <DisplayName="E_ITEMLOG_QUEST_PREPAY">,
	E_ITEMLOG_QUEST_REWARD            <DisplayName="E_ITEMLOG_QUEST_REWARD">,
	E_ITEMLOG_DUNGEON_REWARD          <DisplayName="E_ITEMLOG_DUNGEON_REWARD">,
	E_ITEMLOG_PLAYER_STALL            <DisplayName="E_ITEMLOG_PLAYER_STALL">,
	E_ITEMLOG_PLAYER_TRADE            <DisplayName="E_ITEMLOG_PLAYER_TRADE">,
	E_ITEMLOG_BUY_BACK                <DisplayName="E_ITEMLOG_BUY_BACK">,
	E_ITEMLOG_CREATE_ROLE             <DisplayName="E_ITEMLOG_CREATE_ROLE">,
	E_ITEMLOG_CREATE_HERO             <DisplayName="E_ITEMLOG_CREATE_HERO">,
	E_ITEMLOG_SCRIPT                  <DisplayName="E_ITEMLOG_SCRIPT">,
	E_ITEMLOG_GET_MAIL_ATTACH         <DisplayName="E_ITEMLOG_GET_MAIL_ATTACH">,
	E_ITEMLOG_GUILD_STORAGE           <DisplayName="E_ITEMLOG_GUILD_STORAGE">,
	E_ITEMLOG_GUILD_DESTORY           <DisplayName="E_ITEMLOG_GUILD_DESTORY">,
	E_ITEMLOG_ACHIEVEMENT_REWARD      <DisplayName="E_ITEMLOG_ACHIEVEMENT_REWARD">,
	E_ITEMLOG_UPGRADE_SOUL            <DisplayName="E_ITEMLOG_UPGRADE_SOUL">,
	E_ITEMLOG_REMOVE_SOUL             <DisplayName="E_ITEMLOG_REMOVE_SOUL">,
	E_ITEMLOG_NPC_DROP                <DisplayName="E_ITEMLOG_NPC_DROP">,
	E_ITEMLOG_USER_REQUEST            <DisplayName="E_ITEMLOG_USER_REQUEST">,
	E_ITEMLOG_SELL                    <DisplayName="E_ITEMLOG_SELL">,
	E_ITEMLOG_CAST_SPELL              <DisplayName="E_ITEMLOG_CAST_SPELL">,
	E_ITEMLOG_REFINE_ITEM             <DisplayName="E_ITEMLOG_REFINE_ITEM">,
	E_ITEMLOG_COMPLETE_QUEST          <DisplayName="E_ITEMLOG_COMPLETE_QUEST">,
	E_ITEMLOG_CREATE_GUILD            <DisplayName="E_ITEMLOG_CREATE_GUILD">,
	E_ITEMLOG_USE_ITEM                <DisplayName="E_ITEMLOG_USE_ITEM">,
	E_ITEMLOG_INSERT_SOUL             <DisplayName="E_ITEMLOG_INSERT_SOUL">,
	E_ITEMLOG_AUCTION_SELL            <DisplayName="E_ITEMLOG_AUCTION_SELL">,
	E_ITEMLOG_RUNE                    <DisplayName="E_ITEMLOG_RUNE">,
	E_ITEMLOG_DECOMPOSE               <DisplayName="E_ITEMLOG_DECOMPOSE">,
	E_ITEMLOG_COMPOSE                 <DisplayName="E_ITEMLOG_COMPOSE">,
	E_ITEMLOG_ACTIVITY_REWARD         <DisplayName="E_ITEMLOG_ACTIVITY_REWARD">,
	E_ITEMLOG_REVIVE                  <DisplayName="E_ITEMLOG_REVIVE">,
	E_ITEMLOG_GM                      <DisplayName="E_ITEMLOG_GM">,
	E_ITEMLOG_OPEN_DOODAD             <DisplayName="E_ITEMLOG_OPEN_DOODAD">,
	E_ITEMLOG_REAL_MONEY_SHOP         <DisplayName="E_ITEMLOG_REAL_MONEY_SHOP">,
	E_ITEMLOG_TEAM_AUCTION            <DisplayName="E_ITEMLOG_TEAM_AUCTION">,
	E_ITEMLOG_TEAM_AUCTION_FAILED     <DisplayName="E_ITEMLOG_TEAM_AUCTION_FAILED">,
	E_ITEMLOG_QUEST_RECYCLE           <DisplayName="E_ITEMLOG_QUEST_RECYCLE">,
	E_ITEMLOG_EXCHANGE                <DisplayName="E_ITEMLOG_EXCHANGE">,
	E_ITEMLOG_ARENA_COMMON_REWARD     <DisplayName="E_ITEMLOG_ARENA_COMMON_REWARD">,
	E_ITEMLOG_SABAC_AWARD             <DisplayName="E_ITEMLOG_SABAC_AWARD">,
	E_ITEMLOG_REPUTATION_AWARD        <DisplayName="E_ITEMLOG_REPUTATION_AWARD">,
	E_ITEMLOG_SPLIT                   <DisplayName="E_ITEMLOG_SPLIT">,
	E_ITEMLOG_MOVE                    <DisplayName="E_ITEMLOG_MOVE">,
	E_ITEMLOG_ORGANIZE_BAG            <DisplayName="E_ITEMLOG_ORGANIZE_BAG">,
	E_ITEMLOG_EQUIP_LEVELUP           <DisplayName="E_ITEMLOG_EQUIP_LEVELUP">,
	E_ITEMLOG_EQUIP_LEVELUP_FAILED    <DisplayName="E_ITEMLOG_EQUIP_LEVELUP_FAILED">,
	E_ITEMLOG_EQUIP_LEVELUP_SUCCEEDED <DisplayName="E_ITEMLOG_EQUIP_LEVELUP_SUCCEEDED">,
	E_ITEMLOG_RECOVER_USED_UP         <DisplayName="E_ITEMLOG_RECOVER_USED_UP">,
	E_ITEMLOG_MINE                    <DisplayName="E_ITEMLOG_MINE">,
	E_ITEMLOG_ENROLL_SABAC            <DisplayName="E_ITEMLOG_ENROLL_SABAC">,
	E_ITEMLOG_DEATH                   <DisplayName="E_ITEMLOG_DEATH">,
	E_ITEMLOG_SPELL                   <DisplayName="E_ITEMLOG_SPELL">,
	E_ITEMLOG_ATTACK                  <DisplayName="E_ITEMLOG_ATTACK">,
	E_ITEMLOG_ATTACKED                <DisplayName="E_ITEMLOG_ATTACKED">,
	E_ITEMLOG_TRUMPET                 <DisplayName="E_ITEMLOG_TRUMPET">,
	E_ITEMLOG_CHECKIN_AWARD           <DisplayName="E_ITEMLOG_CHECKIN_AWARD">,
	E_ITEMLOG_AWARD                   <DisplayName="E_ITEMLOG_AWARD">,
	E_ITEMLOG_BLACK_MARKET            <DisplayName="E_ITEMLOG_BLACK_MARKET">,
	E_ITEMLOG_MATERIAL                <DisplayName="E_ITEMLOG_MATERIAL">,
	E_ITEMLOG_GAMBLE                  <DisplayName="E_ITEMLOG_GAMBLE">,
	E_ITEMLOG_PACKAGE                 <DisplayName="E_ITEMLOG_PACKAGE">,
	E_ITEMLOG_IDENTIFY                <DisplayName="E_ITEMLOG_IDENTIFY">,
	E_ITEMLOG_RANDOMIZE               <DisplayName="E_ITEMLOG_RANDOMIZE">,
	E_ITEMLOG_LUCK_OIL                <DisplayName="E_ITEMLOG_LUCK_OIL">,
	E_ITEMLOG_SIGN                    <DisplayName="E_ITEMLOG_SIGN">,
	E_ITEMLOG_STORAGE_ROLLBACK        <DisplayName="E_ITEMLOG_STORAGE_ROLLBACK">,
	E_ITEMLOG_SPELL_BOOK_GAMBLE       <DisplayName="E_ITEMLOG_SPELL_BOOK_GAMBLE">,
	E_ITEMLOG_GM_FIX_ROLE             <DisplayName="E_ITEMLOG_GM_FIX_ROLE">,
	E_ITEMLOG_RUNE_ADVANCE_CONFIRM    <DisplayName="E_ITEMLOG_RUNE_ADVANCE_CONFIRM">,
	E_ITEMLOG_POTION_GAMBLE           <DisplayName="E_ITEMLOG_POTION_GAMBLE">,
	E_ITEMLOG_SELL_RESOURCE_ITEM      <DisplayName="E_ITEMLOG_SELL_RESOURCE_ITEM">,
	E_ITEMLOG_TOTAL                   <DisplayName="E_ITEMLOG_TOTAL">,
	E_ITEMLOG_MAX
};

enum ESPELL_ATTACK_TYPE
{
	E_spat_None                       <DisplayName="E_spat_None">,
	E_spat_Phy                        <DisplayName="E_spat_Phy">,
	E_spat_Mag                        <DisplayName="E_spat_Mag">,
	E_spat_Tao                        <DisplayName="E_spat_Tao">,
	E_spat_Stab                        <DisplayName="E_spat_Stab">,
	E_spat_Archery                        <DisplayName="E_spat_Archery">,
	E_spat_Total                      <DisplayName="E_spat_Total">,
	E_spat_MAX
};

enum EFRIEND_STATE
{
	E_FS_ONLINE                       <DisplayName="E_FS_ONLINE">,
	E_FS_BUSY                         <DisplayName="E_FS_BUSY">,
	E_FS_AFK                          <DisplayName="E_FS_AFK">,
	E_FS_OFFLINE                      <DisplayName="E_FS_OFFLINE">,
	E_FS_MAX
};

enum EERoleState
{
	E_rs_Invalid                      <DisplayName="E_rs_Invalid">,
	E_rs_PreEntered                   <DisplayName="E_rs_PreEntered">,
	E_rs_WaitRoleData                 <DisplayName="E_rs_WaitRoleData">,
	E_rs_WaitMSApprove                <DisplayName="E_rs_WaitMSApprove">,
	E_rs_WaitClientLoad               <DisplayName="E_rs_WaitClientLoad">,
	E_rs_Playing                      <DisplayName="E_rs_Playing">,
	E_rs_InJump                       <DisplayName="E_rs_InJump">,
	E_rs_JumpInGSWaitClientLoad       <DisplayName="E_rs_JumpInGSWaitClientLoad">,
	E_rs_WaitUnlockRoleData           <DisplayName="E_rs_WaitUnlockRoleData">,
	E_rs_Logout                       <DisplayName="E_rs_Logout">,
	E_rs_Total                        <DisplayName="E_rs_Total">,
	E_rs_MAX
};

enum ECHAT_LOCATION_TYPE
{
	E_clt_Invalid                     <DisplayName="E_clt_Invalid">,
	E_clt_Normal                      <DisplayName="E_clt_Normal">,
	E_clt_Lighting                    <DisplayName="E_clt_Lighting">,
	E_clt_TvShow                      <DisplayName="E_clt_TvShow">,
	E_clt_Url                         <DisplayName="E_clt_Url">,
	E_clt_Dialog                      <DisplayName="E_clt_Dialog">,
	E_clt_Trumpet                     <DisplayName="E_clt_Trumpet">,
	E_clt_GMTrumpet                   <DisplayName="E_clt_GMTrumpet">,
	E_clt_Total                       <DisplayName="E_clt_Total">,
	E_clt_MAX
};

enum ESPELL_COST_TYPE
{
	E_spcot_Fury                      <DisplayName="E_spcot_Fury">,
	E_spcot_Energy                    <DisplayName="E_spcot_Energy">,
	E_spcot_Mana                      <DisplayName="E_spcot_Mana">,
	E_spcot_White                     <DisplayName="E_spcot_White">,
	E_spcot_Total                     <DisplayName="E_spcot_Total">,
	E_spcot_MAX
};

enum ESIGNAL_TYPE
{
	E_sig_Invalid                     <DisplayName="E_sig_Invalid">,
	E_sig_SpreadHate                  <DisplayName="E_sig_SpreadHate">,
	E_sig_AssistOnce                  <DisplayName="E_sig_AssistOnce">,
	E_sig_Assist                      <DisplayName="E_sig_Assist">,
	E_sig_HelpSpellHeal               <DisplayName="E_sig_HelpSpellHeal">,
	E_sig_HelpSpellHarm               <DisplayName="E_sig_HelpSpellHarm">,
	E_sig_Reserved_6                  <DisplayName="E_sig_Reserved_6">,
	E_sig_Reserved_7                  <DisplayName="E_sig_Reserved_7">,
	E_sig_Reserved_8                  <DisplayName="E_sig_Reserved_8">,
	E_sig_Reserved_9                  <DisplayName="E_sig_Reserved_9">,
	E_sig_Reserved_10                 <DisplayName="E_sig_Reserved_10">,
	E_sig_Reserved_11                 <DisplayName="E_sig_Reserved_11">,
	E_sig_Reserved_12                 <DisplayName="E_sig_Reserved_12">,
	E_sig_Reserved_13                 <DisplayName="E_sig_Reserved_13">,
	E_sig_Reserved_14                 <DisplayName="E_sig_Reserved_14">,
	E_sig_Reserved_15                 <DisplayName="E_sig_Reserved_15">,
	E_sig_Reserved_16                 <DisplayName="E_sig_Reserved_16">,
	E_sig_Reserved_17                 <DisplayName="E_sig_Reserved_17">,
	E_sig_Reserved_18                 <DisplayName="E_sig_Reserved_18">,
	E_sig_Reserved_19                 <DisplayName="E_sig_Reserved_19">,
	E_sig_Reserved_20                 <DisplayName="E_sig_Reserved_20">,
	E_sig_Reserved_21                 <DisplayName="E_sig_Reserved_21">,
	E_sig_Reserved_22                 <DisplayName="E_sig_Reserved_22">,
	E_sig_Reserved_23                 <DisplayName="E_sig_Reserved_23">,
	E_sig_Reserved_24                 <DisplayName="E_sig_Reserved_24">,
	E_sig_Reserved_25                 <DisplayName="E_sig_Reserved_25">,
	E_sig_Reserved_26                 <DisplayName="E_sig_Reserved_26">,
	E_sig_Reserved_27                 <DisplayName="E_sig_Reserved_27">,
	E_sig_Reserved_28                 <DisplayName="E_sig_Reserved_28">,
	E_sig_Reserved_29                 <DisplayName="E_sig_Reserved_29">,
	E_sig_Reserved_30                 <DisplayName="E_sig_Reserved_30">,
	E_sig_Reserved_31                 <DisplayName="E_sig_Reserved_31">,
	E_sig_UserDefine                  <DisplayName="E_sig_UserDefine">,
	E_sig_Reserved_33                 <DisplayName="E_sig_Reserved_33">,
	E_sig_Reserved_34                 <DisplayName="E_sig_Reserved_34">,
	E_sig_Reserved_35                 <DisplayName="E_sig_Reserved_35">,
	E_sig_Reserved_36                 <DisplayName="E_sig_Reserved_36">,
	E_sig_Reserved_37                 <DisplayName="E_sig_Reserved_37">,
	E_sig_Reserved_38                 <DisplayName="E_sig_Reserved_38">,
	E_sig_Reserved_39                 <DisplayName="E_sig_Reserved_39">,
	E_sig_Reserved_40                 <DisplayName="E_sig_Reserved_40">,
	E_sig_Reserved_41                 <DisplayName="E_sig_Reserved_41">,
	E_sig_Reserved_42                 <DisplayName="E_sig_Reserved_42">,
	E_sig_Reserved_43                 <DisplayName="E_sig_Reserved_43">,
	E_sig_Reserved_44                 <DisplayName="E_sig_Reserved_44">,
	E_sig_Reserved_45                 <DisplayName="E_sig_Reserved_45">,
	E_sig_Reserved_46                 <DisplayName="E_sig_Reserved_46">,
	E_sig_Reserved_47                 <DisplayName="E_sig_Reserved_47">,
	E_sig_Reserved_48                 <DisplayName="E_sig_Reserved_48">,
	E_sig_Reserved_49                 <DisplayName="E_sig_Reserved_49">,
	E_sig_Reserved_50                 <DisplayName="E_sig_Reserved_50">,
	E_sig_Reserved_51                 <DisplayName="E_sig_Reserved_51">,
	E_sig_Reserved_52                 <DisplayName="E_sig_Reserved_52">,
	E_sig_Reserved_53                 <DisplayName="E_sig_Reserved_53">,
	E_sig_Reserved_54                 <DisplayName="E_sig_Reserved_54">,
	E_sig_Reserved_55                 <DisplayName="E_sig_Reserved_55">,
	E_sig_Reserved_56                 <DisplayName="E_sig_Reserved_56">,
	E_sig_Reserved_57                 <DisplayName="E_sig_Reserved_57">,
	E_sig_Reserved_58                 <DisplayName="E_sig_Reserved_58">,
	E_sig_Reserved_59                 <DisplayName="E_sig_Reserved_59">,
	E_sig_Reserved_60                 <DisplayName="E_sig_Reserved_60">,
	E_sig_Reserved_61                 <DisplayName="E_sig_Reserved_61">,
	E_sig_Reserved_62                 <DisplayName="E_sig_Reserved_62">,
	E_sig_Reserved_63                 <DisplayName="E_sig_Reserved_63">,
	E_sig_End                         <DisplayName="E_sig_End">,
	E_sig_MAX
};

enum EITEM_COLOR
{
	E_ITEMCOLOR_WHITE                 <DisplayName="E_ITEMCOLOR_WHITE">,
	E_ITEMCOLOR_GREEN                 <DisplayName="E_ITEMCOLOR_GREEN">,
	E_ITEMCOLOR_BLUE                  <DisplayName="E_ITEMCOLOR_BLUE">,
	E_ITEMCOLOR_PURPLE                <DisplayName="E_ITEMCOLOR_PURPLE">,
	E_ITEMCOLOR_ORANGE                <DisplayName="E_ITEMCOLOR_ORANGE">,
	E_ITEMCOLOR_RESERVED1             <DisplayName="E_ITEMCOLOR_RESERVED1">,
	E_ITEMCOLOR_RESERVED2             <DisplayName="E_ITEMCOLOR_RESERVED2">,
	E_ITEMCOLOR_TOTAL                 <DisplayName="E_ITEMCOLOR_TOTAL">,
	E_ITEMCOLOR_MAX
};

enum EAURA_EFFECT_TYPE
{
	E_auet_None                       <DisplayName="E_auet_None">,
	E_auet_Type01                     <DisplayName="E_auet_Type01">,
	E_auet_Type02                     <DisplayName="E_auet_Type02">,
	E_auet_Type03                     <DisplayName="E_auet_Type03">,
	E_auet_Type04                     <DisplayName="E_auet_Type04">,
	E_auet_Type05                     <DisplayName="E_auet_Type05">,
	E_auet_Type06                     <DisplayName="E_auet_Type06">,
	E_auet_Type07                     <DisplayName="E_auet_Type07">,
	E_auet_Type08                     <DisplayName="E_auet_Type08">,
	E_auet_Type09                     <DisplayName="E_auet_Type09">,
	E_auet_Type10                     <DisplayName="E_auet_Type10">,
	E_auet_Type11                     <DisplayName="E_auet_Type11">,
	E_auet_Type12                     <DisplayName="E_auet_Type12">,
	E_auet_Type13                     <DisplayName="E_auet_Type13">,
	E_auet_Type14                     <DisplayName="E_auet_Type14">,
	E_auet_Type15                     <DisplayName="E_auet_Type15">,
	E_auet_Type16                     <DisplayName="E_auet_Type16">,
	E_auet_Type17                     <DisplayName="E_auet_Type17">,
	E_auet_Type18                     <DisplayName="E_auet_Type18">,
	E_auet_Type19                     <DisplayName="E_auet_Type19">,
	E_auet_Type20                     <DisplayName="E_auet_Type20">,
	E_auet_Type21                     <DisplayName="E_auet_Type21">,
	E_auet_Type22                     <DisplayName="E_auet_Type22">,
	E_auet_Type23                     <DisplayName="E_auet_Type23">,
	E_auet_Type24                     <DisplayName="E_auet_Type24">,
	E_auet_Total                      <DisplayName="E_auet_Total">,
	E_auet_MAX
};

enum ECOOLDOWN_SAVE_TYPE
{
	E_COOLDOWN_SAVE_TYPE_INVALID      <DisplayName="E_COOLDOWN_SAVE_TYPE_INVALID">,
	E_COOLDOWN_SAVE_TYPE_OFFLINE_TIME_VALID <DisplayName="E_COOLDOWN_SAVE_TYPE_OFFLINE_TIME_VALID">,
	E_COOLDOWN_SAVE_TYPE_OFFLINE_TIME_INVALID <DisplayName="E_COOLDOWN_SAVE_TYPE_OFFLINE_TIME_INVALID">,
	E_COOLDOWN_SAVE_TYPE_MAX
};

enum EERoleNameColor
{
	E_rnc_White                       <DisplayName="E_rnc_White">,
	E_rnc_Yellow                      <DisplayName="E_rnc_Yellow">,
	E_rnc_Red                         <DisplayName="E_rnc_Red">,
	E_rnc_Total                       <DisplayName="E_rnc_Total">,
	E_rnc_MAX
};

enum EETradeState
{
	E_ets_Free                        <DisplayName="E_ets_Free">,
	E_ets_Asking                      <DisplayName="E_ets_Asking">,
	E_ets_BeingAsked                  <DisplayName="E_ets_BeingAsked">,
	E_ets_Trading                     <DisplayName="E_ets_Trading">,
	E_ets_Locked                      <DisplayName="E_ets_Locked">,
	E_ets_Confirmed                   <DisplayName="E_ets_Confirmed">,
	E_ets_Success                     <DisplayName="E_ets_Success">,
	E_ets_MAX
};

enum ECOOLDOWN_TYPE
{
	E_COOLDOWN_GROUP                  <DisplayName="E_COOLDOWN_GROUP">,
	E_COOLDOWN_SPELL                  <DisplayName="E_COOLDOWN_SPELL">,
	E_COOLDOWN_ITEM                   <DisplayName="E_COOLDOWN_ITEM">,
	E_COOLDOWN_QUEST                  <DisplayName="E_COOLDOWN_QUEST">,
	E_COOLDOWN_AI                     <DisplayName="E_COOLDOWN_AI">,
	E_COOLDOWN_SPELL_EVENT_TRIGGER    <DisplayName="E_COOLDOWN_SPELL_EVENT_TRIGGER">,
	E_COOLDOWN_MISC                   <DisplayName="E_COOLDOWN_MISC">,
	E_COOLDOWN_SCRIPT_SPELL           <DisplayName="E_COOLDOWN_SCRIPT_SPELL">,
	E_COOLDOWN_SPECIAL_ITEM           <DisplayName="E_COOLDOWN_SPECIAL_ITEM">,
	E_COOLDOWN_GUILD                  <DisplayName="E_COOLDOWN_GUILD">,
	E_COOLDOWN_TOTAL                  <DisplayName="E_COOLDOWN_TOTAL">,
	E_COOLDOWN_MAX
};

enum ESPELL_FLAG_BITS
{
// conflicted enum definition sfbRoundTableEnd(6)
	E_sfb_Crit                        <DisplayName="E_sfb_Crit">,
	E_sfb_Block                       <DisplayName="E_sfb_Block">,
	E_sfb_Dodge                       <DisplayName="E_sfb_Dodge">,
	E_sfb_Parry                       <DisplayName="E_sfb_Parry">,
	E_sfb_Miss                        <DisplayName="E_sfb_Miss">,
	E_sfb_Normal                      <DisplayName="E_sfb_Normal">,
	E_sfb_Immune                      <DisplayName="E_sfb_Immune">,
	E_sfb_Dead                        <DisplayName="E_sfb_Dead">,
	E_sfb_EmptyEffect                 <DisplayName="E_sfb_EmptyEffect">,
	E_sfb_Total                       <DisplayName="E_sfb_Total">,
	E_sfb_MAX
};

enum EPET_AI_VARIABLES
{
	E_PET_AI_V_MAGE_PET_SPELL_ID      <DisplayName="E_PET_AI_V_MAGE_PET_SPELL_ID">,
	E_PET_AI_V_FIGHT_STATE_INDEX      <DisplayName="E_PET_AI_V_FIGHT_STATE_INDEX">,
	E_PET_AI_Reserved_2               <DisplayName="E_PET_AI_Reserved_2">,
	E_PET_AI_CONTROL                  <DisplayName="E_PET_AI_CONTROL">,
	E_PET_AI_IS_EXTRA_WALK            <DisplayName="E_PET_AI_IS_EXTRA_WALK">,
	E_PET_AI_MAGE_CHASE_THRESHOLD     <DisplayName="E_PET_AI_MAGE_CHASE_THRESHOLD">,
	E_PET_AI_MAGE_PREFER_DISTANCE     <DisplayName="E_PET_AI_MAGE_PREFER_DISTANCE">,
	E_PET_AI_GO_HOME                  <DisplayName="E_PET_AI_GO_HOME">,
	E_PET_AI_MAX
};

enum ESPELL_FAKE_PREPARE_TYPE
{
	E_sfppt_Channeled                 <DisplayName="E_sfppt_Channeled">,
	E_sfppt_Prepared                  <DisplayName="E_sfppt_Prepared">,
	E_sfppt_PreparedEx                <DisplayName="E_sfppt_PreparedEx">,
	E_sfppt_InstantCast               <DisplayName="E_sfppt_InstantCast">,
	E_sfppt_Accumulated               <DisplayName="E_sfppt_Accumulated">,
	E_sfppt_Total                     <DisplayName="E_sfppt_Total">,
	E_sfppt_MAX
};

enum ESPELL_CANCEL_TYPE
{
	E_spclt_None                      <DisplayName="E_spclt_None">,
	E_spclt_DependOnValue             <DisplayName="E_spclt_DependOnValue">,
	E_spclt_Instant                   <DisplayName="E_spclt_Instant">,
	E_spclt_Total                     <DisplayName="E_spclt_Total">,
	E_spclt_MAX
};

enum EERoleVarType
{
	E_ervt_Total                      <DisplayName="E_ervt_Total">,
	E_ervt_MAX
};

enum EMISSILE_COLLISION_GROUP
{
	E_mcg_Reserved_0                  <DisplayName="E_mcg_Reserved_0">,
	E_mcg_Role                        <DisplayName="E_mcg_Role">,
	E_mcg_Npc                         <DisplayName="E_mcg_Npc">,
	E_mcg_Reserved_3                  <DisplayName="E_mcg_Reserved_3">,
	E_mcg_Doodad                      <DisplayName="E_mcg_Doodad">,
	E_mcg_MAX
};

enum ESPELL_LOCK_TYPE
{
	E_splt_Self                       <DisplayName="E_splt_Self">,
	E_splt_Object                     <DisplayName="E_splt_Object">,
	E_splt_Direction                  <DisplayName="E_splt_Direction">,
	E_splt_Pos                        <DisplayName="E_splt_Pos">,
	E_splt_SelfPos                    <DisplayName="E_splt_SelfPos">,
	E_splt_SmartPos                   <DisplayName="E_splt_SmartPos">,
	E_splt_Total                      <DisplayName="E_splt_Total">,
	E_splt_MAX
};

enum ECHAR_DIE_REASON
{
	E_cdr_Default                     <DisplayName="E_cdr_Default">,
	E_cdr_Spell                       <DisplayName="E_cdr_Spell">,
	E_cdr_Aura                        <DisplayName="E_cdr_Aura">,
	E_cdr_Recovery                    <DisplayName="E_cdr_Recovery">,
	E_cdr_TimeToDie                   <DisplayName="E_cdr_TimeToDie">,
	E_cdr_AIBehavior                  <DisplayName="E_cdr_AIBehavior">,
	E_cdr_Silent                      <DisplayName="E_cdr_Silent">,
	E_cdr_Leech                       <DisplayName="E_cdr_Leech">,
	E_cdr_Suicide                     <DisplayName="E_cdr_Suicide">,
	E_cdr_Count                       <DisplayName="E_cdr_Count">,
	E_cdr_MAX
};

enum EMISSILE_MOVE_TYPE
{
	E_mmt_Line                        <DisplayName="E_mmt_Line">,
	E_mmt_AccordingToTerrain          <DisplayName="E_mmt_AccordingToTerrain">,
	E_mmt_Static                      <DisplayName="E_mmt_Static">,
	E_mmt_LockTarget                  <DisplayName="E_mmt_LockTarget">,
	E_mmt_Total                       <DisplayName="E_mmt_Total">,
	E_mmt_MAX
};

enum ESPELL_DAMAGE_TYPE
{
// conflicted enum definition spdtDamageTypeBegin(1)
	E_spdt_None                       <DisplayName="E_spdt_None">,
	E_spdt_Phy                        <DisplayName="E_spdt_Phy">,
	E_spdt_Earth                      <DisplayName="E_spdt_Earth">,
	E_spdt_Fire                       <DisplayName="E_spdt_Fire">,
	E_spdt_Water                      <DisplayName="E_spdt_Water">,
	E_spdt_Wind                       <DisplayName="E_spdt_Wind">,
	E_spdt_Thunder                    <DisplayName="E_spdt_Thunder">,
	E_spdt_Light                      <DisplayName="E_spdt_Light">,
	E_spdt_Shadow                     <DisplayName="E_spdt_Shadow">,
	E_spdt_Nature                     <DisplayName="E_spdt_Nature">,
	E_spdt_DamageTypeEnd              <DisplayName="E_spdt_DamageTypeEnd">,
	E_spdt_Total                      <DisplayName="E_spdt_Total">,
	E_spdt_MAX
};

enum EMSG_PAOPAO_TYPE
{
	E_MSG_PP_PERSONAL_ENEMY_Reserved_0 <DisplayName="E_MSG_PP_PERSONAL_ENEMY_Reserved_0">,
	E_MSG_PP_PERSONAL_ENEMY_          <DisplayName="E_MSG_PP_PERSONAL_ENEMY_">,
	E_MSG_PP_PERSONAL_ENEMY_MAX
};

enum EMASTER_TEACHER_APP_TYPE
{
	E_MTAT_FROM_COMMON                <DisplayName="E_MTAT_FROM_COMMON">,
	E_MTAT_FROM_TWITTER               <DisplayName="E_MTAT_FROM_TWITTER">,
	E_MTAT_FROM_MAX
};

enum ERUNE_DURABILITY_TYPE
{
	E_rdt_Never                       <DisplayName="E_rdt_Never">,
	E_rdt_ByUseTimes                  <DisplayName="E_rdt_ByUseTimes">,
	E_rdt_ByDuration                  <DisplayName="E_rdt_ByDuration">,
	E_rdt_Count                       <DisplayName="E_rdt_Count">,
	E_rdt_MAX
};

enum ENORMAL_PRACTICE_TYPE
{
	E_npt_Invalid                     <DisplayName="E_npt_Invalid">,
	E_npt_Type1                       <DisplayName="E_npt_Type1">,
	E_npt_Type2                       <DisplayName="E_npt_Type2">,
	E_npt_Type3                       <DisplayName="E_npt_Type3">,
	E_npt_Count                       <DisplayName="E_npt_Count">,
	E_npt_MAX
};

enum EGUILD_VARIABLE
{
	E_avGuildWarCanOpen_Reserved_0    <DisplayName="E_avGuildWarCanOpen_Reserved_0">,
	E_avGuildWarCanOpen_              <DisplayName="E_avGuildWarCanOpen_">,
	E_avGuildWarCanOpen_MAX
};

enum EOBJ_BODY_TYPE
{
	E_body_Invalid                    <DisplayName="E_body_Invalid">,
	E_body_0x0                        <DisplayName="E_body_0x0">,
	E_body_1x1                        <DisplayName="E_body_1x1">,
	E_body_3x3                        <DisplayName="E_body_3x3">,
	E_body_Total                      <DisplayName="E_body_Total">,
	E_body_MAX
};

enum ECOOLDOWN_MISC_TYPE
{
	E_COOLDOWN_MISC_HERO_REVIVE       <DisplayName="E_COOLDOWN_MISC_HERO_REVIVE">,
	E_COOLDOWN_MISC_ARREST            <DisplayName="E_COOLDOWN_MISC_ARREST">,
	E_COOLDOWN_MISC_TOTAL             <DisplayName="E_COOLDOWN_MISC_TOTAL">,
	E_COOLDOWN_MISC_MAX
};

enum EWEAPON_TYPE
{
	E_WEAPONTYPE_HAND                 <DisplayName="E_WEAPONTYPE_HAND">,
	E_WEAPONTYPE_01                   <DisplayName="E_WEAPONTYPE_01">,
	E_WEAPONTYPE_02                   <DisplayName="E_WEAPONTYPE_02">,
	E_WEAPONTYPE_03                   <DisplayName="E_WEAPONTYPE_03">,
	E_WEAPONTYPE_04                   <DisplayName="E_WEAPONTYPE_04">,
	E_WEAPONTYPE_05                   <DisplayName="E_WEAPONTYPE_05">,
	E_WEAPONTYPE_06                   <DisplayName="E_WEAPONTYPE_06">,
	E_WEAPONTYPE_07                   <DisplayName="E_WEAPONTYPE_07">,
	E_WEAPONTYPE_08                   <DisplayName="E_WEAPONTYPE_08">,
	E_WEAPONTYPE_09                   <DisplayName="E_WEAPONTYPE_09">,
	E_WEAPONTYPE_10                   <DisplayName="E_WEAPONTYPE_10">,
	E_WEAPONTYPE_11                   <DisplayName="E_WEAPONTYPE_11">,
	E_WEAPONTYPE_12                   <DisplayName="E_WEAPONTYPE_12">,
	E_WEAPONTYPE_13                   <DisplayName="E_WEAPONTYPE_13">,
	E_WEAPONTYPE_14                   <DisplayName="E_WEAPONTYPE_14">,
	E_WEAPONTYPE_15                   <DisplayName="E_WEAPONTYPE_15">,
	E_WEAPONTYPE_16                   <DisplayName="E_WEAPONTYPE_16">,
	E_WEAPONTYPE_17                   <DisplayName="E_WEAPONTYPE_17">,
	E_WEAPONTYPE_TOTAL                <DisplayName="E_WEAPONTYPE_TOTAL">,
	E_WEAPONTYPE_MAX
};

enum ECHAR_CLASS_TYPE
{
// conflicted enum definition clsPlayerBegin(0)
	E_cls_Warrior                     <DisplayName="E_cls_Warrior">,
	E_cls_Mage                        <DisplayName="E_cls_Mage">,
	E_cls_Rogue                       <DisplayName="E_cls_Rogue">,
	E_cls_Archer                      <DisplayName="E_cls_Archer">,
	E_cls_Taoist                      <DisplayName="E_cls_Taoist">,
	E_cls_PlayerTotal                 <DisplayName="E_cls_PlayerTotal">,
	E_cls_Total                       <DisplayName="E_cls_Total">,
	E_cls_MAX
};

enum EVIP_LEVEL
{
	E_vl_Level0                       <DisplayName="E_vl_Level0">,
	E_vl_Level1                       <DisplayName="E_vl_Level1">,
	E_vl_Level2                       <DisplayName="E_vl_Level2">,
	E_vl_Level3                       <DisplayName="E_vl_Level3">,
	E_vl_Total                        <DisplayName="E_vl_Total">,
	E_vl_MAX
};

enum EMODIFIER_EVENT_OCCASION
{
// conflicted enum definition meoBulletEventEnd(13)
// conflicted enum definition meoCharEventBegin(13)
	E_meo_OnBulletCreate              <DisplayName="E_meo_OnBulletCreate">,
	E_meo_BeforeRoll                  <DisplayName="E_meo_BeforeRoll">,
	E_meo_AfterRollAsCaster           <DisplayName="E_meo_AfterRollAsCaster">,
	E_meo_AfterRollAsTarget           <DisplayName="E_meo_AfterRollAsTarget">,
	E_meo_OnBulletHitAsCaster         <DisplayName="E_meo_OnBulletHitAsCaster">,
	E_meo_OnBulletHitAsTarget         <DisplayName="E_meo_OnBulletHitAsTarget">,
	E_meo_AfterApplyDefenceAsCaster   <DisplayName="E_meo_AfterApplyDefenceAsCaster">,
	E_meo_AfterApplyDefenceAsTarget   <DisplayName="E_meo_AfterApplyDefenceAsTarget">,
	E_meo_BeforeDamageEnhancedAsCaster <DisplayName="E_meo_BeforeDamageEnhancedAsCaster">,
	E_meo_BeforeDamageEnhancedAsTarget <DisplayName="E_meo_BeforeDamageEnhancedAsTarget">,
	E_meo_AfterDamageAsCaster         <DisplayName="E_meo_AfterDamageAsCaster">,
	E_meo_AfterDamageAsTarget         <DisplayName="E_meo_AfterDamageAsTarget">,
	E_meo_BeforeApplyDamageAsTarget   <DisplayName="E_meo_BeforeApplyDamageAsTarget">,
	E_meo_OnCharCast                  <DisplayName="E_meo_OnCharCast">,
	E_meo_OnCharCastSuccess           <DisplayName="E_meo_OnCharCastSuccess">,
	E_meo_OnCharPreCast1              <DisplayName="E_meo_OnCharPreCast1">,
	E_meo_OnCharPreCast2              <DisplayName="E_meo_OnCharPreCast2">,
	E_meo_OnCharPreCast3              <DisplayName="E_meo_OnCharPreCast3">,
	E_meo_OnCharPreCast4              <DisplayName="E_meo_OnCharPreCast4">,
	E_meo_OnCharPreCast5              <DisplayName="E_meo_OnCharPreCast5">,
	E_meo_OnTimeup                    <DisplayName="E_meo_OnTimeup">,
	E_meo_OnAppendAura                <DisplayName="E_meo_OnAppendAura">,
	E_meo_OnRemoveAura                <DisplayName="E_meo_OnRemoveAura">,
	E_meo_OnEquipItem                 <DisplayName="E_meo_OnEquipItem">,
	E_meo_OnAuraTimeup                <DisplayName="E_meo_OnAuraTimeup">,
	E_meo_OnAuraEffect                <DisplayName="E_meo_OnAuraEffect">,
	E_meo_OnAppendAuraAsCaster        <DisplayName="E_meo_OnAppendAuraAsCaster">,
	E_meo_OnDying                     <DisplayName="E_meo_OnDying">,
	E_meo_Total                       <DisplayName="E_meo_Total">,
	E_meo_MAX
};

enum ECHAR_MOVE_STATE
{
	E_CMS_INVALID                     <DisplayName="E_CMS_INVALID">,
	E_CMS_ACTIVE_MOVE                 <DisplayName="E_CMS_ACTIVE_MOVE">,
	E_CMS_SIT                         <DisplayName="E_CMS_SIT">,
	E_CMS_WALK                        <DisplayName="E_CMS_WALK">,
	E_CMS_RUN                         <DisplayName="E_CMS_RUN">,
	E_CMS_MOVE                        <DisplayName="E_CMS_MOVE">,
	E_CMS_MINE                        <DisplayName="E_CMS_MINE">,
	E_CMS_PASSIVE_MOVE                <DisplayName="E_CMS_PASSIVE_MOVE">,
	E_CMS_DASH                        <DisplayName="E_CMS_DASH">,
	E_CMS_KNOCK_BACK                  <DisplayName="E_CMS_KNOCK_BACK">,
	E_CMS_THROWN                      <DisplayName="E_CMS_THROWN">,
	E_CMS_FREEZE                      <DisplayName="E_CMS_FREEZE">,
	E_CMS_FEAR                        <DisplayName="E_CMS_FEAR">,
	E_CMS_DIE                         <DisplayName="E_CMS_DIE">,
	E_CMS_CLIENT_SHOW_TIME            <DisplayName="E_CMS_CLIENT_SHOW_TIME">,
	E_CMS_STALL                       <DisplayName="E_CMS_STALL">,
	E_CMS_TOTAL                       <DisplayName="E_CMS_TOTAL">,
	E_CMS_MAX
};

enum EDOODAD_TYPE
{
	E_dt_Invalid                      <DisplayName="E_dt_Invalid">,
	E_dt_Resource                     <DisplayName="E_dt_Resource">,
	E_dt_TreasureBox                  <DisplayName="E_dt_TreasureBox">,
	E_dt_Quest                        <DisplayName="E_dt_Quest">,
	E_dt_Destrucible                  <DisplayName="E_dt_Destrucible">,
	E_dt_Corpse                       <DisplayName="E_dt_Corpse">,
	E_dt_Total                        <DisplayName="E_dt_Total">,
	E_dt_MAX
};

