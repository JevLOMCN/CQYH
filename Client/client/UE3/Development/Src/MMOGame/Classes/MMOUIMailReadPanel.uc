class MMOUIMailReadPanel extends MMOUIBaseSlotContainer
	dependson(TypeDefine)
	native(UI)
	config(UIVisibleMask);

enum EMailStatusIcon
{
	EMailStatusIcon_New,
	EMailStatusIcon_Read,
	EMailStatusIcon_System
};

var int _CurrentMailID;
var GFxObject msgBox;

event InitLoad(MMOGFXPlayer pl)
{
	super.InitLoad(pl);
}

// mail
static final delegate OpenReadMailDelegate(int mailID, int mailTime, EMailStatusIcon mailType, int mailAttachmentItemID, int mailAttachmentItemCount,
	string sender, string subject, string content,int totalCD, int cd);


native function NativeInitOnce();
native final function OpenReadMail(int mailID, int mailTime, EMailStatusIcon mailType, int mailAttachmentItemID, int mailAttachmentItemCount,
	string sender, string subject, string content,int totalCD, int cd);
native final function UpdateAcquireAttachment();

native final function DeleteSelected();
native final function OnDeleteSelected();

event CallUpdateMailInfo(int mailID, EMailStatusIcon mailType, int mailAttachmentItemID, string sender, string title, string content,int totalCD,int CD,string msg)
{
	ActionScriptVoid("UCUpdateMailInfo");
}

event CallUpdateAcquireAttachment()
{
	ActionScriptVoid("UCUpdateAcquireAttachment");
}

event InitOnce()
{
	// Note: initialize here first before calling NativeInit below
	super.InitOnce();
}

native function OnMailDelete(int mailID);
native function OnDeleteMail(GFxObject mc);
native function OnDeleteMail_Cancel(GFxObject mc);

event CallSetReadPanelPostion(int x, int y)
{
	ActionScriptVoid("UCSetReadPanelPostion");
}

//------------moved-----------------
native final function ASReplyMail(int mailID);//moved
native final function ASDeleteMail(int mailID);//moved
native final function ASAcquireMailItems(int mailID);//moved

DefaultProperties
{
	IsDynamicLoad=true
}
