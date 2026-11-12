class MMOUISelectRolePanel extends MMOUIBase
native(UI)
config(UIVisibleMask);

enum EAvatarTurnFlag
{
	EATF_None,
	EATF_TurnLeft,
	EATF_TurnRight,
};

var string RandomNums;
var EAvatarTurnFlag AvatarTurn;
var MMOActor   RotActor;
var int TurnRotateSpeed;


event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);
	
	pl.AddPostAdvanceDelegate(PostAdvance);
}

event InitOnce()
{
	super.InitOnce();

}

native function PostAdvance(float DeltaTime);

native final function DeleteRole();
native final function CallBackDeleteRole( GFxObject mc );

native final function TurnBtnUp();
native final function SelectTurnRightPress();
native final function SelectTurnLeftPress();

native final function SelectRole(int SelectRoleIndex);
native final function EnterGame(int SelectRoleIndex);
native final function UpdateRoleListUI();
native final function UpdateRoleList();
native final function ASRoleMove(int offX,int offY);

native final function CallBack_PreLoad();

event CallUpdateRoleList(GFxObject Provider)
{
	ActionScriptVoid("UCUpdateRoleList");
}

event CallUCSetSelectRole(int SelectIndex)
{
	ActionScriptVoid("UCSetSelectRole");
}

event CallEffectshow()
{
	ActionScriptVoid("Effectshow");
}

event CallASSetGameLeftTime( string titleStr, int sec )
{
	ActionScriptVoid("SetLeftGameTime");
}

native function ASCheckFirstOnSelect(GFxObject mc);

native final function ASRequestReturnLogin();
native final function ASRequestJump2CreateRole();
native final  function ASRequestFindRole();

function ASDeleteRole()
{
	DeleteRole();
}

function ASDirectSelectRole(int SelectRoleIndex) //Double-click character selection box
{
	ASSelectRole(SelectRoleIndex);
	ASEnterGame(SelectRoleIndex);
}

function ASSelectRole( int SelectRoleIndex )
{
	SelectRole(SelectRoleIndex);
}

function ASEnterGame(int SelectRoleIndex)
{
	EnterGame(SelectRoleIndex);
}

//Rotate button released, left-handed coordinate system
function ASTurnBtnUp()
{
	TurnBtnUp();
}

//Select character panel, rotate right
function ASSelectTurnRightPress()
{
	SelectTurnRightPress();	
}

//Select character panel, rotate left
function ASSelectTurnLeftPress()
{
	SelectTurnLeftPress();	
}

native function EnterScene();
native function ChangeScene();
native function EnterLogin();

DefaultProperties
{
	IsDynamicLoad=true
	DefaultVisible=false
	AvatarTurn=EATF_None
	RotActor=none
	TurnRotateSpeed=9800
}
