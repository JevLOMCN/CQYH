//
// TeleportTarget teleport target point
//

class TeleportTarget extends Actor implements (IPickableActor, IInteractableActor)
	placeable
	native
	hidecategories(Object,Debug,Display,Attachment,Collision,Physics,Advanced)
	ClassGroup(__Server__);

var(MMOServer) editoronly float ServerRangeRadius;
var(MMOServer) editoronly string DisplayName;
//var(MMOServer) editconst int AutoClientID;
var() editoronly EPickPriority PickPriority;

// add by zhangrui05
// fix bug #20450, bound npc (needed for pathfinding, indoor NPC navigation)
var() editoronly int NpcId;

var editoronly ArrowComponent EditorArrow;
var editoronly SpriteComponent EditorSprite;

native function vector GetServerPosition();

native function EPickPriority GetPickPriority();
native function EMMOCursorType GetMouseCursor();

native function vector GetInteractActorLocation();
native function float GetMaxInteractRadius();
native function DoInteract();
native function bool CanInteract() const;
native function SetSilhouette( bool show, Vector sc = vect( 1.f, 1.f, 0.f ) );

cpptext
{
	virtual UBOOL NeedsLoadForClient() const { return FALSE; } // oscar@snda never loaded in game
}


DefaultProperties
{
	Begin Object Class=SpriteComponent Name=Sprite
		Sprite=Texture2D'MMOEditor.TeleportTarget'
		HiddenGame=True
		AlwaysLoadOnClient=False
		AlwaysLoadOnServer=False
		SpriteCategoryName="Server"		
	End Object
	EditorSprite=Sprite
	Components.Add(Sprite);
	//DrawScale=0.2

	ServerRangeRadius=0

	Begin Object Class=ArrowComponent Name=Arrow
		ArrowColor=(R=150,G=200,B=255)
		ArrowSize=5
		bTreatAsASprite=True
		HiddenGame=true
		AlwaysLoadOnClient=False
		AlwaysLoadOnServer=False
		SpriteCategoryName="Navigation"
	End Object
	EditorArrow=Arrow
	Components.Add(Arrow)
	PickPriority=EPP_Floor
	IsPickable=true
	NpcId=0
}
