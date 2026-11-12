
class MMOVolume_Trigger extends TriggerVolume implements(IUniqueID)//implements(IServerArea)
	placeable
	native
	hidecategories(Object, Attachment, Physics, Advanced, Debug, Mobile, Volume);

enum EVolumeTriggerType
{
	EVTT_Trigger <DisplayName="EVTT_Trigger">,				//Trigger zone
	EVVT_PKType <DisplayName="EVVT_PKType">,				//PK zone type
	EVVT_StallZone <DisplayName="EVVT_StallZone">,			//stall zone
	EVVT_Surface <DisplayName="EVVT_Surface">,				//surface type
	EVVT_QuestZone <DisplayName="EVVT_QuestZone">,			//area triggers quest
	EVVT_Door <DisplayName="EVVT_Door">,					//Door
	EVVT_Guide <DisplayName="EVVT_Guide">,					//area triggers guide
	EVVT_Mine <DisplayName="EVVT_Mine">,					//mining allowed
	EVVT_CannotTeleport <DisplayName="EVVT_CannotTeleport">,//teleport forbidden
	EVVT_CannotLoseItem <DisplayName="EVVT_RoleCannotLoseItem">,//player drops equipment on death
	EVVT_CannotDropItem <DisplayName="EVVT_RoleCannotDropItem">,//player throws equipment
	EVVT_NpcCannotDropItem <DisplayName="EVVT_CannotDropItem">,//Npc throws equipment
	EVVT_SlayerArea <DisplayName="EVVT_SlayerArea">,		//slaughter area
};

var(MMOServer) const EVolumeTriggerType	 VolumeType;
// Area ID, consistent with the server config
//var(MMOTrigger) editconst int AreaID;//TODO: currently replaced by ServerUniqueID

var bool EditTriggerProperty;
var(MMOTrigger) int Priority <editcondition=EditTriggerProperty>;
var(MMOTrigger) int MaxTriggerCount <editcondition=EditTriggerProperty>;
var(MMOTrigger) string BehaviorTreeName <editcondition=EditTriggerProperty>;


enum EPKType
{
	EPKT_Normal <DisplayName="EPKT_Normal">,
	EPKT_Free <DisplayName="EPKT_Free">,
	EPKT_Safe <DisplayName="EPKT_Safe">
};

var bool EditPKType;
var(MMOPK) EPKType PKType <editcondition=EditPKType>;

var bool EditStall;
// stall tax rate level
var(MMOStall) int StallLevel <editcondition=EditStall>;

// add by zhangrui05@snda.com >
enum EQuestTriggerType
{
	EQTT_Explore <DisplayName="EQTT_Explore">,
	EQTT_Accept <DisplayName="EQTT_Accept">,
	EQTT_Complete <DisplayName="EQTT_Complete">
};

var bool EditQuestTriggerType;
var(MMOQuest) EQuestTriggerType QuestTriggerType <editcondition=EditQuestTriggerType>;
var(MMOQuest) array<int> Quests;
// < add by zhangrui05@snda.com

var bool EditGuideTriggerType;
var(MMOGuide) int GuideId <editcondition=EditGuideTriggerType>;
// < add by zhangrui05@snda.com

var bool EditBlock;
var(MMOBlock) bool BlockMove <editcondition=EditBlock>;
var(MMOBlock) bool BlockLeap <editcondition=EditBlock>;
var(MMOBlock) bool BlockMissile <editcondition=EditBlock>;
var(MMOBlock) bool BlockBullet <editcondition=EditBlock>;

var bool EditDoor;
var(MMODoor) int DoorID <editcondition=EditDoor>;

var bool EditMine;
var(MMOMine) int nMineLevel <editcondition=EditMine>;

var bool EditTeleport;
var(MMOTeleport) bool CannotTeleport <editcondition=EditTeleport>;

var bool EditLoseItem;
var(MMORoleLoseItem) bool CannotLoseItem <editcondition=EditLoseItem>;

var bool EditDropItem;
var(MMORoleDropItem) bool CannotDropItem <editcondition=EditDropItem>;

var bool EditNpcDropItem;
var(MMODropItem) bool NpcCannotDropItem <editcondition=EditNpcDropItem>;

var (MMOServer)editinline int ServerUniqueID;			//song@snda.com

cpptext
{
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);

	virtual void PostDuplicate() { Super::PostDuplicate(); SetUniqueID(-1); InitUniqueID(); }

	#if WITH_EDITOR
	virtual void CheckForErrors()
	{
		AVolume::CheckForErrors();
	}
	#endif
}

// Register with Facade for lookup
//native function RegisterToFacade();

simulated function PreBeginPlay()
{	
	Super.PreBeginPlay();

	// Collision is enabled in the editor and normally disabled, notified by the server
// 	if(!WorldInfo.IsPlayInEditor())
// 	{
// 		SetCollision(false, false, true);
// 	}

	//RegisterToFacade();
}

//native function int GetAreaID();

//native function SetAutoID(int id);

// Get object size
//native function Box GetObjectServerBounds();

// get object position
//native function vector GetObjectServerPosition();

//event OnServerNotify(bool on)
//{
//	`log("OSCAR:!!!!_______ MMOVolume_Trigger OnServerNotify:" @on);

//	if(on)
//	{
//		// enter
//		TriggerEvent(0);
//	}
//	else
//	{
//		// exit
//		TriggerEvent(1);
//	}
//}

// function TriggerEvent(int outputIndex)
// {
// 	local Sequence GameSeq;
// 	local array<SequenceObject> AllSeqEvents;
// 	local array<int> ActivateIndices;
// 	local int i;
// 	local SeqEvent_Server_AreaChange eventObj;
// 
// 	GameSeq = WorldInfo.GetGameSequence();
// 	if (GameSeq != None)
// 	{
// 		// find any Level Loaded events that exist
// 		GameSeq.FindSeqObjectsByClass(class'SeqEvent_Server_AreaChange', true, AllSeqEvents);
// 
// 		// activate them
// 		ActivateIndices[0] = outputIndex;
// 		for (i = 0; i < AllSeqEvents.Length; i++)
// 		{
// 			eventObj = SeqEvent_Server_AreaChange(AllSeqEvents[i]);
// 			
// 			if(eventObj.IsDesiredInstigator(self))
// 			{
// 				`log("OSCAR:!!!!_______ MMOVolume_Trigger Trigger Event Index:" @outputIndex);
// 				eventObj.CheckActivate(WorldInfo, self, false, ActivateIndices);
// 			}
// 		}
// 	}
// }

//native function array<DynamicBlockingVolume> GetBlockingVolumes();
//native function array<NPCSpawnPoint> GetNPCSpawnPoints();
// Return an identifier name for debugging
//native function string GetClientName();

// native function array<IServerTrigger> GetLinkedTrigger();
// 
// native function ETrapEventType GetTrapEventType();
native function InitUniqueID();			//song@snda.com
native function INT GetUniqueID();
native function SetUniqueID(int id);

// add by zhangrui05@snda.com
native final function OnEnterQuestZone(MMOPlayer Other);
native final function OnLeaveQuestZone(MMOPlayer Other);

// add by zhangrui05@snda.com
native final function OnTriggerGuide(MMOPlayer Other);

//add by JimZhang@snda.com
native final function OnEnterSafe(MMOPlayer Other);
native final function OnLeaveSafe(MMOPlayer Other);
native final function OnEnterFree(MMOPlayer Other);


simulated event Touch( Actor Other, PrimitiveComponent OtherComp, vector HitLocation, vector HitNormal )
{
	Super.Touch(Other, OtherComp, HitLocation, HitNormal);
	
	if (VolumeType == EVVT_QuestZone)
	{
		OnEnterQuestZone(MMOPlayer(Other));
	}
	else if (VolumeType == EVVT_Guide)
	{
		OnTriggerGuide(MMOPlayer(Other));
	}
	else if (VolumeType == EVVT_PKType)
	{
		switch(PKType)
		{
		case EPKT_Safe:
			OnEnterSafe(MMOPlayer(Other));
			break;
		//case EPKT_Free:
		//	OnEnterFree(MMOPlayer(Other));
		//	break;
		}
	}
}

event UnTouch(Actor Other)
{
	super.UnTouch(Other);

	if (Pawn(Other) != none)
	{
		if (VolumeType == EVVT_QuestZone)
		{
			 OnLeaveQuestZone(MMOPlayer(Other));
		}
		else if(VolumeType == EVVT_PKType)
		{
			switch(PKType)
			{
			case EPKT_Safe:
				OnLeaveSafe(MMOPlayer(Other));
				break;
			}
		}
	}
}

// < add by zhangrui05@snda.com

DefaultProperties
{	
	ServerUniqueID=0
	//SupportedEvents(2)=class'SeqEvent_Server_AreaChange'

	Quests(0)=0
	Quests(1)=0
	Quests(2)=0
	Quests(3)=0
}
