class MMOCrowdSystem extends Actor
	native
	placeable
	ClassGroup(__Crowd__)
	hidecategories(Object,MMO,Mobile,Debug,Advanced,Physics,Collision,Attachment,Display);

//struct native CrowdInstance
//{
//	var() Vector Location;
//	var() Vector DrawScale3D;
//	var() Vector Direction;
//};

//struct native CrowdGroup
//{
//	var() int CrowdCount;
//	var() int GroupLeaderIndex;
//};

enum EInstanceRenderType
{
	EIRT_Sprite <DisplayName="Sprite">,
	EIRT_StaticMesh <DisplayName="StaticMesh">,
	EIRT_SkeletalMesh <DisplayName="SkeletalMesh">,
};

// StaticMesh
var bool UseStaticMesh;
struct native StaticMeshRenderDataInfo
{
	var() StaticMesh StaticMeshRes;
};
var(Rendering) StaticMeshRenderDataInfo StaticMeshRenderData <editcondition=UseStaticMesh>;


// SkeletalMesh
var bool UseSkeletalMesh;
struct native SkeletalMeshRenderDataInfo
{
	var() SkeletalMesh SkeleMeshRes;
};
var(Rendering) SkeletalMeshRenderDataInfo SkeletalMeshRenderData <editcondition=UseSkeletalMesh>;


// Sprite
var bool UseSprite;
struct native SpriteRenderDataInfo
{
	var() MaterialInterface SpriteMaterial;
};
var(Rendering) SpriteRenderDataInfo SpriteRenderData <editcondition=UseSprite>;

var(Rendering) bool DrawDebugLine;

var() int InstanceCount;
var() float SteeringForceTweaker;
var(Rendering) EInstanceRenderType RenderType;
var(Rendering) bool RenderEditorData;


var() editinline array<MMOCrowdInstance> CustomCrowdInstanceData;
var editinline array<MMOCrowdInstance> CrowdInstanceData;

//var() array<CrowdGroup> CrowdGroupList;

//var() class<MMOCrowdController> CrowdCtrlTemplate;
var() const editinline MMOCrowdInstance CrowdInstanceConfig;
var() const editinline MMOCrowdController CrowdController;
var array<InstancedStaticMeshComponent> InstanceComponents;

//var() SplineActor FollowPath;
var() Vector InitOffsetExtent;
var() Vector WrapBoundingExtent;
//var() float InstanceBoundingRadius;

var(Rendering) MMOCrowdRenderComponentSM RenderComponentSM;

native final function ResetAllInstances();

event PostBeginPlay()
{
	super.PostBeginPlay();

	`log("________________ MMOCrowdSystem PostBeginPlay");

// 	if(CrowdCtrlTemplate != none)
// 	{
// 		CrowdController = Spawn(CrowdCtrlTemplate, self, , , , , true);
// 		CrowdController.Init(self);
// 	}

// 	if(CrowdController != none)
// 	{
// 		CrowdController.Init(self);
// 	}
}

event PreInitCrowdInstance()
{
}


event PostInitCrowdInstance()
{
	local int i;
	local MMOCrowdInstance inst;
	local MMOSteeringBehavior tempSteer;

	for(i = 0; i < CrowdInstanceData.Length; ++i)
	{
		inst = CrowdInstanceData[i];
		inst.PostBeginPlay();

		inst.SteeringBehavior.CrowdInstance = inst;

		// Only handle non-custom instances
		if(i >= CustomCrowdInstanceData.Length)
		{
			tempSteer = CrowdInstanceConfig.SteeringBehavior;

			// Uniformly configure group instance parameters
			inst.DrawScale3D = CrowdInstanceConfig.DrawScale3D;
			inst.Mass = CrowdInstanceConfig.Mass;
			inst.MaxSpeed = CrowdInstanceConfig.MaxSpeed;
			inst.MaxForce = CrowdInstanceConfig.MaxForce * SteeringForceTweaker;
			inst.MaxTurnRate = CrowdInstanceConfig.MaxTurnRate;
			inst.BoundingRadius = CrowdInstanceConfig.BoundingRadius;

		}
		else
		{
			tempSteer = CustomCrowdInstanceData[i].SteeringBehavior;

			// Uniformly configure group instance parameters
			inst.DrawScale3D = CustomCrowdInstanceData[i].DrawScale3D;
			inst.Mass = CustomCrowdInstanceData[i].Mass;
			inst.MaxSpeed = CustomCrowdInstanceData[i].MaxSpeed;
			inst.MaxForce = CustomCrowdInstanceData[i].MaxForce * SteeringForceTweaker;
			inst.MaxTurnRate = CustomCrowdInstanceData[i].MaxTurnRate;
			inst.BoundingRadius = CustomCrowdInstanceData[i].BoundingRadius;

		}

		inst.SteeringBehavior.InitProc = tempSteer.InitProc;
		
		inst.SteeringBehavior.DBoxLength = tempSteer.DBoxLength;

		inst.SteeringBehavior.WallDetectionFeelerLength = tempSteer.WallDetectionFeelerLength;

		inst.SteeringBehavior.WanderJitter = tempSteer.WanderJitter;
		inst.SteeringBehavior.WanderRadius = tempSteer.WanderRadius;
		inst.SteeringBehavior.WanderDistance = tempSteer.WanderDistance;

		inst.SteeringBehavior.WeightSeparation = tempSteer.WeightSeparation;
		inst.SteeringBehavior.WeightCohesion = tempSteer.WeightCohesion;
		inst.SteeringBehavior.WeightAlignment = tempSteer.WeightAlignment;
		inst.SteeringBehavior.WeightWander = tempSteer.WeightWander;
		inst.SteeringBehavior.WeightObstacleAvoidance = tempSteer.WeightObstacleAvoidance;
		inst.SteeringBehavior.WeightWallAvoidance = tempSteer.WeightWallAvoidance;
		inst.SteeringBehavior.WeightSeek = tempSteer.WeightSeek;
		inst.SteeringBehavior.WeightFlee = tempSteer.WeightFlee;
		inst.SteeringBehavior.WeightArrive = tempSteer.WeightArrive;
		inst.SteeringBehavior.WeightPursuit = tempSteer.WeightPursuit;
		inst.SteeringBehavior.WeightOffsetPursuit = tempSteer.WeightOffsetPursuit;
		inst.SteeringBehavior.WeightInterpose = tempSteer.WeightInterpose;
		inst.SteeringBehavior.WeightHide = tempSteer.WeightHide;
		inst.SteeringBehavior.WeightEvade = tempSteer.WeightEvade;
		inst.SteeringBehavior.WeightFollowPath = tempSteer.WeightFollowPath;

		inst.SteeringBehavior.ViewDistance = tempSteer.ViewDistance;

		inst.SteeringBehavior.WaypointSeekDistSq = tempSteer.WaypointSeekDistSq;

		inst.SteeringBehavior.Offset = tempSteer.Offset;
		inst.SteeringBehavior.TheFollowPath = tempSteer.TheFollowPath;
		inst.SteeringBehavior.FollowMode = tempSteer.FollowMode;


		inst.SteeringBehavior.PostBeginPlay();
	}

	if(CrowdController != none)
	{
		CrowdController.NativePostInitCrowdInstance();
	}
}

DefaultProperties
{
	begin object class=MMOCrowdDebugComponent name=DebugComp
	end object
	Components.Add(DebugComp);

	Begin Object Class=SpriteComponent Name=SpriteHUD
		Sprite=Texture2D'MMOEditor.MonsterSpawnPoint'
		HiddenGame=True
		AlwaysLoadOnClient=False
		AlwaysLoadOnServer=False
		SpriteCategoryName="Server"
		Translation=(X=0,Y=0,Z=200);
	End Object
	Components.Add(SpriteHUD);

	begin object class=MMOCrowdController_Bat name=ctrl0
 	end object
 	CrowdController=ctrl0
	Components.Add(ctrl0);

	Begin Object class=MMOCrowdRenderComponentSM name=crcsm
	End Object
	RenderComponentSM=crcsm
	Components.Add(crcsm)

	InstanceCount=2

// 	begin object class=MMOCrowdInstance name=InstCfg
// 	end object
// 	CrowdInstanceConfig=InstCfg
	

	UseSprite=true
	SteeringForceTweaker=200
	InitOffsetExtent=(X=100,Y=100,Z=100)
	WrapBoundingExtent=(X=2000,Y=2000,Z=500)
	//InstanceBoundingRadius=200

}
