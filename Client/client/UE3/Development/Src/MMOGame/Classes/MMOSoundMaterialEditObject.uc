//
// Sound material editor
//

class MMOSoundMaterialEditObject extends Object
	native
	hidecategories(Object)
	dependson(GamePlayDataManager);

var() EWeaponHitType     HitType;
var() EItemMaterialType     TargetArmorMatType;
var() SoundCue   ToPlaySound;
