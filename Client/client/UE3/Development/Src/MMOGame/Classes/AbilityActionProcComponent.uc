/*
 * 
 * 
 * */
class AbilityActionProcComponent extends AbilityComponent
	hidecategories(Object)
	editinlinenew
	native;

struct native ComponentActionProcInfo
{
	var() int ActionID;
	var() editinline array<IMMOStatusProc> Proc;
};
var(MMOStatusProc) editinline array<ComponentActionProcInfo> ActionProcList<DisplayName="ActionProcList">;

cpptext
	{
		virtual struct BaseComponent* CreateCplusplusNode();
	}
event DoActionProc(Actor BaseActor)
{
	//add by jimzhang, allow calling action in the ability component
	local int i, j;
	local MMOActor actor;
	//`log("****************************************** AbilityAnimationComponent  ");
	if(ActionProcList.Length>0)
	{
		//`log("****************************************** AbilityAnimationComponent  ActionProcList.Length>0 ");
		actor = MMOActor(BaseActor);
		if(actor != none)
		{
			for(i = ActionProcList.Length - 1; i >= 0; --i)
			{
				for(j = 0; j < ActionProcList[i].Proc.Length; ++j)
				{
					ActionProcList[i].Proc[j].OnExecute(actor);
				}
			}			
		}
	}
}



DefaultProperties
{
	TitleString="ActionProc"
	LineColor=(R=0.7098,G=0.9019,B=0.1137,A=1.0)
	bHaveTrail=true
}
