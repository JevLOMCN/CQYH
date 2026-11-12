//
// new NPC spawn point
//
class NPCNewSpawnPoint extends Actor
	placeable
	dependson(NPC, TypeDefine)
	native
	hidecategories(Object,Debug,Display,Attachment,Collision,Physics,Advanced, MMO, Mobile)
	ClassGroup(__Server__);

struct native NPCTemplateSet
{
	var() int Weight;
	var deprecated NPC NPCTemplate;
	var() NPCTemplate NPCTemplateNew;
};

var(MMOServer) editoronly editfixedsize array<NPCTemplateSet> NPCTemplateList;
var(MMOServer) editoronly ENPC_QUALITY Quality;
var(MMOServer) editoronly int Range;
var(MMOServer) editoronly IntervalTimeData SpawnTime;
var(MMOServer) editoronly IntervalTimeData ReviveTime;
var(MMOServer) editoronly INT AIData;
var(MMOServer) editoronly int CurrentHP;
var(MMOServer) editoronly int CurrentMP;
var(MMOServer) editoronly bool CanDropItem;
var(MMOServer) editoronly MMOServerPoint PatrolOriginServerPoint;
var(MMOServer) editoronly EAI_PATROL_STYLE PatrolStyle;
var(MMOServer) editoronly int FollowHostID;
var(MMOServer) editoronly int FollowOffsetX;
var(MMOServer) editoronly int FollowOffsetY;

var editoronly ArrowComponent EditorArrow;
var editoronly SpriteComponent EditorSprite;

native function vector GetServerPosition();

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

	NPCTemplateList(0)=()
	NPCTemplateList(1)=()
	NPCTemplateList(2)=()
	NPCTemplateList(3)=()

	CanDropItem=true
	//Quality=E_nq_Random
}
