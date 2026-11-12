class MMOUICreateRolePanel extends MMOUIBase
native(UI)
config(UIVisibleMask);


var NPC     CreateShowCharacter;
var MMOActor   RotActor;

event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);
}

event InitOnce()
{
	super.InitOnce();

}

native final function CreateRoleChanged(int career);
native final function CreateSexChanged(int sex);
native final function CreateRole(string createName);

native final function CreateTurnBtnUp();
native final function CreateTurnRightPress();
native final function CreateTurnLeftPress();

native final function ASOnCreateRoleNameInput( string roleName );
native final function ASRequestReturnRoleList();
native final function bool CheckRoleName(string createName);

native final function DispatchChangeEvent(int career,int sex,int careerLast,int sexLast);

native final function ASGetAppearanceData();

native final function ASCallUpdateAppearance(int type, int id);

native final function ASResetAll();
native final function ASRandomCreate(array<GfxObject> randomResult);
native final function ASReturnCreatePanel();

native final function ASRoleMove_Create(int offX,int offY);

native function EnterScene();
native function ChangeScene();
native function EnterLogin();

event CallUCSetAppearanceData(GfxObject dat)
{
	ActionScriptVoid("UCSetAppearanceData");
}

// 1.05 backport: create-screen class description / four featured skills / totem frame per class
event CallSetLocalizationData(GfxObject dat)
{
	ActionScriptVoid("UCSetLocalizationData");
}

event CallSetAllSkillShow(GfxObject arr)
{
	ActionScriptVoid("UCSetAllSkillShow");
}

event CallSetTotemConfig(GfxObject arr)
{
	ActionScriptVoid("UCSetTotemConfig");
}

event CallSetRoleName(string roleName)
{
	ActionScriptVoid("UCSetRoleName");
}

event CallUpdateInput(bool bSaveRole,string strScript)
{
	ActionScriptVoid("UCUpdateInput");
}

event CallSetCreateInfo(int sex,int classID)
{
	ActionScriptVoid("UCSetCreateInfo");
}

event CallSetCreateSexGrpEx(int sex,bool isEx)
{
	ActionScriptVoid("UCSetCreateSexGrpEx");
}

event CallEffectshow()
{
	ActionScriptVoid("Effectshow");
}

event CallSetRoleEnable(int role, bool enable)
{
	ActionScriptVoid("SetRoleEnable");
}

event CallShowCreateModule(int type)
{
	ActionScriptVoid("ShowCreateModule");
}

native function ASCheckFirstOnCreate(GFxObject mc);
native final function CreateRoleCancel();

// function CreateRoleChanged(int RoleCareer)
// {
// 	CreateRoleChanged(RoleCareer);
// }
// 
// function CreateSexChanged(int sex)
// {
// 	CreateSexChanged(sex);
// }
// 
// function CreateRole(string createName)
// {
// 	CreateRole(createName);
// }
// 
// function ASRequestReturnRoleList()
// {
// 	ASRequestReturnRoleList();
// }

// function ASReturnCreatePanel()
// {
// 	ASReturnCreatePanel();
// }
//Rotate button released, left-handed coordinate system
function ASCreateTurnBtnUp()
{
	CreateTurnBtnUp();
}

//Select character panel, rotate right
function ASCreateTurnRightPress()
{
	CreateTurnRightPress();	
}

//Select character panel, rotate left
function ASCreateTurnLeftPress()
{
	CreateTurnLeftPress();	
}

// function ASOnCreateRoleNameInput( string roleName )
// {
// 	ASOnCreateRoleNameInput(roleName);
// }

function ASCallUCGetAppearanceData()
{
	ASGetAppearanceData();
}

function ASCallResetAll()
{
	ASResetAll();
}

function ASCallRandomCreate(array<GfxObject> randomResult)
{
	ASRandomCreate(randomResult);
}

// function ASCallUpdateAppearance(int type, int id)
// {
// 	ASCallUpdateAppearance(type, id);
// }

DefaultProperties
{
	IsDynamicLoad=true
	DefaultVisible=false
}
