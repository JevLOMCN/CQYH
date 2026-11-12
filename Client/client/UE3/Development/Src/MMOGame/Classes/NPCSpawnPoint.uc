//
// NPC spawn point
//
// implements(IMMOActorHUD) no Player in Editor
class NPCSpawnPoint extends Actor
	placeable
	dependson(NPC)
	native
	hidecategories(Object,Debug,Display,Attachment,Collision,Physics,Advanced)
	ClassGroup(__Server_Old__);

//specific NPC template info
struct native SpawnNpcData
{
	var() NPC  NPCTemplateArchi;
	var() int     Weight;
};
// NPC template reference
var(MMOServer) editoronly editinline  array<SpawnNpcData> NPCTemplateList;

//@todo ------------Start---------------- gradually delete this block

// spawn group ID, NPCSpawnGroupId, currently assigned automatically only after export, configured manually
var(MMOServerOld) editoronly int GroupID;

// NPC type ID (to be changed to reference Archetype later)
var(MMOServerOld) editoronly int KindID;

// spawn count
var(MMOServerOld) editoronly int SpawnCount;

// In Manual mode, the server must spawn monsters after receiving a specific message (e.g. after a cutscene finishes)
var(MMOServerOld) editoronly bool ManualSpawn;

// AI script function name
var(MMOServerOld) editoronly init string AIScriptFunctionID;

// which area it spawns from, must be set automatically from AreaID
var editoronly int SpawnByAreaID;

var editoronly ArrowComponent EditorArrow;
var editoronly SpriteComponent EditorSprite;

//@todo ------------End---------------- gradually delete this block

native function vector GetServerPosition();

cpptext
{
	//virtual INT GetServerID(){ return AutoServerID; }
	//virtual INT GetObjectServerType() const { return (INT)sbtNpcSpawnPoint; }
	virtual void CheckForErrors();
}
event Tick(  float deltaTime  )
{
	super.Tick( deltaTime );
	//drawing is buffered
	//if ( !class'Engine'.static.IsGame() )
	//	DrawMyHUD( HUD( class'HUD'.static ) );
}
function bool OnlyDrawHUDForOwner(){return false;}

//show SpawnPoint info on the HUD
function bool DrawMyHUD( MMOHud H )
{
// 	local Vector screenPos;
// 	local Vector objectIconWorldPos;
// 	local float cameraDistFromPos;
// 	local float iconWidth, iconHeight;
// 	local float IconDistanceScale;
// 	local int drawX, drawY;
// 	local int nNPCCount;
// 
// 
// 	objectIconWorldPos = Location;//actual position
// 	objectIconWorldPos.Z += 120;
// 	screenPos = H.Canvas.Project(objectIconWorldPos);
// 
// 	cameraDistFromPos = VSize(objectIconWorldPos - H.PlayerOwner.PlayerCamera.ViewTarget.POV.Location);
// 	IconDistanceScale = FClamp(1.0 - cameraDistFromPos/4000,0.25,1)*(H.Canvas.ClipY / 768.0f);
// 
// 	iconWidth = 166*IconDistanceScale;
// 	iconHeight = 18*IconDistanceScale;
// 
// 	H.Canvas.SetDrawColor(255, 0, 0, 255);
// 
// 	drawY = screenPos.y - iconHeight/2;
// 	drawX = screenPos.x - iconWidth/2;
// 
// 	H.Canvas.SetPos(drawX, drawY);
// 	nNPCCount =  NPCTemplateList.Length;
// 	H.Canvas.DrawText("NPCSpawnPoint  Group ID:" @ GroupID @ "NPC Count:"@ nNPCCount );
// 
// 	`log("NPCSpawnPoint::DrawMyHUD"@name @"Location:" @screenPos @"DrawPos:" @drawX @drawY);
	return true;
}

DefaultProperties
{
	Begin Object Class=SpriteComponent Name=Sprite
		Sprite=Texture2D'MMOEditor.MonsterSpawnPoint'
		HiddenGame=True
		AlwaysLoadOnClient=False
		AlwaysLoadOnServer=False
		SpriteCategoryName="Server"		
	End Object
	EditorSprite=Sprite
	Components.Add(Sprite);
	//DrawScale=0.2

	Begin Object Class=ArrowComponent Name=Arrow
		ArrowColor=(R=150,G=200,B=255)
		ArrowSize=3
		bTreatAsASprite=True
		HiddenGame=true
		AlwaysLoadOnClient=False
		AlwaysLoadOnServer=False
		SpriteCategoryName="Navigation"
	End Object
	EditorArrow=Arrow
	Components.Add(Arrow)


	ManualSpawn=false
}
