class MMOGFXPlayer_Charactor extends MMOGFXPlayer
	native(UI)
	config(MMOGame);


var MMOUICharacter            Character;
var TurnButtonStats TurnButton;

function RegisterCharactor( MMOUICharacter mc )
{
	Character = mc;
	RegisterUI( mc, class'MMOUIManager'.default.Panel_Character);
}

//function DisplayReputationData(int ForceID,int ReputationNow, int ReputationLast )
//{
//	Character.DisplayReputationData(ForceID, ReputationNow,  ReputationLast);
//}

function UpdateReputationData()
{
	Character.UpdateReputationData();
}

function UpdateTitleData(bool bReset)
{
	Character.UpdateTitleData(bReset);
}

function ASCharactorTab( int TabId )
{
	Character.ClickTab( TabId );
}


function ASShowInfoBox( GFxObject data)
{
	//Character.NativeShowInfoBox( data );
}

function ASRefreshInfoBox( GFxObject data )
{
	Character.NativeRefreshInfoBox( data );
}

function ASChangeReputationType( int ID,int ReputationType )
{
	Character.ChangeReputationType( ID, ReputationType );
}

function ASShowTraceReputationInfoPanel()
{
	//Character.ShowTraceReputationInfoPanel();
}

native function ASTitleTabClick(int ShowType);
native function ASUseCurrentTitle( int TitleId );
native function ASClickTitle( int TitleId );

native function ASShowTotalEnhancProp();
native function ASHideTotalEnhancProp();

native function ASShowTips_EnhanceGuide( GFxObject mc );
native function ASHideTips_EnhanceGuide( GFxObject mc );

function TurnRelease()
{
	TurnButton = TBS_None;

}

function ButtonDrag( int deltaX )
{
	local rotator newRot;
	newRot = ShowAvatar.Rotation;
	newRot.Yaw += -deltaX*100; //3030 is the coefficient between mouse movement pixels and angle value
	ShowAvatar.SetPhysics(PHYS_None);
	ShowAvatar.SetRotation(newRot);
}

function TurnPress( bool bLeft)
{
	if(ShowAvatar != none)
		ShowAvatar.SetPhysics(PHYS_None);
	if( bLeft)
		TurnButton = TBS_Left;
	else
		TurnButton = TBS_Right;
}

native function PostAdvance(float DeltaTime );

//Physique enhancement
native function ASSelectEquipSlot(int slotId);
native function ASShowTips_EquipSlot(int id);
native function ASHideTips_EquipSlot(int id);
native function ASShowTips_ActiveIcon(GFxObject mc);
native function ASEnhanceEquipSlot();

native function EnhanceEquipSlotOK();
native function EnhanceEquipSlotCancel();

DefaultProperties
{
	MovieInfoStr="UI_CharactorWidget.CharactorWidget"

	TurnButton=TBS_None
	Priority=5
}
