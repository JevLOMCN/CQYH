//
// 1.05 backport: status proc that exchanges one skill (slot) for another.
// Used by CQ_Skill_Archer/Assassin/Mage/Spear/Taoist.upk inside
// Buff.StartProcList / Buff.EndProcList (buff start: Source -> Change,
// buff end: a second proc with Source and Change swapped back).
//
// RECOVERED from 1.05 archetypes (all 5 packages, 20 objects):
//     SourceAbilityId     : IntProperty   e.g. 2056, 1541, 2554, 1206, 3024
//     ChangeAbilityId     : IntProperty   e.g. 2456, 1934, 2030, 1606, 3401
//     SourceAbilityChange : BoolProperty  (only serialized when true; true
//                           on the start-side proc of several Assassin /
//                           Mage / Lancer buffs, false/absent on the rest)
// Example: Buff 20563 (Archer): StartProcList -> (2056 -> 2456),
//          EndProcList -> (2456 -> 2056).
//
// GUESSED: the meaning of SourceAbilityChange (probably "the source skill's
// own slot data (icon/cooldown) is changed too"). The real effect needs the
// native MMOSkillManager (skill slot redirection); no script API exists for
// it, so this script-only proc is a harmless no-op: the buff loads and runs
// but the skill bar is not swapped. See also MMOStatusProc_SetSkillSlotIcon /
// MMOStatusProc_LoopSkillSlot for the native pattern to copy.
//
class MMOStatusProc_ExchangeSkill extends Object implements(IMMOStatusProc)
	native
	editinlinenew
	hidecategories(Object);

var() int  SourceAbilityId;
var() int  ChangeAbilityId;
var() bool SourceAbilityChange;

event OnExecute(MMOActor actor)
{
	// STUB: intentionally does nothing (see header comment).
}

// IMMOStatusProc declares "native function Update"; implemented as an empty native stub
// (see MMOStatusProc.cpp). The native monster path (ANPC) never uses this class,
// buffs only call OnExecute.
native function Update(MMOActor actor);

DefaultProperties
{
	SourceAbilityChange=false
}