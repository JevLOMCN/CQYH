//
// TeleportOrig teleport origin
//

class SearchPoint extends Actor
	placeable
	native
	hidecategories(Object,Debug,Display,Attachment,Collision,Physics,Advanced);


var(MMOSearch) string Script  <DisplayName="Script">;
var(MMOSearch) int SearchPointID  <DisplayName="ID">;

var editoronly SpriteComponent EditorSprite;

simulated event PostBeginPlay()
{
	super.PostBeginPlay();
}

DefaultProperties
{
	Begin Object Class=SpriteComponent Name=Sprite
		Sprite=Texture2D'MMOEditor.SearchPointFlag'
		HiddenGame=True
		AlwaysLoadOnClient=False
		AlwaysLoadOnServer=False
		SpriteCategoryName="SearchPoint"		
	End Object
	EditorSprite=Sprite
	Components.Add(Sprite);

	SearchPointID = 0;
}
