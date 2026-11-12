
interface IPickablePoint
	native;
	//dependson(TypeDefine);

// Item pickup priority
enum EPickPointPriority
{
	//EPPP_Trigger	    <DisplayName="Trigger">,
	//EPPP_NPC         <DisplayName="NPC">,
	//EPPP_Totem       <DisplayName="Totem">,		
	//EPPP_Retinue     <DisplayName="Retinue">,
	//EPPP_Pet         <DisplayName="Pet">,
	//EPPP_DropItem	<DisplayName="DropItem">,
	//EPPP_OtherPlayer	<DisplayName="OtherPlayer">,
	//EPPP_Body	    <DisplayName="Body">,
	EPPP_StaticMeshActor <DisplayName="StaticMeshActor">,
	EPPP_InterpActor <DisplayName="StaticMeshActor">,
	EPPP_Terrain     <DisplayName="Terrain">,
	EPPP_Landscape	<DisplayName="Landscape">,
	//EPPP_Player      <DisplayName="Player">,
	EPPP_ObstacleMesh      <DisplayName="ObstacleMesh">,
	EPPP_PickMax      <DisplayName="PickMax">,
};

native function EPickPointPriority GetPickPointPriority();
