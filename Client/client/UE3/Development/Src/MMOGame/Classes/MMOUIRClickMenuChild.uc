class MMOUIRClickMenuChild extends MMOUIBase
	native(UI);

var GFxObject _ConfirmBox;
var const int ButtonMaxNum;
var GFxObject CurClickButton; //clicked button
var int ChildStep;
var int ChildHigh;
var int OwnerRoleId;
var array<delegate<ClickFunc> > FuncListChild; // secondary

delegate ClickFunc( int idx );

event ClickItem( int MenuLevel , int itemid )
{
	local delegate<ClickFunc> tClickFunc;
	CurClickButton = GetObject("RClicklistItem"$itemid);
	if( FuncListChild.Length >itemid  && FuncListChild[itemid]!= none)
	{
		tClickFunc = FuncListChild[itemid];
		tClickFunc( itemid );
	}	
	
}

event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);
}

event CallResetChildMenuButton()
{
	ActionScriptVoid("ResetChildMenuButton");		
}
native function ASClickRightChildMenuItem( int MenuLevel, int ItemId );
native function AddChildMenuItem( delegate<ClickFunc> func , string StrName , optional bool UseStringMap = true );

final native function MoveFriend(int x, int y, int id);
final native function ClickMoveFriendToGroup( INT inMenuIdx );

final native function ChangeTeamAuctionLimit( int x,int y, int w);
final native function ClickChangeTeamAuctionLimitColor( int inMenuIdx );

final native function CleanUI();

DefaultProperties
{
	ButtonMaxNum = 20
	DefaultVisible=false
	CloseOnEsc=false
}
