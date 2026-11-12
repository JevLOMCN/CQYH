//
// NPC spawn point
//
// implements(IMMOActorHUD) no Player in Editor
class DoodadSpawnPoint extends Actor
	placeable
	dependson(NPC)
	native
	hidecategories(Object,Debug,Display,Attachment,Collision,Physics,Advanced)
	ClassGroup(__Server__);


var() editoronly SkeletalMeshComponent SkelComponent;

native function vector GetServerPosition();

cpptext
{
	//virtual INT GetServerID(){ return AutoServerID; }
	//virtual INT GetObjectServerType() const { return (INT)sbtNpcSpawnPoint; }
	virtual UBOOL NeedsLoadForClient() const { return FALSE; } // oscar@snda never loaded in game
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
	//local Vector screenPos;
	//local Vector objectIconWorldPos;
	//local float cameraDistFromPos;
	//local float iconWidth, iconHeight;
	//local float IconDistanceScale;
	//local int drawX, drawY;
	//local int nNPCCount;


	//objectIconWorldPos = Location;//actual position
	//objectIconWorldPos.Z += 120;
	//screenPos = H.Canvas.Project(objectIconWorldPos);

	//cameraDistFromPos = VSize(objectIconWorldPos - H.PlayerOwner.PlayerCamera.ViewTarget.POV.Location);
	//IconDistanceScale = FClamp(1.0 - cameraDistFromPos/4000,0.25,1)*(H.Canvas.ClipY / 768.0f);

	//iconWidth = 166*IconDistanceScale;
	//iconHeight = 18*IconDistanceScale;

	//H.Canvas.SetDrawColor(255, 0, 0, 255);

	//drawY = screenPos.y - iconHeight/2;
	//drawX = screenPos.x - iconWidth/2;

	//H.Canvas.SetPos(drawX, drawY);
	//H.Canvas.DrawText("DoodadSpawnPoint  Group ID:" @ GroupID @ "Doodad Count:"@ nNPCCount );

	//`log("DoodadSpawnPoint::DrawMyHUD"@name @"Location:" @screenPos @"DrawPos:" @drawX @drawY);
	return true;
}

DefaultProperties
{
	Begin Object Class=SpriteComponent Name=Sprite
		Sprite=Texture2D'MMOEditor.DoodadSpawnPoint'
		HiddenGame=True
		AlwaysLoadOnClient=False
		AlwaysLoadOnServer=False
		SpriteCategoryName="Server"		
	End Object
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
	Components.Add(Arrow)

	Begin Object Class=SkeletalMeshComponent Name=SkeletalMeshComponent0
		bUpdateSkelWhenNotRendered=FALSE
		CollideActors=FALSE
		BlockActors=FALSE
		BlockZeroExtent=FALSE
		BlockNonZeroExtent=FALSE
		BlockRigidBody=FALSE
		RBChannel=RBCC_Nothing
		RBCollideWithChannels=()
		AlwaysLoadOnClient=False
		AlwaysLoadOnServer=False
		HiddenGame=True
	End Object
	SkelComponent=SkeletalMeshComponent0
	Components.Add(SkeletalMeshComponent0)
}
